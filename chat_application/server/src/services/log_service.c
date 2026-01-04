#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <stdarg.h>

static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

void log_activity(const char *fmt, ...) {
    pthread_mutex_lock(&log_mutex);
    FILE *f = fopen("./logs/server.log", "a");
    if (f) {
        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);
        char ts[64];
        strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", tm_info);

        fprintf(f, "[%s] ", ts);
        va_list ap;
        va_start(ap, fmt);
        vfprintf(f, fmt, ap);
        va_end(ap);
        fprintf(f, "\n");
        fclose(f);
    }
    pthread_mutex_unlock(&log_mutex);
}

