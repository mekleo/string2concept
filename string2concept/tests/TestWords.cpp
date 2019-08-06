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

#include "../core/Vector.hpp"
#include "../Words.hpp"
#include "TestWords.hpp"

namespace Concept {

  const char* TestWords::name() const {
    return "Checking Concept::Words";
  }

  void TestWords::operator()() {

      {
        //Count words without copy
        const char* cstr = "Indian";
        Words words(Vector<char>(cstr, strlen(cstr), false), false);
        ASSERT_EQUAL(1UL, words.length());
        ASSERT_EQUAL(cstr, words.begin()->data());
      }

      {
        // Get sub-string of 2 words without copy
        const char* cstr = "Which restaurants do East Asian food";
        Words words(Vector<char>(cstr, strlen(cstr), false), false);
        ASSERT_EQUAL(6UL, words.length());
        auto substr = words.get(words.begin() + 3, 2);
        ASSERT_EQUAL(Vector<char>("East Asian", 10), substr);
        ASSERT_EQUAL(size_t(cstr + 21), size_t(substr.data()));
      }

      {
        // Get sub-string of 2 words with copy
        const char* cstr = "Which restaurants do East Asian food";

        Words words = Vector<char>(String<>(cstr));

        ASSERT_EQUAL(6UL, words.length());
        auto substr = words.get(words.begin() + 3, 2);
        ASSERT_EQUAL(Vector<char>("East Asian", 10), substr);
      }

      ASSERT_EQUAL(6UL, Words(Vector<char>(String<>(" Where can   I find good sushi "))).length());
  }

} //end namespace Concept

