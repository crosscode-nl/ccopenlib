#ifndef COLL_THREAD_THREAD_WRAPPER_HXX
#define COLL_THREAD_THREAD_WRAPPER_HXX
#include <functional>
#include <thread>
#include <type_traits>
#include <ccol/util/always_false.hxx>

namespace ccol {
    namespace thread {

        /**
         * \brief Thread wrapper template to allow methods to be wrapped and run on a thread or ThreadPool
         */
        template <class T>
        std::function<void()> thread_wrapper(const std::function<void()>& /*job*/, T& /*thread*/)
        {
            static_assert(::ccol::util::always_false<T>::value, "Specialize thread_wrapper for current type.");
        }

        /**
         * \brief Thread wrapper template to allow methods to be wrapped and run on a thread or ThreadPool
         */
        template <class T>
        std::function<void()> thread_wrapper(const std::function<void()>& /*job*/)
        {
            static_assert(::ccol::util::always_false<T>::value, "Specialize thread_wrapper for current type.");
        }

        /**
         * \brief Wrap provided job lambda function in a lambda function that executes the provided method on a detached thread.
         *
         * The returned lambda could be given to a timer as a callback to create reentrant callbacks.
         *
         * \param job A lambda function to be wrapped.
         * \return The lambda function that will run the job in a detached thread when executed.
         */
        template<>
        std::function<void()> thread_wrapper<std::thread>(const std::function<void()> &job) {
            return [job]{
                std::thread thread{job};
                thread.detach();
            };
        }

    }
}

#endif // COLL_THREAD_THREAD_WRAPPER_HXX
