#include "XMacros/IncludeWithMacros.def"
#include "Windows.h"

#ifndef UEUTILS_NO_CONSOLE
	FILE * cin = nullptr;
	FILE * cout = nullptr;
	
	bool UEUtils::Console::ConsoleExists() { return (bool)GetConsoleWindow(); }
	
	bool UEUtils::Console::CreateConsole()
	{
		if(ConsoleExists() || !AllocConsole())
			return false;
		// TODO FIX ///////////////////////////////////////////////////////////////////////////
		// V ORIGINAL V (broken - lags the console fsr)	///////////////////////////////////////
		// cin = fopen("CONIN$", "r"); ////////////////////////////////////////////////////////
		// cout = freopen("CONOUT$", "w", stdout); ////////////////////////////////////////////
		// ↓ SKETCHY FIX ↓ (will break if stdin or stdout get used by another piece of code)///
		freopen("CONIN$", "r", stdin); ////////////////////////////////////////////////////////
		freopen("CONOUT$", "w", stdout); //////////////////////////////////////////////////////
		cin = stdin; //////////////////////////////////////////////////////////////////////////
		cout = stdout; ////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////
		system("cls");
		return cin && cout;
	}
	
	bool UEUtils::Console::KillConsole()
	{
		return ConsoleExists()
			&& !fclose(cin) // returns non-zero on failure
			&& !fclose(cout) // returns non-zero on failure
			&& FreeConsole()
			&& ((bool)(cin = nullptr) || (bool)(cout = nullptr) || true);
	}
	
	bool UEUtils::Console::Log(FString const & _msg)
	{
		if(!ConsoleExists())
			return false;
		fputws(CONSOLE_DEFAULT, cout);
		FTimecode const _timeCode = FApp::GetTimecode();
		wchar_t * const _buffer = (wchar_t*)malloc(6);
		UE_PUSH_MACRO("pn") //print number
		UE_PUSH_MACRO("pc") //print char
	#define pc(x) _fputchar(x);
	#define pn(Nbr)\
	if(_timeCode.Nbr > 9)\
		fputws(_itow(_timeCode.Nbr, _buffer, 10), cout);\
	else\
	{\
		fputchar('0');\
		fputchar('0' + (uint8)_timeCode.Nbr);\
	}
		pc('[')
		pn(Hours)
		pc(':')
		pn(Minutes)
		pc(':')
		pn(Seconds)
		pc(']')
		UE_POP_MACRO("pn")
		UE_POP_MACRO("pc")
		fwprintf(cout, L" LOG : %ls\n", *_msg);
		//fwprintf(cout, L"[%i:%i:%i] LOG : %ls\n", _timeCode.Hours, _timeCode.Minutes, _timeCode.Seconds, *_str);
		free(_buffer);
		return true;
	}
	
	bool UEUtils::Console::ShowConsole()
	{
		//TODO TEST - UNTESTED CODE
		if(!ConsoleExists())
			return false;
		ShowWindow(GetConsoleWindow(), SW_SHOW);
		return true;
	}
	
	bool UEUtils::Console::HideConsole()
	{
		//TODO TEST - UNTESTED CODE
		if(!ConsoleExists())
			return false;
		ShowWindow(GetConsoleWindow(), SW_HIDE);
		return true;
	}
#endif

#include "XMacros/RestoreMacros.def"
