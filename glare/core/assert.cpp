#include "glare/core/assert.h"
#include "glare/core/string_utilities.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace glare
{

static const char* __get_filename_from_path(const char* file_path)
{
	if (file_path == nullptr)
		return nullptr;

	const size_t path_len = strlen(file_path);
	const char* current_char = file_path + path_len;
	while (current_char > file_path)
	{
		--current_char;

		if (*current_char == '/' || *current_char == '\\')
		{
			++current_char;
			break;
		}
	}
	return current_char;
}

static bool __msgbox_yes_no(const std::string& title, const std::string& text)
{
	bool answer_yes = false;
#if defined(_WIN32)
	::ShowCursor(TRUE);
	const int button_clicked =
		::MessageBoxA(nullptr, text.c_str(), title.c_str(), MB_ICONHAND | MB_YESNO | MB_TOPMOST);
	answer_yes = (button_clicked == IDYES);
	::ShowCursor(FALSE);
#endif
	return answer_yes;
}

static void __msgbox_ok(const std::string& title, const std::string& text)
{
#if defined(_WIN32)
	::ShowCursor(TRUE);
	::MessageBoxA(nullptr, text.c_str(), title.c_str(), MB_ICONHAND | MB_OK | MB_TOPMOST);
	::ShowCursor(FALSE);
#endif
}


/////////////////////////////////
bool is_debugger_available()
{
#if defined(_WIN32)
	//typedef BOOL (CALLBACK IsDebuggerPresentFunc)();
	using is_debugger_present_func_t = BOOL CALLBACK();
	
	// Get kernal32.dll handle
	static HINSTANCE h_instance_kernel = GetModuleHandle(TEXT("KERNEL32"));
	if (!h_instance_kernel) {
		return false;
	}
	// Get IsDebuggerPresent function address
	static auto is_debugger_present_func = 
		reinterpret_cast<is_debugger_present_func_t*>(GetProcAddress(h_instance_kernel, "IsDebuggerPresent"));
	if (!is_debugger_present_func) {
		return false;
	}
	
	// Invoke it
	static BOOL isDebuggerAvailable = is_debugger_present_func();
	return (isDebuggerAvailable == TRUE);
#else
	return false;
#endif
}

////////////////////////////////
void error_and_die(const char* file_path, const char* function_full_name, int line_number, const std::string& error_message, const char* condition_text /*= nullptr*/)
{
	std::string error_text = error_message;
	if (error_text.empty()) {
		if (condition_text) {
			error_text = format("[error] (%s) == false", condition_text);
		} else {
			error_text = "Fatal error";
		}
	}
	error_text += "\n\nThe application will now close.";

	const char* file_name = file_path ? __get_filename_from_path(file_path) : "Unknown File";
	
	const std::string msgbox_title = "Glare::Error";
	const bool has_debugger = is_debugger_available();
	if (has_debugger) {
		error_text += "\n*******Debugger Detected*******\n";
		error_text += format("%s at line %d (%s)\n", (function_full_name ? function_full_name : "Unknown Function"), line_number, file_name);
		error_text += "(YES = Debug | No = Quit)";
	}

	// #Todo: add visual studio output here

	if (has_debugger) {
		const bool yes = __msgbox_yes_no(msgbox_title, error_text);
		if (yes) {
			DEBUG_BREAK;
		}
	} else {
		__msgbox_ok(msgbox_title, error_text);
	}
	exit(-1);
}

void alert_and_continue(const char* file_path, const char* function_full_name, int line_number, const std::string& error_message, const char* condition_text)
{
	std::string error_text = error_message;
	if (error_text.empty()) {
		if (condition_text) {
			error_text = format("[alert] (%s) == false", condition_text);
		} else {
			error_text = "Fatal error";
		}
	}
	error_text += "\n\nThe application will now close.";

	const char* file_name = file_path ? __get_filename_from_path(file_path) : "Unknown File";

	const std::string msgbox_title = "Glare::Alert";
	const bool has_debugger = is_debugger_available();
	if (has_debugger) {
		error_text += "\n*******Debugger Detected*******\n";
		error_text += format("%s at line %d (%s)\n", (function_full_name ? function_full_name : "Unknown Function"), line_number, file_name);
		error_text += "(YES = Debug | No = Continue)";
	}

	// #Todo: add visual studio output here

	if (has_debugger) {
		const bool yes = __msgbox_yes_no(msgbox_title, error_text);
		if (yes) {
			DEBUG_BREAK;
		}
	} else {
		__msgbox_ok(msgbox_title, error_text);
	}
}

};