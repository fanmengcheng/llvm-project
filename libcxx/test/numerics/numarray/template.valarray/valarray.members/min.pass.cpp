//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <valarray>

// template<class T> class valarray;

// value_type min() const;

#include <valarray>
#include <cassert>

int main()
{
    {
        typedef double T;
        T a1[] = {1.5, 2.5, -3, 4, 5.5};
        const unsigned N1 = sizeof(a1)/sizeof(a1[0]);
        std::valarray<T> v1(a1, N1);
        assert(v1.min() == -3.0);
    }
    {
        typedef double T;
        std::valarray<T> v1;
        v1.min();
    }
    {
        typedef double T;
        T a1[] = {1.5, 2.5, -3, 4, 5.5};
        const unsigned N1 = sizeof(a1)/sizeof(a1[0]);
        std::valarray<T> v1(a1, N1);
        assert((2*v1).min() == -6.0);
    }
}
