#pragma once
#include <string>
//////////////////////////////////////////////////////////////////////////

namespace glare
{
bool is_debugger_available();

void error_and_die(const char* file_path, const char* function_full_name, int line_number
                   , const std::string& error_message, const char* condition_text = nullptr);
};


#define ASSERT(condition, message)						\
{														\
	if (!(condition)) {									\
		const char* condition_text = #condition;			\
		error_and_die(__FILE__, __FUNCTION__, __LINE__, message, condition_text); \
	} \
}

#define FATAL(message)\
{\
	error_and_dir(__FILE__, __FUNCTION__, __LINE__, message, nullptr);\
}

#define DEBUG_BREAK {__debugbreak();}