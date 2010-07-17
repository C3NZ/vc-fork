/*  This file is part of the Vc library.

    Copyright (C) 2009 Matthias Kretz <kretz@kde.org>

    Vc is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version.

    Vc is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with Vc.  If not, see <http://www.gnu.org/licenses/>.

*/

/**
 * Type that simplifies scalar write access for a single vector object.
 *
 * The following ways of initializing a vector are not allowed:
 * \code
 * int_v v(3, 2, 8, 0); // constructor does not exist because it is not portable
 * int_v v;
 * v[0] = 3; v[1] = 2; v[2] = 8; v[3] = 0; // scalar assignments are not implemented because they would harm performance
 * \endcode
 *
 * Instead, if really necessary you can do:
 * \code
 * int_v v;
 * for (int i = 0; i < int_v::Size; ++i) {
 *   v[i] = f(i);
 * }
 * \endcode
 *
 * This is equivalent to:
 * \code
 * int_v::Memory m;
 * for (int i = 0; i < int_v::Size; ++i) {
 *   m[i] = f(i);
 * }
 * int_v v(m);
 * \endcode
 */
typedef Memory<VECTOR_TYPE, VECTOR_TYPE::Size> Memory;

/**
 * The type of the vector used for indexes in gather and scatter operations.
 */
typedef INDEX_TYPE IndexType;

/**
 * The type of the entries in the vector.
 */
typedef ENTRY_TYPE EntryType;

/**
 * The type of the mask used for masked operations and returned from comparisons.
 */
typedef MASK_TYPE Mask;

enum {
    /**
     * The size of the vector. I.e. the number of scalar entries in the vector. Do not make any
     * assumptions about the size of vectors. If you need a vector of float vs. integer of the same
     * size make use of IndexType instead. Note that this still does not guarantee the same size
     * (e.g. double_v on SSE has two entries but there exists no 64 bit integer vector type in Vc -
     * which would have two entries; thus double_v::IndexType is uint_v).
     *
     * Also you can easily use if clauses that compare sizes. The
     * compiler can statically evaluate and fully optimize dead code away (very much like #ifdef,
     * but with syntax checking).
     */
    Size
};

/**
 * Construct an uninitialized vector.
 */
VECTOR_TYPE();

/**
 * Construct a vector with the entries initialized to zero.
 *
 * \see Vc::Zero, Zero()
 */
VECTOR_TYPE(Vc::Zero);

/**
 * Returns a vector with the entries initialized to zero.
 */
static VECTOR_TYPE Zero();

/**
 * Construct a vector with the entries initialized to one.
 *
 * \see Vc::One
 */
VECTOR_TYPE(Vc::One);

#ifdef INTEGER
/**
 * Construct a vector with the entries initialized to 0, 1, 2, 3, 4, 5, ...
 *
 * \see Vc::IndexesFromZero, IndexesFromZero()
 */
VECTOR_TYPE(Vc::IndexesFromZero);

/**
 * Returns a vector with the entries initialized to 0, 1, 2, 3, 4, 5, ...
 */
static VECTOR_TYPE IndexesFromZero();
#endif

/**
 * Construct a vector loading its entries from \p alignedMemory.
 *
 * \param alignedMemory A pointer to data. The pointer must be aligned on a
 *                      Vc::VectorAlignment boundary.
 */
VECTOR_TYPE(ENTRY_TYPE *alignedMemory);

/**
 * Convert from another vector type.
 */
template<typename OtherVector> explicit VECTOR_TYPE(const OtherVector &);

/**
 * Broadcast Constructor.
 *
 * Constructs a vector with all entries of the vector filled with the given value.
 *
 * \note If you want to set it to 0 or 1 use the special initializer constructors above. Calling
 * this constructor with 0 will cause a compilation error because the compiler cannot know which
 * constructor you meant.
 */
VECTOR_TYPE(ENTRY_TYPE x);

/**
 * Construct a vector from an array of vectors with different Size.
 *
 * E.g. convert from two double_v to one float_v.
 *
 * \see expand
 */
//VECTOR_TYPE(const OtherVector *array);

/**
 * Expand the values into an array of vectors that have a different Size.
 *
 * E.g. convert from one float_v to two double_v.
 *
 * This is the reverse of the above constructor.
 */
//void expand(OtherVector *array) const;

/**
 * Load the vector entries from \p memory, overwriting the previous values.
 *
 * \param memory A pointer to data.
 * \param align  Determines whether \p memory is an aligned pointer or not.
 *
 * \see Memory
 */
void load(const ENTRY_TYPE *memory, AlignmentFlags align = Aligned);

/**
 * Set all entries to zero.
 */
void makeZero();

/**
 * Set all entries to zero where the mask is set. I.e. a 4-vector with a mask of 0111 would
 * set the last three entries to 0.
 */
void makeZero(const MASK_TYPE &mask);

/**
 * Store the vector data to \p memory.
 *
 * \param memory A pointer to memory, where to store.
 * \param align  Determines whether \p memory is an aligned pointer or not.
 *
 * \see Memory
 */
void store(EntryType *memory, AlignmentFlags align = Aligned) const;

/**
 * Return a reference to the vector entry at the given \p index.
 *
 * This operator can be used to modify scalar entries of the vector.
 *
 * \param index A value between 0 and Size. This value is not checked internally so you must make/be
 *              sure it is in range.
 */
ENTRY_TYPE &operator[](int index);

/**
 * Return the vector entry at the given \p index.
 *
 * \param index A value between 0 and Size. This value is not checked internally so you must make/be
 *              sure it is in range.
 */
ENTRY_TYPE operator[](int index) const;

/**
 * Returns an object that can be used for any kind of masked assignment.
 *
 * The returned object is only to be used for assignments and should not be assigned to a variable.
 *
 * Examples:
 * \code
 * float_v v = float_v::Zero();         // v  = [0, 0, 0, 0]
 * int_v v2 = int_v::IndexesFromZero(); // v2 = [0, 1, 2, 3]
 * v(v2 < 2) = 1.f;                     // v  = [1, 1, 0, 0]
 * v(v2 < 3) += 1.f;                    // v  = [2, 2, 1, 0]
 * ++v2(v < 1.f);                       // v2 = [0, 1, 2, 4]
 * \endcode
 */
MaskedVector operator()(const MASK_TYPE &mask);

/**
 * \name Gather and Scatter Functions
 * The gather and scatter functions allow you to easily use vectors with structured data and random
 * accesses.
 *
 * There are two variants:
 * \li For random access in arrays.
 * \li For random access of members of structs in an array.
 *
 * All gather and scatter functions optionally take a mask as last argument. In that case only the
 * entries that are selected in the mask are read in memory and copied to the vector. This allows
 * you to have invalid indexes in the \p indexes vector if those are masked off in \p mask.
 *
 * Accessing values of a struct works like this:
 * \code
 * struct MyData {
 *   float a;
 *   int b;
 * };
 *
 * void foo(MyData *data, uint_v indexes) {
 *   const float_v v1(data, &MyData::a, indexes);
 *   const int_v   v2(data, &MyData::b, indexes);
 *   v1.scatter(data, &MyData::a, indexes - float_v::Size);
 *   v2.scatter(data, &MyData::b, indexes - 1);
 * }
 * \endcode
 */
//@{
VECTOR_TYPE(const ENTRY_TYPE *array, const INDEX_TYPE &indexes);
VECTOR_TYPE(const ENTRY_TYPE *array, const INDEX_TYPE &indexes, const MASK_TYPE &mask);

template<typename S1> VECTOR_TYPE(const S1 *array, const ENTRY_TYPE S1::* member1, const INDEX_TYPE &indexes);
template<typename S1> VECTOR_TYPE(const S1 *array, const ENTRY_TYPE S1::* member1, const INDEX_TYPE &indexes, const MASK_TYPE &mask);

template<typename S1, typename S2> VECTOR_TYPE(const S1 *array, const S2 S1::* member1, const ENTRY_TYPE S2::* member2, const INDEX_TYPE &indexes);
template<typename S1, typename S2> VECTOR_TYPE(const S1 *array, const S2 S1::* member1, const ENTRY_TYPE S2::* member2, const INDEX_TYPE &indexes, const MASK_TYPE &mask);

/////////////////////////

void gather(const ENTRY_TYPE *array, const INDEX_TYPE &indexes);
void gather(const ENTRY_TYPE *array, const INDEX_TYPE &indexes, const MASK_TYPE &mask);

template<typename S1> void gather(const S1 *array, const ENTRY_TYPE S1::* member1, const INDEX_TYPE &indexes);
template<typename S1> void gather(const S1 *array, const ENTRY_TYPE S1::* member1, const INDEX_TYPE &indexes, const MASK_TYPE &mask);

template<typename S1, typename S2> void gather(const S1 *array, const S2 S1::* member1, const ENTRY_TYPE S2::* member2, const INDEX_TYPE &indexes);
template<typename S1, typename S2> void gather(const S1 *array, const S2 S1::* member1, const ENTRY_TYPE S2::* member2, const INDEX_TYPE &indexes, const MASK_TYPE &mask);

/////////////////////////

void scatter(ENTRY_TYPE *array, const INDEX_TYPE &indexes) const ;
void scatter(ENTRY_TYPE *array, const INDEX_TYPE &indexes, const MASK_TYPE &mask) const ;

template<typename S1> void scatter(S1 *array, ENTRY_TYPE S1::* member1, const INDEX_TYPE &indexes) const ;
template<typename S1> void scatter(S1 *array, ENTRY_TYPE S1::* member1, const INDEX_TYPE &indexes, const MASK_TYPE &mask) const ;

template<typename S1, typename S2> void scatter(S1 *array, S2 S1::* member1, ENTRY_TYPE S2::* member2, const INDEX_TYPE &indexes) const ;
template<typename S1, typename S2> void scatter(S1 *array, S2 S1::* member1, ENTRY_TYPE S2::* member2, const INDEX_TYPE &indexes, const MASK_TYPE &mask) const ;
//@}

/**
 * \name Comparisons
 *
 * All comparison operators return a mask object.
 *
 * \code
 * void foo(const float_v &a, const float_v &b) {
 *   const float_m mask = a < b;
 *   ...
 * }
 * \endcode
 */
//@{
MASK_TYPE operator==(const VECTOR_TYPE &x) const;
MASK_TYPE operator!=(const VECTOR_TYPE &x) const;
MASK_TYPE operator> (const VECTOR_TYPE &x) const;
MASK_TYPE operator>=(const VECTOR_TYPE &x) const;
MASK_TYPE operator< (const VECTOR_TYPE &x) const;
MASK_TYPE operator<=(const VECTOR_TYPE &x) const;
//@}

/**
 * \name Arithmetic Operations
 *
 * The vector classes implement all the arithmetic and (bitwise) logical operations as you know from
 * builtin types.
 *
 * \code
 * void foo(const float_v &a, const float_v &b) {
 *   const float_v product    = a * b;
 *   const float_v difference = a - b;
 * }
 * \endcode
 */
//@{
VECTOR_TYPE operator+(VECTOR_TYPE x) const;
VECTOR_TYPE operator-(VECTOR_TYPE x) const;
VECTOR_TYPE operator*(VECTOR_TYPE x) const;
VECTOR_TYPE operator/(VECTOR_TYPE x) const;
VECTOR_TYPE operator|(VECTOR_TYPE x) const;
VECTOR_TYPE operator&(VECTOR_TYPE x) const;
VECTOR_TYPE operator^(VECTOR_TYPE x) const;
//@}
