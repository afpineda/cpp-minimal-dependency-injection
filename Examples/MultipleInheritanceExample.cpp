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

// Declare a service provider for both
// MyServiceInterface1 and MyServiceInterface2
// thanks to multiple inheritance
//
// We want a singleton instance for **both**
// services, not a singleton instance for each
// service, so extra code is needed.
class MyServiceProvider : public MyServiceInterface1,
                          public MyServiceInterface2
{
public:
    virtual void doSomething() override
    {
        std::cout << this << ".doSomething()" << std::endl;
    }

    virtual void doSomethingElse() override
    {
        std::cout << this << ".doSomethingElse()" << std::endl;
    }

    virtual ~MyServiceProvider() noexcept = default;

public:
    // "Constructor functions" follow for both services
    //
    // **IMPORTANT**
    // This example requires a c++20 compiler

    // The managed singleton instance for MyServiceInterface1
    static std::shared_ptr<MyServiceInterface1> getService1Singleton()
    {
        static_assert(__cplusplus >= 202002L, "C++20 is required for this example");
        return std::static_pointer_cast<MyServiceInterface1, MyServiceProvider>(getSingleton());
    }

    // The managed singleton instance for MyServiceInterface2
    static std::shared_ptr<MyServiceInterface2> getService2Singleton()
    {
        static_assert(__cplusplus >= 202002L, "C++20 is required for this example");
        return std::static_pointer_cast<MyServiceInterface2, MyServiceProvider>(getSingleton());
    }

private:
    // The  singleton instance
    static std::shared_ptr<MyServiceProvider> getSingleton()
    {
        static auto singleton = std::make_shared<MyServiceProvider>();
        return singleton;
    }
};

int main()
{
    std::cout << "-- main begin" << std::endl;

    // Inject a dependency for each service
    DependencyManager<MyServiceInterface1>::inject(MyServiceProvider::getService1Singleton);
    DependencyManager<MyServiceInterface2>::inject(MyServiceProvider::getService2Singleton);

    // Run both services.
    // We are logging the instance address to evidence
    // there is a singleton instance for both services.
    auto service1 = DependencyManager<MyServiceInterface1>::getInstance();
    auto service2 = DependencyManager<MyServiceInterface2>::getInstance();
    service1->doSomething();
    service2->doSomethingElse();

    std::cout << "-- main end" << std::endl;
}