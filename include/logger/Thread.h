#ifndef SHUVLOG_THREAD_H
#define SHUVLOG_THREAD_H

namespace logger
{

    /**
     * @brief   Thread-local human-readable label for the current thread.
     *
     * Defaults to "Unknown" until set via @code setThreadLabel()@endcode.
     * Each thread receives its own independent instance of this variable
     * (@code thread_local@endcode).
     */
    inline thread_local auto threadLabel = "Unknown";

    /**
     * @brief   Sets the label for the current thread.
     *
     * @param   name    The thread's name
     */
    inline void setThreadLabel(const char* name) { threadLabel = name; }

    /**
     * @return  The current thread's label
     */
    inline const char* getThreadLabel() { return threadLabel; }

    /**
     * @brief   Formats a thread ID
     *
     * @param   id ID of the thread
     * @return  A prettier thread ID, in a hexadecimal form
     */
    inline std::string getPrettyThreadId(std::thread::id id)
    {
        std::ostringstream output;
        output << id;
        return output.str();
    }

}

#endif //SHUVLOG_THREAD_H
