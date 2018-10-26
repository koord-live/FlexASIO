#include "asio.h"

#include <cassert>
#include <cstring>
#include <utility>

#include "..\ASIOSDK2.3.1\common\asio.h"

namespace flexasio {
	namespace {
		enum class Endianness { LITTLE, BIG };
		Endianness GetEndianness() {
			uint16_t uint16 = 1;
			char bytes[sizeof uint16];
			memcpy(bytes, &uint16, sizeof uint16);
			if (bytes[0] == 1 && bytes[1] == 0) {
				return Endianness::LITTLE;
			}
			if (bytes[0] == 0 && bytes[1] == 1) {
				return Endianness::BIG;
			}
			abort();
		}
	}

	// Somewhat surprisingly, ASIO 64-bit integer data types store the most significant 32 bits half *first*, even on little endian architectures. See the ASIO SDK documentation.
	// On x86, which is little endian, that means we can't simply represent the value as an int64_t - we need to swap the two halves first.

	template <typename ASIOInt64> int64_t ASIOToInt64(ASIOInt64 asioInt64) {
		if (GetEndianness() == Endianness::LITTLE) std::swap(asioInt64.hi, asioInt64.lo);
		int64_t result;
		static_assert(sizeof asioInt64 == sizeof result);
		memcpy(&result, &asioInt64, sizeof result);
		return result;
	}
	template int64_t ASIOToInt64<ASIOTimeStamp>(ASIOTimeStamp);
	template int64_t ASIOToInt64<ASIOSamples>(ASIOSamples);

	template <typename ASIOInt64> ASIOInt64 Int64ToASIO(int64_t int64) {
		ASIOInt64 result;
		static_assert(sizeof int64 == sizeof result);
		memcpy(&result, &int64, sizeof result);
		if (GetEndianness() == Endianness::LITTLE) std::swap(result.hi, result.lo);
		return result;
	}
	template ASIOTimeStamp Int64ToASIO<ASIOTimeStamp>(int64_t);
	template ASIOSamples Int64ToASIO<ASIOSamples>(int64_t);
}