#pragma once
#define JUCE_64BIT 1
//==============================================================================
// Definitions for the int8, int16, int32, int64 and pointer_sized_int types.
/** A platform-independent 8-bit signed integer type. */
using int8 = signed char;
/** A platform-independent 8-bit unsigned integer type. */
using uint8 = unsigned char;
/** A platform-independent 16-bit signed integer type. */
using int16 = signed short;
/** A platform-independent 16-bit unsigned integer type. */
using uint16 = unsigned short;
/** A platform-independent 32-bit signed integer type. */
using int32 = signed int;
/** A platform-independent 32-bit unsigned integer type. */
using uint32 = unsigned int;
/** A platform-independent 64-bit integer type. */
using int64 = __int64;
/** A platform-independent 64-bit unsigned integer type. */
using uint64 = unsigned __int64;

#if JUCE_64BIT
/** A signed integer type that's guaranteed to be large enough to hold a pointer without truncating it. */
using pointer_sized_int = int64;
/** An unsigned integer type that's guaranteed to be large enough to hold a pointer without truncating it. */
using pointer_sized_uint = uint64;
#elif JUCE_MSVC
/** A signed integer type that's guaranteed to be large enough to hold a pointer without truncating it. */
using pointer_sized_int = _W64 int;
/** An unsigned integer type that's guaranteed to be large enough to hold a pointer without truncating it. */
using pointer_sized_uint = _W64 unsigned int;
#else
/** A signed integer type that's guaranteed to be large enough to hold a pointer without truncating it. */
using pointer_sized_int = int;
/** An unsigned integer type that's guaranteed to be large enough to hold a pointer without truncating it. */
using pointer_sized_uint = unsigned int;
#endif
/** 返回两个数的较小一个 */
template <typename Type>
Type jmin(Type a, Type b) { return b < a ? b : a; }
/** Returns the smaller of three values. */
template <typename Type>
 Type jmin(Type a, Type b, Type c) { return b < a ? (c < b ? c : b) : (c < a ? c : a); }

/** Returns the smaller of four values. */
template <typename Type>
 Type jmin(Type a, Type b, Type c, Type d) { return jmin(a, jmin(b, c, d)); }
/** 返回两个数的较大一个 */
template <typename Type>
Type jmax(Type a, Type b) { return a < b ? b : a; }
/** Returns the larger of three values. */
template <typename Type>
 Type jmax(Type a, Type b, Type c) { return a < b ? (b < c ? c : b) : (a < c ? c : a); }

/** Returns the larger of four values. */
template <typename Type>
 Type jmax(Type a, Type b, Type c, Type d) { return jmax(a, jmax(b, c, d)); }
template <typename FloatType>
struct MathConstants
{
    /** A predefined value for Pi */
    static constexpr FloatType pi = static_cast<FloatType> (3.141592653589793238L);

    /** A predefined value for 2 * Pi */
    static constexpr FloatType twoPi = static_cast<FloatType> (2 * 3.141592653589793238L);

    /** A predefined value for Pi / 2 */
    static constexpr FloatType halfPi = static_cast<FloatType> (3.141592653589793238L / 2);

    /** A predefined value for Euler's number */
    static constexpr FloatType euler = static_cast<FloatType> (2.71828182845904523536L);

    /** A predefined value for sqrt(2) */
    static constexpr FloatType sqrt2 = static_cast<FloatType> (1.4142135623730950488L);
};

/*
快速浮点到整型转换。
转换为最近的整型，而非向下取整。

*/
template <typename FloatType>
int roundToInt(const FloatType value) noexcept
{
#ifdef __INTEL_COMPILER
#pragma float_control (precise,on,push)
#endif
	union { int asInt[2]; double asDouble; }n;
	n.asDouble = ((double)value) + 6755399441055744.0;
#if BIG_ENDIAN
	return n.asInt[1];
#else
	return n.asInt[0];
#endif
}
