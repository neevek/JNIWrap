#ifndef NUL_LOG_H_
#define NUL_LOG_H_
#include <stdio.h>
#include <string.h>
#include <stdarg.h>    // for va_list, va_start and va_end
#include <time.h>
#include <sys/time.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__FILENAME__)
#define __FILENAME__\
  (strrchr(__FILE__, '/') ?\
   strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define MAX_FMT_SIZE 0xFF
#define TIME_BUFFER_SIZE 24

#ifdef __ANDROID__
#include <android/log.h>
#define LOG_LEVEL_VERBOSE ANDROID_LOG_VERBOSE 
#define LOG_LEVEL_DEBUG ANDROID_LOG_DEBUG
#define LOG_LEVEL_INFO ANDROID_LOG_INFO
#define LOG_LEVEL_WARN ANDROID_LOG_WARN
#define LOG_LEVEL_ERROR ANDROID_LOG_ERROR
#else
#define LOG_LEVEL_VERBOSE 2
#define LOG_LEVEL_DEBUG 3
#define LOG_LEVEL_INFO 4
#define LOG_LEVEL_WARN 5
#define LOG_LEVEL_ERROR 6
#endif

inline char *log_strtime(char *buffer) {
  struct timeval now;
  gettimeofday(&now, NULL);

  size_t len = strftime(buffer, TIME_BUFFER_SIZE, "%Y-%m-%d %H:%M:%S.",
      localtime(&now.tv_sec));
  int milli = now.tv_usec / 1000;
  sprintf(buffer + len, "%03d", milli);

  return buffer;
}

inline static const char *log_prio_str_(int prio) {
  switch(prio) { 
    case LOG_LEVEL_VERBOSE: return "V";
    case LOG_LEVEL_DEBUG: return "D";
    case LOG_LEVEL_INFO: return "I";
    case LOG_LEVEL_WARN: return "W";
    case LOG_LEVEL_ERROR: return "E";
  } 
  return "";
}

// log to file
#if defined(LOG_TO_FILE) && defined(LOG_FILE_PATH)
#define DO_LOG_(prio, color, fmt, ...) do { \
  FILE *f = fopen(LOG_FILE_PATH, "a+"); \
  char _LogTimeBuf_[TIME_BUFFER_SIZE];  \
  fprintf(f, "%s %s [%s] [%s:%d] %s - " fmt "\n", \
      log_strtime(_LogTimeBuf_), LOG_TAG_NAME, log_prio_str_(prio), \
      __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
  fclose(f); \
} while (0)

// log to Android logcat
#elif __ANDROID__
#define DO_LOG_(prio, color, fmt, ...) do { \
  __android_log_print(prio, LOG_TAG_NAME, "[%s:%d] %s - " fmt "\n", \
      __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
} while (0)

#else
#ifndef NO_TERM_COLOR
#define NO_TERM_COLOR
#define KNRM  "\x1B[0m"
#define KBLU  "\x1b[34m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[92m"
#define KYEL  "\x1B[93m"
#define KEND  KNRM
#else
#define KNRM
#define KBLU
#define KRED
#define KGRN
#define KYEL 
#define KEND
#endif

// log to stderr
#define DO_LOG_(prio, color, fmt, ...) do { \
  char _LogTimeBuf_[TIME_BUFFER_SIZE];  \
  fprintf(stderr, color "%s %s [%s] [%s:%d] %s - " fmt KEND "\n", \
      log_strtime(_LogTimeBuf_), LOG_TAG_NAME, log_prio_str_(prio), __FILENAME__, \
      __LINE__, __FUNCTION__, ##__VA_ARGS__); \
} while (0)
#endif

#if defined(LOG_VERBOSE)
#define LOG_V(fmt, ...) DO_LOG_(LOG_LEVEL_VERBOSE, KNRM, fmt, ##__VA_ARGS__)
#define LOG_DEBUG
#define LOG_INFO
#define LOG_WARN
#define LOG_ERROR

#define LOG_LEVEL LOG_LEVEL_VERBOSE 
#else
#define LOG_V(fmt, ...)
#endif

#if defined(LOG_DEBUG)
#define LOG_D(fmt, ...) DO_LOG_(LOG_LEVEL_DEBUG, KGRN, fmt, ##__VA_ARGS__)
#define LOG_INFO
#define LOG_WARN
#define LOG_ERROR

#undef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_VERBOSE 
#else
#define LOG_D(fmt, ...)
#endif

#if defined(LOG_INFO)
#define LOG_I(fmt, ...) DO_LOG_(LOG_LEVEL_INFO, KBLU, fmt, ##__VA_ARGS__)
#define LOG_WARN
#define LOG_ERROR

#undef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INFO
#else
#define LOG_I(fmt, ...)
#endif

#if defined(LOG_WARN)
#define LOG_W(fmt, ...) DO_LOG_(LOG_LEVEL_WARN, KYEL, fmt, ##__VA_ARGS__)
#define LOG_ERROR

#undef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_WARN
#else
#define LOG_W(fmt, ...)
#endif

#if defined(LOG_ERROR)
#define LOG_E(fmt, ...) DO_LOG_(LOG_LEVEL_ERROR, KRED, fmt, ##__VA_ARGS__)

#undef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_ERROR
#else
#define LOG_E(fmt, ...)
#endif

#ifndef LOG_LEVEL
#define LOG_LEVEL 7
#endif

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: NUL_LOG_H_ */


