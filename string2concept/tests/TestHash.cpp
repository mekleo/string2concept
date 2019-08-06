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

#include "stdafx.h"
#include "../core/Hash.hpp"
#include "TestHash.hpp"

namespace Concept {

  const char* TestHash::name() const {
    return "Checking Concept::Hash";
  }

  void TestHash::operator()() {
    // Test 32 and 64 bits hashing over c-strings and Vectors, with and without copy

    const char* str = "Thai";
    if (sizeof(size_t) == 8) {
      ASSERT_EQUAL(static_cast<size_t>(5763134156844494491ULL), FowlerNollVoHash()(str, strlen(str)));
      ASSERT_EQUAL(static_cast<size_t>(5763134156844494491ULL), Hash<Vector<char> >()(Vector<char>(str, strlen(str))));
    } else {
      ASSERT_EQUAL(static_cast<size_t>(1615110235U), FowlerNollVoHash()(str, strlen(str)));
      ASSERT_EQUAL(static_cast<size_t>(1615110235U), Hash<Vector<char> >()(Vector<char>(str, strlen(str))));
    }

    str = "West indian";
    if (sizeof(size_t) == 8) {
      ASSERT_EQUAL(static_cast<size_t>(16501320808760270041ULL), FowlerNollVoHash()(str, strlen(str)));
      ASSERT_EQUAL(static_cast<size_t>(16501320808760270041ULL), Hash<Vector<char> >()(Vector<char>(str, strlen(str), false))); // No copy
    } else {
      ASSERT_EQUAL(static_cast<size_t>(130654201U), FowlerNollVoHash()(str, strlen(str)));
      ASSERT_EQUAL(static_cast<size_t>(130654201U), Hash<Vector<char> >()(Vector<char>(str, strlen(str), false))); // No copy
    }
  }

} //end namespace Concept

