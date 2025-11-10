/*
 * @copyright Minimal Dependency Injection Framework for C++
 *            © 2025 by Ángel Fernández Pineda. Madrid. Spain. 2025.
 *            is licensed under Creative Commons Attribution 4.0 International
 *
 */

// Utilities
#include <iostream>

// Import the framework
#include "../dip.hpp"

// Declare a service.
class MyService
{
public:
    virtual void foo() = 0;
    virtual ~MyService() {};
};

// Declare a service provider.
class MyServiceProvider1 : public MyService
{
public:
    virtual void foo() override
    {
        std::cout << this << ".MyServiceProvider1::foo()" << std::endl;
    };
};

// Declare another service provider.
class MyServiceProvider2 : public MyService
{
public:
    virtual void foo() override
    {
        std::cout << this << ".MyServiceProvider2::foo()" << std::endl;
    };
};

// Consume a set of service providers
void test()
{
    dip::instance_set<MyService> instance_set;
    for (auto instance: instance_set)
    {
        instance->foo();
    }
}

int main()
{
    // Inject
    dip::add_singleton<MyService, MyServiceProvider1>();
    dip::add_singleton<MyService, MyServiceProvider2>();

    // Consume
    test();
}