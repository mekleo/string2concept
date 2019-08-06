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
#include "../core/HashTable.hpp"
#include "../core/String.hpp"
#include "../core/Vector.hpp"
#include "TestHashTable.hpp"

namespace Concept {

  const char* TestHashTable::name() const {
    return "Checking Concept::HashTable";
  }

  void TestHashTable::operator()() {

    {
      // Test prime numbers
      ASSERT_EQUAL(1031UL,     nextPrimeFrom(1024));
      ASSERT_EQUAL(1048583UL,  nextPrimeFrom(1048576));
      //ASSERT_EQUAL(1000000007, nextPrimeFrom(1000000000));
    }

    {
      // Test hash table keyed by size_t
      HashTable<size_t, Vector<char> > table;
      Vector<char> value = String<>("BBQ");
      table[128UL] = value;
      ASSERT_EQUAL(value, table[128UL]);
    }

    // Test hash table keyed by Vector<char>
    HashTable<Vector<char>, Vector<char>, 2> table;

    {
      Vector<char> key = String<>("Indian");
      table[key] = key;
      ASSERT_EQUAL(key, table[key]);
    }

    ASSERT_EQUAL(2, table.bucketCount());
    ASSERT_EQUAL(1, table.size());

    {
      Vector<char> key = String<>("East Asian");
      table[key] = key;
      ASSERT_EQUAL(key, table[key]);
    }

    ASSERT_EQUAL(4, table.bucketCount());
    ASSERT_EQUAL(2, table.size());

    {
      Vector<char> key = String<>("east asian");
      table[key] = key;
      ASSERT_EQUAL(key, table[key]);
   }

    ASSERT_EQUAL(4, table.bucketCount());
    ASSERT_EQUAL(3, table.size());

    {
      Vector<char> key = String<>("Which restaurants do East Asian food");
      table[key] = key;
      ASSERT_EQUAL(key, table[key]);
    }

    ASSERT_EQUAL(8, table.bucketCount());
    ASSERT_EQUAL(4, table.size());

    {
      Vector<char> key = String<>("Sushi");
      Vector<char> value = String<>("Where can I find good sushi");
      table[key] = value;
      ASSERT_EQUAL(value, table.find(key)->second);
      ASSERT_EQUAL(key, table.find(Vector<char>(String<>("Sushi")))->first);
      ASSERT_TRUE(table.end() == table.find(Vector<char>(String<>("sushi"))));
    }

    ASSERT_EQUAL(8, table.bucketCount());
    ASSERT_EQUAL(5, table.size());
  }

} //end namespace Concept

