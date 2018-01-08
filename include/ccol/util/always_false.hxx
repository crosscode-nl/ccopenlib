#ifndef COLL_UTIL_ALWAYSFALSE_WRAPPER_HXX
#define COLL_UTIL_ALWAYSFALSE_WRAPPER_HXX
#include <functional>
#include <thread>
#include <type_traits>

namespace ccol {
    namespace util {

        /**
         * \brief always_false template helper to assist static_assert to detect unspecialized templates.
         */
        template <typename T>
        struct always_false : std::false_type
        {
        };
    }
}
#endif
