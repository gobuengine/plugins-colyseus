#ifndef COLYSEUS_TLS_CERTIFICATES_H
#define COLYSEUS_TLS_CERTIFICATES_H

#include <stddef.h>
#include <nativeextension_interface.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Set the NativeExtension API function for accessing singletons.
 * Must be called from colyseus_sdk_init() before gobuext_tls_certificates_init().
 */
void gobuext_tls_certificates_set_api(NativeExtensionInterfaceGetProcAddress p_get_proc_address);

/**
 * Initialize TLS certificates for the Gobu platform.
 * 
 * This function checks Gobu's ProjectSettings for a certificate bundle override.
 * If found, it loads certificates from that file.
 * Otherwise, it uses the bundled Mozilla CA certificates.
 * 
 * Should be called during extension initialization.
 */
void gobuext_tls_certificates_init(void);

/**
 * Clean up TLS certificates.
 * Should be called during extension deinitialization.
 */
void gobuext_tls_certificates_cleanup(void);

/**
 * Get the loaded CA certificate data.
 * Returns a pointer to PEM-formatted certificate data (null-terminated).
 * Returns NULL if no certificates are loaded.
 */
const unsigned char* gobuext_tls_get_ca_certificates(void);

/**
 * Get the length of the loaded CA certificate data.
 * Returns the length including the null terminator.
 * Returns 0 if no certificates are loaded.
 */
size_t gobuext_tls_get_ca_certificates_len(void);

#ifdef __cplusplus
}
#endif

#endif /* COLYSEUS_TLS_CERTIFICATES_H */
