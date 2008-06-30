/*
 *   Copyright (c) 2007, Michael Lehn
 *
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1) Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2) Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *   3) Neither the name of the FLENS development group nor the names of
 *      its contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FLENS_CRS_H
#define FLENS_CRS_H 1

#include <flens/array.h>
#include <flens/densevector.h>
#include <map>

namespace flens {

//-- CRS -----------------------------------------------------------------------

enum CRS_Storage {
    CRS_General,
    CRS_UpperTriangular,
    CRS_LowerTriangular
};

template <typename T, CRS_Storage Storage>
class CRS_Initializer;

template <typename T, CRS_Storage Storage>
class CRS_ConstIterator;

template <typename T, CRS_Storage Storage>
class CRS_Iterator;

template <typename T, CRS_Storage Storage=CRS_General>
class CRS
{
    public:
        typedef T                              ElementType;
        typedef CRS_Initializer<T, Storage>    Initializer;
        typedef CRS_ConstIterator<T, Storage>  const_iterator;
        typedef CRS_ConstIterator<T, Storage>  iterator;

        // -- constructors -----------------------------------------------------
        CRS();
        
        CRS(int numRows, int numCols, int approxBandwidth=1);

        ~CRS();

        // -- operators --------------------------------------------------------
        CRS &
        operator*=(T alpha);

        CRS &
        operator/=(T alpha);

        // -- methods ----------------------------------------------------------
        Initializer *
        initializer();

        void
        allocate(int numNonZeros);

        int
        numRows() const;

        int
        numCols() const;

        int
        numNonZeros() const;

        const_iterator
        begin() const;

        iterator
        begin();

        const_iterator
        end() const;

        iterator
        end();

        DenseVector<Array<T> >    values;
        DenseVector<Array<int> >  columns, rows;

    private:
        int  _numRows, _numCols, _k;
};

//-- CRS_Coordinate ------------------------------------------------------------

template <typename T>
struct CRS_Coordinate
{
    CRS_Coordinate(int _row, int _col, T _value);

    int row, col;
    T   value;
};

//-- CRS_CoordinateCmp ---------------------------------------------------------

struct CRS_CoordinateCmp
{
    // return true if a < b
    //   <=>  a(1)<b(1)  or  a(1)==b(1) and a(2)<b(2)
    template <typename T>
    bool
    operator()(const CRS_Coordinate<T> &a,
               const CRS_Coordinate<T> &b) const;
};

//-- CRS_Initializer -----------------------------------------------------------

template <typename T, CRS_Storage Storage>
class CRS_Initializer
{
    public:
        CRS_Initializer(CRS<T, Storage> &crs, int k);

        ~CRS_Initializer();

        void
        sort();

        T &
        operator()(int row, int col);

    private:
        typedef std::vector<CRS_Coordinate<T> > Coordinates;
        Coordinates _coordinates;

        CRS_CoordinateCmp _less;
        size_t _lastSortedCoord;
        bool _isSorted;

        CRS<T, Storage> &_crs;
};

//-- CRS_ConstIterator ---------------------------------------------------------

template <typename T, CRS_Storage Storage>
class CRS_Iterator;

template <typename T, CRS_Storage Storage>
class CRS_ConstIterator
{
    public:
        typedef std::pair<int,int>                      key_type;
        typedef T                                       mapped_type;
        typedef std::pair<key_type, mapped_type>        value_type;

        CRS_ConstIterator(const CRS_ConstIterator &rhs);

        CRS_ConstIterator(const CRS_Iterator<T, Storage> &rhs);

        CRS_ConstIterator(const CRS<T, Storage> &crs, int pos);

        bool
        operator==(const CRS_ConstIterator &rhs) const;

        bool
        operator!=(const CRS_ConstIterator &rhs) const;

        CRS_ConstIterator &
        operator++();

        const value_type &
        operator*() const;

        const value_type *
        operator->() const;

        const CRS<T, Storage>  &_crs;
        int                    _pos;
        value_type             _value;
};

//-- CRS_Iterator --------------------------------------------------------------

template <typename T, CRS_Storage Storage>
class CRS_Iterator
{
    public:
        typedef std::pair<int,int>                      key_type;
        typedef T                                       mapped_type;
        typedef std::pair<key_type, mapped_type>        value_type;

        CRS_Iterator(const CRS_Iterator &rhs);

        CRS_Iterator(const CRS<T, Storage> &crs, int pos);

        bool
        operator==(const CRS_Iterator &rhs) const;

        bool
        operator!=(const CRS_Iterator &rhs) const;

        CRS_Iterator<T, Storage> &
        operator++();

        value_type &
        operator*();

        value_type *
        operator->();

        const CRS<T, Storage>  &_crs;
        int                    _pos;
        value_type             _value;
};

} // namespace flens

#include <flens/crs.tcc>

#endif // FLENS_CRS_H
