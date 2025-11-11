/*
 * @copyright Minimal Dependency Injection Framework for C++
 *            © 2025 by Ángel Fernández Pineda. Madrid. Spain. 2025.
 *            is licensed under Creative Commons Attribution 4.0 International
 *
 */

// Utilities
#include <iostream>
#include <string>

// Import the framework
#include "../dip.hpp"

// Declare a service
class MyService1
{
public:
    virtual void foo() = 0;
    virtual ~MyService1() {};
};

// Declare another service

class MyService2
{
public:
    virtual void bar() = 0;
    virtual ~MyService2() {};
};

// Declare a service provider for MyService1
// This class consumes MyService2
// Let's pretend this code is not aware of MyServiceProvider2
class MyServiceProvider1 : public MyService1
{
public:
    virtual void foo() override
    {
        std::cout << this << ".MyServiceProvider1::foo()" << std::endl;
        dip::instance<MyService2> provider2;
        provider2->bar();
    };
};

// Declare a service provider for MyService2
// This class consumes MyService1
// Let's pretend this code is not aware of MyServiceProvider1
class MyServiceProvider2 : public MyService2
{
public:
    virtual void bar() override
    {
        std::cout << this << ".MyServiceProvider2::bar()" << std::endl;
        dip::instance<MyService1> provider1;
        provider1->foo();
    };
};

int main()
{
    // This program will enter an infinite loop since
    // the providers for each service are calling each other
    // known being aware of that.
    std::cout << "-- This program will enter an infinite loop, ending in a stack overflow" << std::endl;

    dip::inject_singleton<MyService1, MyServiceProvider1>();
    dip::inject_singleton<MyService2, MyServiceProvider2>();

    // Consume first service
    dip::instance<MyService1> provider1;
    provider1->foo();

    std::cout << "-- main end" << std::endl;
}