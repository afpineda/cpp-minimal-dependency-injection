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

// Declare an abstract class as a service.
class MyService1
{
public:
    virtual void foo() = 0;
    virtual ~MyService1() {};
};

// Declare another abstract class as another service.
class MyService2
{
public:
    virtual void bar() = 0;
    virtual ~MyService2() {};
};

// Declare a single service provider for both services
// thanks to multiple inheritance
//
class MyServiceProvider : public MyService1,
                          public MyService2
{
public:
    virtual void foo() override
    {
        std::cout << this << ".foo()" << std::endl;
    }

    virtual void bar() override
    {
        std::cout << this << ".bar()" << std::endl;
    }
};

// Consume both services
void test()
{
    dip::instance<MyService1> i1;
    dip::instance<MyService2> i2;
    i1->foo();
    i2->bar();
}

// Custom injector class for both services
// using a singleton life cycle.
struct CustomInjector
{
    inline static MyServiceProvider singleton;
    inline static dip::Injector<MyService1> service1{
        .retrieve = []() -> MyService1 *
        {
            return &singleton;
        }};
    inline static dip::Injector<MyService2> service2{
        .retrieve = []() -> MyService2 *
        {
            return &singleton;
        }};
};

int main()
{
    // Inject
    CustomInjector injector;
    dip::inject<MyService1>(injector.service1);
    dip::inject<MyService2>(injector.service2);

    // Consume both services
    test();
}