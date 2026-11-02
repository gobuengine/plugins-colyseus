/* No-op TLS certificate stubs for web builds.
 * The browser handles TLS natively — no certificate bundle needed. */

#include "tls_certificates.h"

void gobuext_tls_certificates_set_api(NativeExtensionInterfaceGetProcAddress p_get_proc_address) {
    (void)p_get_proc_address;
}

void gobuext_tls_certificates_init(void) {}

void gobuext_tls_certificates_cleanup(void) {}

const unsigned char* gobuext_tls_get_ca_certificates(void) {
    return NULL;
}

size_t gobuext_tls_get_ca_certificates_len(void) {
    return 0;
}
