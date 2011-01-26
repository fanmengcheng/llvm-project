//===-- lldb_InstructionUtils.h ---------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef lldb_InstructionUtils_h_
#define lldb_InstructionUtils_h_

// Common utilities for manipulating instruction bit fields.

namespace lldb_private {

static inline uint32_t
Bits32 (const uint32_t value, const uint32_t msbit, const uint32_t lsbit)
{
    assert(msbit < 32 && lsbit <= msbit);
    return (value >> lsbit) & ((1u << (msbit - lsbit + 1)) - 1);
}

// Create a mask that starts at bit zero and includes "bit"
static inline uint64_t
MaskUpToBit (const uint64_t bit)
{
    return (1ull << (bit + 1ull)) - 1ull;
}

// Returns an integer result equal to the number of bits of x that are ones.
static inline uint32_t
BitCount (uint64_t x)
{
    // c accumulates the total bits set in x
    uint32_t c;
    for (c = 0; x; ++c)
    {
        x &= x - 1; // clear the least significant bit set
    }
    return c;
}

static inline bool
BitIsSet (const uint64_t value, const uint64_t bit)
{
    return (value & (1ull << bit)) != 0;
}

static inline bool
BitIsClear (const uint64_t value, const uint64_t bit)
{
    return (value & (1ull << bit)) == 0;
}

static inline uint64_t
UnsignedBits (const uint64_t value, const uint64_t msbit, const uint64_t lsbit)
{
    uint64_t result = value >> lsbit;
    result &= MaskUpToBit (msbit - lsbit);
    return result;
}

static inline int64_t
SignedBits (const uint64_t value, const uint64_t msbit, const uint64_t lsbit)
{
    uint64_t result = UnsignedBits (value, msbit, lsbit);
    if (BitIsSet(value, msbit))
    {
        // Sign extend
        result |= ~MaskUpToBit (msbit - lsbit);
    }
    return result;
}

}   // namespace lldb_private

#endif  // lldb_InstructionUtils_h_
