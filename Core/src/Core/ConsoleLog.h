#pragma once
#include <string>
#include <vector>
#include <mutex>



namespace libCore
{
    enum class LogLevel {
        L_INFO,
        L_WARNING,
        L_ERROR
    };

    struct LogEntry {
        LogLevel level;
        std::string message;
    };

    class ConsoleLog
    {
    public:
        static ConsoleLog& GetInstance() {
            static ConsoleLog instance;
            return instance;
        }

        void AddLog(LogLevel level, const std::string& message) {
            std::lock_guard<std::mutex> lock(mutex_);
            logs_.emplace_back(LogEntry{ level, message });
        }

        void ClearLogs() {
            std::lock_guard<std::mutex> lock(mutex_);
            logs_.clear();
        }

        const std::vector<LogEntry>& GetLogs() const {
            return logs_;
        }

    private:
        ConsoleLog() = default;
        ~ConsoleLog() = default;

        ConsoleLog(const ConsoleLog&) = delete;
        ConsoleLog& operator=(const ConsoleLog&) = delete;

        std::vector<LogEntry> logs_;
        mutable std::mutex mutex_;

    };
}
