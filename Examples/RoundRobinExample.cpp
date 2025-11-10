/*
 * @copyright Minimal Dependency Injection Framework for C++
 *            © 2025 by Ángel Fernández Pineda. Madrid. Spain. 2025.
 *            is licensed under Creative Commons Attribution 4.0 International
 *
 */

// Utilities
#include <iostream>
#include <array>

// Import the framework
#include "../dip.hpp"

// Declare a service as an abstract class
class MyService
{
public:
    virtual void foo() = 0;

    // A virtual destructor is required
    virtual ~MyService() {};
};

// Declare a service provider.
// A service provider implements all abstract methods from the service.
//
// For demonstration purposes, this provider
// has constructor parameters, but a good practice
// is to declare parameterless constructors.
class MyServiceProvider : public MyService
{
public:
    virtual void foo() override
    {
        std::cout << this << ".foo(), index=" << index << std::endl;
    };

    MyServiceProvider(int index)
    {
        this->index = index;
    };

private:
    int index = 0;
};

// Simulate several consumers
// each one having its own service provider instance
void test()
{
    for (int i = 0; i < 6; i++)
    {
        dip::instance<MyService> provider;
        provider->foo();
    }
}

struct RoundRobin
{
    static void initialize()
    {
        for (std::size_t i = 0; i < providers.size(); i++)
        {
            assert(providers[i] == nullptr);
            providers[i] = new MyServiceProvider(i);
        }
    }

    static MyService *acquire()
    {
        MyService *instance = providers[round];
        round++;
        if (round >= providers.size())
            round = 0;
        return instance;
    }

private:
    inline static std::array<MyServiceProvider *, 3> providers;
    inline static std::size_t round = 0;
};

// Main program
int main()
{
    // Inject
    RoundRobin::initialize();
    std::cout << "Note: 3 service provider instances in round robin" << std::endl;
    dip::Injector<MyService> custom_injector = {
        .acquire = RoundRobin::acquire};
    dip::inject<MyService>(custom_injector);

    // Consume
    test();
    return 0;
}