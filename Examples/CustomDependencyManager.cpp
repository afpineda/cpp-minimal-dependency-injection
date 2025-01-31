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

// Declare an abstract class as a service.
// This is not self-managed since we are
// declaring a single custom dependency manager
// for two services.
class MyServiceInterface1
{
public:
    virtual void doSomething() = 0;
};

// Declare another abstract class as another service.
class MyServiceInterface2
{
public:
    virtual void doSomethingElse() = 0;
};

// Declare a custom dependency manager for
// both services.
class CustomDependencyManager : public DependencyManager<MyServiceInterface1>,
                                public DependencyManager<MyServiceInterface2>
{
public:
    using Service1 = DependencyManager<MyServiceInterface1>;
    using Service2 = DependencyManager<MyServiceInterface2>;
};

// Declare a service provider for MyServiceInterface1
class MyServiceProvider1 : public MyServiceInterface1
{
public:
    virtual void doSomething() override
    {
        std::cout << this << ".doSomething()" << std::endl;
    };
};

// Declare a service provider for MyServiceInterface2
class MyServiceProvider2 : public MyServiceInterface2
{
public:
    virtual void doSomethingElse() override
    {
        std::cout << this << ".doSomethingElse()" << std::endl;
    };
};

// A consumer of both services for demonstration purposes.
// A consumer class is not mandatory.
class MyConsumer
{
public:
    CustomDependencyManager::Service1::Provider service1;
    CustomDependencyManager::Service2::Provider service2;
    MyConsumer()
    {
        service1 = CustomDependencyManager::Service1::getInstance();
        service2 = CustomDependencyManager::Service2::getInstance();
    }

    void runService()
    {
        service1->doSomething();
        service2->doSomethingElse();
    }
};

int main()
{
    std::cout << "-- main begin" << std::endl;

    // Inject a dependency for each service
    CustomDependencyManager::Service1::inject<MyServiceProvider1>();
    CustomDependencyManager::Service2::inject<MyServiceProvider2>();

    // create a consumer that runs both services
    MyConsumer consumer;
    consumer.runService();

    std::cout << "-- main end" << std::endl;
}