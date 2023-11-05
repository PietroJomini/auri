#pragma once

#ifndef TAK_LOGL
#define TAK_LOGL 2  // default to warning
#endif

#if TAK_LOGL >= 4  // trace
#define tak_logt(...) tak_log(4, __FILE__, __LINE__, __VA_ARGS__)
#else
#define tak_logt(...) \
    {}
#endif

#if TAK_LOGL >= 3  // info
#define tak_logi(...) tak_log(3, __FILE__, __LINE__, __VA_ARGS__)
#else
#define tak_logi(...) \
    {}
#endif

#if TAK_LOGL >= 2  // warning
#define tak_logw(...) tak_log(2, __FILE__, __LINE__, __VA_ARGS__)
#else
#define tak_logw(...) \
    {}
#endif

#if TAK_LOGL >= 1  // error
#define tak_loge(...) tak_log(1, __FILE__, __LINE__, __VA_ARGS__)
#else
#define tak_loge(...) \
    {}
#endif

extern const char *tak_log_leveln[4];
extern const char *tak_log_levelc[4];

void tak_log(const int level, const char *file, int line, const char *format, ...);