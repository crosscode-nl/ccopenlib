# Util Examples

## CancellationTokenSource

Include header:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
#include <ccol/util/cancellationtokensource.hxx>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Use a CancellationTokenSource to create CancellationTokens. 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
ccol::util::CancellationTokenSource cts;
ccol::util::CancellationToken t1 = cts.token();
ccol::util::CancellationToken t2 = cts.token();
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    

When the tokens are created from a CancellationTokenSource they are not in cancelled state: 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
EXPECT_FALSE(t1.isCancelled());
EXPECT_FALSE(t2.isCancelled());
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When the CancellationTokenSource is used to signal a cancellation 
the tokens are notified. 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
cts.cancel();
EXPECT_TRUE(t1.isCancelled());
EXPECT_TRUE(t2.isCancelled());
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When a CancellationToken is instantiated without a TokenSource it will 
default to a cancelled state.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
ccol::util::CancellationToken t1;
EXPECT_TRUE(t1.isCancelled());
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When a CancellationTokenSource is destructed all tokens will be cancelled. 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
ccol::util::CancellationToken t1;
ccol::util::CancellationToken t2;
EXPECT_TRUE(t1.isCancelled());
EXPECT_TRUE(t2.isCancelled());
{
    ccol::util::CancellationTokenSource cts;
    t1 = cts.token();
    t2 = cts.token();
    EXPECT_FALSE(t1.isCancelled());
    EXPECT_FALSE(t2.isCancelled());
}
EXPECT_TRUE(t1.isCancelled());
EXPECT_TRUE(t2.isCancelled());
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
