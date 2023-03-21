#ifndef LOGGING_H
#define LOGGING_H

#ifdef __cplusplus
extern "C" {
#endif

/* Print logging output to stdout in addition to the trace log */
// #define LOGGING_STDOUT_MIRROR

#define INFO(fmt, ...) logging_info(fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) logging_warn(fmt, ##__VA_ARGS__)
#define ERR(fmt, ...) logging_error(fmt, ##__VA_ARGS__)

void logging_init(void);

void logging_info(const char* fmt, ...) __attribute__ ((format (printf, 1, 2)));

void logging_warn(const char* fmt, ...) __attribute__ ((format (printf, 1, 2)));

void logging_error(const char* fmt, ...) __attribute__ ((format (printf, 1, 2)));

#ifdef __cplusplus
}
#endif

#endif /* LOGGING_H */
