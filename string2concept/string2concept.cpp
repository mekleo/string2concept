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

#include <cstring>
#include <iostream>
#include <memory>

#include "core/String.hpp"
#include "tests/TestConceptExtractor.hpp"
#include "tests/TestHash.hpp"
#include "tests/TestHashTable.hpp"
#include "tests/TestString.hpp"
#include "tests/TestVector.hpp"
#include "tests/TestWords.hpp"

#include "ConceptExtractor.hpp"

/**
* Print help
*/
inline int usage(int argc, char** argv, int index) {
  const char * usageStr = " [OPTIONS] [<text>]\n\n"
                          "  Extract concepts from a text.\n\n" 
                          "Options:\n"
                          "-c, --concept <concept list path> <text> :\n" 
                          "    Find in <text> those of the concepts listed in <concept list path> .\n"
                          "-h, --help    : Show this help\n"
                          "-t, --test: Run unit tests\n";

  std::cout << argv[0] << usageStr << std::endl;
  return 0;
}

/**
* Run unit tests
*/
int runtests(int argc, char** argv, int index) {

  // Unit test section
  Concept::UnitTest::TestFixture allTests;
  allTests.add(std::make_shared<Concept::TestString>());
  allTests.add(std::make_shared<Concept::TestVector>());
  allTests.add(std::make_shared<Concept::TestHash>());
  allTests.add(std::make_shared<Concept::TestHashTable>());
  allTests.add(std::make_shared<Concept::TestWords>());
  allTests.add(std::make_shared<Concept::TestConceptExtractor>());

  return !allTests.run();
}

/**
* Extract concepts from text
*/
inline int extractConcepts(int argc, char** argv, int index) {

  if (index + 2 >= argc) return usage(argc, argv, 0);

  Concept::ConceptExtractor extractor(argv[index + 1]);
  auto concepts = extractor.get(Concept::String<>(argv[index + 2]));

  auto len = concepts.size();
  std::cout << std::endl << len << (len > 1 ? " concepts ": " concept ")
            << "found" << (len > 0 ? " : " :".") << std::endl;

  for (auto& concept: concepts)
    std::cout << concept << std::endl;

  return 0;
}

/**
* Execute an option handler
* @param[in] option the option to check
* @param[in] argc the program argument count
* @param[in] argv the program arguments
* @param[in] index the argument index
* @param[in] handler the option handler
* @param[out] status The handler return status
* @return true if executed, false otherwise
*/
inline  bool executeOption(const char* option,
                          int argc,
                          char** argv,
                          int index,
                          int(*handler)(int, char**, int), int& status) {

  if (strncmp(option, argv[index], strlen(option)) == 0) {
    status = handler(argc, argv, index);
    return true;
  }

  return false;
}

// The program entry point
int main(int argc, char** argv) {

  int status = 0;

  int i = 1;
    for (; i < argc; ++i) {
        if (executeOption("-t",         argc, argv, i, &runtests, status))        break;
        if (executeOption("--test",    argc, argv, i, &runtests, status))        break;
        if (executeOption("-h",         argc, argv, i, &usage, status))           break;
        if (executeOption("--help",     argc, argv, i, &usage, status))           break;
        if (executeOption("-c",         argc, argv, i, &extractConcepts, status)) break;
        if (executeOption("--concepts", argc, argv, i, &extractConcepts, status)) break;
      }
    // No valid options
    if (i == argc) status = usage(argc, argv, 0);

    return status;
}

