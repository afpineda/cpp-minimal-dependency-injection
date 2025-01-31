#include <iostream>
#include <string>
#include "../InternalServices.hpp"

using namespace InternalServices;

class SERVICE(MyServiceInterface)
{
public:
    virtual void doSomething() = 0;
};

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

std::shared_ptr<MyServiceProvider> global_constructor_function()
{
    static auto instance = std::make_shared<MyServiceProvider>("global");
    return instance;
}

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

int main()
{
    std::cout << "-- main begin" << std::endl;

    MyServiceInterface::inject<MyServiceProvider, Lifetime::Transient>("transient");
    run("== Transient lifetime ==");

    MyServiceInterface::clearInjectedInstancesForTesting();
    MyServiceInterface::inject<
        MyServiceProvider,
        Lifetime::Singleton,
        ServiceConsumerMode::getInstance>("singleton");
    run("== Singleton lifetime ==");

    MyServiceInterface::clearInjectedInstancesForTesting();
    MyServiceInterface::inject(
        global_constructor_function,
        ServiceConsumerMode::getInstance);
    run("== Singleton lifetime using a constructor function ==");

    std::cout << "-- main end" << std::endl;
}