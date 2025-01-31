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

// Declare an abstract class as a self-managed service.
// MyServiceInterface works as a dependency manager for itself.
class SERVICE(MyServiceInterface)
{
public:
    virtual void doSomething() = 0;
};

// Declare a provider for the MyServiceInterface service
// A service provider implements all abstract methods
// from the service interface.
//
// For demonstration purposes, this provider
// has constructor parameters, but a good practice
// is to declare parameterless constructors.
//
// Instance creation/destruction is logged to evidence
// lifetimes.
class MyServiceProvider : public MyServiceInterface
{
public:
    virtual void doSomething() override
    {
        std::cout << this << ".doSomething(" << data << ")" << std::endl;
    };

    MyServiceProvider(std::string param)
    {
        data = param;
        std::cout << "MyServiceProvider::MyServiceProvider(" << data << ") -> " << this << std::endl;
    };

    ~MyServiceProvider()
    {
        std::cout << this << ".~MyServiceProvider()" << std::endl;
    };

private:
    std::string data;
};

// A consumer of the MyServiceInterface service
// for demonstration purposes.
// A consumer class is not mandatory.
// Any code calling getInstance() or getAllInstances()
// is a service consumer.
class MyConsumer
{
public:
    MyServiceInterface::Provider service1;
    MyConsumer()
    {
        service1 = MyServiceInterface::getInstance();
    }

    void runService()
    {
        service1->doSomething();
    }
};

// Example of a "constructor function" for MyServiceProvider.
// This one creates a singleton instance for demonstration purposes,
// since it is not strictly needed.
std::shared_ptr<MyServiceProvider> global_constructor_function()
{
    static auto instance = std::make_shared<MyServiceProvider>("global");
    return instance;
}

// Create two service consumers and use MyServiceInterface.
void run(std::string header)
{
    std::cout << std::endl
              << header << std::endl
              << std::endl;
    MyConsumer consumer1, consumer2;
    std::cout << "-- consumer1.runService():" << std::endl;
    consumer1.runService();
    std::cout << "-- consumer2.runService():" << std::endl;
    consumer2.runService();
    std::cout << "--" << std::endl;
}

// Main program
int main()
{
    std::cout << "-- main begin" << std::endl;

    // First demonstration:
    // Each consumer will get a different instance of the service provider
    MyServiceInterface::inject<MyServiceProvider, Lifetime::Transient>("transient");
    run("== Transient lifetime ==");

    // Second demonstration:
    // All consumers share the same instance of the service provider
    MyServiceInterface::clearInjectedInstancesForTesting();
    MyServiceInterface::inject<
        MyServiceProvider,
        Lifetime::Singleton,
        ServiceConsumerMode::getInstance>("singleton");
    run("== Singleton lifetime ==");

    // Third demonstration:
    // A "constructor function" determines the lifetime of each instance
    // of the service provider.
    MyServiceInterface::clearInjectedInstancesForTesting();
    MyServiceInterface::inject(
        global_constructor_function,
        ServiceConsumerMode::getInstance);
    run("== Singleton lifetime using a constructor function ==");

    std::cout << "-- main end" << std::endl;
}