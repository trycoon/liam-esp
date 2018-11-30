#include "configuration.h"

namespace Utils {

    /**
     * Generate a XX character long key/password of ASCII characters
     * @param length length of key in characters
     */
    String generateKey(uint8_t length) {
        const String CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        String key = "";
        // get true random number from the hardware RNG. this must be called after WiFi has been enabled to be truly random.
        for (int i = 0; i < length; i++) {
            key += CHARS[random(0, CHARS.length())];
        }
        
        return key;
    }

    /**
     * Convert uint64_t to an String
     */
    String uint64String(uint64_t value, unsigned char base)
    {
        char buf[1 + 8 * sizeof(uint64_t)];
        ultoa(value, buf, base);
        return String(buf);
    }
}