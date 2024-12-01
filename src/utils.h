/****************************************************************************
 * @file utils.h                                                            *
 * @brief Utility functions and macros for the memory allocator.            *
 *                                                                          *
 * This file contains utility function declarations and macro definitions   *
 * used throughout the memory allocator project.                            *
 *                                                                          *
 * @author Ameed Othman                                                     *
 * @date 30/11/2024                                                         *
 ****************************************************************************/

#ifndef __UTILS_H__
#define __UTILS_H__

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
// Macros

/****************************************************************************
 * @brief Aligns size up to the nearest multiple of alignment.              *
 *                                                                          *
 * @param size The size to align.                                           *
 * @param alignment The alignment boundary.                                 *
 * @return size_t The aligned size.                                         *
 ****************************************************************************/
#define ALIGN_UP(size, alignment) \
    (((size) + ((alignment) - 1)) & ~((alignment) - 1))

/****************************************************************************
 * @brief Aligns pointer up to the nearest multiple of alignment.           *
 *                                                                          *
 * @param ptr The pointer to align.                                         *
 * @param alignment The alignment boundary.                                 *
 * @return void* The aligned pointer.                                       *
 ****************************************************************************/
#define PTR_ALIGN_UP(ptr, alignment) \
    ((void*)((((size_t)(ptr)) + ((alignment) - 1)) & ~((alignment) - 1)))

// Function Declarations

/****************************************************************************
 * @brief Logs a message with a specified log level.                        *
 *                                                                          *
 * This function formats and logs a message with a specified log level      *
 * (e.g., "DEBUG", "ERROR"). It uses a variable argument list to handle     *
 * the additional arguments.                                                *
 *                                                                          *
 * @param level The log level (e.g., "DEBUG", "ERROR").                     *
 * @param format The format string.                                         *
 * @param args The variable argument list.                                  *
 ****************************************************************************/
static void log_message(const char* level, const char* format, va_list args);

/****************************************************************************
 * @brief Logs a debug message.                                             *
 *                                                                          *
 * @param format The format string.                                         *
 * @param ... Additional arguments.                                         *
 ****************************************************************************/
void log_debug(const char* format, ...);

/****************************************************************************
 * @brief Logs an error message.                                            *
 *                                                                          *
 * @param format The format string.                                         *
 * @param ... Additional arguments.                                         *
 ****************************************************************************/
void log_error(const char* format, ...);

#endif