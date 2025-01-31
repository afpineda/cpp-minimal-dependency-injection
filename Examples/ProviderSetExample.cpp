/*
 * @copyright Minimal Dependency Injection Framework for C++
 *            © 2025 by Ángel Fernández Pineda. Madrid. Spain. 2025.
 *            is licensed under Creative Commons Attribution 4.0 International
 *
 */

// Utilities
#include <iostream>

// Import the framework
#include "../InternalServices.hpp"
using namespace InternalServices;

// Declare an abstract class as a self-managed service.
class SERVICE(MyServiceInterface)
{
public:
    virtual void doSomething() = 0;
};

// A provider for MyServiceInterface.
// Instance addresses are logged to evidence
// the lifetime of service provider instances.
class MyServiceProvider1 : public MyServiceInterface
{
public:
    virtual void doSomething() override
    {
        std::cout << this << ".MyServiceProvider1::doSomething()" << std::endl;
    };
};

// Another provider for MyServiceInterface.
// Instance addresses are logged to evidence
// the lifetime of service provider instances.
class MyServiceProvider2 : public MyServiceInterface
{
public:
    virtual void doSomething() override
    {
        std::cout << this << ".MyServiceProvider2::doSomething()" << std::endl;
    };
};

// A consumer of the MyServiceInterface service
// for demonstration purposes.
// A consumer class is not mandatory.
//
// This class will invoke all available service
// providers, which are MyServiceProvider1 and
// MyServiceProvider2 in this example.
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
        for (auto service : services)
            service->doSomething();
    }
};

int main()
{
    std::cout << "-- main begin" << std::endl;

    // We inject both service providers:
    // MyServiceProvider1 first, then MyServiceProvider2.
    // Note that instances from MyServiceProvider1 will
    // have a different lifetime than instances
    // from MyServiceProvider2.
    MyServiceInterface::inject<
        MyServiceProvider1,
        Lifetime::Singleton,
        ServiceConsumerMode::getAllInstances>();
    MyServiceInterface::inject<
        MyServiceProvider2,
        Lifetime::Transient,
        ServiceConsumerMode::getAllInstances>();

    // There are two consumers running the service
    // from all service providers.
    MyConsumer consumer1, consumer2;
    std::cout << ".. consumer1 .." << std::endl;
    consumer1.runService();
    std::cout << ".. consumer2 .." << std::endl;
    consumer2.runService();

    std::cout << "-- main end" << std::endl;
}