/**
 * @file
 * @brief
 * @date 2024-11-29
 * @author wuppu
 */

// 시스템 헤더 파일
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/**
 * @brief 로그메시지를 출력한다.
 * @param[in] func 로그 출력을 수행하는 함수명
 * @param[in] format 출력 라인
 * @param[in] ... 출력라인
 *
 * 본 함수는 직접 호출되지 않으며, 항상 Log() 및 Err() 매크로를 통해 간접 호출된다. \n
 * 전달된 출력문 앞에 함수명이 추가되어 출력된다.
 */
void WSM_PrintLog(const char *func, const char *format, ...)
{
  va_list arg;
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);

  struct tm tm_now;
  memset(&tm_now, 0, sizeof(struct tm));
  tzset();
  localtime_r((time_t *)&(ts.tv_sec), &tm_now);

#if defined(_DEBUG_STDOUT_)
  fprintf(stderr, "[%04u%02u%02u.%02u%02u%02u.%06ld]",
          tm_now.tm_year+1900, tm_now.tm_mon+1, tm_now.tm_mday,
          tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec, ts.tv_nsec / 1000);

  fprintf(stderr, "[%s] ", func);
  va_start(arg, format);
  vfprintf(stderr, format, arg);
  va_end(arg);
#elif defined(_DEBUG_SYSLOG_)
  char log[255];
  sprintf(log, "[%06ld][%s] ", ts.tv_nsec / 1000, func);
  va_start(arg, format);
  vsprintf(log + strlen(log), format, arg);
  va_end(arg);
  syslog(LOG_INFO | LOG_LOCAL0, "%s", log);
#endif
}
