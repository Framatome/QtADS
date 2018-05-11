/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * QtADS Library
 * Copyright (C) 2012-2018   Framatome, Inc.
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

#ifndef QADSARRAY_H
#define QADSARRAY_H

#include "qadsglobal.h"
#include <vector>
#include <QVector>
#include <QString>

/*!
  \brief A template class for supporting PLC variable arrays in Qt.

  \details ARRAYS in TwinCAT can be of any data type and can be 1, 2, or 3 dimensions.
  The template class below is designed to handle these requirements.
  TwinCAT uses C-style arrays (row major order), i.e array[z,y,x] instead
  of array[x,y,z].  Also, TwinCAT uses any arbitary range of integers (defined
  when it is declared) for indices.  This class does not support arbitary
  indices but is always zero indexed.  However the array size and dimension
  will be the same.
*/

template<typename T>
class QTADS_EXPORT QADSARRAY
{

public:
    /*!
        Default Constructor (required for Qt)
    */
    QADSARRAY()
        :m_isBOOL(false), m_x(1), m_y(1), m_z(1)
    {
        m_array = new T[count()];
        Q_ASSERT(m_array != NULL);
        initialize();
    }

    /*!
        1D Constructor
    */
    QADSARRAY(unsigned int x)
        :m_isBOOL(false)
    {
        if(x == 0){ m_x = 1; } else { m_x = x; }
        m_y = 1;
        m_z = 1;
        m_array = new T[count()];
        Q_ASSERT(m_array != NULL);
        initialize();
    }

    /*!
        2D Constructor
    */
    QADSARRAY(unsigned int y, unsigned int x)
        :m_isBOOL(false)
    {
        if(x == 0){ m_x = 1; } else { m_x = x; }
        if(y == 0){ m_y = 1; } else { m_y = y; }
        m_z = 1;
        m_array = new T[count()];
        Q_ASSERT(m_array != NULL);
        initialize();
    }

    /*!
        3D Constructor
    */
    QADSARRAY(unsigned int z, unsigned int y, unsigned int x)
        :m_isBOOL(false)
    {
        if(x == 0){ m_x = 1; } else { m_x = x; }
        if(y == 0){ m_y = 1; } else { m_y = y; }
        if(z == 0){ m_z = 1; } else { m_z = z; }
        m_array = new T[count()];
        Q_ASSERT(m_array != NULL);
        initialize();
    }

    /*!
        Copy Constructor
        Must receive a reference to prevent infinite recursion.
    */
    QADSARRAY(const QADSARRAY &init)
        :m_isBOOL(false), m_x(init.m_x), m_y(init.m_y), m_z(init.m_z)
    {
        m_array = new T[count()];
        Q_ASSERT(m_array != NULL);
        copy(init.m_array,init.m_x*init.m_y*init.m_z);
    }

    /*!
        Copy Constructor using std::vector
    */
    QADSARRAY(const std::vector<T> &init)
       :m_isBOOL(false)
    {
        if(init.size() == 0){ m_x = 1; } else { m_x = (unsigned int)init.size(); }
        m_y = 1;
        m_z = 1;
        m_array = new T[count()];
        Q_ASSERT(m_array != NULL);
        for(unsigned int ii=0; ii<m_x; ii++)
        {
           m_array[ii] = init[ii];
        }
    }

    /*!
        Copy Constructor using QVector
    */
    QADSARRAY(const QVector<T> &init)
       :m_isBOOL(false)
    {
        if(init.size() == 0){ m_x = 1; } else { m_x = (unsigned int)init.size(); }
        m_y = 1;
        m_z = 1;
        m_array = new T[count()];
        Q_ASSERT(m_array != NULL);
        for(unsigned int ii=0; ii<m_x; ii++)
        {
           m_array[ii] = init[ii];
        }
    }

    ~QADSARRAY()
    { delete [] m_array; }

    /*!
        Size of x dimension.
    */
    unsigned int x() const
    { return m_x; }

    /*!
    Size of y dimension.
    */
    unsigned int y() const
    { return m_y; }

    /*!
        Size of z dimension.
    */
    unsigned int z() const
    { return m_z; }

    /*!
        Returns a pointer to the array.
    */
    T* array()
    { return m_array; }

    /*!
        Returns a const pointer to the array.
    */
    const T* array() const
    { return m_array; }

    /*!
        Returns the array formated as a QString.
    */
    QString arrayAsString() const
    {
        QString out("");

        for(unsigned int k=0; k < m_z; k++)
        {
            if( k == 0 ){out += '(';}
            for(unsigned int j=0; j < m_y; j++)
            {
                if( j == 0 ){out += '{';}
                for(unsigned int i=0; i < m_x; i++)
                {
                    if( i == 0 ){ out += '['; }
                    if( m_isBOOL )
                    {
                        out += (*(m_array + i + (j*m_x) + (k*m_x*m_y)))?"true":"false";
                    }
                    else
                    {
                        out += QString::number(*(m_array + i + (j*m_x) + (k*m_x*m_y)));
                    }
                    if( i < (m_x-1) ){out += ", ";}
                }
                out += ']';
                if( ( m_y > 1 ) && (j < (m_y-1)) ){out += ", ";}
            }
            out += '}';
            if( ( m_z > 1 ) && (k < (m_z-1)) ){out += ", ";}
        }
        out += ')';

        return out;
    }

    /*!
        Returns the number of elements in the array.
    */
    unsigned int count() const
    { return (m_x*m_y*m_z); }

    /*!
        Returns the total size of the array in bytes.
    */
    size_t sizeInBytes() const
    { return (count()*sizeof(T)); }

    /*!
        Copy the array to the pointer "input".
        Returns false if the copy cannot performed.
    */
    bool copy(const T *input, unsigned int num)
    {
        if( num != count() )
        {
            return false;
        }
        for(unsigned int i=0;i<num;i++)
        {
            *(m_array+i) = *(input+i);
        }
        return true;
    }

    /*!
        Overloaded assignment operator
        const return avoids: ( a1 = a2 ) = a3
    */
    const QADSARRAY &operator=(const QADSARRAY &right)
    {
        // Check for self assignment.
        if( &right != this )
        {
            // For arrays of different sizes,
            // deallocate original left side
            // array, then allocate new left
            // size array.
            unsigned int rightCount = right.m_x*right.m_y*right.m_z;
            if( count() != rightCount )
            {
                delete [] m_array;
                m_x = right.m_x;
                m_y = right.m_y;
                m_z = right.m_z;
                m_array = new T[count()];
                Q_ASSERT(m_array != NULL);
            }
            copy(right.m_array,rightCount);
        }
        return *this; // enables x = y = z;
    }

    /*!
        Overloaded 1D subscript operator
    */
    T &operator[](unsigned int index)
    {
        Q_ASSERT(index<count());
        return *(m_array+index);
    }

    /*!
        Overloaded 1D subscript operator
    */
    const T &operator[](unsigned int index) const
    {
        Q_ASSERT(index<(m_x*m_y*m_z));
        return *(m_array+index);
    }

    /*!
        Overloaded 3D subscript operator
    */
    T &operator()(unsigned int z, unsigned int y, unsigned int x)
    {
        Q_ASSERT(z<m_z);
        Q_ASSERT(y<m_y);
        Q_ASSERT(x<m_x);
        return *(m_array + x + (y*m_x) + (z*m_x*m_y));
    }

    /*!
        Overloaded 3D subscript operator
    */
    const T &operator()(unsigned int z, unsigned int y, unsigned int x) const
    {
        Q_ASSERT(z<m_z);
        Q_ASSERT(y<m_y);
        Q_ASSERT(x<m_x);
        return *(m_array + x + (y*m_x) + (z*m_x*m_y));
    }

    /*!
        Overloaded 2D subscript operator
    */
    T &operator()(unsigned int y, unsigned int x)
    {
        Q_ASSERT(y<m_y);
        Q_ASSERT(x<m_x);
        return *(m_array + x + (y*m_x));
    }

    /*!
        Overloaded 2D subscript operator
    */
    const T &operator()(unsigned int y, unsigned int x) const
    {
        Q_ASSERT(y<m_y);
        Q_ASSERT(x<m_x);
        return *(m_array + x + (y*m_x));
    }

    /*!
        Compare equal operator
    */
    bool operator==(const QADSARRAY &a) const
    {
        unsigned int tempCount = m_x*m_y*m_z;
        if( tempCount != (a.m_x*a.m_y*a.m_z) )
        {
            return false; // different size of arrays.
        }
        for(unsigned int i=0; i < tempCount; i++)
        {
            if( *(m_array+i) != *(a.m_array+i) )
            {
                return false;
            }
        }
        return true;
    }

    /*!
        Compare not equal operator.
    */
    bool operator!=(const QADSARRAY &a) const
    { return ! (*this == a); }

protected:
    bool m_isBOOL;

private:
    void initialize()
    {
        for (unsigned int i=0;i<count();i++)
        {
            *(m_array + i) = (char)0;
        }
    }

    unsigned int m_x, m_y, m_z;
    T *m_array;
};

#endif // QADSARRAY_H
