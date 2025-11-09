#ifndef SHOVOLOGGER_LOGGEREXCEPTION_H
#define SHOVOLOGGER_LOGGEREXCEPTION_H
#include <stdexcept>

namespace logger::exception {

class LoggerException : public std::runtime_error {
public:
    explicit LoggerException(const std::string& what_arg)
        : std::runtime_error(what_arg) {}
};

}

#endif //SHOVOLOGGER_LOGGEREXCEPTION_H