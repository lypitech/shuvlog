#ifndef SHUVLOG_LOGGEREXCEPTION_H
#define SHUVLOG_LOGGEREXCEPTION_H

#include <iostream>
#include <stdexcept>

namespace logger::exception
{

class LoggerException : public std::runtime_error
{
public:
    explicit LoggerException(const std::string& what_arg)
        : std::runtime_error(what_arg) {}
};

inline std::ostream &operator<<(
    std::ostream& os,
    const LoggerException& exception
)
{
    os << exception.what() << std::endl;
    return os;
}

}

#endif //SHUVLOG_LOGGEREXCEPTION_H
