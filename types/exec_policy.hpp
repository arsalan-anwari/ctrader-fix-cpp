#pragma once

#include "../settings.hpp"
#include "../tools/numbers.hpp"

namespace ctrader {

	enum class exec_policy : u8 {
		scalar, sse, avx, avx2, avx512, sve, sve2, co_processor, gpu 
	};

	consteval exec_policy policy_from_max_size_epi8(unsigned size) {
		if (settings::AVX_AVAILABLE) {
			if (size <= 16U) { return exec_policy::avx; }

			if (size > 16U && size <= 32U) {
				return settings::AVX2_AVAILABLE ? exec_policy::avx2 : exec_policy::avx;
			}

			if (size > 32U) {
				return settings::AVX512_AVAILABLE ? 
					exec_policy::avx512 : 
					settings::AVX2_AVAILABLE ? exec_policy::avx2 : exec_policy::avx;
			}
		}

		if (settings::SSE_AVAILABLE && size >= 16U) { return exec_policy::sse; }
		return exec_policy::scalar;
	}

	consteval unsigned permutations_from_stride_epi8(const unsigned size, const unsigned stride) {
		float frac = (size / (stride + 0.01f));
		i32 perms = ceil(frac);
		return perms > 0 ? perms : 1;
	}

	consteval u32 get_maximum_stride_epi8() {
		if (settings::AVX_AVAILABLE) {
			if (settings::AVX2_AVAILABLE) { return 32U; }
			if (settings::AVX512_AVAILABLE) { return 64; }
			return 16U;
		}
		return settings::SSE_AVAILABLE ? 16U : 0U;
	}

	
}