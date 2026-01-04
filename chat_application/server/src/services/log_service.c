#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <stdarg.h>
#include <string.h>

// ANSI color codes
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[1;31m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_BLUE    "\033[1;34m"

static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

static const char* get_color_for_level(const char *message) {
    if (strstr(message, "[ERROR]") != NULL) {
        return COLOR_RED;
    } else if (strstr(message, "[WARNING]") != NULL) {
        return COLOR_YELLOW;
    } else if (strstr(message, "[INFO]") != NULL) {
        return COLOR_GREEN;
    } else if (strstr(message, "[DEBUG]") != NULL) {
        return COLOR_BLUE;
    }
    return COLOR_RESET;
}

void log_activity(const char *fmt, ...) {
    pthread_mutex_lock(&log_mutex);
    
    // Format the message first
    char message[2048];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(message, sizeof(message), fmt, ap);
    va_end(ap);
    
    // Get timestamp
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char ts[64];
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", tm_info);
    
    // Write to file (without colors)
    FILE *f = fopen("./logs/server.log", "a");
    if (f) {
        fprintf(f, "[%s] %s\n", ts, message);
        fclose(f);
    }
    
    // Write to console (with colors)
    const char *color = get_color_for_level(message);
    printf("%s[%s] %s%s\n", color, ts, message, COLOR_RESET);
    fflush(stdout);
    
    pthread_mutex_unlock(&log_mutex);
}

