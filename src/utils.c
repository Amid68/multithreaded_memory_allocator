/****************************************************************************
 * @file utils.c                                                            *
 * @brief Implementation of utility functions for the memory allocator.     *
 *                                                                          *
 * This file provides implementations for logging and debugging utilities.  *
 *                                                                          *
 * @author Ameed Othman                                                     *
 * @date 30/11/2024                                                         *
 ****************************************************************************/

#include "utils.h"

// Enable or disable debug logging
#ifdef DEBUG
#define LOG_DEBUG_ENABLED 1
#else
#define LOG_DEBUG_ENABLED 0
#endif

void log_debug(const char* format, ...) {
    if (LOG_DEBUG_ENABLED) {
        va_list args;
        va_start(args, format);
        log_message("DEBUG", format, args);
        va_end(args);
    }
}

void log_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    log_message("ERROR", format, args);
    va_end(args);
}

static void log_message(const char* level, const char* format, va_list args) {
    time_t now = time(NULL);
    struct tm* local_time = localtime(&now);

    fprintf(stderr, "[%02d:%02d:%02d] %s: ",
            local_time->tm_hour,
            local_time->tm_min,
            local_time->tm_sec,
            level);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
}