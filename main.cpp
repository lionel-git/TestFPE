#include <iostream>
#include <float.h>
#include <Windows.h>
#include <exception>

void test1()
{
	try
	{
		std::cout << "Hello world" << std::endl;

		unsigned int cw;
		_controlfp_s(&cw, 0, 0);
		unsigned int new_value = cw & ~(_EM_ZERODIVIDE | _EM_INVALID);
		_controlfp_s(&cw, new_value, _MCW_EM);

		int i = 1;
		int j = 0;
		//auto v = i / j;

		double a = 1.0;
		double b = 0.0;
		auto c = a / b;
		std::cout << c << std::endl;
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}


int filter_exception(unsigned int code, struct _EXCEPTION_POINTERS* ep)
{
    std::cout << "Filter: 0x" << std::hex << code << std::endl;
    if (code == EXCEPTION_INT_DIVIDE_BY_ZERO)
    {
        std::cout << "Divide by zero" << std::endl;
    }
    else if (code == EXCEPTION_FLT_DIVIDE_BY_ZERO)
    {
        std::cout << "Floating point divide by zero" << std::endl;
    }
    else if (code == EXCEPTION_FLT_INVALID_OPERATION)
    {
        std::cout << "Floating point invalid operation" << std::endl;
    }
    else
    {
        std::cout << "Unknown exception" << std::endl;
    }
    return EXCEPTION_EXECUTE_HANDLER;
}

int main(int argc, char** argv)
{
	__try
	{
		test1();
	}
	__except (filter_exception(GetExceptionCode(), GetExceptionInformation()))
	{
        exit(GetExceptionCode());
	}
}