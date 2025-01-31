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
#include "../InternalServices.hpp"
using namespace InternalServices;

// Declare a service
class SERVICE(MyServiceInterface1)
{
public:
    virtual void doSomething() = 0;
};

// Declare another service
class SERVICE(MyServiceInterface2)
{
public:
    virtual void doSomethingElse() = 0;
};

// Declare a service provider for MyServiceInterface1
// This class uses MyServiceInterface2
class MyServiceProvider1 : public MyServiceInterface1
{
public:
    virtual void doSomething() override
    {
        std::cout << this << ".MyServiceProvider1::doSomething()" << std::endl;
        MyServiceInterface2::getInstance()->doSomethingElse();
    };
};

// Declare a service provider for MyServiceInterface2
// This class uses MyServiceInterface1
class MyServiceProvider2 : public MyServiceInterface2
{
public:
    virtual void doSomethingElse() override
    {
        std::cout << this << ".MyServiceProvider2::doSomethingElse()" << std::endl;
        MyServiceInterface1::getInstance()->doSomething();
    };
};

int main()
{
    // This program will enter an infinite loop since
    // the providers for each service are calling each other
    // known being aware of that.
    std::cout << "-- This program will enter an infinite loop, ending in a stack overflow" << std::endl;

    MyServiceInterface1::inject<MyServiceProvider1, Lifetime::Singleton>();
    MyServiceInterface2::inject<MyServiceProvider2, Lifetime::Singleton>();

    MyServiceInterface1::Provider service1 = MyServiceInterface1::getInstance();
    service1->doSomething();

    std::cout << "-- main end" << std::endl;
}