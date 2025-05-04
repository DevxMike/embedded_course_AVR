#ifndef logger_hpp
#define logger_hpp

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#define LOG_BUF_SIZE 128

template <typename CommIface, typename Timestamp_t>
class Logger {
private:   
    CommIface& iface;
    volatile Timestamp_t& ts;

public:
    Logger(CommIface& ciface, volatile Timestamp_t& timstmp):
        iface { ciface }, ts { timstmp } {}

    void log(const char* fmt, ...) {
        char buf[LOG_BUF_SIZE];
        va_list args;

        va_start(args, fmt);
        int len = snprintf(buf, LOG_BUF_SIZE, fmt, args);
        va_end(args);

        char out[LOG_BUF_SIZE + 32];
        snprintf(out, sizeof(out), "[%lu] %s", ts, buf);

        iface.puts(out);
    }
};

#endif