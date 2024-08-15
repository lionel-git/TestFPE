#include <iostream>
#include <float.h>
#include <Windows.h>
#include <exception>
#include <cfenv>
#include <format>

void test1()
{
	try
	{
		std::cout << "Hello world" << std::endl;

        throw std::runtime_error("Test exception");

		

		int i = 1;
		int j = 0;
		//auto v = i / j;

        std::cout << "Generate divide by zero..." << std::endl;
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

int submain()
{
    try
    {
        double a= 1.0;
        double b= 0.0;
        auto c = a / b;

        char* p = 0;
        *p = 123;
        test1();
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    return 1;
}

void show(unsigned long code)
{
        std::cout << std::hex << "SEH: 0x" << code << std::endl;
        std::cout << std::dec << (int)code << std::endl;

    if (code == EXCEPTION_FLT_DIVIDE_BY_ZERO)
    {
        std::cout << "Divide by zero" << std::endl;
    }
}

class MyStruct
{
public:
    MyStruct()
    {
        std::cout << "MyStruct()" << std::endl;
    }
    ~MyStruct()
    {
        std::cout << "~MyStruct()" << std::endl;
    }
};

MyStruct G_struct;

errno_t my_controlfp_s(unsigned int* cw, unsigned int  value, unsigned int  mask)
{
    auto ret = _controlfp_s(cw, value, mask);
    if (ret != 0)
    {
        std::cout << "Error: " << ret << std::endl;
        exit(-1);
    }
    return ret;
}



int filter(unsigned int code, struct _EXCEPTION_POINTERS* ep)
{
    std::cout << "in filer" << std::endl;
    if (code== EXCEPTION_FLT_DIVIDE_BY_ZERO)
    {
        puts("Divide by zero, continue");
        unsigned int cw0;
        my_controlfp_s(&cw0, 0, 0);
        std::cout << "S1: " << std::hex << _clearfp() << std::endl;
        std::cout << "S2: " << std::hex << _clearfp() << std::endl;
        unsigned int cw;
        my_controlfp_s(&cw, 0, 0);

        unsigned int new_value = cw | _EM_ZERODIVIDE | _EM_INVALID;
        my_controlfp_s(&cw, new_value, _MCW_EM);

        unsigned int cw2;
        my_controlfp_s(&cw2, 0, 0);

        return EXCEPTION_CONTINUE_EXECUTION;
    }
    return EXCEPTION_EXECUTE_HANDLER;
   /* 
    if (code == EXCEPTION_ACCESS_VIOLATION)
    {
        puts("caught AV as expected.");
        return EXCEPTION_EXECUTE_HANDLER;
    }
    else
    {
        puts("didn't catch AV, unexpected.");
        return EXCEPTION_CONTINUE_SEARCH;
    };*/
}

double test_different_calculation()
{
    double v = 10.3;
    for (size_t i = 0; i < 10; ++i)
    {
        for (size_t j = 0; j < 100000; ++j)
        {
            v += std::sin(i+j) + std::cos(i+j) + 1.0 / (1.0 + j * j) + std::sqrt(j * j + 1.0);
        }
    }
    std::cout << std::format("v: {}", v) << std::endl;
    return v;
}

void parseOptions(int argc, char** argv, bool& throwfpe)
{
    throwfpe = false;
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-throwfpe") == 0)
        {
            throwfpe = true;
        }
    }
}

int main(int argc, char** argv)
{
    //std::cout << sizeof(unsigned long) << std::endl;
    //std::cout << sizeof(unsigned int) << std::endl;

    bool throwfpe = false;
    parseOptions(argc, argv, throwfpe);
    if (throwfpe)
    {
        std::cout << "Throw FPE" << std::endl;
        unsigned int cw;
        my_controlfp_s(&cw, 0, 0);
        unsigned int new_value = cw | _EM_ZERODIVIDE | _EM_INVALID | _EM_OVERFLOW;
        my_controlfp_s(&cw, new_value, _MCW_EM);
    }
    test_different_calculation(); return 0;



    //submain();

	__try
	{
        submain();
	}
	__except (filter(GetExceptionCode(), GetExceptionInformation()))
	{
        auto code = GetExceptionCode();       
        show(code);
        _exit(code);
	}
}