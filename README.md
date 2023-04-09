# Boost.Range Solution To Find Most Frequently Used Words In A Text Stream Input

This program is a solution to the problem described in Ivan Cukic's book, **Functional Programming in C++**, in chapter 4, section 4.3. It is the problem that Donald Knuth implemented a solution for in 10 pages of procedural Pascal code and was published in **Communications of the ACM** journal in 1986.

Because of being limited to a gcc 4.8.3 C++ compiler at the time, this solution is implemented using the `boost::range` header file library. That compiler version supports just C++11 standard. Better would have been to use Eric Niebler's `range-v3` implementation, however, that would require a latter version of gcc C++ compiler. The Niebler implementation is more likely to resemble the Ranges feature as incorporated into the C++20 standard given the standard was based off of his work.

One obvious limitation of the `boost::range` library is that it does not always support pipe operator overloads for some functions such as its `sort()` and `unique()` functions, so they could not be as conveniently chained together with piping.

Also, note that the template class `collection_append` was not strictly needed in the implementation - it was merely a personal exercise to discover what would be required in a custom implemented collection class that works with, say, `boost::range::push_back()`.

It is necessary to install boost 1.71.0 and set include path to reference the install in order to build this program.