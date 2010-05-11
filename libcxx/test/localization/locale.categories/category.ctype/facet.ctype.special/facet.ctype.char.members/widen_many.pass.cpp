//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <locale>

// template <> class ctype<char>;

// const char* widen(const char* low, const char* high, char* to) const;

#include <locale>
#include <string>
#include <vector>
#include <cassert>

int main()
{
    std::locale l = std::locale::classic();
    {
        typedef std::ctype<char> F;
        const F& f = std::use_facet<F>(l);
        std::string in(" A\x07.a1");
        std::vector<char> v(in.size());

        assert(f.widen(&in[0], in.data() + in.size(), v.data()) == in.data() + in.size());
        assert(v[0] == ' ');
        assert(v[1] == 'A');
        assert(v[2] == '\x07');
        assert(v[3] == '.');
        assert(v[4] == 'a');
        assert(v[5] == '1');
    }
}
