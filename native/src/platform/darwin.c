/**
 * PRTS Native - Darwin (macOS) Platform Implementation
 */

#include "prts/types.h"
#include <mach/mach_time.h>
#include <stdio.h>
#include <time.h>

static mach_timebase_info_data_t timebase_info;
static bool timebase_initialized = false;

prts_timestamp_t prts_timestamp_now(void) {
    if (!timebase_initialized) {
        mach_timebase_info(&timebase_info);
        timebase_initialized = true;
    }

    uint64_t mach_time = mach_absolute_time();
    return mach_time * timebase_info.numer / timebase_info.denom;
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
