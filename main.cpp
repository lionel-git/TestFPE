#define _CRT_SECURE_NO_WARNINGS 1

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

bool G_throwFPE = false;

void setThrowFPE()
{
    unsigned int cw;
    _controlfp_s(&cw, 0, 0);
    unsigned int new_value = cw & ~(_EM_ZERODIVIDE | _EM_INVALID | _EM_OVERFLOW);
    _controlfp_s(&cw, new_value, _MCW_EM);
    std::cout << "Thow FPE activated" << std::endl;
    G_throwFPE = true;
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
    std::cout << std::format("Exception code: {:x} ", code);
    if (ep != nullptr && ep->ExceptionRecord != nullptr)
        std::cout << std::format(", Address: 0x{} : ", ep->ExceptionRecord->ExceptionAddress);
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

void test_gen()
{
    std::mt19937_64 gen64;
    for (int i = 0; i < 10; i++)
    {
        std::cout << i << " " << gen64() << std::endl;
    }
}

void save_result(const char* data, size_t size)
{
    std::string fileName = "result.bin";
    if (G_throwFPE)
        fileName = "result_fpe.bin";
    FILE* f = fopen(fileName.c_str(), "wb");
    if (f == nullptr)
    {
        std::cout << "Error open file" << std::endl;
        return;
    }
    fwrite(data, 1, size, f);
    fclose(f);
    std::cout << "Result saved" << std::endl;
}

size_t G_last_index = 0;

double myfunc(double a)
{
    return  1 / a + 1 / std::sqrt((a * a + 1.235)) + std::log(a * a + 0.12546);
}

void test_calculations(size_t N)
{
    std::cout << "N = " << N << std::endl;
    std::mt19937_64 gen64;

    std::vector<double> v(N);
    unsigned long long* p = (unsigned long long*)v.data();
    std::cout << "Start gen" << std::endl;
    for (size_t i = 0; i < v.size(); ++i)
    {
       // std::cout << i << std::endl;
        do
        {
            p[i] = gen64();
        } 
        while ((p[i] >= 0x7FF0000000000001 && p[i] <= 0x7FF7FFFFFFFFFFFF) || 
               (p[i] >= 0xFFF0000000000001 && p[i] <= 0xFFF7FFFFFFFFFFFF));

        double a = *(double*)&p[i];
        if (std::isnan(a))
        {
            
            p[i] = 0x12345578556;
            double b = *(double*)&p[i];
            //std::cout << i << " Found nan  " << b << std::endl;
        }
   //     std::cout << i << " " << p[i] << std::endl;
    }
    std::cout << "OK" << std::endl;
    // v[5] = 0.0;

    for (size_t i = 0; i < v.size(); ++i)
    {
        G_last_index = i;

        if (i == 6024)
        {
            //std::cout << v[i] << std::endl;
            unsigned long long A = *(unsigned long long*) & v[i];
            std::cout  << A << std::endl;
            std::cout << "Before is finite" << std::endl;
            if (!std::isfinite(v[i]))
            {
                std::cout << "Not finite" << std::endl;
            }
            std::cout << "After is finite" << std::endl;

            if (!std::isfinite(v[i]))
                v[i] = 0.3;
            double a = MIN(v[i], 1e+100);
            a = MAX(a, -1e+100);
            if (a > 0)
                a = MAX(a, 1e-100);
            if (a < 0)
                a = MIN(a, -1e-100);
            double zz = myfunc(a);
            std::cout << "zz = " << std::format("{}", zz) << std::endl;
        }
        //std::cout << i << " " << v[i] << std::endl;
        if (!std::isfinite(v[i]))
            v[i] = 0.3;
        double a = MIN(v[i], 1e+100);
        a = MAX(a, -1e+100);
        if (a > 0)
            a = MAX(a, 1e-100);
        if (a < 0)
            a = MIN(a, -1e-100);
        v[i] = myfunc(a);
        if (i==6024)
        {
            std::cout << "Result: " << std::format("{}", v[i]) << std::endl;
        }
    }
    std::cout << "Start hashing" << std::endl;
    hash_result((const char*)v.data(), v.size() * sizeof(double));
    save_result((const char*)v.data(), v.size() * sizeof(double));
}


void test_case()
{
    unsigned long long A = 9219209713614924562;
    double a = *(double*)&A;
    std::cout << "Value: " <<  a << std::endl;
    std::cout.flush();
    double b = a + 1;
    std::cout << "Value b : " << b << std::endl;
}

void test_case2()
{
    unsigned long long A = 5603783088855566994;
    double a = *(double*)&A;
    std::cout << "Value: " << std::format("{}", a) << std::endl;
    double b = myfunc(a);
    std::cout << "Res: " << std::format("{}", b) << std::endl;
}

void load_file(const char* fileName, std::vector<double>& v)
{
    FILE* f = fopen(fileName, "rb");
    if (f == nullptr)
    {
        std::cout << "Error open file" << std::endl;
        return;
    }
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);
    v.resize(size / sizeof(double));
    fread(v.data(), 1, size, f);
    fclose(f);
    std::cout << "File loaded" << std::endl;
}

void compare_results()
{
    std::vector<double> v1;
    std::vector<double> v2;
    load_file("result.bin", v1);
    load_file("result_fpe.bin", v2);
    if (v1.size() != v2.size())
    {
        std::cout << "Different sizes" << std::endl;
        return;
    }
    std::cout << "size =" << v1.size() << std::endl;
    for (size_t i = 0; i < v1.size(); ++i)
    {
        if (v1[i] != v2[i])
        {
            std::cout << "Different values " << i << " " << std::format("{}", v1[i]) << " " << std::format("{}", v2[i]) << std::endl;
            return;
        }
    }
    std::cout << "Results are equal" << std::endl;
}

int main(int argc, char** argv)
{
  //  test_gen(); return 0;

	__try
	{
        bool throwFPE = false;
        size_t N = 1000;
        parseOption(argc, argv, throwFPE, N);
        if (throwFPE)
            setThrowFPE();

        //test_case2(); return 0;


        test_calculations(N);

        
        compare_results();

	}
	__except (filter_exception(GetExceptionCode(), GetExceptionInformation()))
	{
        std::cout << "last index: " << G_last_index << std::endl;
        exit(GetExceptionCode());
	}
}