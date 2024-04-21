#include "Logger.h"

#include <filesystem>

std::map<real::LogLevel, std::string> real::Logger::m_LevelToStr = {
		{LogLevel::debug, "DEBUG"},
		{LogLevel::info, "INFO"},
		{LogLevel::warning, "WARNING"},
		{LogLevel::error, "ERROR"} };

std::map<real::LogLevel, std::string> real::Logger::m_LevelToConsoleStyle = {
		{LogLevel::debug, "\033[32m"},
		{LogLevel::info, "\033[36m"},
		{LogLevel::warning, "\033[33m"},
		{LogLevel::error, "\033[31m"} };

void real::Logger::Initialize()
{
#ifdef _DEBUG
	m_pConsoleLogger = std::make_unique<ConsoleLogger>();
#endif
}

void real::Logger::Release()
{
	StopFileLogging();

	const auto ptr = m_pConsoleLogger.release();
	delete ptr;
}

void real::Logger::ClearConsole()
{
	std::system("cls");
}

void real::Logger::StartFileLogging(const std::string& filePath)
{
	const auto now = std::chrono::system_clock::now();
	const std::time_t time = std::chrono::system_clock::to_time_t(now);

	char buffer[80];
	std::tm timeinfo;
	localtime_s(&timeinfo, &time);
	std::strftime(buffer, sizeof(buffer), "[%Y-%m-%d][%H-%M-%S]", &timeinfo);

	const auto s = filePath + buffer + ".txt";

	m_pFileLogger.reset(new FileLogger(s));
}

void real::Logger::StopFileLogging()
{
	const auto ptr = m_pFileLogger.release();
	delete ptr;
}

bool real::Logger::ProcessLog(LogLevel level, const LogString& log, std::format_args args)
{
#ifndef _DEBUG
	if (level == LogLevel::debug)
		return false;
#endif

	std::vector<bool> logs;
	if (m_pFileLogger)
		logs.push_back(false); // => no colors needed
	if (m_pConsoleLogger)
		logs.push_back(true); // => colors needed

	if (logs.empty())
		return true;

	const auto logMessage = std::vformat(log.message, args);
	const auto filePath = std::string(log.location.file_name());
	const auto fileName = filePath.substr(filePath.find_last_of('\\') + 1);

	const auto functionDesc = std::string(log.location.function_name());
	const auto functionName = functionDesc.substr(functionDesc.find_last_of(':') + 1);

	for (const auto& l : logs)
	{
		std::string levelStr;
		std::string finalLog;

		if (l)
			levelStr = std::format("{}[{}]\033[0m", m_LevelToConsoleStyle[level], m_LevelToStr[level]);
		else
			levelStr = std::format("[{}]", m_LevelToStr[level]);

		if (level == LogLevel::error || level == LogLevel::warning)
		{
			finalLog = std::format("[{}] {} @ {} - {} (line {})\n => {}\n\n",
				GetTimeAndDate(),
				levelStr,
				fileName,
				functionName,
				log.location.line(),
				logMessage);
		}
		else
		{
			finalLog = std::format("[{}] {} {}\n\n",
				GetTimeAndDate(),
				levelStr,
				logMessage);
		}

		if (l)
			m_pConsoleLogger->Log(finalLog);
		else
			m_pFileLogger->Log(finalLog);

	}

	return true;
}

std::string real::Logger::GetTimeAndDate()
{
	const auto now = std::chrono::system_clock::now();
	const std::time_t time = std::chrono::system_clock::to_time_t(now);

	char timeStr[26];
	ctime_s(timeStr, sizeof timeStr, &time);
	timeStr[strlen(timeStr) - 1] = '\0';

	if (timeStr[8] == ' ')
		timeStr[8] = '0';

	return timeStr;
}