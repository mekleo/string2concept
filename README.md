# From Text to Concepts (String2Concept)
  
Utility to extract all concepts contained in a text.

It is based on its own core data structures and algorithms:
Vector, HashTable, String, Hash.
They are optimized to minimize copies and heap allocations.

The C++ class ConceptExtractor implements the main algorithm.
It is a variant of Rabin-Karp pattern searching.
It differs from vanilla Rabin-Karp by the variable-length patterns and
because the number of comparisions is linear with the number of words,
rather that linear with the number of characters.

We intend to search for a huge number of concepts of various lengths in an input text.
For reaching linear time complexity on average, independent from the number of concepts,
we store concepts in a Hash table keyed by lowercased words.
As core data structure, the Hash table is designed as follows :
-# the keys are lowercased concepts or the first word of concepts
-# the values are pairs of the key in its original case, and a vector of concepts lengths (number of words)
  - If that length is one, the item itself is a concept
  - If that length m is greater than 1, there exist concepts of size m in the hashtable.
    .
Whenever we find an input word in the HashTable, we try to find the next m words.
Every match for the expected length is a Concept and must be returned.

This algorithm's complexity in time is, on average, linear with
the number k of words in input + the average number l of words in the concepts
To that, we add a linear term for an initial segmentation of the text into words (zero-copy process)
i.e. O(k + l) + O(n).
Its theoretical worst-case is O(k*l) + O(n). That only occurs in the hypothetical case where all hashes collide. This implies a rather poor hash function.

We prefer this algorithm to the Aho-Corasick algorithm (AC). While AC achieves an interesting linear asymptotic worst-case time complexity O(n + m),
our algorithm might be faster because, as it jumps from word to word, it achieves less comparisions.
Indeed, Aho-Corasick's automata are character-oriented.


## Prerequisites
* [GCC](https://gcc.gnu.org/) - GNU C++ compiler with C++11 support
or 
* [LLVM](https://clang.llvm.org/) - LLVM Clang with C++11 support

## Build

```
make
```

## Run unit tests

```
make test
```

## Run example

```
./string2concept -c conceptlist.txt "Which restaurants do East Asian food"
```
