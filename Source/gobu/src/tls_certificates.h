#ifndef COLYSEUS_TLS_CERTIFICATES_H
#define COLYSEUS_TLS_CERTIFICATES_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize TLS certificates for the Gobu platform.
 * 
 * This function checks Gobu's ProjectSettings for a certificate bundle override.
 * If found, it loads certificates from that file.
 * Otherwise, it uses the bundled Mozilla CA certificates.
 * 
 * Called from gobu_plugin_init() (plugin.c).
 */
void gobuext_tls_certificates_init(void);

/**
 * Clean up TLS certificates.
 * Called when the plugin is deinitialized.
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
