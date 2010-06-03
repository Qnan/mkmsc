/****************************************************************************
 * Copyright (C) 2009-2010 SciTouch LLC
 * 
 * This file is part of Indigo toolkit.
 * 
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 3 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.
 * 
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 ***************************************************************************/

#ifndef _d_bitset_
#define _d_bitset_
#include "array.h"
#include "base_cpp/tlscont.h"

class Dbitset {
   //bitsets are packed into arrays of "words."  Currently a word is
   //a long long, which consists of 64 bits, requiring 6 address bits.
   //The choice of word size is determined purely by performance concerns.

private:
   
   enum {
      ADDRESS_BITS_PER_WORD = 6,
      BITS_PER_WORD = 1 << ADDRESS_BITS_PER_WORD,
      MAX_SHIFT_NUMBER = 63
   };
   static const qword WORD_MASK = 0xFFFFFFFFFFFFFFFFULL;

   //the number of words in the logical size of this BitSet.
   int _wordsInUse;
   //bits number in bitset
   int _bitsNumber;
   //words size
   int _length;
   //given a bit index, return word index containing it.
   static int _wordIndex(int bitIndex) { return bitIndex >> ADDRESS_BITS_PER_WORD; }
   //set the field wordsInUse with the logical size in words of the bit
   //set.  WARNING:This method assumes that the number of words actually
   //in use is less than or equal to the current value of wordsInUse!
   void _recalculateWordsInUse();
   //creates a new bit set. All bits are initially false.
   void _initWords(int nbits);
   // ensures that the BitSet can accommodate a given wordIndex
   void _expandTo(int wordIndex);

   int _bitCount(qword b) const;

   int _leastSignificantBitPosition(qword n) const;


   TL_CP_DECL(Array<qword>, _words);

   Dbitset(const Dbitset& ); //no implicit copy
public:
   
   Dbitset();
   //creates a bit set whose initial size
   Dbitset(int nbits);
   ~Dbitset();
   
   //sets the bit at the specified index to the complement of its current value
   void flip(int bitIndex);
   //sets each bit from the specified fromIndex to the specified toIndex (exclusive) 
   //to the complement of its current value
   void flip(int fromIndex, int toIndex);
   //sets all bits to the complement values
   void flip();

   //sets all bits to true
   void set();
   //sets the bit at the specified index to true
   void set(int bitIndex);
   //sets the bit at the specified index to the specified value.
   void set(int bitIndex, bool value);
   //sets the bits from the specified fromIndex to the specified toIndex(exclusive) 
   void set(int fromIndex, int toIndex);
   //sets the bit specified by the index to false
   void reset(int bitIndex);
   //sets all of the bits in this BitSet to false
   void clear();
   //returns the value of the bit with the specified index
   bool get(int bitIndex) const;
   //returns the index of the first bit that is set to true
   //that occurs on or after the specified starting index. If no such
   //bit exists then -1 is returned
   int nextSetBit(int fromIndex) const;
   //returns true if BitSet contains no bits that are set true
   bool isEmpty() const {return _wordsInUse == 0; };
   //Returns true if the specified BitSet has any bits set to true
   //that are also set to true in this BitSet
   bool intersects(const Dbitset& set) const;
   //Performs a logical AND of this target BitSet with the argument BitSet
   void andWith(const Dbitset& set);
   //Performs a logical OR of this target BitSet with the argument BitSet
   void orWith(const Dbitset& set);
   //Performs a logical XOR of this target BitSet with the argument BitSet
   void xorWith(const Dbitset& set);
   //Clears all of the bits in this BitSet whose corresponding
   // bit is set in the specified BitSet.
   void andNotWith(const Dbitset& set);
   //Returns the number of bits of space actually in use by this
   //BitSet to represent bit values
   int size() const {return _bitsNumber; };
   //Compares this BitSet against the specified BitSet. 
   bool equals(const Dbitset& set) const;
   //Cloning this BitSet produces a new BitSet
   void copy(const Dbitset& set);
   //copy part of BitSet
   void copySubset(const Dbitset& set);
   //resizes this BitSet
   void resize(int size);
   //checks if this BitSet is subset of argument BitSet
   bool isSubsetOf(const Dbitset& set) const;
   //checks if this BitSet is proper subset of argument BitSet
   bool isProperSubsetOf(const Dbitset& set) const;

   //fills with false all bits in this BitSet
   void zeroFill();

   void bsOrBs(const Dbitset& set1,const Dbitset& set2);
   //Clears all of the bits in first argument BitSet whose corresponding
   // bits is set in the specified second argument BitSet. Result -> this
   void bsAndNotBs(const Dbitset& set1,const Dbitset& set2);
   //Performs a logical AND of two argument BitSets. result saves to this Bitset
   void bsAndBs(const Dbitset& set1,const Dbitset& set2);
   int bitsNumber() const;

   qword shiftOne(int shiftNumber);


   DEF_ERROR("Dynamic bitset");
};

#endif
