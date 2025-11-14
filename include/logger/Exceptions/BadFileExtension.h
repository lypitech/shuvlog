#ifndef SHOVOLOGGER_BADFILEEXTENSION_H
#define SHOVOLOGGER_BADFILEEXTENSION_H

#include <format>

#include "LoggerException.h"

namespace logger::exception
{

class BadFileExtension : public LoggerException
{
public:
  explicit BadFileExtension(std::string extension)
      : LoggerException(std::format(
          "CAUTION: Prefer piping {} to .{} files.",
          extension,
          [] (std::string s) {
              std::ranges::transform(s, s.begin(),
              [](const unsigned char c) {
                  return static_cast<char>(std::tolower(c));
              });
              return s;
          }(extension) // C++ way of lowering case of string with an
                          // immediately invoked lambda expression. nice
      )) {}
};

}

#endif // SHOVOLOGGER_BADFILEEXTENSION_H
