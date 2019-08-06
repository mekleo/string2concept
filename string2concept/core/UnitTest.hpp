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

#ifndef CONCEPT_UNITTEST_HPP
#define CONCEPT_UNITTEST_HPP

#include <iostream>
#include <memory>
#include "Vector.hpp"

namespace Concept {

  namespace UnitTest
  {
    /**
    * Abstract class defining the test interface
    */
    class Test {

    public:

      /**
      * Default constructor
      */
      Test(): _status(true) {}

      /**
      * The test name
      */
      virtual const char* name() const = 0;

      /**
      * The test body
      * @return true if sucessful, otherwise false
      */
      virtual void operator()() = 0;

      bool status() const { return _status; }

      virtual ~Test() {}

    protected:

      /**
      * Check equality
      */
      template <typename T1, typename T2>
      bool ASSERT_EQUAL(const T1& expected, const T2& actual) {
        bool status = (expected == actual);

        std::cout << statusString(status) << " : "
                  << expected << " == " << actual << std::endl;

        _status |= status;

        return status;
      }

      /**
      * Check inequality
      */
      template <typename T1, typename T2>
      bool ASSERT_NOT_EQUAL(const T1& expected, const T2& actual) {
        bool status = (expected != actual);

        std::cout << statusString(status) << " : "
          << expected << " != " << actual << std::endl;

        _status |= status;

        return status;
      }

      /**
      * Check a condition
      */
      #define ASSERT_TRUE(condition) assertTrue(condition, #condition)
       bool assertTrue(bool condition, const char* text) {
        std::cout << statusString(condition) << " : " << text << std::endl;
         _status |= condition;

        return condition;
       }

      /**
      * Define ANSI colors for POSIX terminals
      * TODO: Check terminal capability beforehand
      */
      #if defined(_MSC_VER)
        #define GREEN
        #define RED
        #define ENDCOLOR
      #else
        #define GREEN    "\033[1;32m"
        #define RED      "\033[1;31m"
        #define ENDCOLOR "\033[0m"
      #endif

      /**
      * Text representation of the status
      */
      virtual const char* statusString(bool status) {
        return status ? GREEN "Success" ENDCOLOR : RED "Failure" ENDCOLOR;
      }

      /// The test status
      bool _status;
    };

    /**
    * Fixture for adding and running unit tests
    */
    class TestFixture {

    public:

      /**
      * Add a test
      */
      void add(const std::shared_ptr<Test>& test) {
        _tests.push_back(test);
      }

      /**
      * Add all tests
      */
      bool run() {

        bool status = true;
        size_t count = 0;

        std::cout << std::endl;

        for (auto& test : _tests) {

          std::cout << "  =========== [ Test " << ++count <<  " : " << test->name() << " ] =========== \n" << std::endl;

          (*test)();
          status |= test->status();

          std::cout << std::endl << std::endl;
        }

        return status;
      }

      /**
      * Run all functions
      */
    private:

      Vector<std::shared_ptr<Test>> _tests;
    };

  } //end namespace UnitTest
} // end namespace Concept

#endif
