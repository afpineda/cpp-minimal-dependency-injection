#include <iostream>
#include <string>
#include "../InternalServices.hpp"

using namespace InternalServices;

class MyServiceInterface1
{
public:
    virtual void doSomething() = 0;
};

class MyServiceInterface2
{
public:
    virtual void doSomethingElse() = 0;
};

class CustomDependencyManager : public DependencyManager<MyServiceInterface1>,
                          public DependencyManager<MyServiceInterface2>
{
public:
    using Service1 = DependencyManager<MyServiceInterface1>;
    using Service2 = DependencyManager<MyServiceInterface2>;
};

class MyServiceProvider : public MyServiceInterface1, public MyServiceInterface2
{
public:
    virtual void doSomething() override
    {
        std::cout << this << ".doSomething()" << std::endl;
    };

    virtual void doSomethingElse() override
    {
        std::cout << this << ".doSomethingElse()" << std::endl;
    };
};

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

std::shared_ptr<MyServiceProvider> shared_constructor()
{
    static auto instance = std::make_shared<MyServiceProvider>();
    return instance;
}

int main()
{
    std::cout << "-- main begin" << std::endl;

    CustomDependencyManager::Service1::inject([]() { return shared_constructor(); });
    CustomDependencyManager::Service2::inject([]() { return shared_constructor(); });

    run("== Singleton lifetime ==");

    std::cout << "-- main end" << std::endl;
}