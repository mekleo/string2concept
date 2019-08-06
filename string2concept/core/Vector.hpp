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

#ifndef CONCEPT_VECTOR_HPP
#define CONCEPT_VECTOR_HPP

#include <functional>
#include <initializer_list>
#include <iostream>
#include <type_traits>
#include "String.hpp"

namespace Concept {

  /// Default size of small vector storage
  const size_t DEFAULT_SMALL_VECTOR_MAX_SIZE = 8;
  /**
  * Vector implementation with configurable small vector optimization
  */
  template <typename T, size_t SmallVectorSize = DEFAULT_SMALL_VECTOR_MAX_SIZE>
  class Vector {

  public:

    // Defining an iterator
    using iterator = T*;

    // Defining an iterator over constant values
    using const_iterator = const T*;

  // The insertion policy
    enum Insertion {
      UNORDERED, /// The collection is assumed unordered. Cannot be used along with UNIQUE
      ORDERED,   ///  The collection is assumed ordered
      UNIQUE     /// The collection is assumed to be a set. Requires ORDERED
    };

    /**
    * Default constructor
    */
    Vector() : _bufferOwned(true), _bufferSize(SmallVectorSize), _vectorSize(0) {}

    /**
    * Construct the vector from an initialization list
    */
    Vector(std::initializer_list<T> initList) :
      _bufferOwned(true), _bufferSize(SmallVectorSize), _vectorSize(0) {

      for (auto& item : initList) push_back(item);
    }

    /**
    * Copy constructor.
    * Share buffer if not owned, otherwise copy it
    */
    Vector(const Vector& other) {

      if (other._bufferOwned) {
        // Copy buffer
        _bufferOwned   = true;
        _bufferSize = SmallVectorSize;
        _vectorSize = 0;
        assign(other.data(), other._vectorSize);
      } else {
        // Overlay the same buffer
        _bufferOwned = false;
        _bufferSize = other._bufferSize;
        _vectorSize = other._vectorSize;
        _buffers.large = other._buffers.large;
      }
    }

    /**
    * Move constructor.
    *  Transfer buffer if heap-allocated or not owned. Otherwise copy it
    */
    Vector(Vector&& other) {

      if (!other._bufferOwned || other._bufferSize > SmallVectorSize) {
        // Transfer the heap-allocated buffer
        _bufferOwned   = false;
        _bufferSize    = other._bufferSize;
        _vectorSize    = other._vectorSize;
        _buffers.large = other._buffers.large;

        // Reset the other 
        other._bufferOwned = true;
        other._bufferSize  = SmallVectorSize;
        other._vectorSize  = 0;
      } else {
        // Copy buffer
        _bufferOwned = true;
        _bufferSize = SmallVectorSize;
        _vectorSize = 0;
        assign(other.data(), other._vectorSize);
      }
    }

    /**
    * Constructor from a buffer.
    * @param buffer the buffer to copy or to overlay
    * @param len the buffer len
    * @param copy if false, the vector does not own the buffer. It acts as  a not owned overlay.
    */
    Vector(const T* buffer, size_t len, bool copy = true) {
      _bufferOwned = copy;
      _vectorSize = len;

      if (copy) {
        _bufferSize = (len <= SmallVectorSize ? SmallVectorSize : len);

        // pBuffer points to either the small internal buffer
        // or the larger one for larger inputs
        T* pBuffer = (_vectorSize <= SmallVectorSize ?
                          _buffers.small : _buffers.large = new T[_bufferSize]);

        copyObjects(pBuffer, buffer, len);

      } else {
        // buffer is not owned
        _bufferSize = len;
        _buffers.large = const_cast<T*>(buffer);
      }
    }

    /**
    *  SFINAE-reserved constructor for Vectors of characters:
    * Construct from a String lvalue reference
    */
    template <size_t N,
              typename CHAR_ONLY = std::enable_if<std::is_same<T, char>::value, void> >
    Vector(const String<N>& str, bool copy = true) {

      _bufferOwned = copy;
      _vectorSize = str.length();

      if (copy) {
        // Buffer copy mode
        _bufferSize = (str.length() < SmallVectorSize ? SmallVectorSize : _vectorSize);

        // pBuffer points to either the small internal buffer
        // or the larger one for larger inputs
        char* pBuffer = (_vectorSize <= SmallVectorSize ?
          _buffers.small : _buffers.large = new char[_bufferSize]);

        copyObjects(pBuffer, str.c_str(), _vectorSize);
      }
      else {
        // Overlay mode
        _bufferSize = str.capacity() + 1;
        _buffers.large = const_cast<char*>(str.c_str());
      }
    }

    /**
    * SFINAE-reserved constructor for Vectors of characters:
    * Construct from a string rvalue reference
    */
    template <size_t N,
              typename CHAR_ONLY = std::enable_if<std::is_same<T, char>::value, void> >
    Vector(String<N>&& str) {
      _bufferOwned = true;

      if (str.capacity() > N + 1) {
        // Transfer the heap-allocated buffer
        _buffers.large = const_cast<char*>(str.c_str());
        _bufferSize = str.capacity() + 1;
        _vectorSize = str.length();

        str._bufferSize = N + 1;
        str._stringLength = 0;
      }
      else {
        _bufferSize = SmallVectorSize;
        _vectorSize = 0;
        assign(str.c_str(), str.length());
      }
    }

    /**
    * Destructor
    */
    ~Vector() {
      // If the buffer is heap-allocated and owned, delete it
     if (_bufferOwned && _bufferSize > SmallVectorSize) delete[] _buffers.large;
    }

    /**
    * Extend the internal buffer if smaller.
    * This function has no effect if the vector is not owned
    */
    void reserve(size_t vectorSize) {

      // Enough space, return
      if (!_bufferOwned || _bufferSize >= vectorSize) return;

      // Allocate a new buffer
      size_t newBufferSize = vectorSize;
      T* pNewBuffer = new T[newBufferSize];

      // Do nothing if the vector size is zero
      // Otherwise, copy old content
      copyObjects(pNewBuffer, data(), _vectorSize);

      // If the old buffer is heap-allocated, delete it
      if (_bufferSize > SmallVectorSize) delete[] _buffers.large;

      // Replace the heap-allocated buffer
      _buffers.large = pNewBuffer;
      _bufferSize = newBufferSize;
    }

    /**
    * Copy-assignment of another Vector.
    * Share buffer if not owned, Otherwise copy it
    */
    Vector& operator=(const Vector& other) {

      // Reset self
      if (!_bufferOwned) {
        _bufferOwned = true;
      }
      else if (_bufferSize > SmallVectorSize) {
        // self owns a heap-allocated buffer, free it
        delete[] _buffers.large;
      }

      _bufferSize = SmallVectorSize;
      _vectorSize = 0;

      // Transfer
      if (other._bufferOwned) {
        // Copy other's buffer
        _bufferOwned = true;
        assign(other.data(), other.size());
      } else {
        // Share a the heap-allocated buffer
        _bufferOwned = false;
        _buffers.large = other._buffers.large;
        _bufferSize = other._bufferSize;
        _vectorSize = other._vectorSize;
      }

      return *this;
    }

    /**
    * Move assignment of another Vector.
    * Move buffer if heap-allocated or not owned, Otherwise copy it
    */
    Vector& operator=(Vector&& other) {

      // Reset self
      if (!_bufferOwned) {
        _bufferOwned = false;
      }
      else if (_bufferSize > SmallVectorSize) {
        // self owns a heap-allocated buffer, free it
        delete[] _buffers.large;
      }

      _bufferSize = SmallVectorSize;
      _vectorSize = 0;

      // Transfer
      if (!other._bufferOwned || other._bufferSize > SmallVectorSize) {
        // Move the heap-allocated or external buffer
        _bufferOwned   = other._bufferOwned;
        _buffers.large = other._buffers.large;
        _bufferSize    = other._bufferSize;
        _vectorSize    = other._vectorSize;

        // Reset other
        other._bufferOwned = true;
        other._bufferSize = SmallVectorSize;
        other._vectorSize = 0;
      }
      else {
        // Copy other's buffer
        _bufferOwned = true;
        assign(other.data(), other.size());
      }

      return *this;
    }

    /**
    * Append a value.
    * This function has no effect if the vector is not owned
    */
    Vector& operator+=(const T& value) {
      if (_bufferOwned) append(&value, 1);

      return *this;
    }

    /**
    * Append another Vector.
    * This function has no effect if the vector is not owned
    */
    template <size_t N>
    Vector& operator+=(const Vector<T, N>& other) {
      if (_bufferOwned) append(other.data(), other.size());

      return *this;
    }

    /**
    * Equality check
    */
    template <size_t N>
    bool operator==(const Vector<T, N>& other) const {

      if (_vectorSize !=  other._vectorSize) return false;

      return compare(data(), other.data(), _vectorSize);

    }

    /**
    * Append a value.
    * This function has no effect if the vector is not owned
    */
    void push_back(const T& value) {
      if (!_bufferOwned) return;

      *this += value;
    }

    /**
    * @return an iterator pointing to the first element
    */
    iterator begin() {
      return data();
    }

    /**
    * @return an iterator pointing to the pass-the-end element
    */
    iterator end() {
      return data() + _vectorSize;
    }

    /**
    * @return an iterator pointing to the last element
    */
    iterator last() {
      return end() - 1;
    }

    /**
    * @return a const iterator pointing to the first element
    */
    const_iterator begin() const {
      return data();
    }

    /**
    * @return a const iterator pointing to the pass-the-end element
    */
    const_iterator end() const {
      return data() + _vectorSize;
    }

    /**
    * @return a const iterator pointing to the last element
    */
    const_iterator last() const {
      return end() - 1;
    }

    /**
    * begin iterator
    */
    T& operator[](size_t pos) {
      return data()[pos];
    }

    /**
    * Bracket operator.
    * @return a constant reference to the item at pos
    */
    const T& operator[](size_t pos) const {
      return data()[pos];
    }

    /**
    * Inserts a value.
    * @param value to be inserted
    * @param policy: if Insertion::ORDERED, the set is assumed ordered
    *                if Insertion::UNIQUE, the collection is assumed to be an ordered set
    * @return end() if the vector is not owned. Otherwise return the insertion position
    */
    iterator insert(const T& value, int policy = Insertion::UNORDERED) {

      if (!_bufferOwned) return end();

      if (policy & Insertion::ORDERED) {

        // Find the insertion position.
        auto pos = begin();
        for (; pos != end(); ++pos) {
          if ((*pos == value) && (policy & Insertion::UNIQUE)) return pos;
          if (*pos > value) break;
        }

        resize(_vectorSize + 1);

        // Shift all items
        for (auto it = last() ; it != pos; --it) {
          *it = *(it - 1);
        }

        // Insert value at pos
        *pos = value;
        return pos;

      } else {
        push_back(value);
        return last();
      }
    }

    /**
    * @return true if the vector contains that element
    * @param value The value to search
    * @param comparator The comparisiom function. Set by default to the equality operator
    */
    bool contains(const T& value,
                  std::function<bool(const T&, const T&)> isEqual = [](const T& a, const T& b){ return a == b; }) const {
      return find(value, isEqual) != end();
    }

    /**
    * Search linearly an element matching value
    * @param value The value to search
    * @return an iterator pointing to the found element or end()
    */
    iterator find(const T& value,
                        std::function<bool(const T&, const T&)> isEqual = [](const T& a, const T& b) { return a == b; }) {

      for (auto it = begin(); it != end(); ++it) {
          if (isEqual(*it, value)) return it;
      }

      return end();
    }

    /**
    * Search linearly an element matching value
    * @param value The value to search
    * @return a const iterator pointing to the found element or end()
    */
    const_iterator find(const T& value,
      std::function<bool(const T&, const T&)> isEqual = [](const T& a, const T& b) { return a == b; }) const {

      for (auto it = begin(); it != end(); ++it) {
        if (isEqual(*it, value)) return it;
      }

      return end();
    }

    /**
    * Resize the Vector.
    * @note the actual storage never reduces.
    * This function has no effect if the vector is not owned
    * and the length exceeds the buffer size
    */
    void resize(size_t len) {
      if (!_bufferOwned && len > _bufferSize) return;

       reserve(len);
      _vectorSize = len;
    }

    /**
    * @return the Vector size
    */
    size_t size() const {
      return _vectorSize;
    }

    /**
    * @return the Vector capacity
    */
    size_t capacity() const {
      return _bufferSize;
    }

    /**
    * @return the internal buffer
    */
    T* data() {
      return (_bufferOwned && _bufferSize <= SmallVectorSize ? _buffers.small : _buffers.large);
    }

    /**
    * @return the internal buffer
    */
    const T* data() const {
      return (_bufferOwned && _bufferSize <= SmallVectorSize ? _buffers.small : _buffers.large);
    }

  private:

    /**
    * Assign a C-vector
    */
    void assign(const T* buffer, size_t len) {

      if (!_bufferOwned) return;

      if (len > _bufferSize) {

        // Reset the vector size
        // to prevent reserve from copying old contents
        _vectorSize = 0;

        // Allocate more space
        reserve(len);
      }

      // Copy new vector
      copyObjects(data(), buffer, len);

      _vectorSize = len;
    }

    /**
    * Append a C-vector
    */
    void append(const T* buffer, size_t len) {
      reserve(_vectorSize + len);
      if (len > 0) copyObjects(data() + _vectorSize, buffer, len);

      _vectorSize += len;
    }

    /**
    * Copy array of objects
    */
    static void copyObjects(T* dst, const T* src, size_t len) {
      for (size_t i = 0; i < len; ++i) dst[i] = src[i];
    }


    /**
    * compare array of objects
    */
    static bool compare(const T* a, const T* b, size_t len) {

      for (size_t i = 0; i < len; ++i) if (a[i] != b[i]) return false;

      return true;
    }

    /// Reuse the pointer stack storage for small vectors
    struct {
      T* large;
      T  small[SmallVectorSize];
    } _buffers;

    /// Whether the instance is owned.
    /// This happens when the vector operates on its own storage.
    /// Otherwise, the vector is just an overlay on an external buffer
    bool _bufferOwned;

    /// Keep track of internal buffer size
    size_t _bufferSize;

    /// Keep track of the use vector size
    size_t _vectorSize;

    // Make all Vectors friends for accessing private members
    template <typename U, size_t N>
    friend class Vector;
  };

  /**
  * Stream operator
  */
  template <typename T, size_t N>
  std::ostream& operator<<(std::ostream& os, const Vector<T, N>& buffer) {
    for (auto& item : buffer)  os << item;

    return os;
  }

} //end namespace Concept

#endif
