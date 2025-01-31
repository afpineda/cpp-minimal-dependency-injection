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
// This time we are using another namespace
#include "../InternalServices.hpp"
using namespace SingletonServices;

// Declare a service interface
struct ServiceInterface
{
    virtual void doSomething() = 0;
};

// Declare a service provider
struct ServiceProvider: ServiceInterface
{
    virtual void doSomething() override
    {
        std::cout << "doSomething()" << std::endl;
    };
};

int main()
{
    std::cout << "-- main begin" << std::endl;

    // Inject singleton dependency.
    // Equivalent to:
    // DependencyManager<ServiceInterface>::inject(std::make_shared<ServiceProvider>());
    DependencyManager<ServiceInterface>::inject<ServiceProvider>();

    // Run the service
    auto instance = DependencyManager<ServiceInterface>::getInstance();
    instance->doSomething();

    std::cout << "-- main end" << std::endl;
}