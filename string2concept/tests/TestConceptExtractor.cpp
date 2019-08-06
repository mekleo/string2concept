//==============================================================================
//
// The MIT License
//
// Copyright(c) 2019 Leonce Mekinda (https://sites.google.com/site/leoncemekinda/)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE
//
//------------------------------------------------------------------------------

#include "stdafx.h"

#include "../ConceptExtractor.hpp"
#include "../core/String.hpp"
#include "TestConceptExtractor.hpp"

namespace Concept {

  const char* TestConceptExtractor::name() const {
    return "Checking Concept::ConceptExtractor";
  }

  void TestConceptExtractor::operator()() {

    {
      // Test input normalization

     String<> str(" I would   like, some thai food ! ");

      // View this string as a character vector
      Vector<char> buffer(str, false);

     ConceptExtractor::normalize(buffer);

     ASSERT_EQUAL(Vector<char>(String<>("i would like some thai food")), buffer);
    }

    {
      // Test concept extraction

      ConceptExtractor extractor {
        "Indian",
        "Thai",
        "Sushi",
        "Caribbean",
        "Italian",
        "West Indian",
        "Pub",
        "East Asian",
        "BBQ",
        "Chinese",
        "Portuguese",
        "Spanish",
        "French",
        "East European"
        };

      String<50> input = "I would like some thai food";

      auto concepts = extractor.get(input);

      if (ASSERT_EQUAL(1UL, concepts.size()))
        ASSERT_EQUAL("Thai", concepts[0]);

      input = "Where can I find good sushi";
      concepts = extractor.get(input);
      if (ASSERT_EQUAL(1UL, concepts.size()))
        ASSERT_EQUAL("Sushi", concepts[0]);

      input = "Find me a place that does tapas";
      concepts = extractor.get(input);
      ASSERT_EQUAL(0UL, concepts.size());

      input = "Which restaurants do East Asian food";
      concepts = extractor.get(input);
      if (ASSERT_EQUAL(1UL, concepts.size()))
        ASSERT_EQUAL("East Asian", concepts[0]);

      input = "Which restaurants do West Indian food";
      concepts = extractor.get(input);
      if (ASSERT_EQUAL(2UL, concepts.size())) {
        ASSERT_EQUAL("West Indian", concepts[0]);
        ASSERT_EQUAL("Indian", concepts[1]);
      }

      input = "What is the weather like today";
      concepts = extractor.get(input);
      ASSERT_EQUAL(0UL, concepts.size());
    }

  }

} //end namespace Concept

