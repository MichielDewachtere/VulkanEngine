#ifndef LOGGER_H
#define LOGGER_H

#include <format>
#include <source_location>
#include <string>
#include <iostream>
#include <map>
#include <fstream>

namespace real
{
	enum class LogLevel
	{
		debug = 0,
		info = 1,
		warning = 2,
		error = 3
	};

	struct LogString
	{
		LogString(std::string message, const std::source_location& location = std::source_location::current())
			: message(std::move(message)), location(location) {}


		std::string message;
		std::source_location location;
	};

	class Logger
	{
#pragma region LogTypes
		class BaseLogger
		{
		public:
			BaseLogger() = default;
			virtual ~BaseLogger() = default;

			virtual void Log(const std::string& message)
			{
				(*m_pOs) << message;
				m_pOs->flush();
			}

		protected:
			std::ostream* m_pOs = nullptr;
		};

		class FileLogger final : public BaseLogger
		{
		public:
			FileLogger(const FileLogger& other) = delete;
			FileLogger(FileLogger&& other) noexcept = delete;
			FileLogger& operator=(const FileLogger& other) = delete;
			FileLogger& operator=(FileLogger&& other) noexcept = delete;

			explicit FileLogger(std::string fileName)
				: m_Filename(std::move(fileName))
			{
				m_pOs = new std::ofstream(m_Filename.c_str(), std::ios_base::app); // Allocate a new std::ofstream
			}

			~FileLogger() override
			{
				if (m_pOs)
				{
					auto* of = dynamic_cast<std::ofstream*>(m_pOs);
					of->close();
					delete m_pOs;
				}
			}
		private:
			std::string m_Filename;
		};

		class ConsoleLogger final : public BaseLogger
		{
		public:
			ConsoleLogger()
			{
				m_pOs = &std::cout;
			}
			~ConsoleLogger() override = default;

			ConsoleLogger(const ConsoleLogger& other) = delete;
			ConsoleLogger(ConsoleLogger&& other) noexcept = delete;
			ConsoleLogger& operator=(const ConsoleLogger& other) = delete;
			ConsoleLogger& operator=(ConsoleLogger&& other) noexcept = delete;
		};
#pragma endregion

	public:
		~Logger() = delete;
		Logger(const Logger& other) = delete;
		Logger(Logger&& other) noexcept = delete;
		Logger& operator=(const Logger& other) = delete;
		Logger& operator=(Logger&& other) noexcept = delete;

		static void Initialize();
		static void Release();

		template <typename ... Args>
		static void LogDebug(const LogString& log, Args&&... args);

		template <typename ... Args>
		static void LogWarning(const LogString& log, Args&&... args);

		template <typename ... Args>
		static void LogInfo(const LogString& log, Args&&... args);

		template <typename ... Args>
		static void LogError(const LogString& log, Args&&... args);

		static void ClearConsole();
		static void StartFileLogging(const std::string& filePath);
		static void StopFileLogging();

	private:
		Logger() = default;

		inline static std::unique_ptr<ConsoleLogger> m_pConsoleLogger{ nullptr };
		inline static std::unique_ptr<FileLogger> m_pFileLogger{ nullptr };

		static bool ProcessLog(LogLevel level, const LogString& log, std::format_args args);
		static std::string GetTimeAndDate();

		static std::map<LogLevel, std::string> m_LevelToStr;
		static std::map<LogLevel, std::string> m_LevelToConsoleStyle;
	};

	template <typename ... Args>
	void Logger::LogDebug(const LogString& log, Args&&... args)
	{
		ProcessLog(LogLevel::debug, log, std::make_format_args(args...));
	}

	template <typename ... Args>
	void Logger::LogWarning(const LogString& log, Args&&... args)
	{
		ProcessLog(LogLevel::warning, log, std::make_format_args(args...));
	}

	template <typename ... Args>
	void Logger::LogInfo(const LogString& log, Args&&... args)
	{
		ProcessLog(LogLevel::info, log, std::make_format_args(args...));
	}

	template <typename ... Args>
	void Logger::LogError(const LogString& log, Args&&... args)
	{
		ProcessLog(LogLevel::error, log, std::make_format_args(args...));
	}
}

#endif // LOGGER_H