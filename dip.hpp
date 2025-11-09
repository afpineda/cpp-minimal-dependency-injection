/**
 * @file dip.hpp
 *
 * @author Ángel Fernández Pineda. Madrid. Spain.
 * @brief Minimal dependency injection framework
 * @date 2025-11-09
 *
 * @copyright Minimal Dependency Injection Framework for C++
 *            © 2025 by Ángel Fernández Pineda. Madrid. Spain. 2025.
 *            is licensed under Creative Commons Attribution 4.0 International
 *
 */

#pragma once

#include <memory>
#include <type_traits>
#include <cassert>
#include <functional>
#include <vector>

// #include <iostream> // For testing

/**
 * @brief Dependency injection pattern
 *
 */
namespace dip
{
    /**
     * @brief Custom injector
     *
     * @tparam Service Service to be injected
     */
    template <class Service>
    struct Injector
    {
        /**
         * @brief Type of a function able to retrieve instances
         *
         */
        typedef std::function<Service *()> RetrieveFunction;

        /**
         * @brief Type of a function able to remove unneeded instances
         *
         */
        typedef std::function<void(Service *)> ForgetFunction;

        /**
         * @brief Custom function to retrieve instances
         *
         */
        RetrieveFunction retrieve;

        /**
         * @brief Custom function to remove uneeded instances
         *
         */
        ForgetFunction forget;
    };

    /**
     * @brief Injected instance of a service
     *
     * @tparam Service Service to be injected
     */
    template <class Service>
    struct instance
    {
        static_assert(std::is_abstract<Service>::value, "Only abstract classes are injectable");
        static_assert(std::has_virtual_destructor<Service>::value, "An injectable service must declare a virtual destructor");

        typedef Service service_type;

        /**
         * @brief Retrieve an instance providing the service
         *
         */
        instance()
        {
            assert(_injector.retrieve && "Missing dependency injection");
            _instance = _injector.retrieve();
            assert(_instance && "An injector retrieved a null provider");
        }

        /**
         * @brief Remove the instance providing the service
         *
         */
        ~instance() noexcept
        {
            if (_injector.forget)
                _injector.forget(_instance);
        }

        /**
         * @brief Access the instance providing the service
         *
         * @note Ownership is not transferred
         *
         * @return service_type* Pointer to the service provider
         */
        service_type *operator->() const noexcept { return _instance; }

        /**
         * @brief Get the instance providing the service
         *
         * @note Ownership is not transferred
         *
         * @return service_type* Pointer to the service provider
         */
        service_type operator*() const noexcept { return _instance; }

        instance(const instance &&) = delete;
        instance(instance &&) = delete;
        instance &operator=(const instance &) = delete;
        instance &operator=(instance &&) = delete;

        /**
         * @brief Inject a service provider using a custom injector
         *
         * @param injector Service injector
         */
        static void inject(const Injector<Service> &injector) noexcept
        {
            assert((_injector.retrieve == nullptr) && (_injector.forget == nullptr) && "Dependency already injected");
            assert(injector.retrieve && "Invalid injector");
            _injector = injector;
        }

        /**
         * @brief Inject a service provider with singleton life cycle
         *
         * @tparam Provider Service provider
         * @tparam _Args Constructor parameter types
         * @param __args Constructor parameters
         */
        template <class Provider, typename... _Args>
        static void inject_singleton(_Args &&...__args)
        {
            static_assert(std::is_base_of<Service, Provider>::value, "Provider does not implement Service");
            assert((_injector.retrieve == nullptr) && (_injector.forget == nullptr) && "Dependency already injected");
            _injector.forget = nullptr;
            _injector.retrieve = [... args = std::forward<_Args>(__args)]() -> Service *
            {
                static Provider p(args...);
                return &p;
            };
        }

        /**
         * @brief Inject a service provider with per-thread singleton life cycle
         *
         * @tparam Provider Service provider
         * @tparam _Args Constructor parameter types
         * @param __args Constructor parameters
         */
        template <class Provider, typename... _Args>
        static void inject_thread_singleton(_Args &&...__args)
        {
            static_assert(std::is_base_of<Service, Provider>::value, "Provider does not implement Service");
            assert((_injector.retrieve == nullptr) && (_injector.forget == nullptr) && "Dependency already injected");
            _injector.forget = nullptr;
            _injector.retrieve = [... args = std::forward<_Args>(__args)]() -> Service *
            {
                static thread_local Provider p(args...);
                return &p;
            };
        }

        /**
         * @brief Inject a service provider with transient life cycle
         *
         * @tparam Provider Service provider
         * @tparam _Args Constructor parameter types
         * @param __args Constructor parameters
         */
        template <class Provider, typename... _Args>
        static void inject_transient(_Args &&...__args)
        {
            static_assert(std::is_base_of<Service, Provider>::value, "Provider does not implement Service");
            assert((_injector.retrieve == nullptr) && (_injector.forget == nullptr) && "Dependency already injected");
            _injector.retrieve = [... args = std::forward<_Args>(__args)]() -> Service *
            {
                return new Provider(args...);
            };
            _injector.forget = [](Service *provider) -> void
            {
                delete provider;
            };
        }

        /**
         * @brief Clear the injected dependency for testing purposes
         *
         * @warning Do not call in production code.
         *          Will cause memory leaks if transient instances are not destroyed before calling.
         *
         */
        static void clear_injection() noexcept
        {
            _injector.retrieve = nullptr;
            _injector.forget = nullptr;
        }

    private:
        service_type *_instance = nullptr;
        inline static Injector<Service> _injector;
    }; // struct instance

    /**
     * @brief Inject a service provider using a custom injector
     *
     * @note To be consumed using std::instance<Service>
     *
     * @tparam Service Injectable service
     * @param injector Service injector
     */
    template <class Service>
    inline void inject(const Injector<Service> &injector)
    {
        instance<Service>::inject(injector);
    }

    /**
     * @brief Inject a transient instance to a Service
     *
     * @note To be consumed using std::instance<Service>
     *
     * @tparam Service Injectable service
     * @tparam Provider Service provider
     * @tparam _Args Constructor argument types
     * @param args Constructor arguments
     */
    template <class Service, class Provider, typename... _Args>
    inline void inject_transient(_Args &&...args)
    {
        instance<Service>::template inject_transient<Provider>(std::forward<_Args>(args)...);
    }

    /**
     * @brief Inject a singleton instance to a Service
     *
     * @note To be consumed using std::instance<Service>
     *
     * @tparam Service Injectable service
     * @tparam Provider Service provider
     * @tparam _Args Constructor argument types
     * @param args Constructor arguments
     */
    template <class Service, class Provider, typename... _Args>
    inline void inject_singleton(_Args &&...args)
    {
        instance<Service>::template inject_singleton<Provider>(std::forward<_Args>(args)...);
    }

    /**
     * @brief Inject a per-thread singleton instance to a Service
     *
     * @tparam Service Injectable service
     * @tparam Provider Service provider
     * @tparam _Args Constructor argument types
     * @param args Constructor arguments
     */
    template <class Service, class Provider, typename... _Args>
    inline void inject_thread_singleton(_Args &&...args)
    {
        instance<Service>::template inject_thread_singleton<Provider>(std::forward<_Args>(args)...);
    }

    /**
     * @brief Set of injected instances of a service
     *
     * @tparam Service Service to be injected
     */
    template <class Service>
    struct instance_set
    {
        static_assert(std::is_abstract<Service>::value, "Only abstract classes are injectable");
        static_assert(std::has_virtual_destructor<Service>::value, "An injectable service must declare a virtual destructor");

        typedef Service service_type;

        /**
         * @brief Retrieve a set of instances providing the service
         *
         */
        instance_set()
        {
            assert(!_injectors.empty() && "No dependency injections");
            for (auto injector : _injectors)
            {
                assert(injector.retrieve && "Missing dependency injection");
                auto instance = injector.retrieve();
                assert(instance && "An injector retrieved a null provider");
                _instances.push_back(instance);
            }
        }

        /**
         * @brief Remove the set of instances providing the service
         *
         */
        ~instance_set() noexcept
        {
            for (std::size_t i = 0; i < _injectors.size(); i++)
                if (_injectors[i].forget)
                    _injectors[i].forget(_instances.at(i));
        }

        instance_set(const instance_set &&) = delete;
        instance_set(instance_set &&) = delete;
        instance_set &operator=(const instance_set &) = delete;
        instance_set &operator=(instance_set &&) = delete;

        /**
         * @brief Get the count of instances injected into the service
         *
         * @return constexpr std::size_t Count of instances
         */
        constexpr std::size_t size() const noexcept
        {
            return _instances.size();
        }

        service_type *operator[](std::size_t index)
        {
            return _instances[index];
        }

        const service_type *operator[](std::size_t index) const
        {
            return _instances[index];
        }

        service_type *at(std::size_t index)
        {
            return _instances.at(index);
        }

        const service_type *at(std::size_t index) const
        {
            return _instances.at(index);
        }

        /**
         * @brief Inject a service provider using a custom injector
         *
         * @param injector Service injector
         */
        static void add(const Injector<Service> &injector) noexcept
        {
            assert(injector.retrieve && "Invalid injector");
            _injectors.push_back(injector);
        }

        /**
         * @brief Inject a service provider with singleton life cycle
         *
         * @tparam Provider Service provider
         * @tparam _Args Constructor parameter types
         * @param __args Constructor parameters
         */
        template <class Provider, typename... _Args>
        static void add_singleton(_Args &&...__args)
        {
            static_assert(std::is_base_of<Service, Provider>::value, "Provider does not implement Service");
            Injector<Service> injector{
                .retrieve = [... args = std::forward<_Args>(__args)]() -> Service *
                {
                    static Provider p(args...);
                    return &p;
                }};
            add(injector);
        }

        /**
         * @brief Inject a service provider with per-thread singleton life cycle
         *
         * @tparam Provider Service provider
         * @tparam _Args Constructor parameter types
         * @param __args Constructor parameters
         */
        template <class Provider, typename... _Args>
        static void add_thread_singleton(_Args &&...__args)
        {
            static_assert(std::is_base_of<Service, Provider>::value, "Provider does not implement Service");
            Injector<Service> injector{
                .retrieve = [... args = std::forward<_Args>(__args)]() -> Service *
                {
                    static thread_local Provider p(args...);
                    return &p;
                }};
            add(injector);
        }

        /**
         * @brief Inject a service provider with transient life cycle
         *
         * @tparam Provider Service provider
         * @tparam _Args Constructor parameter types
         * @param __args Constructor parameters
         */
        template <class Provider, typename... _Args>
        static void add_transient(_Args &&...__args)
        {
            static_assert(std::is_base_of<Service, Provider>::value, "Provider does not implement Service");
            Injector<Service> injector
            {
                .retrieve = [... args = std::forward<_Args>(__args)]() -> Service *
                {
                    static thread_local Provider p(args...);
                    return &p;
                },
                .forget = [](Service *provider) -> void
                {
                    delete provider;
                }
            };
            add(injector);
        }

        /**
         * @brief Clear all the injected dependencies for testing purposes
         *
         * @warning Do not call in production code.
         *          Will cause memory leaks if transient instances are not destroyed before calling.
         *
         */
        static void clear_injections() noexcept
        {
            _injectors.clear();
        }

    private:
        std::vector<service_type *> _instances;
        inline static std::vector<Injector<Service>> _injectors;
    }; // struct instances

    /**
     * @brief Inject a service provider using a custom injector
     *
     * @note To be consumed using std::instance_set<Service>
     *
     * @tparam Service Injectable service
     *  @param injector Service injector
     */
    template <class Service>
    inline void add(const Injector<Service> &injector)
    {
        instance_set<Service>::add(injector);
    }

    /**
     * @brief Inject a transient instance to a Service
     *
     * @note To be consumed using std::instance_set<Service>
     *
     * @tparam Service Injectable service
     * @tparam Provider Service provider
     * @tparam _Args Constructor argument types
     * @param args Constructor arguments
     */
    template <class Service, class Provider, typename... _Args>
    inline void add_transient(_Args &&...args)
    {
        instance_set<Service>::template add_transient<Provider>(std::forward<_Args>(args)...);
    }

    /**
     * @brief Inject a singleton instance to a Service
     *
     * @note To be consumed using std::instance_set<Service>
     *
     * @tparam Service Injectable service
     * @tparam Provider Service provider
     * @tparam _Args Constructor argument types
     * @param args Constructor arguments
     */
    template <class Service, class Provider, typename... _Args>
    inline void add_singleton(_Args &&...args)
    {
        instance_set<Service>::template add_singleton<Provider>(std::forward<_Args>(args)...);
    }

    /**
     * @brief Inject a per-thread singleton instance to a Service
     *
     * @note To be consumed using std::instance_set<Service>
     *
     * @tparam Service Injectable service
     * @tparam Provider Service provider
     * @tparam _Args Constructor argument types
     * @param args Constructor arguments
     */
    template <class Service, class Provider, typename... _Args>
    inline void add_thread_singleton(_Args &&...args)
    {
        instance_set<Service>::template add_thread_singleton<Provider>(std::forward<_Args>(args)...);
    }

}; // namespace dip
