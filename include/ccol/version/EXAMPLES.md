# Version Examples

Version methods are pretty self explanatory.

Include header:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
#include <ccol/version/version.hxx>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Retrieving the version.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
std::string version = version();
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Retrieving the ABI version.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
std::string abiversion = abiversion();
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

C char* array's are also available:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
char* version = version_cstr();
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Get a large informative string with build information:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
std::string version = fullname();
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

See the API documentation for more detailed information.
