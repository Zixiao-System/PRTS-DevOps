/**
 * PRTS Native - Linux Platform Implementation
 */

#include "prts/types.h"
#include <time.h>
#include <stdio.h>

prts_timestamp_t prts_timestamp_now(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000 + (uint64_t)ts.tv_nsec;
}

void prts_timestamp_to_str(prts_timestamp_t ts, char* buf, size_t buf_size) {
    if (!buf || buf_size == 0) return;

    time_t seconds = ts / 1000000000;
    uint64_t nanos = ts % 1000000000;

    struct tm tm_info;
    localtime_r(&seconds, &tm_info);

    size_t offset = strftime(buf, buf_size, "%Y-%m-%d %H:%M:%S", &tm_info);
    if (offset < buf_size - 10) {
        snprintf(buf + offset, buf_size - offset, ".%09llu", (unsigned long long)nanos);
    }
}
