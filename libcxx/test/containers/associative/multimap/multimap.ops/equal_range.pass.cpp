//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <map>

// class multimap

// pair<iterator, iterator>             equal_range(const key_type& k);
// pair<const_iterator, const_iterator> equal_range(const key_type& k) const;

#include <map>
#include <cassert>

int main()
{
    typedef std::pair<const int, double> V;
    typedef std::multimap<int, double> M;
    {
        typedef std::pair<M::iterator, M::iterator> R;
        V ar[] =
        {
            V(5, 1),
            V(5, 2),
            V(5, 3),
            V(7, 1),
            V(7, 2),
            V(7, 3),
            V(9, 1),
            V(9, 2),
            V(9, 3)
        };
        M m(ar, ar+sizeof(ar)/sizeof(ar[0]));
        R r = m.equal_range(4);
        assert(r.first == m.begin());
        assert(r.second == m.begin());
        r = m.equal_range(5);
        assert(r.first == m.begin());
        assert(r.second == next(m.begin(), 3));
        r = m.equal_range(6);
        assert(r.first == next(m.begin(), 3));
        assert(r.second == next(m.begin(), 3));
        r = m.equal_range(7);
        assert(r.first == next(m.begin(), 3));
        assert(r.second == next(m.begin(), 6));
        r = m.equal_range(8);
        assert(r.first == next(m.begin(), 6));
        assert(r.second == next(m.begin(), 6));
        r = m.equal_range(9);
        assert(r.first == next(m.begin(), 6));
        assert(r.second == next(m.begin(), 9));
        r = m.equal_range(10);
        assert(r.first == m.end());
        assert(r.second == m.end());
    }
    {
        typedef std::pair<M::const_iterator, M::const_iterator> R;
        V ar[] =
        {
            V(5, 1),
            V(5, 2),
            V(5, 3),
            V(7, 1),
            V(7, 2),
            V(7, 3),
            V(9, 1),
            V(9, 2),
            V(9, 3)
        };
        const M m(ar, ar+sizeof(ar)/sizeof(ar[0]));
        R r = m.equal_range(4);
        assert(r.first == m.begin());
        assert(r.second == m.begin());
        r = m.equal_range(5);
        assert(r.first == m.begin());
        assert(r.second == next(m.begin(), 3));
        r = m.equal_range(6);
        assert(r.first == next(m.begin(), 3));
        assert(r.second == next(m.begin(), 3));
        r = m.equal_range(7);
        assert(r.first == next(m.begin(), 3));
        assert(r.second == next(m.begin(), 6));
        r = m.equal_range(8);
        assert(r.first == next(m.begin(), 6));
        assert(r.second == next(m.begin(), 6));
        r = m.equal_range(9);
        assert(r.first == next(m.begin(), 6));
        assert(r.second == next(m.begin(), 9));
        r = m.equal_range(10);
        assert(r.first == m.end());
        assert(r.second == m.end());
    }
}
