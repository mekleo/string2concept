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

#ifndef CONCEPT_HASHTABLE_HPP
#define CONCEPT_HASHTABLE_HPP

#include <functional>
#include <memory>
#include <limits>
#include <unordered_map>

#include "Hash.hpp"
#include "String.hpp"

namespace Concept {

  /**
  * Verify that n prime, stating from l, at compile time
  * Must not exceed 512 iterations due to compiler limitations.
  */
  inline bool isPrime(size_t n, size_t l) {
    return l * l > n || (n % l && n % (l + 2) && isPrime(n , l + 6)); 
  }

  /**
  * Compute the next prime after n, at compile time
  */
  inline size_t nextPrimeFrom(size_t n) {
   return (n == 2 || n == 3 || (n % 2 && n % 3 && isPrime(n, 5))) ? n : nextPrimeFrom(n + 1);
  }

  /** 
  * Choose large prime numbers for bucket counts.
  * This improves the bucket allocation uniformity as they do not have factors.
  * Indeed, if the bucket count K is not a prime number, hash values that are bigger-than-K multiples
  * of its factors collide with other multiples.
  * The less uniformly-distributed the Hash function, the more severe the issue.
  */
  constexpr size_t DEFAULT_SMALL_HASHTABLE_BUCKET_COUNT = 1031;


  /**
  * Class Hash table
  * The default Hash function behind the Hash class is Fowler-Noll-Vo. 
  */
  template <typename Key, typename Value, size_t SmallHashTableBucketCount = DEFAULT_SMALL_HASHTABLE_BUCKET_COUNT, typename HashType = Hash<Key>>
  class HashTable {

    using Entry = std::pair<Key, Value>;
    using Bucket = Vector<Entry>;

  public:

    static constexpr size_t MAX_BUCKET_COUNT = 1000000007; // A billion
    static constexpr double LOAD_FACTOR_REHASH_THRESHOLD  = 0.8;

    /**
    * The Hash table const iterator
    */
    using const_iterator = typename Bucket::const_iterator;

    /**
    * Constructor
    */
    HashTable() :
        _hashTableSize(0) {
            // Initialize buckets
            size_t bucketCount = nextPrimeFrom(SmallHashTableBucketCount);

            for (size_t i = 0; i < bucketCount; ++i) _storage.push_back(nullptr);
        } 

    /**
    * Destructor
    * Delete all uckets
    */
    ~HashTable() {
        for (auto& bucket : _storage) delete bucket;
    }

    /**
    * @return the Hashtable load factor
    */
    double loadFactor() const { return static_cast<double>(_hashTableSize) / _storage.size(); }
    
    /**
    * @return the number of elements
    */
    size_t size() const { return _hashTableSize; }

    /**
    * @return the number of buckets
    */
    size_t bucketCount() const { return _storage.size(); }
    
    /***
    * @return  an automatically-increased size
    */
    size_t autoResizeValue() const {
        size_t newSize = 2 * _storage.size();
        return  newSize <= MAX_BUCKET_COUNT  ? newSize : MAX_BUCKET_COUNT;   
    }
    
    /**
    * Find or create the entry at key if missing 
    * @return the entry at key
    */
    Value& operator[](const Key& key)
    {
     // Extend and rehash the hash table
     if (loadFactor() >= LOAD_FACTOR_REHASH_THRESHOLD) rehash(autoResizeValue());

     // Find the bucket for that key
     size_t bucketIndex = HashType()(key) % _storage.size();
      auto& bucket = _storage[bucketIndex];

      typename Bucket::iterator entry;

      if (bucket == nullptr) {
        // The bucket is not allocated, create it
        bucket = new Bucket();
        entry = bucket->end();
      } else {
        // The bucket is not empty search for the key
         entry = bucket->find(std::make_pair(key, Value()),
                              [](const Entry& a, const Entry& b ){ return a.first == b.first; });
      }

      // The bucket does not contain the key, add it
      if (entry == bucket->end()) {

        bucket->push_back(std::make_pair(key, Value()));

        ++_hashTableSize;

        entry = bucket->last();
      }

      return entry->second;
    }

    /**
    * @return the entry at key or end() if missing
    */
    const_iterator find(const Key& key) const
    {
      // Find the bucket for that key
      size_t bucketIndex = HashType()(key) % _storage.size();
      auto& bucket = _storage[bucketIndex];

      typename Bucket::iterator entry;

      if (bucket == nullptr) {
         return end();
      }
      else {
        // The bucket is not empty search for the key
        entry = bucket->find(std::make_pair(key, Value()),
                             [](const Entry& a, const Entry& b) { return a.first == b.first; });

        if (entry == bucket->end()) return end();

      }

      return entry;
    }

    /**
    * Extend and rehash.
    * @note the table size can only be increased
    */
    void rehash(size_t newBucketCount) {
        if (newBucketCount <= _storage.size()) return;

        // Copy the  bucket pointers to a temporary index
        Vector<Bucket*,
               SmallHashTableBucketCount> tempStorage = _storage;
       
        // Resize the index to prevent a copy during the reserve()
        _storage.resize(0);
        _storage.reserve(newBucketCount);
        for (size_t i = 0; i < newBucketCount; ++i) _storage.push_back(nullptr);

        _hashTableSize = 0;

        // Move back entries to the original storage in new buckets
        for (auto& bucket : tempStorage) {
            if (!bucket) continue;

            for (auto& entry : *bucket) {
                (*this)[entry.first] = entry.second;
            }

           delete bucket;
        }
    }

    /**
    * @return a constant iterator positioned above the last entry
    */
    const_iterator end() const
    {
      return reinterpret_cast<const_iterator>(_storage.end());
    }

  private:

   /**
   * Internal storage, a vector of unique pointers to buckets
   * Every bucket is a vector of pairs (key, value).
   */
   Vector<Bucket*,
          SmallHashTableBucketCount> _storage;
   size_t _hashTableSize;
  };

} // end namespace Concept

#endif
