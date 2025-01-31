#include <iostream>
#include <string>
#include "../InternalServices.hpp"

using namespace SingletonServices;

struct ServiceInterface
{
    virtual void doSomething() = 0;
};

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

    DependencyManager<ServiceInterface>::inject<ServiceProvider>();
    // Equivalent to:
    // DependencyManager<ServiceInterface>::inject(std::make_shared<ServiceProvider>());

    auto instance = DependencyManager<ServiceInterface>::getInstance();
    instance->doSomething();

    std::cout << "-- main end" << std::endl;
}