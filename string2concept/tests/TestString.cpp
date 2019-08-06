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
#include "../core/String.hpp"
#include "TestString.hpp"

namespace Concept {

  const char* TestString::name() const {
    return "Checking Concept::String";
  }

  void TestString::operator()() {

    String<> str;
    ASSERT_EQUAL(DEFAULT_SMALL_STRING_MAX_LENGTH, str.capacity());

    // Test Short String Construction
    String<20> str1("I would");

    ASSERT_EQUAL(20ULL, str1.capacity());

    ASSERT_EQUAL("I would", str1);

    // Test Short + Large string concatenation
    str1 += " like some thai food";
    ASSERT_EQUAL("I would like some thai food", str1);
    ASSERT_EQUAL(27ULL, str1.length());
    ASSERT_EQUAL(27ULL, str1.capacity());

    // Test large string contructor
    ASSERT_EQUAL("I would like some thai food", String<>("I would like some thai food"));

    // Test string assignment
    str1 = "John Doe";
    ASSERT_EQUAL("John Doe", str1);

    // Test inequality operator
    ASSERT_TRUE("Thai" != str1);

    String<10> str2("Where");

    ASSERT_EQUAL(10ULL, str2.capacity());

    ASSERT_EQUAL("Where", str2);

    // Test non-member equality operator
    ASSERT_EQUAL("Where can I find good sushi", str2 + " can I find good sushi");

    // Test non-member equality operator and move constructor
    ASSERT_EQUAL("Where can I find good sushi", str2 + String<>(" can I find good sushi"));
  }

} //end namespace Concept

