/* 
	NOTE: <stdfloat>
	clang status: P1467R9 - No - https://clang.llvm.org/cxx_status.html
	MSVC status: No
	APPLE/clang status: No
*/ 

// Example usage:
/*
	using namespace eta;

	auto float16  = 3.14159_f16;
	auto float32  = 2.71828_f32;
	auto float128 = 0.57721_f128;

	// Explicitly pass std::cout to satisfy GCC 14's partial C++23 implementation
	std::println(std::cout, "f16 value:   {}", float16); 
	std::println(std::cout, "f32 value:   {}", float32);
	std::println(std::cout, "f128 value:  {}", float128);

*/

#pragma once
#include <cstdint>
#include <ostream>

// ============================================================
//  eta — Extended Type Aliases
//  Portable numeric type aliases with fallbacks for platforms
//  missing C++23 <stdfloat> (MSVC, Apple Clang, older LLVM).
//
//  Usage:
//      #include "eta.hxx"
//      using namespace eta;   // or qualify: eta::f32, eta::u64
//
//  Reuse:
//      namespace myprog { using namespace eta; }
// ============================================================

// ── Float availability flags ─────────────────────────────────
// ETA_FLOAT16_NATIVE  — 1 if f16 is a true 16-bit type, 0 if widened to f32
// ETA_FLOAT128_NATIVE — 1 if f128 is a true 128-bit type, 0 if widened to f64
// ETA_FLOAT32_NATIVE  — always 1
// ETA_FLOAT64_NATIVE  — always 1

#if __has_include(<stdfloat>)
	#include <stdfloat>
	#define ETA_FLOAT16_NATIVE  1
	#define ETA_FLOAT32_NATIVE  1
	#define ETA_FLOAT64_NATIVE  1
	#define ETA_FLOAT128_NATIVE 1

	namespace eta {
		using float16_t  = std::float16_t;
		using float32_t  = std::float32_t;
		using float64_t  = std::float64_t;
		using float128_t = std::float128_t;
	}
#else
	// ── 16-bit ───────────────────────────────────────────────
	#if defined(__FLT16_MANT_DIG__)
		#define ETA_FLOAT16_NATIVE 1
	#else
		#define ETA_FLOAT16_NATIVE 0
	#endif

	// ── 128-bit ──────────────────────────────────────────────
	#if defined(__SIZEOF_FLOAT128__)
		#define ETA_FLOAT128_NATIVE 1
	#else
		#define ETA_FLOAT128_NATIVE 0
	#endif

	#define ETA_FLOAT32_NATIVE 1
	#define ETA_FLOAT64_NATIVE 1

	namespace eta {
		using float32_t = float;
		using float64_t = double;

		#if ETA_FLOAT16_NATIVE
			using float16_t = _Float16;
		#else
			// Fallback: MSVC, Intel Mac, unconfigured x86
			// f16 is 32-bit on this platform — not suitable for
			// packed storage, serialization, or ML inference
			using float16_t = float;
			static_assert(sizeof(float16_t) == 4, "eta: float16 fallback active — not 16-bit");
		#endif

		#if ETA_FLOAT128_NATIVE
			using float128_t = __float128;
		#else
			// Fallback: MSVC, Apple, SunOS/illumos
			// f128 is 64-bit on this platform — quad precision unavailable
			using float128_t = double;
			static_assert(sizeof(float128_t) == 8, "eta: float128 fallback active — not 64-bit");
		#endif
	} // namespace eta
#endif // __has_include(<stdfloat>)

// ============================================================
//  eta — aliases, literals, operators
// ============================================================
namespace eta {

// ── Integer aliases ──────────────────────────────────────────
using u8    = std::uint8_t;
using u16   = std::uint16_t;
using u32   = std::uint32_t;
using u64   = std::uint64_t;

using i8    = std::int8_t;
using i16   = std::int16_t;
using i32   = std::int32_t;
using i64   = std::int64_t;

using usize = std::size_t;
using isize = std::ptrdiff_t;

using uptr  = std::uintptr_t;
using iptr  = std::intptr_t;

// ── Float aliases ────────────────────────────────────────────
// Check ETA_FLOAT16_NATIVE / ETA_FLOAT128_NATIVE before using
// f16 or f128 for packed storage, serialization, or precision-
// sensitive work — they may be widened fallbacks on this platform
using f16  = float16_t;
using f32  = float32_t;
using f64  = float64_t;
using f128 = float128_t;
// using b16 = std::bfloat16_t; // LLM weights — no fallback exists

// ── Integer literal operators ────────────────────────────────
constexpr u8  operator""_u8 (unsigned long long v) { return static_cast<u8 >(v); }
constexpr u16 operator""_u16(unsigned long long v) { return static_cast<u16>(v); }
constexpr u32 operator""_u32(unsigned long long v) { return static_cast<u32>(v); }
constexpr u64 operator""_u64(unsigned long long v) { return static_cast<u64>(v); }
constexpr i8  operator""_i8 (unsigned long long v) { return static_cast<i8 >(v); }
constexpr i16 operator""_i16(unsigned long long v) { return static_cast<i16>(v); }
constexpr i32 operator""_i32(unsigned long long v) { return static_cast<i32>(v); }
constexpr i64 operator""_i64(unsigned long long v) { return static_cast<i64>(v); }

// ── Float literal operators ──────────────────────────────────
// _f128 precision is capped at long double (~80-bit on x86,
// ~64-bit on MSVC/ARM) — full quad precision requires string
// parsing and is not supported here
constexpr f16  operator""_f16 (long double v) { return static_cast<f16 >(v); }
constexpr f32  operator""_f32 (long double v) { return static_cast<f32 >(v); }
constexpr f64  operator""_f64 (long double v) { return static_cast<f64 >(v); }
constexpr f128 operator""_f128(long double v) { return static_cast<f128>(v); }

// ── ostream overrides ────────────────────────────────────────
// u8/i8:  suppress char interpretation
inline std::ostream& operator<<(std::ostream& os, u8  v) { return os << static_cast<int>(v); }
inline std::ostream& operator<<(std::ostream& os, i8  v) { return os << static_cast<int>(v); }
// f16: no stdlib ostream overload exists — promote to f32 for output
//      no-op when f16 is already a float fallback
inline std::ostream& operator<<(std::ostream& os, f16 v) { return os << static_cast<f32>(v); }

} // namespace eta