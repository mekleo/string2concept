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

#include <utility>
#include "../core/String.hpp"
#include "../core/Vector.hpp"
#include "TestVector.hpp"

namespace Concept {

  const char* TestVector::name() const {
    return "Checking Concept::Vector";
  }

  void TestVector::operator()() {

    {
      // Test copy vectors
      Vector<char> buffer = { 'I', 'n', 'd', 'i', 'a', 'n', 0 };
      const char* cstr = "Indian";

      ASSERT_EQUAL(String<>(cstr), String<>(buffer.data()));
      ASSERT_NOT_EQUAL(cstr, buffer.data());
      ASSERT_EQUAL(DEFAULT_SMALL_VECTOR_MAX_SIZE, buffer.capacity());
    }

    {
     // Test character string´copy
      Vector<char> buffer("East Asian", 10);
      ASSERT_EQUAL(10ULL, buffer.capacity());
    }

    {
      // Test creation from a String object
      Vector<char> buffer = String<>("Which restaurants do East Asian food");
      ASSERT_EQUAL(37ULL, buffer.capacity());
    }

    {
      // Test vector of objects
      class Country {
      public:
        Country(){}
        Country(const Country& other) : _name(other._name) {}
        Country(const String<>& name) : _name(name) {}
        Country& operator=(const Country& other) { _name = other._name; return *this; }
        const String<>& operator()() { return _name; }
      private:
        String<> _name;
      };

      Vector<Country> buffer = { Country("Thailand"), Country("India") };
      ASSERT_EQUAL(2ULL, buffer.size());
      ASSERT_EQUAL(String<>("Thailand"), buffer[0]());
      ASSERT_EQUAL(String<>("India"), buffer[1]());
    }

    {
      // Test vector of vectors
      Vector<char> thailand(String<>("Thailand"));
      Vector<char> question(String<>("Which restaurants do East Asian food"));

      Vector<Vector<char> > buffer = { thailand, question };

      ASSERT_EQUAL(2ULL, buffer.size());
      ASSERT_EQUAL(thailand, buffer[0]);
      ASSERT_EQUAL(question, buffer[1]);
    }

    {
      // Test overlay vectors on not-owned small buffers
      // by checking pointer equality

      const char* cstr = "Thai";
      const size_t cBufferSize = strlen(cstr) + 1;
      Vector<char> buffer1(cstr, cBufferSize, false);
      ASSERT_EQUAL(cstr, buffer1.data());
      ASSERT_EQUAL(cBufferSize, buffer1.capacity());

      // Test with Copy constructor
      Vector<char> buffer2(buffer1);
      ASSERT_EQUAL(cstr, buffer2.data());
      ASSERT_EQUAL(cBufferSize, buffer2.capacity());

      // Test with Move constructor
      Vector<char> buffer3(Vector<char>(cstr, cBufferSize, false));
      ASSERT_EQUAL(cstr, buffer3.data());
      ASSERT_EQUAL(cBufferSize, buffer3.capacity());

      // Test with String lvalue
      String<> str(cstr);
      Vector<char> buffer4(str, false);
      ASSERT_EQUAL(str.c_str(), buffer4.data());
      ASSERT_EQUAL(str.capacity() + 1, buffer4.capacity());

      // Test with String rvalue
      Vector<char> buffer5 = String<>(cstr);
      ASSERT_EQUAL(Vector<char>(cstr, 4, false), buffer5);
    }

    {
      // Test overlay vectors on a not-owned larger buffer
      const char* cstr = "Which restaurants do East Asian food";
      const size_t cbufferSize = strlen(cstr) + 1;
      Vector<char> buffer(cstr, cbufferSize, false);
      ASSERT_EQUAL(cstr, buffer.data());
      ASSERT_EQUAL(cbufferSize, buffer.size());
      ASSERT_EQUAL(cbufferSize, buffer.capacity());
    }

    {
     // Test insertion in an ordered set
      Vector<int> vec = { 1, 3 };
      ASSERT_EQUAL(vec.begin() + 1, vec.insert(2, Vector<int>::Insertion::ORDERED | Vector<int>::Insertion::UNIQUE));

      ASSERT_EQUAL(3ULL, vec.size());
      ASSERT_EQUAL(DEFAULT_SMALL_VECTOR_MAX_SIZE, vec.capacity());

      ASSERT_EQUAL(1, vec[0]);
      ASSERT_EQUAL(2, vec[1]);
      ASSERT_EQUAL(3, vec[2]);
    }

    {
      // Test insertion in an ordered set
      Vector<int> vec = { 1, 2, 3 };
      ASSERT_EQUAL(vec.begin() + 1, vec.insert(2, Vector<int>::Insertion::ORDERED | Vector<int>::Insertion::UNIQUE));

      ASSERT_EQUAL(3ULL, vec.size());
      ASSERT_EQUAL(DEFAULT_SMALL_VECTOR_MAX_SIZE, vec.capacity());

      ASSERT_EQUAL(1, vec[0]);
      ASSERT_EQUAL(2, vec[1]);
      ASSERT_EQUAL(3, vec[2]);
    }

    {
      // Test insertion in an ordered collection
      Vector<int> vec = { 1, 2, 3 };
      ASSERT_EQUAL(vec.begin() + 2, vec.insert(2, Vector<int>::Insertion::ORDERED));

      ASSERT_EQUAL(4ULL, vec.size());
      ASSERT_EQUAL(DEFAULT_SMALL_VECTOR_MAX_SIZE, vec.capacity());

      ASSERT_EQUAL(1, vec[0]);
      ASSERT_EQUAL(2, vec[1]);
      ASSERT_EQUAL(2, vec[2]);
      ASSERT_EQUAL(3, vec[3]);
    }

    {
      // Test insertion in an unordered collection
      Vector<int> vec = { 1, 2, 3 };
      auto pos = vec.insert(2);
      ASSERT_EQUAL(vec.last(), pos);

      ASSERT_EQUAL(4ULL, vec.size());
      ASSERT_EQUAL(DEFAULT_SMALL_VECTOR_MAX_SIZE, vec.capacity());

      ASSERT_EQUAL(1, vec[0]);
      ASSERT_EQUAL(2, vec[1]);
      ASSERT_EQUAL(3, vec[2]);
      ASSERT_EQUAL(2, vec[3]);
    }
  }

} //end namespace Concept

