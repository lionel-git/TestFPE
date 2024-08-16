#include <iostream>
#include <float.h>
#include <Windows.h>
#include <exception>
#include <random>
#include <cassert>
#include <vector>
#include <algorithm>
#include <format>

#include "sha256.h"

#pragma warning(disable: 4189)

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

void setThrowFPE()
{
    unsigned int cw;
    _controlfp_s(&cw, 0, 0);
    unsigned int new_value = cw & ~(_EM_ZERODIVIDE | _EM_INVALID | _EM_OVERFLOW);
    _controlfp_s(&cw, new_value, _MCW_EM);
    std::cout << "Thow FPE activated" << std::endl;
}

void testFPE()
{
	try
	{
		std::cout << "Hello world" << std::endl;

        setThrowFPE();

      /*  int* P = 0;
        *P = 0;*/

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
    std::cout << "Exception code: 0x" << std::hex << code << std::endl;
    if (ep != nullptr && ep->ExceptionRecord != nullptr)
    {
        std::cout << "Address: 0x" << (void*)ep->ExceptionRecord->ExceptionAddress << std::endl;
    }
   // a = EXCEPTION_NONCONTINUABLE;

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

void parseOption(int argc, char** argv, bool& throwFPE, size_t& N)
{
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-throwfpe") == 0)
        {
            throwFPE = true;
        }
        if (strcmp(argv[i], "-N") == 0)
        {
            N = atoll(argv[i+1]);
        }
    }
}

void hash_result(const char *data, size_t size)
{
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, (const BYTE*)data, size);
    unsigned char out[32];
    sha256_final(&ctx, out);

    for (int i = 0; i < 32; i++)
    {
        printf("%02x", (int)out[i]);
    }
    printf("\n");
}


void test_calculations(size_t N)
{
    std::cout << "N = " << N << std::endl;
    std::mt19937_64 gen64;
    gen64.discard(10000 - 1);
    assert(gen64() == 9981545732273789042ull);

    std::vector<double> v(N);
    unsigned long long* p = (unsigned long long*)v.data();
    for (size_t i = 0; i < v.size(); ++i)
    {
        p[i] = gen64();
    }
    std::cout << "OK" << std::endl;
    // v[5] = 0.0;

    for (size_t i = 0; i < v.size(); ++i)
    {
        if (!std::isfinite(v[i]))
            v[i] = 0.3;
        double a = MIN(v[i], 1e+100);
        a = MAX(a, -1e+100);
        if (a > 0)
            a = MAX(a, 1e-100);
        if (a < 0)
            a = MIN(a, -1e-100);
        v[i] = 1 / a + 1 / std::sqrt((a * a + 1.235));
    }
    std::cout << "Start hashing" << std::endl;
    hash_result((const char*)v.data(), v.size() * sizeof(double));
}

int main(int argc, char** argv)
{
	__try
	{
        bool throwFPE = false;
        size_t N = 1000;
        parseOption(argc, argv, throwFPE, N);
        if (throwFPE)
            setThrowFPE();
        test_calculations(N);
	}
	__except (filter_exception(GetExceptionCode(), GetExceptionInformation()))
	{
        exit(GetExceptionCode());
	}
}