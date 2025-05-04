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

public:
    Logger(CommIface& ciface):
        iface { ciface } {}

    void log(const char* fmt, ...) {
        char buf[LOG_BUF_SIZE];
        va_list args;

        va_start(args, fmt);
        int len = snprintf(buf, LOG_BUF_SIZE, fmt, args);
        va_end(args);

        iface.puts(buf);
    }
};

#endif