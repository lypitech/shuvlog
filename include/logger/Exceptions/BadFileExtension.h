#ifndef SHUVLOG_BADFILEEXTENSION_H
#define SHUVLOG_BADFILEEXTENSION_H

#include <algorithm>
#include <format>

#include "LoggerException.h"

namespace logger::exception
{

class BadFileExtension : public LoggerException
{
public:
  explicit BadFileExtension(std::string type, std::string extension)
      : LoggerException(std::format(
          "CAUTION: Prefer piping {} to {} files.",
          type, extension
      )) {}
};

}

#endif // SHUVLOG_BADFILEEXTENSION_H
