#include <iostream>
#include <string>
#include "../InternalServices.hpp"

using namespace InternalServices;

class SERVICE(MyServiceInterface1)
{
public:
    virtual void doSomething() = 0;
};

class SERVICE(MyServiceInterface2)
{
public:
    virtual void doSomethingElse() = 0;
};

class MyServiceProvider1 : public MyServiceInterface1
{
public:
    virtual void doSomething() override
    {
        std::cout << this << ".MyServiceProvider1::doSomething()" << std::endl;
        MyServiceInterface2::getInstance()->doSomethingElse();
    };
};

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
    std::cout << "-- This program will enter an infinite loop, ending in a stack overflow" << std::endl;

    MyServiceInterface1::inject<MyServiceProvider1, Lifetime::Singleton>();
    MyServiceInterface2::inject<MyServiceProvider2, Lifetime::Singleton>();

    MyServiceInterface1::Provider service1 = MyServiceInterface1::getInstance();
    service1->doSomething();

    std::cout << "-- main end" << std::endl;
}