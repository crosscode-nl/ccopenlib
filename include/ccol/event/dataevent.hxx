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
#ifndef COLL_EVENT_DATAEVENT_HXX
#define COLL_EVENT_DATAEVENT_HXX

#include <ccol/event/baseevent.hxx>
#include <utility>
#include <memory>

namespace ccol {

    namespace util {

        /**
         * @brief The DataEvent class
         *
         * Base class of all DataEvents.
         */
        class DataEvent : public BaseEvent
        {

        };

        /**
         * @brief The StaticDataEvent class
         *
         * A class template to create classes that descent from DataEvent
         * that contains a static type.
         */
        template<class T>
        class StaticDataEvent : public DataEvent
        {
            T _data;
            public:
            /**
             * \brief Constructor of StaticDataEvent that takes static data.
             * \param data The data this class will hold.
             */
            StaticDataEvent(const T &data) : _data(data) {}

            /**
             * \brief Constructor of StaticDataEvent that takes static data using move semantics.
             * \param data The data this class will hold.
             */
            StaticDataEvent(T &&data) : _data(std::move(data)) {}

            /**
             * \brief Get the data by reference.
             * \return The data by reference.
             */
            T& dataRef(){
               return _data;
            }

            /**
             * \brief Get the data by copy.
             * \return The data by copy.
             */
            T dataCopy(){
               return _data;
            }
        };

        /**
         * @brief The SharedDataEvent class
         *
         * A class template to create classes that descent from DataEvent
         * that contains a type in a shared_ptr.
         */
        template<class T>
        class SharedDataEvent : public DataEvent
        {
            std::shared_ptr<T> _data;
            public:
            /**
             * \brief Constructor of SharedDataEvent that takes a shared_ptr.
             * \param data The data this class will hold.
             */
            SharedDataEvent(const std::shared_ptr<T> &data) : _data(data) {}
            /**
             * \brief Constructor of SharedDataEvent that takes a shared_ptr using move semantics.
             * \param data The data this class will hold.
             */
            SharedDataEvent(std::shared_ptr<T> &&data) : _data(std::move(data)) {}

            /**
             * \brief Get the data by reference.
             * \return The data by reference.
             */
            std::shared_ptr<T>& dataRef(){
               return _data;
            }

            /**
             * \brief Get the data by copy.
             * \return The data by copy.
             */
            std::shared_ptr<T> dataCopy(){
               return _data;
            }
        };

    }

}

#endif // COLL_EVENT_CALLBACKEVENT_HXX
