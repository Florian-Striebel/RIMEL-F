


























#if !defined(LIB9P_LOG_H)
#define LIB9P_LOG_H

enum l9p_log_level {
L9P_DEBUG,
L9P_INFO,
L9P_WARNING,
L9P_ERROR
};

void l9p_logf(enum l9p_log_level level, const char *func, const char *fmt, ...);

#if defined(L9P_DEBUG)
#define L9P_LOG(level, fmt, ...) l9p_logf(level, __func__, fmt, ##__VA_ARGS__)
#else
#define L9P_LOG(level, fmt, ...)
#endif

#endif
