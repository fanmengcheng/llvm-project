//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <future>

// class promise<R>

// void promise<R&>::set_value(R& r);

#include <future>
#include <cassert>

int main()
{
    {
        typedef int& T;
        int i = 3;
        std::promise<T> p;
        std::future<T> f = p.get_future();
        p.set_value(i);
        assert(f.get() == 3);
        ++i;
        f = p.get_future();
        assert(f.get() == 4);
        try
        {
            p.set_value(i);
            assert(false);
        }
        catch (const std::future_error& e)
        {
            assert(e.code() == make_error_code(std::future_errc::promise_already_satisfied));
        }
    }
}
