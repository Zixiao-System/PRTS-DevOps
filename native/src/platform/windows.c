/**
 * PRTS Native - Windows Platform Implementation
 */

#include "prts/types.h"
#include <windows.h>
#include <stdio.h>

static LARGE_INTEGER frequency;
static LARGE_INTEGER start_time;
static bool initialized = false;

prts_timestamp_t prts_timestamp_now(void) {
    if (!initialized) {
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&start_time);
        initialized = true;
    }

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    /* Convert to nanoseconds */
    return (uint64_t)(now.QuadPart - start_time.QuadPart) * 1000000000 / frequency.QuadPart;
}

void prts_timestamp_to_str(prts_timestamp_t ts, char* buf, size_t buf_size) {
    if (!buf || buf_size == 0) return;

    FILETIME ft;
    SYSTEMTIME st;

    /* Convert nanoseconds since epoch to FILETIME */
    ULARGE_INTEGER uli;
    uli.QuadPart = ts / 100 + 116444736000000000ULL;
    ft.dwLowDateTime = uli.LowPart;
    ft.dwHighDateTime = uli.HighPart;

    FileTimeToSystemTime(&ft, &st);

    snprintf(buf, buf_size, "%04d-%02d-%02d %02d:%02d:%02d.%09llu",
        st.wYear, st.wMonth, st.wDay,
        st.wHour, st.wMinute, st.wSecond,
        (unsigned long long)(ts % 1000000000));
}
