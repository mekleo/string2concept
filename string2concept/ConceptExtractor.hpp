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

#ifndef CONCEPT_EXTRACTOR_HPP
#define CONCEPT_EXTRACTOR_HPP

#include <initializer_list>
#include <fstream>
#include <utility>

#include "core/HashTable.hpp"
#include "core/String.hpp"
#include "core/Vector.hpp"
#include "Words.hpp"

namespace Concept {
  /**
  * Main class. It extracts concepts from a text.
  * It is a variant of the Rabin-Karp algorithm.
  * It differs from vanilla Rabin-Karp by the variable-length patterns and
  * because the number of comparisions is linear with the number of words,
  * rather that linear with the number of characters.
  *
  * We intend to search for a huge number of concepts of various lengths in an input text.
  * For reaching linear time complexity on average, independent from the number of concepts,
  * we store concepts in a Hash table keyed by lowercase words.
  * As core data structure, the Hash table is designed as follows :
  * -# the keys are lowercased concepts or the first word of concepts
  * -# the values are pairs of the key in its original case , and a vector of concepts lengths (number of words)
  *   - If that length is one, the item itself is a concept
  *   - If that length m is greater than 1, there exist concepts of size m in the hashtable.
  *     .
  * Whenever we find an input word in the HashTable, we try to find the next m words.
  * Every match for the expected length is a Concept and must be returned.

  * This algorithm's complexity in time is, on average, linear with
  * the number k of words in input + the average number l of words in the concepts
  * To that, we add a linear term for an initial segmentation of the text into words (zero-copy process)
  * i.e. O(k + l) + O(n).
  * Its theoretical worst-case is O(k*l) + O(n). That only occurs in the hypothetical case where all hashes collide.
  * This implies a rather poor hash function.
  *
  * We prefer this algorithm to the Aho-Corasick algorithm (AC). While AC achieves an interesting linear asymptotic worst-case time complexity O(n + m),
  * our algorithm might be faster because, as it jumps from word to word, it achieves less comparisions.
  * Indeed, Aho-Corasick's automata are character-oriented.
  */
  class ConceptExtractor {

  public:

    static const size_t MAX_CONCEPT_LENGTH = 1024;

    /**
    * Construct concepts from list of null-terminated strings
    */
    ConceptExtractor(std::initializer_list<const char*> conceptList) {
      for (auto& concept : conceptList) addConcept(concept);
    }

    /**
    * Construct concepts from a file path
    */
    ConceptExtractor(const char* conceptFilePath) {

      std::ifstream conceptFile(conceptFilePath);
      Vector<char> concept;

      concept.reserve(MAX_CONCEPT_LENGTH);

      while (conceptFile.getline(concept.data(), concept.capacity())) {
        addConcept(concept.data());
      }
    }

    /**
    * Insert a concept in the Hash table
    */
    void addConcept(const char* concept) {

      // Insert the concept to the hash table with
      // its lowercase version as key
      Vector<char> key(concept, strlen(concept));
      lowerCase(key);
      _concepts[key].first = concept;
      _concepts[key].second.insert(1, Vector<size_t>::Insertion::ORDERED |
                                      Vector<size_t>::Insertion::UNIQUE);

      // Insert the number of words in the concept into 
      // the Hashtable element keyed by the concept's first word
      Words keyWords(key);

      // *(keyWords.begin() is the first word
      _concepts[*(keyWords.begin())].second.insert(keyWords.length(),
                                                    Vector<size_t>::Insertion::ORDERED |
                                                    Vector<size_t>::Insertion::UNIQUE);
    }

    /**
    * Extract concepts from an input text
    * @param input The input text
    * @return a vector of concepts
    */
    template <size_t N>
    Vector<String<> > get(const String<N>& input) {
      Vector<char, N> lcInput(input);
 
      normalize(lcInput);

      Words words(lcInput, false);

      Vector<String<> > result;
      for (auto itFirst = words.begin(); itFirst != words.end(); ++itFirst) {

        // Lock for the current word among concepts
        auto entry = _concepts.find(*itFirst);

        // Move to the next word if no concept starts with that word
        if (entry == _concepts.end()) continue;

        // A concept starting with that word was found
        for (auto& wordCount: entry->second.second) {

          // if starting from itFirst, this number of words cannot be found in the text, 
          // exit this loop
          if (itFirst + wordCount > words.end()) break;

          if (wordCount == 1) {
            // The word itself is a concept
            result.push_back(entry->second.first);

          } else {

            // The entry is the first word of some concepts, the get next wordCount words
            auto key = words.get(itFirst, wordCount);

            // Lookup that key
            auto concept = _concepts.find(key);
            if (concept == _concepts.end()) continue;

            // Concept found at key, add to result
            result.push_back(concept->second.first);
          }
        }
      }

      return result;
    }

    /**
    * Lowercase ASCII characters
    * @param[in,out] asciiText The text that will be subsequently modified.
    */
    template <size_t N>
    static void lowerCase(Vector<char, N>& asciiText) {

      for (auto& c : asciiText) {
        if (c >= 'A' && c <= 'Z') c += 'a'-'A';
      }
    }

    /**
    * Normalize ASCII characters in lowercasing it,
    * then removing punctuation and extra separators.
    * @param[in,out] text The text that will be subsequently modified.
    */
    template <size_t N>
    static void normalize(Vector<char, N>& text) {

      lowerCase(text);

      size_t offset = 0;
      for (auto it = text.begin(); it != text.end(); ++it) {

        if (isPunctuation(*it) ||
            (Words::isSeparator(*it) &&
              (it == text.begin()        ||
               (it + 1) == text.end()    ||
                *(it + 1) == 0           ||
                isPunctuation(*(it + 1)) ||
                Words::isSeparator(*(it + 1))
               )
              )
             ) {
          ++offset;
          continue;
         }



        *(it - offset) = *it;
      }

      text.resize(text.size() - offset);
    }

    /**
    * @return true if ch is a punctuation character
    */
    static bool isPunctuation(char ch) {
      static const Vector<char> punctuation = {',', ';', '.', '!', '?' };
      return punctuation.contains(ch);
    }

  private:

    HashTable<Vector<char>, std::pair<String<>, Vector<size_t, 1> > > _concepts;
  };

} //end namespace Concept

#endif

