# C++20 Range Solution To Find Most Frequently Used Words In A Text Stream Input

This program is a solution to the problem described in Ivan Cukic's book, **Functional Programming in C++**, in chapter 4, section 4.3. It is the problem that Donald Knuth implemented a solution for in 10 pages of procedural Pascal code and was published in **Communications of the ACM** journal in 1986.

Here in this implementation, features of C++20, principally ranges and concepts, are utilized in the solution. Using the `cloc` tool, it reports 158 lines of C++ code per this implementation:

```sh
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C++                              1             30             69            158
-------------------------------------------------------------------------------

```

About 10 of those lines relate to printing out debug info, then there is a template class `collection_append` that is not strictly necessary but was just an exercise for implementing a class with collection-like behaviors as constrained by concepts, so removing that class would reduce the count by about another 50. Thus the actual implementation is around 90 to a 100 lines.

The compiler used is gcc/g++ at version 12.1 (should be noted that the C++20 format spec won't be supported until version 13 - in the absence of format support there is one case of using `printf`). The C++ library is statically linked.

With ranges the pipe operator (the `|` character) can be used, and is so in this implementation to some degree. This resembles the use of the pipe operator in a Unix shell console where can pipe output of one tool as input into another, allowing the chaining of multiple such processing steps together. In the case of C++ ranges these pipelined operations tend to be views and adapters.

However, there is a bit of frustration in using that syntax as extensively as one might be inclined to, as the ranges `sort()` and `unique()` functions cannot be used via the binary pipe operator. They must be invoked discreetly as a single step operation.

What is nice, though, is that a range can be assigned to a `auto` local variable and then used in a manner to where there is lazy evaluation per actually iterating the range for subsequent processing.