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

The compiler used is gcc/g++ at version 12.1 (should be noted that the C++20 format spec won't be supported until version 13 - in the absence of format support there is one case of using `printf`). The C++ library is statically linked (refer to the project `CMakeLists.txt` file).

With ranges the pipe operator (the `'|'` character) can be used, and is so in this implementation to some degree. This resembles the use of the pipe operator in a Unix shell console where can pipe output of one tool as input into another, allowing the chaining of multiple such processing steps together. In the case of C++ ranges these pipelined operations tend to be range views and adapters.

However, there is a bit of frustration in using that syntax as extensively as one might be inclined to, as the ranges `sort()` and `unique()` functions cannot be used via the binary pipe operator. They must be invoked discreetly as a single step operation.

What is nice, though, is that a range as a function return result can be assigned to a `auto` local variable and then used in a manner to where there is lazy evaluation per actually iterating the range for subsequent processing.

Oh, it can be noted that there is an extremely terse shell script approach, using piping, that is possible from a Unix shell - but just keep in mind that the small Unix tools made use of are each a relatively significant C program in their own right. Herein with this 90 to 100 lines of C++20 code, it is doing the equivalent to that assemblage of Unix small tool programs.

Refer to Ivan Cukic's book, **Functional Programming in C++**, chapter 4, section 4.3 for this example, attributed to Doug McIlroy:

```sh
tr -cs A-Za-z '\n' |
    tr A-Z a-z |
    sort |
    uniq -c |
    sort -rn |
    sed ${1}q
```

***

## Operation of the program

The program reads the `stdin` stream, parsing it for text string tokens that are separated by whitespace.

This amounts to a very primitive lexical parser so should not really be used for any serious analysis purposes as is. For instance, it will recognize alpha-text strings that can also start with `'_'` or `'#'` and that may have embedded `'-'` or `'_'`, but it will not recognize a text token that has an embedded `'.'`, `'->'`, `':'`, `'::'`, digits, nor recognize tokens that consist of all digits. Thus it's not really suitable for, say, source code analysis. Hence regard this program as purely a learning mechanism for C++20 ranges and concepts (and for getting a sense of the efficacy of the C++20 ranges programming paradigm as contrasted against the Pascal procedural coding approach used by Donald Knuth in his published 1986 article).

Also, each produced token is lexically transformed to all lowercase so case does not distinguish recognized words.

Each unique token produced from the input stream will be tracked and counted for its occurrence. The program will sort this set of tokens by the magnitude of a token frequency count, from greatest to least. For words that have the same frequency count, that sub range will be further sorted on the word token. This pairing of count and word is written to `stdout` where a count and its respective word appear on a single line.

The program will also generate some diagnostic info which is written to `stderr`. Consequently `stdout` should be redirected to a file so can be looked at apart from the `stderr` diagnostics appearing on the console.

Here is an example of invoking the program where it processes the MIT License file of this project as input:

```sh
./wrd-frq-rngs < LICENSE > out.txt
```

Here is the output as written to `stdout` and redirected to the file `out.txt`:

```text
13: the
8: of
8: or
8: to
6: in
5: software
4: and
3: any
3: copyright
3: is
3: without
2: a
2: be
2: copies
2: for
2: including
2: notice
2: other
2: permission
2: shall
2: this
1: above
1: action
1: all
1: an
1: arising
1: associated
1: authors
1: but
1: connection
1: copy
1: damages
1: deal
1: dealings
1: do
1: documentation
1: event
1: express
1: files
1: fitness
1: following
1: free
1: furnished
1: hereby
1: holders
1: included
1: liable
1: license
1: limitation
1: limited
1: mit
1: no
1: not
1: obtaining
1: out
1: particular
1: permit
1: person
1: persons
1: portions
1: provided
1: purpose
1: rights
1: sell
1: subject
1: substantial
1: tort
1: use
1: warranties
1: warranty
1: whether
1: whom
1: with
```