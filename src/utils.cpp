#include <sys/time.h>
#include "configuration.h"

namespace Utils {
    /**
     * If ESP has synced its software clock with the NTP-servers, in other words can we get a correct time reading.
     */
    bool isTimeAvailable = false;

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

    /**
     * Gets seconds since Unix epoctime (1970-01-01)
     */
    int64_t getEpocTime() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
    }

    /**
     * Get current date/time as a string
     * @param format e.g. "%d %b %Y, %H:%M:%S%z"
     * @param timeout for how many milliseconds we try to obtain time
     */
    String getTime(String format, uint32_t timeout) {
        struct tm timeinfo;

        if (!getLocalTime(&timeinfo, timeout)) {
            isTimeAvailable = false;
            return F("Failed to obtain time");
        }

        isTimeAvailable = true;

        char outstr[80];
        strftime(outstr, sizeof(outstr), format.c_str(), &timeinfo); // ISO 8601 time

        return String(outstr);
    }
}