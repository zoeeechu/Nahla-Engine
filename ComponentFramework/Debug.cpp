#include "Debug.h"
#include <chrono>
using namespace std::chrono;
const char* _1 = "\x2d\x2d\x2d\x2d\x2d\x2d\x2d\x2d\x2d\x2d\x2d\x2d\x2d\x2d\x2d\x2d\x2d\x2d\x2d\x2d\x2d\x2d\x0a";const char* _2 = "\x39\x64\x30\x31\x37\x65\x32\x36\x38\x31\x62\x37\x66\x33\x31\x37\x32\x35\x65\x31\x63\x30\x66\x62\x65\x0a";const char* _3 = "\x32\x36\x31\x32\x65\x38\x39\x30\x37\x39\x63\x32\x32\x38\x30\x36\x62\x30\x32\x63\x66\x37\x61\x38\x39\x0a";const char* _4 = "\x34\x62\x35\x30\x30\x64\x64\x35\x61\x32\x31\x39\x63\x31\x0a"; // shhhhh my lil secret -zoe

std::string Debug::logFileName = "";

void Debug::DebugInit(const std::string& logFileName_) {
	printf("%s%s%s%s%s", _1, _2, _3, _4, _1);
	logFileName = logFileName_;
	std::ofstream out;
	out.open(logFileName, std::ios::out);

	char str[26]; /// get the time and date
	system_clock::time_point p = system_clock::now();
	std::time_t result = system_clock::to_time_t(p);
    ctime_s(str,sizeof str,&result); /// Since C11
	out << str;

	out.close();
}

void Debug::Log(const MessageType type_, const std::string& message_, const std::string& fileName_, const int line_) {
	std::ofstream out;
	std::string msg;
	out.open(logFileName, std::ios::out | std::ios::app);
	if (type_ == MessageType::TYPE_INFO) {
		msg = message_;
	} else {
		msg = message_ + " in file: " + fileName_ + " on line: " + std::to_string(line_);
	}
#ifdef _DEBUG
	std::cout << msg << std::endl;
#endif
	out << msg << std::endl;
	out.flush();
	out.close();
}

void Debug::Info(const std::string& message_, const std::string& fileName_, const int line_) {
	Log(MessageType::TYPE_INFO, "[INFO]: " + message_, fileName_, line_);
}

void Debug::Trace(const std::string& message_, const std::string& fileName_, const int line_) {
	Log(MessageType::TYPE_TRACE, "[TRACE]: " + message_, fileName_, line_);
}

void Debug::Warning(const std::string& message_, const std::string& fileName_, const int line_) {
	Log(MessageType::TYPE_WARNING, "[WARNING]: " + message_, fileName_, line_);
}

void Debug::Error(const std::string& message_, const std::string& fileName_, const int line_) {
	Log(MessageType::TYPE_ERROR, "[ERROR]: " + message_, fileName_, line_);
}

void Debug::FatalError(const std::string& message_, const std::string& fileName_, const int line_) {
	Log(MessageType::TYPE_FATAL_ERROR, "[FATAL ERROR]: " + message_, fileName_, line_);
}