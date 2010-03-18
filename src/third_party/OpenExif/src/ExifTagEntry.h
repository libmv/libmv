/*
 * Copyright (c) 2000-2009, Eastman Kodak Company
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification,are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright notice, 
 *       this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the 
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Eastman Kodak Company nor the names of its 
 *       contributors may be used to endorse or promote products derived from 
 *       this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 *
 * Creation Date: 07/14/2001
 *
 * Original Author: 
 * George Sotak george.sotak@kodak.com 
 *
 * Contributor(s): 
 * Ricardo Rosario ricardo.rosario@kodak.com
 */ 

#ifndef _EXIF_TAG_ENTRY_H_
#define _EXIF_TAG_ENTRY_H_


#include "ExifTypeDefs.h"
#include <iostream>
#include <vector>
#include <string>

//:
// This abstract class is the base class for representing the tag entry of an Exif image.
// It contains three pieces of inforamtion: tag that identified the field, field type
// and the count of number of values.
//
class EXIF_DECL ExifTagEntry
{
    public:

        // Destructor
        virtual ~ExifTagEntry() {}

        // Create a copy of the ExifTagEntry object.
        //!return: Pointer to the copy.
        virtual ExifTagEntry* clone( void ) const = 0;
    
        // Get the tag number.
        //!return: Tag number
        exiftag_t getTagNum( void ) const
            { return mTagNum; }

        // Get the field type.
        //!return: Field type.
        ttype_t getType( void ) const
            { return mType; }

        // Get the number of values.
        //!return: Reference to the number of values.
        exif_int32& getCount( void )
            { return mCount; }

        // Get the number of values.
        //!return: Number of values.
        exif_int32 getCount( void ) const
            { return mCount; }

        // Set the number of values.
        //!param: cnt - Number of values
        void setCount( exif_int32 cnt )
            { mCount = cnt; return; }

        // Print the tag entry.
        //!param: c - Output stream.
        //!return: Output stream.
        virtual std::ostream& print(std::ostream& c = std::cout) const = 0;

    protected:
        ExifTagEntry(exiftag_t num, ttype_t typ )
            : mTagNum(num), mType(typ)
            {}
        ExifTagEntry(exiftag_t num, ttype_t typ, exif_int32 cnt )
            : mTagNum(num), mType(typ), mCount(cnt)
            {}

        exiftag_t     mTagNum;
        ttype_t    mType;
        exif_int32      mCount;
} ;


inline std::ostream& ExifPrintValue(std::ostream& c, const unsigned char& v) { return c << (int)v; }
inline std::ostream& ExifPrintValue(std::ostream& c, const char& v) { return c << (int)v; }
inline std::ostream& ExifPrintValue(std::ostream& c, const unsigned short& v) { return c << v; }
inline std::ostream& ExifPrintValue(std::ostream& c, const short& v) { return c << v; }
inline std::ostream& ExifPrintValue(std::ostream& c, const unsigned int& v) { return c << v; }
inline std::ostream& ExifPrintValue(std::ostream& c, const int& v) { return c << v; }
inline std::ostream& ExifPrintValue(std::ostream& c, const unsigned long& v) { return c << v; }
inline std::ostream& ExifPrintValue(std::ostream& c, const long& v) { return c << v; }
inline std::ostream& ExifPrintValue(std::ostream& c, const float& v) { return c << v; }
inline std::ostream& ExifPrintValue(std::ostream& c, const double& v) { return c << v; }
inline std::ostream& ExifPrintValue(std::ostream& c, const std::string& v) { return c << v ; }

// ostream implementation for vector. we want to avoid using operator "<<"
// that will conflict the one in ekc
template <class TYPE>
inline std::ostream& ExifPrintValue(std::ostream& c, const std::vector<TYPE>& v)
{
    int i = v.size();
    c << "[" ;
    for (int j=0; j<i; j++) 
    {
        c << (j==0?"":",");
        ExifPrintValue(c, v[j]);
    }
    c << "]";   
    return c;
}

//:
// This template class derives from ExifTagEntry to represent the tag entry of the
// specified type. 
//
template<class Type>
class EXIF_DECL ExifTagEntryT : public ExifTagEntry
{
    public:

        // Create a ExifTagEntry object with the specified tag number and type.
        //!param: num - Tag number.
        //!param: typ - Field type.
        ExifTagEntryT( exiftag_t num, ttype_t typ )
            : ExifTagEntry( num, typ )
            {}

        // Create a ExifTagEntry object with the specified tag number, type, count and value.
        //!param: num - Tag number.
        //!param: typ - Field type.
        //!param: cnt - Number of values
        //!param: value - Field value
        ExifTagEntryT( exiftag_t num, ttype_t typ, exif_int32 cnt, Type value )
            : ExifTagEntry( num, typ, cnt ), mValue(value)
            {}

        // Copy constructor
        //!param: entry - Source to copy
        ExifTagEntryT(const ExifTagEntryT<Type>& entry)
            : ExifTagEntry(entry.mTagNum, entry.mType, entry.mCount), mValue(entry.getValue())
            {}

        // Destructor
        virtual ~ExifTagEntryT() {}

        // Create a copy of the ExifTagEntry object.
        //!return: Pointer to the copy.
        virtual ExifTagEntry* clone( void ) const
            { return new ExifTagEntryT<Type>( *this ); }

        // Get value.
        //!return: Reference to the value.
        Type& getValue() 
            { return mValue; }
 
        // Get value.
        //!return: Reference to the value.
        const Type& getValue( void ) const
            { return mValue; }

        // Set value.
        //!param: sv - Field value.
        void setValue( const Type& sv )
            { mValue = sv; return; }

        // Print the tag entry.
        //!param: c - Output stream.
        //!return: Output stream.
        virtual std::ostream& print(std::ostream& c = std::cout) const
        {
            c << mTagNum << '\t' << mType << '\t' << mCount << '\t';
            ExifPrintValue(c, mValue);
            return c;
        }

    private:
        Type mValue;

} ;

#ifdef EXIF_EXPLICIT

template class EXIF_DECL ExifTagEntryT<vector<char> >;

template class EXIF_DECL ExifTagEntryT<char>;

template class EXIF_DECL ExifTagEntryT<vector<unsigned char> >;

template class EXIF_DECL ExifTagEntryT<unsigned char>;

template class EXIF_DECL ExifTagEntryT<vector<short> >;

template class EXIF_DECL ExifTagEntryT<short>;

template class EXIF_DECL ExifTagEntryT<vector<unsigned short> >;

template class EXIF_DECL ExifTagEntryT<unsigned short>;

template class EXIF_DECL ExifTagEntryT<vector<int> >;

template class EXIF_DECL ExifTagEntryT<int>;

template class EXIF_DECL ExifTagEntryT<vector<unsigned int> >;

template class EXIF_DECL ExifTagEntryT<unsigned int>;

template class EXIF_DECL ExifTagEntryT<vector<bool> >;

template class EXIF_DECL ExifTagEntryT<bool>;

template class EXIF_DECL ExifTagEntryT<vector<float> >;

template class EXIF_DECL ExifTagEntryT<float>;

template class EXIF_DECL ExifTagEntryT<vector<double> >;

template class EXIF_DECL ExifTagEntryT<double>;

template class EXIF_DECL ExifTagEntryT<vector<std::string> >;

template class EXIF_DECL ExifTagEntryT<std::string>;

#endif

#endif // _EXIF_TAG_ENTRY_H_
