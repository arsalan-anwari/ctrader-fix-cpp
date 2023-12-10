#pragma once

#include "../debug_settings.hpp"
#include "../tools/math.hpp"

namespace ctrader {

	enum class exec_policy : u8 {
		scalar, sse, avx, avx2, avx512, sve, sve2, co_processor, gpu 
	};

	consteval unsigned get_maximum_stride_epi8() {
		if (debug_settings::AVX_AVAILABLE) {
			if (debug_settings::AVX2_AVAILABLE) { return 32U; }
			if (debug_settings::AVX512_AVAILABLE) { return 64; }
			return 16U;
		}
		return debug_settings::SSE_AVAILABLE ? 16U : 1U;
	}

	consteval unsigned stride_from_policy_epi8(const exec_policy policy) {
		switch (policy) {
		case exec_policy::sse:
		case exec_policy::avx:
			return 16U;
		case exec_policy::avx2:
			return 32U;
		case exec_policy::avx512:
			return 64U;
		default:
			return 1U;
		}
	}

	consteval exec_policy policy_from_max_size_epi8(unsigned size) {
		if (debug_settings::AVX_AVAILABLE) {
			if (size <= 16U) { return exec_policy::avx; }

			if (size > 16U && size <= 32U) {
				return debug_settings::AVX2_AVAILABLE ? exec_policy::avx2 : exec_policy::avx;
			}

			if (size > 32U) {
				return debug_settings::AVX512_AVAILABLE ?
					exec_policy::avx512 : 
					debug_settings::AVX2_AVAILABLE ? exec_policy::avx2 : exec_policy::avx;
			}
		}

		if (debug_settings::SSE_AVAILABLE && size >= 16U) { return exec_policy::sse; }
		return exec_policy::scalar;
	}

	consteval exec_policy policy_from_min_size_epi8(unsigned size) {
		if (debug_settings::AVX_AVAILABLE) {
			if (size >= 16U && size < 32U) { return exec_policy::avx; }

			if (size >= 32U && size < 64U) {
				return debug_settings::AVX2_AVAILABLE ? exec_policy::avx2 : exec_policy::avx;
			}

			if (size > 64U) {
				return debug_settings::AVX512_AVAILABLE ?
					exec_policy::avx512 :
					debug_settings::AVX2_AVAILABLE ? exec_policy::avx2 : exec_policy::avx;
			}
		}

		if (debug_settings::SSE_AVAILABLE && size >= 16U) { return exec_policy::sse; }
		return exec_policy::scalar;
	}

	consteval unsigned unaligned_size_from_stride_epi8(const unsigned size, const exec_policy policy) {
		return size % stride_from_policy_epi8(policy);
	}

	consteval unsigned permutations_from_stride_epi8(const unsigned size, const unsigned stride) {
		float frac = (size / (stride + 0.01f));
		i32 perms = ceil(frac);
		return perms > 0 ? perms : 1;
	}


	
}