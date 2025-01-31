#include <iostream>
#include "../InternalServices.hpp"

using namespace InternalServices;

class SERVICE(MyServiceInterface)
{
public:
    virtual void doSomething() = 0;
};

class MyServiceProvider1 : public MyServiceInterface
{
public:
    virtual void doSomething() override
    {
        std::cout << this << ".MyServiceProvider1::doSomething()" << std::endl;
    };
};

class MyServiceProvider2 : public MyServiceInterface
{
public:
    virtual void doSomething() override
    {
        std::cout << this << ".MyServiceProvider2::doSomething()" << std::endl;
    };
};

class MyConsumer
{
public:
    MyServiceInterface::ProviderSet services;
    MyConsumer()
    {
        services = MyServiceInterface::getAllInstances();
    }

    void runService()
    {
        for (auto service: services)
            service->doSomething();
    }
};

int main()
{
    std::cout << "-- main begin" << std::endl;

    MyServiceInterface::inject<
        MyServiceProvider1,
        Lifetime::Singleton,
        ServiceConsumerMode::getAllInstances>();
    MyServiceInterface::inject<
        MyServiceProvider2,
        Lifetime::Transient,
        ServiceConsumerMode::getAllInstances>();

    MyConsumer consumer1, consumer2;
    consumer1.runService();
    consumer2.runService();

    std::cout << "-- main end" << std::endl;
}