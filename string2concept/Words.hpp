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

#ifndef CONCEPT_WORDS_HPP
#define CONCEPT_WORDS_HPP

#include <functional>

#include "core/String.hpp"
#include "core/Vector.hpp"

namespace Concept {

  class Words {

  public:
    using WordVector = Vector<Vector<char> >;

    /**
    * Constructor
    * Extract words from text
    * @param text the text to pick words from
    * @param copy if true, the words are copied internally. Otherwise overlay the text buffer.
    */
    template <size_t N>
    Words(const Vector<char, N>& text, bool copy = true) {

      _bufferOwned = copy;

      size_t wordLen = 0;

      // Search for the beginning of the first word
      auto wordStart = text.begin();
      for (; wordStart != text.end() && isSeparator(*wordStart); ++wordStart);

      // Find words
      for (auto it = wordStart; it != text.end(); ++it) {

        // A separator marks the end of a word
        if (isSeparator(*it)) { 
          if (wordLen > 0) {
            _words.push_back(Vector<char>(wordStart, wordLen, copy));
            wordLen = 0;
          }
          continue;
        }

        // A first regular character means new word
        if (wordLen == 0) wordStart = it;

        ++wordLen;
      }

      if (wordLen > 0) _words.push_back(Vector<char>(wordStart, wordLen, copy));
    }

    /**
    * @return an iterator pointing to first word
    */
    WordVector::const_iterator begin() const {
      return _words.begin();
    }

    /**
    * @return an iterator pointing to the pass-the-end word
    */
    WordVector::const_iterator end() const {
      return _words.end();
    }

    /**
    * Get the sequence of 'wordCount' words starting at 'first'
    * @return an empty Vector if the words cannot be found
    */
    Vector<char> get(WordVector::const_iterator first, size_t wordCount) {

      // This number of words cannot be found, return
      if (first + wordCount > end()) return Vector<char>();

      // All words share the same buffer,
       // just return a new not owned overlay vector spanning the requested range
      if (!_bufferOwned) {
        return Vector<char>(first->begin(), (first + wordCount - 1)->end() - first->begin(), false);
       }

      // Get the total buffer size
      size_t bufferLen = 0;
      auto it = first;

      for (size_t i = 0; i < wordCount; ++i, ++it) {
        bufferLen += it->size();
      }

      // Add the number of spaces between the words
      bufferLen += wordCount - 1;

      // Reserve space
      Vector<char> result;
      result.reserve(bufferLen);

      // Concatenate words
      result += (*first);
      it = first + 1;
      for (size_t i = 1; i < wordCount; ++i, ++it) {
       result += ' ';
       result += (*it);
      }
      return result;
    }

    /**
    * @return the number of words
    */
    size_t length() const {
      return _words.size();
    }

    /**
    * @return true if ch is a separator
    */
    static bool isSeparator(char ch) {
      return ch == ' ' || ch == 0;
    }

  private:

    WordVector _words;
    bool       _bufferOwned;
  };

} // end namespace Concept

#endif

