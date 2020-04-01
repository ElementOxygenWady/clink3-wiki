#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

const char *iotx_ca_crt = \
{
    \
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIDdTCCAl2gAwIBAgILBAAAAAABFUtaw5QwDQYJKoZIhvcNAQEFBQAwVzELMAkG\r\n" \
    "A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\r\n" \
    "b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw05ODA5MDExMjAw\r\n" \
    "MDBaFw0yODAxMjgxMjAwMDBaMFcxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\r\n" \
    "YWxTaWduIG52LXNhMRAwDgYDVQQLEwdSb290IENBMRswGQYDVQQDExJHbG9iYWxT\r\n" \
    "aWduIFJvb3QgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDaDuaZ\r\n" \
    "jc6j40+Kfvvxi4Mla+pIH/EqsLmVEQS98GPR4mdmzxzdzxtIK+6NiY6arymAZavp\r\n" \
    "xy0Sy6scTHAHoT0KMM0VjU/43dSMUBUc71DuxC73/OlS8pF94G3VNTCOXkNz8kHp\r\n" \
    "1Wrjsok6Vjk4bwY8iGlbKk3Fp1S4bInMm/k8yuX9ifUSPJJ4ltbcdG6TRGHRjcdG\r\n" \
    "snUOhugZitVtbNV4FpWi6cgKOOvyJBNPc1STE4U6G7weNLWLBYy5d4ux2x8gkasJ\r\n" \
    "U26Qzns3dLlwR5EiUWMWea6xrkEmCMgZK9FGqkjWZCrXgzT/LCrBbBlDSgeF59N8\r\n" \
    "9iFo7+ryUp9/k5DPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8E\r\n" \
    "BTADAQH/MB0GA1UdDgQWBBRge2YaRQ2XyolQL30EzTSo//z9SzANBgkqhkiG9w0B\r\n" \
    "AQUFAAOCAQEA1nPnfE920I2/7LqivjTFKDK1fPxsnCwrvQmeU79rXqoRSLblCKOz\r\n" \
    "yj1hTdNGCbM+w6DjY1Ub8rrvrTnhQ7k4o+YviiY776BQVvnGCv04zcQLcFGUl5gE\r\n" \
    "38NflNUVyRRBnMRddWQVDf9VMOyGj/8N7yy5Y0b2qvzfvGn9LhJIZJrglfCm7ymP\r\n" \
    "AbEVtQwdpf5pLGkkeB6zpxxxYu7KyJesF12KwvhHhm4qxFYxldBniYUr+WymXUad\r\n" \
    "DKqC5JlR3XC321Y9YeRq4VzW9v493kHMB65jUr9TU/Qr6cf9tveCX4XSQRjbgbME\r\n" \
    "HMUfpIBvFSDJ3gyICh3WZlXi/EjJKSZp4A==\r\n" \
    "-----END CERTIFICATE-----"
};

extern int HAL_SSL_Write(uintptr_t handle, const char *buf, int len, int timeout_ms);
extern int32_t HAL_SSL_Destroy(uintptr_t handle);
extern uintptr_t HAL_SSL_Establish(const char *host, uint16_t port, const char *ca_crt, uint32_t ca_crt_len);
extern const char *iotx_ca_crt;

int main(void)
{
    int         res = -1;
    uintptr_t   hdl = 0;
    char        buf[8];

    hdl = HAL_SSL_Establish("public.iot-as-mqtt.cn-shanghai.aliyuncs.com", 443, iotx_ca_crt, strlen(iotx_ca_crt) + 1);
    printf("hdl = 0x%08x\n", (unsigned int)hdl);

    if (hdl) {
        printf("\n=> #1. HAL_SSL_Establish() TEST PASS!\n\n");
    } else {
        printf("\n=> #1. HAL_SSL_Establish() TEST FAIL!\n\n");
        return -1;
    }

#define WRITE_TESTBUF       "hello"

    res = HAL_SSL_Write(hdl, WRITE_TESTBUF, strlen(WRITE_TESTBUF), 2000);
    printf("res = %d\n", res);

    if (strlen(WRITE_TESTBUF) == res) {
        printf("\n=> #2. HAL_SSL_Write() TEST PASS!\n\n");
    } else {
        printf("\n=> #2. HAL_SSL_Write() TEST FAIL!\n\n");
    }

    res = HAL_SSL_Destroy(hdl);
    printf("res = %d\n", res);

    if (!res) {
        printf("\n=> #3. HAL_SSL_Destroy() TEST PASS!\n\n");
    } else {
        printf("\n=> #3. HAL_SSL_Destroy() TEST FAIL!\n\n");
        return -1;
    }

    return 0;
}
