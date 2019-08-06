/**
* @file
* @author  Leonce Mekinda <https://sites.google.com/site/leoncemekinda/>
*
* @section LICENSE
*
* The MIT License
*
* Copyright(c) 2019 Leonce Mekinda
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE
*
* ==============================================================================
*/

#ifndef CONCEPT_HASH_HPP
#define CONCEPT_HASH_HPP



#include <functional>
#include "Vector.hpp"

namespace Concept {

  class FowlerNollVoHash;

  /**
  * Hash functor
  */
  template <typename Key, typename Impl = FowlerNollVoHash>
  class Hash {

  public:

    size_t operator()(const Key& key) const {
     return std::hash<Key>()(key);
    }
  };

  /**
  * Hash functor for Vectors
  */
  template <size_t N, typename Impl>
  class Hash<Vector<char, N>, Impl> {

  public:

    size_t operator()(const Vector<char, N>& key) const {
      return Impl()(key.data(), key.size());
    }
  };


  /**
  * The Fowler-Noll-Vo Hash function
  * https://en.wikipedia.org/wiki/Fowler-Noll-Vo_hash_function
  */
  class FowlerNollVoHash {

  public:

    /**
    * Constructor: set the algorithm parameters for 32 bits and 64 bit cases
    */
    FowlerNollVoHash() :
      _prime (static_cast<size_t>(sizeof(size_t) == 8 ? 1099511628211ULL
                                                      : 16777619U)),
      _offsetBasis(static_cast<size_t>(sizeof(size_t) == 8 ? 14695981039346656037ULL
                                                           : 2166136261U))
     {}

    /**
    * Returned value
    */
    size_t operator()(const char* buffer, size_t len) const {
      size_t result = _offsetBasis;

      for (size_t i = 0; i < len; ++i) {
        result ^= static_cast<size_t>(buffer[i]);
        result *= _prime;
      }

      return result;
    }

  private:

    const size_t _prime;
    const size_t _offsetBasis;
  };

} // end namespace Concept

#endif

