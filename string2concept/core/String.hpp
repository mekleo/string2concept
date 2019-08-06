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

#ifndef CONCEPT_STRING_HPP
#define CONCEPT_STRING_HPP

#include <cstring>
#include <iostream>
#include <type_traits>

namespace Concept {

  /// Reuse pointer size as small string storage
  const size_t DEFAULT_SMALL_STRING_MAX_LENGTH = sizeof(void*);
  /**
  * String implementation with configurable string optimization
  * indeed, the Standard Library does not enforce such an optimization.
  * This means a string might be allocated in the head every time,
  * which is suboptimal.
  * Furthermore, it might be suitable to configure the maximum "small string" length later.
  */
  template <size_t SmallStringLength = DEFAULT_SMALL_STRING_MAX_LENGTH>
  class String {

  public:

    /**
    * Default constructor
    */
    String() : _bufferSize(sizeof(_buffers.small)), _stringLength(0) {
      _buffers.small[0] = 0;
    }

    /**
    * Copy constructor
    */
    String(const String& other) : _bufferSize(sizeof(_buffers.small)), _stringLength(0) {
      assign(other.c_str(), other.length());
    }

    /**
    * Move constructor
    */
    String(String&& other) {

      if (other._bufferSize > SmallStringLength + 1 &&
          _bufferSize > SmallStringLength + 1) {
        // Transfer the heap-allocated buffer
        _buffers.large = other._buffers.large;
        _bufferSize    = other._bufferSize;
        _stringLength  = other._stringLength;

        other._bufferSize = SmallStringLength + 1;
        other._stringLength = 0;
      }
      else {
        _bufferSize = SmallStringLength + 1;
        _stringLength = 0;
        assign(other.data(), other.length());
      }
    }

    /**
    * Constructor from null-terminated strings
    */
    String(const char* cstr) {
      _stringLength = strlen(cstr);
      _bufferSize = (_stringLength <= SmallStringLength ?
                     sizeof(_buffers.small) : _stringLength + 1);

      // pBuffer points to either the small buffer
      // or the larger one for larger null-terminated strings
      char* pBuffer = (_stringLength <= SmallStringLength ?
                        _buffers.small : _buffers.large = new char[_bufferSize]);

      memcpy(pBuffer, cstr, _stringLength + 1);
    }

    /**
    * Destructor
    */
    ~String() {
      // If the buffer is heap-allocated, delete it
       if (_bufferSize > SmallStringLength + 1) delete[] _buffers.large;
    }

    /**
    * @return a constant pointer to the internal buffer
    */
    const char* c_str() const {
      return const_cast<String*>(this)->data();
    }

    /**
    * Extend the internal buffer if smaller
    */
    void reserve(size_t stringLength) {

      // Enough space, return
      if (_bufferSize > stringLength) return;

      // Allocate a new buffer
      size_t newBufferSize = stringLength + 1;
      char* pNewBuffer = new char[newBufferSize];

      // Do nothing if the string length is zero
      // Otherwise, copy old content with the terminating \0
      memcpy(pNewBuffer, data(), _stringLength + 1);

      // If the old buffer is heap-allocated, delete it
      if (_bufferSize > SmallStringLength + 1) delete[] _buffers.large;

      // Replace the heap-allocated buffer
      _buffers.large = pNewBuffer;
      _bufferSize = newBufferSize;
    }


    /**
    * Check if the first character that differs from other has a lower value
    */
    template <size_t N>
    bool operator<(const String<N>& other) const {
      return strncmp(c_str(), other.c_str(), _stringLength + 1) < 0;
    }


    /**
    * Check  equality with null-terminated string
    */
    bool operator==(const char* cstr) const {
      return strncmp(c_str(), cstr, _stringLength + 1) == 0;
    }

    /**
    * Check inequality with null-terminated string
    */
    bool operator!=(const char* cstr) const {
      return !(*this == cstr);
    }

    /**
    * Check equality with another String
    */
    template <size_t N>
    bool operator==(const String<N>& other) const {
      return strncmp(c_str(), other.c_str(), _stringLength + 1) == 0;
    }

    /**
    * Check inequality with another String
    */
    template <size_t N>
    bool operator!=(const String<N>& other) const {
      return !(*this == other);
    }

    /**
    * Assign a null-terminated string
    */
    String& operator=(const char* cstr) {
      assign(cstr, strlen(cstr));
      return *this;
    }

    /**
    * Assign another String
    */
    String& operator=(const String& other) {
      assign(other.c_str(), other.length());
      return *this;
    }

    /**
    * Assign another String
    */
    template <size_t N> 
    String& operator=(const String<N>& other) {
      assign(other.c_str(), other.length());
      return *this;
    }

    /**
    * Append a null-terminated string
    */
    String& operator+=(const char* cstr) {
      append(cstr, strlen(cstr));
      return *this;
    }

    /**
    * Append another String
    */
    template <size_t N>
    String& operator+=(const String<N>& other) {
      append(other.c_str(), other.length());
      return *this;
    }

    /**
    * Concatenate a String with a null-terminated string
    */
    String operator+(const char* cstr) {
      size_t cstrLen = strlen(cstr);

      String result;
      result.reserve(_stringLength + cstrLen);

      result.assign(data(), _stringLength);
      result.append(cstr, cstrLen);

      return result;
    }

    /**
    * Concatenate Strings
    */
    template <size_t N>
    String operator+(const String<N>& other) const {

      String result;
      result.reserve(_stringLength + other.length());

      result.assign(c_str(), _stringLength);
      result.append(other.c_str(), other.length());

      return result;
    }

    /**
    * Bracket operator
    * @return the character at pos or 0 if pos is out-of-range
    */
    char operator[](size_t pos) const {
      if (pos >= _stringLength) return 0;
      return data()[pos];
    }

    /**
    * @return String length
    */
    size_t length() const {
      return _stringLength;
    }

    /**
    * @return the ma
    */
    size_t capacity() const {
      return _bufferSize - 1;
    }

  private:

    /**
    * @return the internal buffer
    */
    char* data() {
      return (_bufferSize <= SmallStringLength + 1 ? _buffers.small : _buffers.large);
    }

    /**
    * Assign a c-string
    */
    void assign(const char* cstr, size_t len) {

      if (len >= _bufferSize) {
        // Reset the string
        _stringLength = 0;

        // Allocate more space
        reserve(len);
      }

      // Copy the new string
      memcpy(data(), cstr, len);

      _stringLength = len;
      // Terminate the string with a null character
      data()[len] = 0;
    }

    /**
    * Append a C-string
    */
    void append(const char* cstr, size_t len) {
      reserve(_stringLength + len);
      memcpy(data() + _stringLength, cstr, len);

      _stringLength += len;
      data()[_stringLength] = 0;
    }

    /// Reuse the pointer stack storage for small strings
    union {
      char* large;
      char  small[SmallStringLength + 1];
    } _buffers;

    /// Keep track of internal buffer size
    size_t _bufferSize;

    /// Keep track of the actual string length
    size_t _stringLength;

    // Make all Strings friends for accessing private members
    template <size_t N>
    friend class String;

    // Make all Vectors friends for accessing private members
    template <typename T,
              size_t N>
    friend class Vector;
  };

  /**
  * Output stream operator overload
  */
  template <size_t N>
  std::ostream& operator<< (std::ostream& os, const String<N>& str) {
    return (os << str.c_str());
  }

  /**
  * Equality check overload
  */
  template <size_t N>
  bool operator==(const char* cstr, const String<N>& str) {
    return str == cstr;
  }

  /**
  * Inequality check overload
  */
  template <size_t N>
  bool operator!=(const char* cstr, const String<N>& str) {
    return str != cstr;
  }

} //end namespace Concept

#endif
