/**
 * @file InternalServices.hpp
 * @author Ángel Fernández Pineda. Madrid. Spain.
 * @brief Minimal dependency injection framework
 * @date 2025-01-30
 *
 * @copyright Minimal Dependency Injection Framework for C++
 *            © 2025 by Ángel Fernández Pineda. Madrid. Spain. 2025.
 *            is licensed under Creative Commons Attribution 4.0 International
 *
 */

//-------------------------------------------------------------------
// Globals
//-------------------------------------------------------------------

#pragma once

#include <functional>
#include <stdexcept>
#include <memory>
#include <stdexcept>
#include <vector>
#include <typeinfo>

/**
 * @brief Declare a self-managed service interface
 *
 */
#define SERVICE(ServiceName) \
    ServiceName:             \
public                       \
    InternalServices::DependencyManager<ServiceName>

/**
 * @brief Framework for dependency injection
 *
 * @details Dependency injection is not achieved by parameter passing,
 *          but by a dependency manager.
 *          The dependency manager is static,
 *          so it is not necessary to pass it as a parameter either.
 *
 * @note Service interfaces are just abstract classes or structs.
 *       Service providers are classes or structs implementing the service interface.
 *
 */
//-------------------------------------------------------------------
namespace InternalServices
//-------------------------------------------------------------------
{
    /**
     * @brief Missing service provider exception
     *
     */
    class missing_service_provider : public std::runtime_error
    {
    public:
        /**
         * @brief Construct a new missing service provider exception
         *
         * @param serviceName Class name of the service interface
         */
        missing_service_provider(std::string serviceName)
            : std::runtime_error(
                  "Service provider not found for " +
                  serviceName) {}

        virtual ~missing_service_provider() noexcept {}
    }; // missing_service_provider

    /**
     * @brief Duplicate service provider exception
     *
     */
    class service_provider_already_injected : public std::runtime_error
    {
    public:
        /**
         * @brief Construct a new service provider already injected exception
         *
         * @param serviceName Class name of the service interface
         */
        service_provider_already_injected(std::string serviceName)
            : std::runtime_error(
                  "Duplicate injection for " +
                  serviceName) {}

        virtual ~service_provider_already_injected() noexcept {}
    }; // service_provider_already_injected

    /**
     * @brief Predefined lifetime type for service providers
     *
     */
    enum class Lifetime
    {
        /**
         * @brief Each consumer gets a new instance of the service provider
         *
         */
        Transient,
        /**
         * @brief All consumers share an instance of the service provider
         *
         */
        Singleton,
        /**
         * @brief All consumers in the same thread share an instance of the service provider
         *
         */
        ThreadLocal
    };

    /**
     * @brief Mode in which a service consumer use a service provider
     *
     */
    enum class ServiceConsumerMode
    {
        /**
         * @brief Service consumers call getInstance()
         *        to get a particular service provider instance
         *
         */
        getInstance,
        /**
         * @brief Service consumers call getAllInstances()
         *        to get a particular service provider instance
         *
         */
        getAllInstances,
        /**
         * @brief Service consumers call getInstance() and/or getAllInstances()
         *        to get a particular service provider instance
         *
         */
        both
    };

    /**
     * @brief Dependency manager for a service interface
     *
     * @tparam T Class or struct declaring the service interface
     */
    template <typename T>
    struct DependencyManager
    {
    public:
        /**
         * @brief Type of all service providers
         *
         */
        typedef std::shared_ptr<T> Provider;

        /**
         * @brief Type of a set of service providers
         *
         */
        typedef std::vector<Provider> ProviderSet;

        /**
         * @brief Function prototype for creating or retrieving an instance of the service provider
         *
         */
        typedef std::function<Provider()> Constructor;

        /**
         * @brief Inject a service provider to a service interface
         *        using a constructor function
         *
         * @note The @p constructor function determines the lifetime
         *       of the service provider instance
         *
         * @throws service_provider_already_injected If two service providers are injected
         *                                           in `getInstance` consumer mode
         *
         * @param constructor Constructor function
         * @param consumerMode Service consumer mode
         */
        static void inject(
            Constructor constructor,
            ServiceConsumerMode consumerMode = ServiceConsumerMode::getInstance)
        {
            if ((consumerMode == ServiceConsumerMode::getInstance) ||
                (consumerMode == ServiceConsumerMode::both))
            {
                dependencyInjection(constructor);
            }
            else if ((consumerMode == ServiceConsumerMode::getAllInstances) ||
                     (consumerMode == ServiceConsumerMode::both))
            {
                dependencyAddition(constructor);
            }
        }

        /**
         * @brief Inject a service provider to a service interface
         *        using existing constructor parameters
         *
         * @throws service_provider_already_injected If two service providers are injected
         *                                           in `getInstance` consumer mode
         *
         * @tparam ServiceProvider Class or struct implementing the service interface
         * @tparam lifetime Service provider instance lifetime
         * @tparam consumerMode Service consumer mode
         * @tparam _Args Constructor parameter types
         * @param __args Constructor parameters
         */
        template <typename ServiceProvider,
                  Lifetime lifetime = Lifetime::Transient,
                  ServiceConsumerMode consumerMode = ServiceConsumerMode::getInstance,
                  typename... _Args>
        static void inject(_Args &&...__args)
        {
            static_assert(std::is_base_of<T, ServiceProvider>::value, "Given ServiceProvider is not derived from T");
            if constexpr (lifetime == Lifetime::Transient)
            {
                Constructor constructor = [... args = std::forward<_Args>(__args)]()
                {
                    return std::make_shared<ServiceProvider>(args...);
                };
                if constexpr (
                    (consumerMode == ServiceConsumerMode::getInstance) ||
                    (consumerMode == ServiceConsumerMode::both))
                {
                    dependencyInjection(constructor);
                }
                if constexpr (
                    (consumerMode == ServiceConsumerMode::getAllInstances) ||
                    (consumerMode == ServiceConsumerMode::both))
                {
                    dependencyAddition(constructor);
                }
            }
            else if constexpr (lifetime == Lifetime::Singleton)
            {
                Constructor constructor = [... args = std::forward<_Args>(__args)]()
                {
                    static auto singleton = std::make_shared<ServiceProvider>(args...);
                    return singleton;
                };
                if constexpr (
                    (consumerMode == ServiceConsumerMode::getInstance) ||
                    (consumerMode == ServiceConsumerMode::both))
                {
                    dependencyInjection(constructor);
                }
                if constexpr (
                    (consumerMode == ServiceConsumerMode::getAllInstances) ||
                    (consumerMode == ServiceConsumerMode::both))
                {
                    dependencyAddition(constructor);
                }
            }
            else if constexpr (lifetime == Lifetime::ThreadLocal)
            {
                Constructor constructor = [... args = std::forward<_Args>(__args)]()
                {
                    static thread_local auto singleton = std::make_shared<ServiceProvider>(args...);
                    return singleton;
                };
                if constexpr (
                    (consumerMode == ServiceConsumerMode::getInstance) ||
                    (consumerMode == ServiceConsumerMode::both))
                {
                    dependencyInjection(constructor);
                }
                if constexpr (
                    (consumerMode == ServiceConsumerMode::getAllInstances) ||
                    (consumerMode == ServiceConsumerMode::both))
                {
                    dependencyAddition(constructor);
                }
            }
        }

        /**
         * @brief Remove all previous dependency injections
         *
         * @warning For testing **only**.
         *          Do not call in production code.
         */
        static void clearInjectedInstancesForTesting()
        {
            dependencyInjection(nullptr, true);
            dependencyAddition(nullptr, true);
        }

        /**
         * @brief Retrieve a managed instance of the service provider
         *
         * @throws missing_service_provider if no service provider was injected
         * @return Provider Instance of the service provider.
         *                  Should be cached for later use since
         *                  transient instances are created on each call.
         */
        static Provider getInstance()
        {
            Constructor constructor = dependencyInjection(nullptr);
            if (constructor == nullptr)
                throw InternalServices::missing_service_provider(typeid(T).name());
            return constructor();
        }

        /**
         * @brief Retrieve a set of managed instances of service providers
         *
         * @throws missing_service_provider if no instances where injected
         * @param allowEmpty If true, the returned set can be empty,
         *                   so no exception is raised.
         * @return ProviderSet Set of instance providers.
         *                     Should be cached for later use since
         *                     transient instances are created on each call.
         */
        static ProviderSet getAllInstances(bool allowEmpty = false)
        {
            std::vector<Constructor> constructorSet = dependencyAddition(nullptr);
            if ((constructorSet.size() == 0) && !allowEmpty)
                throw InternalServices::missing_service_provider(typeid(T).name());
            ProviderSet instanceSet = {};
            instanceSet.reserve(constructorSet.size());
            for (auto constructor : constructorSet)
                instanceSet.push_back(constructor());
            return instanceSet;
        }

        /**
         * @brief Check if a service provider has been injected
         *
         * @return true if a service provider has been injected
         *              for the ServiceConsumerMode::getInstance mode
         * @return false otherwise
         */
        inline static bool injected()
        {
            return dependencyInjection(nullptr) != nullptr;
        }

        /**
         * @brief Get the size of the set of service providers
         *
         * @return size_t Count of service providers in the set
         *                for the ServiceConsumerMode::getAllInstances mode
         */
        inline static ProviderSet::size_type injectedSize()
        {
            return dependencyAddition(nullptr).size();
        }

        virtual ~DependencyManager() = default;

    private:
        static Constructor dependencyInjection(Constructor in, bool clear = false)
        {
            static Constructor dependency = nullptr;
            if (clear)
                dependency = nullptr;
            if (in != nullptr)
            {
                if (dependency == nullptr)
                    dependency = in;
                else
                    throw InternalServices::service_provider_already_injected(typeid(T).name());
            }
            return dependency;
        }

        static std::vector<Constructor> dependencyAddition(Constructor in, bool clear = false)
        {
            static std::vector<Constructor> dependency = {};
            if (clear)
                dependency.clear();
            if (in != nullptr)
                dependency.push_back(in);
            return dependency;
        }

    }; // ServiceInterface

} // namespace InternalServices

/**
 * @brief Optimized framework for dependency injection
 *        using singleton service providers
 *
 * @details For the use case where:
 *
 *          - All service providers are singleton instances.
 *          - All service consumers require just one service
 *            provider.
 */
//-------------------------------------------------------------------
namespace SingletonServices
//-------------------------------------------------------------------
{
    /**
     * @brief Dependency manager for a service interface
     *
     * @tparam T Class or struct declaring the service interface
     */
    template <typename T>
    struct DependencyManager
    {
    public:
        /**
         * @brief Type of all service providers
         *
         */
        typedef std::shared_ptr<T> Provider;

        /**
         * @brief Inject a service provider to a service interface
         *        using existing constructor parameters
         *
         * @note The lifetime for all service providers is singleton
         *
         * @throws service_provider_already_injected If two service providers are injected
         *
         * @tparam ServiceProvider Class or struct implementing the service interface
         * @tparam _Args Constructor parameter types
         * @param __args Constructor parameters
         */
        template <typename ServiceProvider,
                  typename... _Args>
        static void inject(_Args &&...__args)
        {
            static_assert(std::is_base_of<T, ServiceProvider>::value, "Given ServiceProvider is not derived from T");
            if (_instance == nullptr)
                _instance = std::make_shared<ServiceProvider>(std::forward<_Args>(__args)...);
            else
                throw InternalServices::service_provider_already_injected(typeid(T).name());
        }

        /**
         * @brief Inject an existing service provider instance to a service interface
         *
         * @param alreadyCreatedInstance Instance to be injected
         */
        static void inject(Provider alreadyCreatedInstance)
        {
            if (alreadyCreatedInstance==nullptr)
                return;
            if (_instance == nullptr)
                _instance = alreadyCreatedInstance;
            else
                throw InternalServices::service_provider_already_injected(
                    typeid(alreadyCreatedInstance.get()).name());
        }

        /**
         * @brief Remove any previous dependency injection
         *
         * @warning For testing **only**.
         *          Do not call in production code.
         */
        static void clearInjectedInstancesForTesting()
        {
            _instance = nullptr;
        }

        /**
         * @brief Retrieve a managed instance of the service provider
         *
         * @throws missing_service_provider if no service provider was injected
         * @return Provider Instance of the service provider.
         *
         */
        static Provider getInstance()
        {
            if (_instance == nullptr)
                throw InternalServices::missing_service_provider(typeid(T).name());
            return _instance;
        }

        /**
         * @brief Check if a service provider has been injected
         *
         * @return true if a service provider has been injected
         * @return false otherwise
         */
        inline static bool injected()
        {
            return _instance != nullptr;
        }

    private:
        inline static Provider _instance = nullptr;
    };
} // namespace SingletonServices
