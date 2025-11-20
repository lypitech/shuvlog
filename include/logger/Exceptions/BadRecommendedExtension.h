#ifndef SHUVLOG_BADRECOMMENDEDEXTENSION_H
#define SHUVLOG_BADRECOMMENDEDEXTENSION_H

#include "LoggerException.h"

namespace logger::exception
{

class BadRecommendedExtension : public LoggerException
{
public:
    explicit BadRecommendedExtension(std::string extension)
        : LoggerException(std::format(
            "{}: Invalid extension.",
            extension
        )) {}
};

}

#endif //SHUVLOG_BADRECOMMENDEDEXTENSION_H