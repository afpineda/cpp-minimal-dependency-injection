# Minimal Dependency Injection Framework for C++

This is a [**single file**](./InternalServices.hpp) library for implementing the
[*dependency injection*](https://www.geeksforgeeks.org/dependency-injectiondi-design-pattern/)
pattern in C++, a technique for achieving
[*dependency inversion*](https://en.wikipedia.org/wiki/Dependency_inversion_principle):

```mermaid
classDiagram
    class CustomServiceInterface {
        <<interface>>
    }
    CustomServiceInterface <.. ServiceConsumer: interface dependency (no code)
    CustomServiceInterface <|.. ServiceProvider: implements
```

[Render this graph at mermaid.live](https://mermaid.live/view#pako:eNp9UDsOwjAMvUrkCSTEAaKKBRY2JDaUxUpciNTYVT5ICHp3QltgAi-2n5-fP3ew4gg02A5T2nk8RwyGVbURUduSsoQjxau3tOdMsUVL6j5xXtY0_g1vNhM8TO5Hb7NeqxncCqcSKGr10VCOemJHbG9qwaJe-y3_6z2-gocoV-9GwdB3FIhzghXUEQG9q3eOixvIl1ozoGvoqMXSZQOGh0rFkuV4Yws6x0IriFLOF9AtdqlmpXeYaf7Tm9Ijn0TCTBqek1N1pQ)

instead of:

```mermaid
classDiagram
    direction RL
    ServiceConsumer ..> ServiceProvider: unwanted code dependency
```

[Render this graph at mermaid.live](https://mermaid.live/view#pako:eNo9j88OwjAIxl-l4Wx8gB686FET426mF1LQNVlhYa3GLHt3u_iHC_DlxwfMEJUYPMQBp-mQ8G6Yg7gWlIxjSSrucvwoHdsjRd6rTDWzue1299POpo9EbN5VeaIUJrcaO-KRhVjiCzbQZjImatvm1TBA6TlzAN9K4hvWoQQIsjQUa9HuJRF8scobMK33HvwNh6l1dSQs_L32h4woV9X8h5hSUTt931vT8gZDGFJO)

## Pattern implementation (library design)

- Dependency injection is not achieved by parameter passing,
  but by a dependency manager.
  The dependency manager is static,
  so it is not necessary to pass it as a parameter either.

- The library is declared within the `InternalServices` namespace.
  To import the library:

  ```c++
  #include "InternalServices.hpp"
  using namespace InternalServices;
  ```

- There are no *interfaces* in C++.
  The *custom service interface* should be declared as an
  [**abstract class**](https://en.cppreference.com/w/cpp/language/abstract_class)
  to avoid any code dependencies. However, this is not enforced.
  For example:

  ```c++
  class CustomServiceInterface
  {
    virtual void doSomething() = 0;
  }
  ```

- Service providers are implemented as descendants of service interfaces.
  For example:

  ```c++
  class CustomServiceProvider: public CustomServiceInterface
  {
    virtual void doSomething() override { ... };
  }
  ```

  A single service provider can implement multiple service interfaces
  thanks to multiple inheritance, but this is **not recommended**:

  - Dependency injection becomes repetitive.
  - If necessary, you will have to write more custom code
    in order to inject the same service provider instance
    into all service interfaces.
    See [MultipleInheritanceExample.cpp](./Examples/MultipleInheritanceExample.cpp).

- The `DependencyManager<CustomServiceInterface>` type is a dependency
  manager for the `CustomServiceInterface` class:

  - Allows one or more service providers to be injected into the given
    `CustomServiceInterface`.

  - Retrives instances of the injected service providers for the service consumers.

  There is no need to create instances of each dependency manager,
  as all methods are
  [static member functions](https://en.cppreference.com/w/cpp/language/static).

- You have full control over the **lifetime** of each injected dependency
  thanks to `DependencyManager<CustomServiceInterface>::inject()`.
  This method accepts a **"constructor function"** where you choose
  how to create or retrieve the instance of a service provider.
  The [`std::shared_ptr<>`](https://en.cppreference.com/w/cpp/memory/shared_ptr) type
  handles its lyfecycle.
  Typically, the *constructor function* is a
  [lambda expression](https://en.cppreference.com/w/cpp/language/lambda).
  For example:

  ```c++
  DependencyManager<CustomServiceInterface>::inject(
    [](){
            return std::make_shared<ServiceProvider>();
        });
  ```

- For convenience, there is **templated** `inject()` (overloaded) method able to
  inject a service provider using one of its constructors (or the default).
  In the template parameters, you specify:

  - The service provider class or struct.

  - **A lifetime for the instances of the service provider**:

    - *Transient:*
      each service consumer gets a private instance of the service provider.

    - *Singleton:*
      all service consumers share a single instance of the service provider.

    - *Thread local:*
      all service consumers running in the same thread
      share a single instance of the service provider.
      Service consumers running in different threads will never share
      the same instance of the service provider.

  - **A consumption mode** for service consumers.
    Service consumers get one or more instances of their service providers
    thanks to `DependencyManager<>::getInstance()` or
    `DependencyManager<>::getAllInstances()`

    - Service providers injected with the `getInstance` mode
      are instantiated with `getInstance()`,
      but not with `getAllInstances()`.
    - Service providers injected with `getAllInstances` mode
      are instantiated with `getAllInstances()`,
      but not with `getInstance()`.
    - Service providers injected with `both` mode
      are instantiated with `getAllInstances()` and `getInstance()`.

For examples of dependency injection calls,
see [example1.cpp](./Examples/InjectionExample.cpp).

- Service providers can declare parameters in the class constructor.
  However, it is a good practice to declare a single parameterless constructor
  (or none at all).

- A service interface can act as a dependency manager for itself
  by inheriting from the `DependencyManager<>` type.
  This is recommended but not mandatory.
  For example:

  ```c++
  class CustomServiceInterface: public DependencyManager<CustomServiceInterface>
  {
    ...
  }
  ```

  Some syntactic sugar is available:

  ```c++
  class SERVICE(CustomServiceInterface)
  {
    ...
  }
  ```

- Alternatively, you can declare a dependency manager for a set of services,
  but this is not mandatory either.
  See [CustomDependencyManager.cpp](./Examples/CustomDependencyManager.cpp).
  Typically:

    ```c++
    class DependencyManager : public DependencyManager<MyServiceInterface1>,
                              public DependencyManager<MyServiceInterface2>
    {
    public:
        using Service1 = DependencyManager<MyServiceInterface1>;
        using Service2 = DependencyManager<MyServiceInterface2>;
    };
    ```

- You must inject all the required dependencies at **program startup**.
  A runtime exception will be thrown if some dependency is missing.
  To detect missing dependencies ahead of time, call
  `DependencyManager<>::injected()` and/or `DependencyManager<>::injectedSize()`.

- A runtime exception is also thrown if a depedency is injected twice
  using a constructor function, the `getInstance` consumer mode, or
  the `both` consumer mode.

> [!CAUTION]
> A service provider can obtain and use an instance of another service provider,
> but this can lead to **circular references**. Be very careful.
> See [InfiniteLoopExample.cpp](./Examples/InfiniteLoopExample.cpp)

### Use cases

These use cases assume that `CustomServiceInterface` is derived from
`InternalServices::DependencyManager<CustomServiceInterface>`.

#### For service consumers in need of just one service provider:

```mermaid
sequenceDiagram
    MainProgram ->> CustomServiceInterface: CustomServiceInterface::inject(ConstructorFunc)
    create participant ServiceConsumer
    MainProgram ->> ServiceConsumer: create
    ServiceConsumer ->>+ CustomServiceInterface: getInstance()
    CustomServiceInterface ->> CustomServiceInterface: run ConstructorFunc
    note left of CustomServiceInterface: ConstructorFunc determines the lifetime of the service provider
    create participant ServiceProvider
    CustomServiceInterface ->> ServiceProvider: create or retrieve
    CustomServiceInterface -->>- ServiceConsumer: return ServiceProvider
    ServiceConsumer ->> ServiceConsumer: store ServiceProvider in a private attribute or local variable
    ServiceConsumer ->> ServiceProvider: use
```

[Render this graph at mermaid.live](https://mermaid.live/view#pako:eNqFU8FqwzAM_RXjU8faH8ihl45BD4VCbyMX1VFajdjOFDkwSv99ypJuLE26nGLrvef3JHSxLhZoM9vgR8Lg8IXgxODzYPTbAYU9x-7CrNZrs0mNRH9AbsnhNghyCQ6zufuMwjs6WWxiaISTk8ivKbinXtwxgqCpgYUc1RDEDAodPnnkaRMjUDYI9eBRsSM8z9o-oWzVGWjsxWBqGvowPKdgRgl7rRA1XoWlmFjOt-4v0xSoJU8BGyNnpVOJQh47ie7c9AKm5thScWvRfCv3f3AP0o3wt66ayIZRmLDFxxoqsrqfjXITh2k3E7O6F9DHGMd8Q8GA9oDaziKI-jum3mwVHVSmBSY4VvjvQ795U4N2abXsgQpdiEvHza023WNuM_0tsIRUSW7zcFUoJImHz-BspvPDpeWYTmeblVA1ekp1od6GbbpBdDJvMfofEBak8Xb9Bn4v4vUL_CNIIQ)

See [InjectionExample.cpp](./Examples/InjectionExample.cpp).

#### For service consumers in need of all available service providers:

```mermaid
sequenceDiagram
    MainProgram ->> CustomServiceInterface: CustomServiceInterface::add(ConstructorF1)
    MainProgram ->> CustomServiceInterface: CustomServiceInterface::add(ConstructorFN)
    create participant ServiceConsumer
    MainProgram ->> ServiceConsumer: create
    ServiceConsumer ->>+ CustomServiceInterface: getAllInstances()
    CustomServiceInterface ->> CustomServiceInterface: run all constructors (ConstructorF1...ConstructorFN)
    note left of CustomServiceInterface: Each constructor function determines the lifetime of the service provider
    create participant ServiceProvider1
    CustomServiceInterface ->> ServiceProvider1: create or retrieve
    create participant ServiceProviderN
    CustomServiceInterface ->> ServiceProviderN: create or retrieve

    CustomServiceInterface -->>- ServiceConsumer: return vector (ServiceProvider1..ServiceProviderN)
    ServiceConsumer ->> ServiceConsumer: store the vector of service providers in a private attribute or local variable
    ServiceConsumer ->> ServiceProvider1: use
    ServiceConsumer ->> ServiceProviderN: use
```

[Render this graph at mermaid.live](https://mermaid.live/view#pako:eNq1VE1rwzAM_SvCp45tgV1zGIx9QA8thd5GLpqttAbH7mQ7MEr_-5Q23UfW7OOwnGL76UnvSWirdDCkShXpOZPXdGdxxdhUHuSbofULDt0FXF5fw22OKTRL4tZqmvpEXKOmcuy-RGMmt8HHxFmnwA9XZ__DO-95NRMmgg1ystpu0Cfogzt0bohP5x-Ayp7oAB48dgHnoxWvKN04N5XSUMyMk76w0_BvtXP2gM6BftcZ4bObRVGccsEH8cBRnSDUo_T3qNcfuaHOXicbPBgSUGM9RUhrIbI1JdtQR9ad44EKNhxaa46Ojju_6HFXPzoxDDi2AaQ6psSWWvpttvlfs81PZ_ueRnguvw6PRGf20NLe18lQVlEMU5-NDtpXcimEad-Inl_aMmxJBCuzI0fbdoIwiZqnfJDmgkYHLbLFJ0c_Jv7QjBx_D5_v4epCyXuD1siC2XbBlZLKG6pUKb-GaswuVaryO4FiTmH54rUqZSTpQnHIq7Uqa3RRTnljREu_nY4Q6fxjCM0biIwVS2aHjbZfbLtXk_W_2A)

See [ProviderSetExample.cpp](./Examples/ProviderSetExample.cpp).

Other use cases are left to your imagination.

## Optimization

For the use case where:

- All service providers are singleton instances.
- All service consumers require just one service provider.

An optimized library is available.
Use the `SingletonServices` namespace instead of `InternalServices`.

```c++
#include "InternalServices.hpp"
using namespace SingletonServices;
```

Only `DependencyManager<>::getInstance()` is available to retrieve a service provider.
`DependencyManager<>::inject()` requires just a service provider (as a template parameter)
and the constructor parameters.

See [SingletonOptimizationExample.cpp](./Examples/SingletonOptimizationExample.cpp).
