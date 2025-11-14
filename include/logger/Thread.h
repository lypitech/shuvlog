#ifndef SHOVOLOGGER_THREAD_H
#define SHOVOLOGGER_THREAD_H

namespace logger
{

    inline thread_local auto threadLabel = "Unknown";

    inline void setThreadLabel(const char* name) { threadLabel = name; }
    inline const char* getThreadLabel() { return threadLabel; }

}

#endif //SHOVOLOGGER_THREAD_H
