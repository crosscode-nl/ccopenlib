/*
    SPDX-License-Identifier: MIT

    © 2017 CrossCode / Patrick Vollebregt - All rights reserved

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

    If you use this code, please mention usages of this library and the copyright notice visible
    in your end product or distributed documentation. For example:

    This product uses "ccopenlib" written and copyrighted by CrossCode / Patrick Vollebregt.
    Visit http://www.ccopenlib.com for more information.

    If for some reason this not possible, please contact: ccopenlib@crosscode.nl to purchase a license exception.

    If you'd like to modify and/or share this code, share it under the same license, and keep the original copyright notice intact.

    If you have found any errors or improvements you'd like to share, please contact me: ccopenlib@crosscode.nl
*/
#ifndef COLL_UTIL_CANCELLATIONTOKEN_HXX
#define COLL_UTIL_CANCELLATIONTOKEN_HXX
#include <atomic>
#include <memory>

namespace ccol
{
    namespace util
    {
        /**
         * \brief A CancellationToken can be checked for cancellations.
         */
        class CancellationToken
        {
        private:
            std::shared_ptr<std::atomic_bool> _cancellationVariable;
        public:
            /**
             * @brief Default cancellationToken constructor
             *
             * Use a CancellationTokenSource to create an CancellationToken.
             */
            CancellationToken() = default;

            /**
             * @brief CancellationToken constructor
             *
             * Use a CancellationTokenSource to create an CancellationToken.
             *
             * @param cancellationVariable
             */
            CancellationToken(const std::shared_ptr<std::atomic_bool> &cancellationVariable);

            /**
             * \brief Check if cancellationToken is cancelled.
             *
             * See https://gcc.gnu.org/wiki/Atomic/GCCMM/AtomicSync for more information about memory order.
             *
             * \param memoryOrder Memory order to use when checking cancellation variable.
             * \return Returns true when the token is cancelled.
             */
            bool isCancelled(const std::memory_order &memoryOrder = std::memory_order_seq_cst);
            virtual ~CancellationToken();
        };
    }
}


#endif // COLL_UTIL_CANCELLATIONTOKEN_HXX
