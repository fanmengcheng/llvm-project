//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <unordered_set>

// template <class Value, class Hash = hash<Value>, class Pred = equal_to<Value>,
//           class Alloc = allocator<Value>>
// class unordered_set

// pair<iterator, bool> insert(value_type&& x);

#include <unordered_set>
#include <cassert>

#include "../../MoveOnly.h"

int main()
{
    {
        typedef std::unordered_set<double> C;
        typedef std::pair<C::iterator, bool> R;
        typedef double P;
        C c;
        R r = c.insert(P(3.5));
        assert(c.size() == 1);
        assert(*r.first == 3.5);
        assert(r.second);

        r = c.insert(P(3.5));
        assert(c.size() == 1);
        assert(*r.first == 3.5);
        assert(!r.second);

        r = c.insert(P(4.5));
        assert(c.size() == 2);
        assert(*r.first == 4.5);
        assert(r.second);

        r = c.insert(P(5.5));
        assert(c.size() == 3);
        assert(*r.first == 5.5);
        assert(r.second);
    }
#ifdef _LIBCPP_MOVE
    {
        typedef std::unordered_set<MoveOnly> C;
        typedef std::pair<C::iterator, bool> R;
        typedef MoveOnly P;
        C c;
        R r = c.insert(P(3));
        assert(c.size() == 1);
        assert(*r.first == 3);
        assert(r.second);

        r = c.insert(P(3));
        assert(c.size() == 1);
        assert(*r.first == 3);
        assert(!r.second);

        r = c.insert(P(4));
        assert(c.size() == 2);
        assert(*r.first == 4);
        assert(r.second);

        r = c.insert(P(5));
        assert(c.size() == 3);
        assert(*r.first == 5);
        assert(r.second);
    }
#endif
}
