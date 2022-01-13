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


int main(int argc, char** argv)
{
	__try
	{
		test1();
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		auto code = GetExceptionCode();
		std::cout << std::hex <<  "SEH: 0x" << code << std::endl;		
	}
}