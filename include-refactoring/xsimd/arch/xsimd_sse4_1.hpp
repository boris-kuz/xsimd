#ifndef XSIMD_SSE4_1_HPP
#define XSIMD_SSE4_1_HPP

#include "../types/xsimd_sse4_1_register.hpp"

#include <smmintrin.h>

namespace xsimd {

  namespace kernel {
    using namespace types;
    // ceil
    template<class A> batch<float, A> ceil(batch<float, A> const& self, requires<sse4_1>) {
      return _mm_round_ps(self, _MM_FROUND_CEIL);
    }
    template<class A> batch<double, A> ceil(batch<double, A> const& self, requires<sse4_1>) {
      return _mm_round_pd(self, _MM_FROUND_CEIL);
    }

    // eq
    template<class A, class T, class=typename std::enable_if<std::is_integral<T>::value, void>::type>
    batch_bool<T, A> eq(batch<T, A> const& self, batch<T, A> const& other, requires<sse4_1>) {
      switch(sizeof(T)) {
        case 8: return _mm_cmpeq_epi64(self, other);
        default: return eq(self, other, ssse3{});
      }
    }


    // floor
    template<class A> batch<float, A> floor(batch<float, A> const& self, requires<sse4_1>) {
      return _mm_round_ps(self, _MM_FROUND_FLOOR);
    }
    template<class A> batch<double, A> floor(batch<double, A> const& self, requires<sse4_1>) {
      return _mm_round_pd(self, _MM_FROUND_FLOOR);
    }

    // mul
    template<class A, class T, class=typename std::enable_if<std::is_integral<T>::value, void>::type>
    batch<T, A> mul(batch<T, A> const& self, batch<T, A> const& other, requires<sse4_1>) {
      switch(sizeof(T)) {
        case 1: return _mm_or_si128(
                            _mm_and_si128(_mm_mullo_epi16(self, other), _mm_srli_epi16(_mm_cmpeq_epi8(self, self), 8)),
                                    _mm_slli_epi16(_mm_mullo_epi16(_mm_srli_epi16(self, 8), _mm_srli_epi16(other, 8)), 8)
                                        );
        case 2: return _mm_mullo_epi16(self, other);
        case 4: return _mm_mullo_epi32(self, other);
        case 8:
        return _mm_add_epi64(
            _mm_mul_epu32(self, other),
            _mm_slli_epi64(
                _mm_add_epi64(
                    _mm_mul_epu32(other, _mm_shuffle_epi32(self, _MM_SHUFFLE(2, 3, 0, 1))),
                    _mm_mul_epu32(self, _mm_shuffle_epi32(other, _MM_SHUFFLE(2, 3, 0, 1)))),
                32));
              default: assert(false && "unsupported arch/op combination"); return {};
      }
    }

    // nearbyint
    template<class A> batch<float, A> nearbyint(batch<float, A> const& self, requires<sse4_1>) {
      return _mm_round_ps(self, _MM_FROUND_TO_NEAREST_INT);
    }
    template<class A> batch<double, A> nearbyint(batch<double, A> const& self, requires<sse4_1>) {
      return _mm_round_pd(self, _MM_FROUND_TO_NEAREST_INT);
    }

    // select
    namespace detail {
      template<class T>
      constexpr T interleave(T const &cond) {
        return (((cond * 0x0101010101010101ULL & 0x8040201008040201ULL) * 0x0102040810204081ULL >> 49) & 0x5555) |
               (((cond * 0x0101010101010101ULL & 0x8040201008040201ULL) * 0x0102040810204081ULL >> 48) & 0xAAAA);
      }
    }

    template<class A, class T, class=typename std::enable_if<std::is_integral<T>::value, void>::type>
    batch<T, A> select(batch_bool<T, A> const& cond, batch<T, A> const& true_br, batch<T, A> const& false_br, requires<sse4_1>) {
      return _mm_blendv_epi8(false_br, true_br, cond);
    }
    template<class A> batch<float, A> select(batch_bool<float, A> const& cond, batch<float, A> const& true_br, batch<float, A> const& false_br, requires<sse4_1>) {
      return _mm_blendv_ps(false_br, true_br, cond);
    }
    template<class A> batch<double, A> select(batch_bool<double, A> const& cond, batch<double, A> const& true_br, batch<double, A> const& false_br, requires<sse4_1>) {
      return _mm_blendv_pd(false_br, true_br, cond);
    }

    template<class A, class T, bool... Values, class=typename std::enable_if<std::is_integral<T>::value, void>::type>
    batch<T, A> select(batch_bool_constant<batch<T, A>, Values...> const&, batch<T, A> const& true_br, batch<T, A> const& false_br, requires<sse4_1>) {
      constexpr int mask = batch_bool_constant<batch<T, A>, Values...>::mask();
      switch(sizeof(T)) {
        case 1: return _mm_blend_epi8(false_br, true_br, mask);
        case 1: return _mm_blend_epi16(false_br, true_br, mask);
        case 1: return _mm_blend_epi32(false_br, true_br, mask);
        case 8: {
          constexpr int imask = detail::interleave(mask);
          return _mm_blend_epi32(false_br, true_br, imask);
        }
        default: assert(false && "unsupported arch/op combination"); return {};
      }
    }
    template<class A, bool... Values> batch<float, A> select(batch_bool_constant<batch<float, A>, Values...> const& , batch<float, A> const& true_br, batch<float, A> const& false_br, requires<sse4_1>) {
      constexpr int mask = batch_bool_constant<batch<float, A>, Values...>::mask();
      return _mm_blend_ps(false_br, true_br, mask);
    }
    template<class A, bool... Values> batch<double, A> select(batch_bool_constant<batch<double, A>, Values...> const& , batch<double, A> const& true_br, batch<double, A> const& false_br, requires<sse4_1>) {
      constexpr int mask = batch_bool_constant<batch<double, A>, Values...>::mask();
      return _mm_blend_pd(false_br, true_br, mask);
    }


    // trunc
    template<class A> batch<float, A> trunc(batch<float, A> const& self, requires<sse4_1>) {
      return _mm_round_ps(self, _MM_FROUND_TO_ZERO);
    }
    template<class A> batch<double, A> trunc(batch<double, A> const& self, requires<sse4_1>) {
      return _mm_round_pd(self, _MM_FROUND_TO_ZERO);
    }


  }

}

#endif

