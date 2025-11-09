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
//
// Instance creation/destruction is logged to evidence the life cycle.
class MyServiceProvider : public MyService
{
public:
    virtual void foo() override
    {
        std::cout << this << ".foo(" << data << ")" << std::endl;
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

// Consume two instances of the service
void test(const std::string &msg)
{
    std::cout << msg << std::endl;
    dip::instance<MyService> i1, i2;
    i1->foo();
    i2->foo();
}

// Main program
int main()
{

    // First demonstration:
    // Each consumer will get a different instance of the service provider
    dip::inject_transient<MyService,MyServiceProvider>("transient");
    test("== Transient lifetime ==");
    dip::instance<MyService>::clear_injection();

    // Second demonstration:
    // All consumers share the same instance of the service provider
    dip::inject_singleton<MyService,MyServiceProvider>("singleton");
    test("== Singleton lifetime ==");
}