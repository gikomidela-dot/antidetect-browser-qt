/*
 * Timezone Hook Library
 * Intercepts system time calls to spoof timezone
 * Compile: gcc -shared -fPIC -o libtimezone_hook.so timezone_hook.c -ldl
 */

#define _GNU_SOURCE
#include <time.h>
#include <sys/time.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Timezone offset in seconds (set via environment variable)
static int g_timezone_offset = 0;
static int g_initialized = 0;

static void init_timezone_offset() {
    if (g_initialized) return;
    g_initialized = 1;
    
    const char* tz_offset_str = getenv("ANTIDETECT_TZ_OFFSET");
    if (tz_offset_str) {
        g_timezone_offset = atoi(tz_offset_str);
        fprintf(stderr, "[TimezoneHook] Timezone offset set to: %d seconds\n", g_timezone_offset);
    }
}

// Hook time()
time_t time(time_t *tloc) {
    static time_t (*real_time)(time_t *) = NULL;
    if (!real_time) {
        real_time = (time_t (*)(time_t *))dlsym(RTLD_NEXT, "time");
    }
    
    init_timezone_offset();
    
    time_t result = real_time(tloc);
    result += g_timezone_offset;
    
    if (tloc) {
        *tloc = result;
    }
    
    return result;
}

// Hook gettimeofday()
int gettimeofday(struct timeval *__restrict tv, void *__restrict tz) {
    static int (*real_gettimeofday)(struct timeval *__restrict, void *__restrict) = NULL;
    if (!real_gettimeofday) {
        real_gettimeofday = (int (*)(struct timeval *__restrict, void *__restrict))dlsym(RTLD_NEXT, "gettimeofday");
    }
    
    init_timezone_offset();
    
    int result = real_gettimeofday(tv, tz);
    
    if (result == 0 && tv) {
        tv->tv_sec += g_timezone_offset;
    }
    
    return result;
}

// Hook clock_gettime()
int clock_gettime(clockid_t clk_id, struct timespec *tp) {
    static int (*real_clock_gettime)(clockid_t, struct timespec *) = NULL;
    if (!real_clock_gettime) {
        real_clock_gettime = (int (*)(clockid_t, struct timespec *))dlsym(RTLD_NEXT, "clock_gettime");
    }
    
    init_timezone_offset();
    
    int result = real_clock_gettime(clk_id, tp);
    
    if (result == 0 && tp && (clk_id == CLOCK_REALTIME || clk_id == CLOCK_REALTIME_COARSE)) {
        tp->tv_sec += g_timezone_offset;
    }
    
    return result;
}

// Hook localtime()
struct tm *localtime(const time_t *timep) {
    static struct tm *(*real_localtime)(const time_t *) = NULL;
    if (!real_localtime) {
        real_localtime = (struct tm *(*)(const time_t *))dlsym(RTLD_NEXT, "localtime");
    }
    
    init_timezone_offset();
    
    time_t adjusted_time = *timep + g_timezone_offset;
    return real_localtime(&adjusted_time);
}

// Hook localtime_r()
struct tm *localtime_r(const time_t *timep, struct tm *result) {
    static struct tm *(*real_localtime_r)(const time_t *, struct tm *) = NULL;
    if (!real_localtime_r) {
        real_localtime_r = (struct tm *(*)(const time_t *, struct tm *))dlsym(RTLD_NEXT, "localtime_r");
    }
    
    init_timezone_offset();
    
    time_t adjusted_time = *timep + g_timezone_offset;
    return real_localtime_r(&adjusted_time, result);
}
