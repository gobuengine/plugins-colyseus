#include "tls_certificates.h"
#include "gobu_colyseus.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Override certificate storage. This binding only provides an explicit
 * certificate_bundle_override; the bundled Mozilla CA roots are loaded by the
 * core SDK itself, so we leave this NULL when no override is configured. */
static const unsigned char* g_ca_pem_data = NULL;
static size_t g_ca_pem_len = 0;
static unsigned char* g_loaded_pem_data = NULL;  /* Dynamically loaded data (must be freed) */
static bool g_initialized = false;

/* Forward declarations */
static bool load_certificates_from_gobu_path(const char* path);
static char* get_project_settings_override_path(void);

void gobuext_tls_certificates_init(void) {
    if (g_initialized) {
        return;
    }
    g_initialized = true;

    /* Try to get override path from ProjectSettings */
    char* override_path = get_project_settings_override_path();

    if (override_path && override_path[0] != '\0') {
        fprintf(stdout, "[Colyseus] Loading TLS certificate override from: %s\n", override_path);
        fflush(stdout);

        if (!load_certificates_from_gobu_path(override_path)) {
            fprintf(stderr, "[Colyseus] Failed to load certificate override; falling back to built-in CA roots (hint: copy a packed res:// asset to user:// first)\n");
            fflush(stderr);
        }
    }

    free(override_path);

    /* No override configured (or it failed to load): nothing to provide here.
     * The core SDK always loads the bundled Mozilla CA roots, so WSS still
     * verifies against the standard trust store. */
}

void gobuext_tls_certificates_cleanup(void) {
    if (g_loaded_pem_data) {
        free(g_loaded_pem_data);
        g_loaded_pem_data = NULL;
    }
    g_ca_pem_data = NULL;
    g_ca_pem_len = 0;
    g_initialized = false;
}

const unsigned char* gobuext_tls_get_ca_certificates(void) {
    return g_ca_pem_data;
}

size_t gobuext_tls_get_ca_certificates_len(void) {
    return g_ca_pem_len;
}

/* Call a ProjectSettings method that takes one String argument and returns a
 * String. Returns a malloc'd UTF-8 string (caller frees), or NULL on any error
 * or non-String result. */
static char* call_project_settings_string_method(const char* method_name, const char* arg_utf8) {
    GobuObject* project_settings = gobu->get_singleton("ProjectSettings");
    if (!project_settings) {
        return NULL;
    }

    GobuVariant arg_variant;
    gobuext_new_string(&arg_variant, arg_utf8);
    const GobuVariant* args[1] = { &arg_variant };
    GobuVariant result;
    GobuCallError error = { 0, 0, 0 };
    gobu->object_call(project_settings, method_name, args, 1, &result, &error);

    char* result_str = NULL;
    if (error.error == GOBU_CALL_OK && gobu->variant_get_type(&result) == GOBU_TYPE_STRING) {
        result_str = gobuext_to_utf8(&result);
    }

    gobu->variant_destroy(&result);
    gobu->variant_destroy(&arg_variant);
    return result_str;
}

/* Get the certificate bundle override path from ProjectSettings (NULL when it is not set) */
static char* get_project_settings_override_path(void) {
    GobuVariant value;
    char* path = NULL;
    if (gobu->get_setting("network/tls/certificate_bundle_override", &value) && gobu->variant_get_type(&value) == GOBU_TYPE_STRING) {
        path = gobuext_to_utf8(&value);
    }
    gobu->variant_destroy(&value);
    return path;
}

/* Load certificates from a Gobu resource path (res:// or user://) or an
 * absolute OS path. Gobu virtual paths are resolved to a real filesystem path
 * via ProjectSettings.globalize_path() so plain C file I/O can read them; on
 * Android this is what makes a user:// override (the documented workaround)
 * readable. res:// assets packed inside the .pck/.apk have no OS path and won't
 * be loadable this way — copy them to user:// first. */
static bool load_certificates_from_gobu_path(const char* path) {
    if (!path || path[0] == '\0') {
        return false;
    }

    /* Resolve res://, user:// (and leave absolute paths effectively unchanged). */
    char* globalized = call_project_settings_string_method("globalize_path", path);
    const char* fs_path = (globalized && globalized[0] != '\0') ? globalized : path;

    /* Try to load as a filesystem path */
    FILE* fp = fopen(fs_path, "rb");
    if (!fp) {
        fprintf(stderr, "[Colyseus] Failed to open certificate file: %s (resolved: %s)\n", path, fs_path);
        free(globalized);
        return false;
    }
    free(globalized);
    
    /* Get file size */
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    if (file_size <= 0) {
        fclose(fp);
        return false;
    }
    
    /* Allocate buffer (+ 1 for null terminator) */
    g_loaded_pem_data = (unsigned char*)malloc(file_size + 1);
    if (!g_loaded_pem_data) {
        fclose(fp);
        return false;
    }
    
    /* Read file */
    size_t bytes_read = fread(g_loaded_pem_data, 1, file_size, fp);
    fclose(fp);
    
    if (bytes_read != (size_t)file_size) {
        free(g_loaded_pem_data);
        g_loaded_pem_data = NULL;
        return false;
    }
    
    /* Null terminate */
    g_loaded_pem_data[file_size] = '\0';
    
    g_ca_pem_data = g_loaded_pem_data;
    g_ca_pem_len = file_size + 1;
    
    fprintf(stdout, "[Colyseus] Loaded %ld bytes of certificates from: %s\n", file_size, path);
    fflush(stdout);
    
    return true;
}
