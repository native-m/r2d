#pragma once

#include <cstdint>
#include <cstdlib>
#include <emmintrin.h>
#include <memory>
#include <xmmintrin.h>

#ifdef NDEBUG
#if defined(_MSC_VER)
#define R2D_UNREACHABLE() __assume(0)
#define R2D_FORCEINLINE __forceinline
#else
#define R2D_UNREACHABLE() __builtin_unreachable()
#define R2D_FORCEINLINE
#endif
#else
#define R2D_UNREACHABLE() assert(0 && "Unreachable")
#define R2D_FORCEINLINE inline
#endif

#define R2D_CHECK(x) assert(x)

#define R2D_COLOR_CHANNEL_4(c0, c1, c2, c3)                                                        \
    ((uint32_t)(c0) | ((uint32_t)(c1) << 8) | ((uint32_t)(c2) << 16) | ((uint32_t)(c3) << 24))

#define R2D_CLIP_X0 1
#define R2D_CLIP_Y0 2
#define R2D_CLIP_X1 4
#define R2D_CLIP_Y1 8

using R2DFixed32 = int32_t;
using R2DPixel = uint32_t;
using R2DColor8 = uint32_t;

struct R2DColorBitPos {
    uint32_t r;
    uint32_t g;
    uint32_t b;
    uint32_t a;
};

struct R2DBox {
    float x0;
    float y0;
    float x1;
    float y1;
};

template <typename T>
struct R2DVector {
    T* data_{};
    size_t size_{};
    size_t capacity_{};

    inline R2DVector() {}

    inline ~R2DVector() {
        if (data_)
            std::free(data_);
    }

    inline void resize(size_t capacity)
    {

    }

    inline void reserve(size_t capacity)
    {

    }
};

template <typename T>
static T r2d_max(T a, T b) {
    return (a > b) ? a : b;
}

template <typename T>
static T r2d_min(T a, T b) {
    return (b > a) ? b : a;
}

R2D_FORCEINLINE
static R2DFixed32 r2d_iround(const double x) {
    __m128d sd = _mm_load_sd(&x);
    __m128d sign = _mm_set_sd(-0.0);
    __m128d msk = _mm_cmple_sd(sd, _mm_setzero_pd());
    __m128d half = _mm_or_pd(_mm_and_pd(sign, msk), _mm_set_sd(0.5));
    return _mm_cvtsd_si32(_mm_add_sd(sd, half));
}

R2D_FORCEINLINE
static float r2d_sqrt(float x) {
    __m128 ss = _mm_load_ss(&x);
    return _mm_cvtss_f32(_mm_sqrt_ss(ss));
}

R2D_FORCEINLINE
static void r2d_clear_image(R2DColor8* data, uint32_t width, uint32_t height, R2DColor8 color) {
#if 1
    for (uint32_t i = 0; i < width * height; i++) {
        data[i] = color;
    }
#else
    uint32_t num_pixels = width * height;
    uint32_t rem = num_pixels % 16;
    uint32_t count = num_pixels - rem;
    __m128i col = _mm_set1_epi32(color);
    for (uint32_t i = 0; i < count; i += 16) {
        _mm_storeu_si128((__m128i*)(data + i), col);
        _mm_storeu_si128((__m128i*)(data + i + 4), col);
        _mm_storeu_si128((__m128i*)(data + i + 8), col);
        _mm_storeu_si128((__m128i*)(data + i + 12), col);
    }

    for (uint32_t i = count; i < num_pixels; i++) {
        data[i] = color;
    }
#endif
}

R2D_FORCEINLINE
static void r2d_copy_image(R2DColor8* dst_image, uint32_t dst_stride, uint32_t dst_x,
                           uint32_t dst_y, R2DColor8* src_image, uint32_t src_stride,
                           uint32_t src_x, uint32_t src_y, uint32_t width, uint32_t height) {
    for (uint32_t i = 0; i < height; i++) {
        uint32_t dst_pos = dst_stride * (i + dst_y) + dst_x;
        uint32_t src_pos = src_stride * (i + src_y) + src_x;
        std::memcpy(dst_image + dst_pos, src_image + src_pos, width * sizeof(R2DColor8));
    }
}

R2D_FORCEINLINE
static uint32_t r2d_clipping_flag(const float x, const float y, const R2DBox& box) noexcept {
    uint32_t clip_x = ((x < box.x0) << 0) | ((x > box.x1) << 2);
    uint32_t clip_y = ((y < box.y0) << 1) | ((y > box.y1) << 3);
    return clip_x | clip_y;
}

R2D_FORCEINLINE
static uint32_t r2d_fpmul(uint32_t a, uint32_t b) noexcept {
    uint32_t val = (a * b) + 0x80;
    return (val + (val >> 8)) >> 8;
}

R2D_FORCEINLINE
static uint32_t r2d_rgb_alphamult(uint32_t col, uint32_t alpha) noexcept {
    uint32_t a = (0xFF00FF & col) * alpha + 0x800080;
    uint32_t b = (0x00FF00 & col) * alpha + 0x008000;
    a += (a >> 8) & 0xFF00FF;
    b += (b >> 8) & 0x00FF00;
    a = (a & 0xFF00FF00) >> 8;
    b = (b & 0x00FF0000) >> 8;
    return a | b;
}

R2D_FORCEINLINE
static uint32_t r2d_alpharcp(uint32_t alpha) noexcept {
    if (alpha == 0)
        return 0;
    return (0xFE01 + (alpha >> 1)) / alpha;
}

R2D_FORCEINLINE
static uint32_t r2d_rgb_alphadiv(uint32_t col, uint32_t alpha) noexcept {
    if (alpha == 0)
        return 0;
    uint32_t a = (0xFF00FF & col) * 255;
    uint32_t b = (0x00FF00 & col) * 255;
    a += ((alpha >> 1) | (alpha << 15)) & 0xFF00FF;
    b += (alpha << 7) & 0x00FF00;
    uint32_t r = (a & 0xFFFF) / alpha;
    uint32_t bb = (a / alpha) & 0xFF0000;
    uint32_t g = (b / alpha) & 0x00FF00;
    // a = (a / alpha) & 0xFF00FF;
    // b = (b / alpha) & 0x00FF00;
    return r | g | bb;
    // return a | b;
}

// Separate alpha and rearrange color channels in RGB order
R2D_FORCEINLINE
static R2DColor8 r2d_separate_alpha(R2DColor8 color, uint32_t r_shift, uint32_t g_shift,
                                    uint32_t b_shift, uint32_t a_shift,
                                    uint32_t& out_alpha) noexcept {
    uint32_t r = color >> r_shift;
    uint32_t g = color >> g_shift;
    uint32_t b = color >> b_shift;
    out_alpha = color >> a_shift;
    return r | (g << 8) | (b << 16);
}

struct R2DBlendSrcOver {
    inline R2DColor8 operator()(R2DColor8 src_col, uint32_t src_a, R2DColor8 dst_col,
                                uint32_t dst_a, uint32_t& out_alpha) {
        uint32_t dst_alpha_factor = r2d_fpmul(dst_a, 255 - src_a);
        uint32_t dst = r2d_rgb_alphamult(dst_col, dst_alpha_factor);
        uint32_t src = r2d_rgb_alphamult(src_col, src_a);
        uint32_t alpha = src_a + dst_alpha_factor;
        uint32_t alpha_rcp = r2d_alpharcp(alpha);
        out_alpha = alpha;
        return r2d_rgb_alphamult(src + dst, alpha_rcp);
    }
};

struct R2DBlendSrcAtop {
    inline R2DColor8 operator()(R2DColor8 src_col, uint32_t src_a, R2DColor8 dst_col,
                                uint32_t dst_a, uint32_t& out_alpha) noexcept {
        uint32_t src_alpha_factor = r2d_fpmul(src_a, dst_a);
        uint32_t dst_alpha_factor = r2d_fpmul(dst_a, 255 - src_a);
        uint32_t src = r2d_rgb_alphamult(src_col, src_alpha_factor);
        uint32_t dst = r2d_rgb_alphamult(dst_col, dst_alpha_factor);
        uint32_t alpha = src_alpha_factor + dst_alpha_factor;
        uint32_t alpha_rcp = r2d_alpharcp(alpha);
        out_alpha = alpha;
        return r2d_rgb_alphamult(src + dst, alpha_rcp);
    }
};

struct R2DBlendSrcIn {
    R2DColor8 operator()(R2DColor8 src_col, uint32_t src_a, R2DColor8 dst_col, uint32_t dst_a,
                         uint32_t& out_alpha) noexcept {
        uint32_t alpha = r2d_fpmul(src_a, dst_a);
        uint32_t alpha_rcp = r2d_alpharcp(alpha);
        uint32_t src = r2d_rgb_alphamult(src_col, alpha);
        out_alpha = alpha;
        return r2d_rgb_alphamult(src, alpha_rcp);
    }
};

struct R2DBlendSrcOut {
    R2DColor8 operator()(R2DColor8 src_col, uint32_t src_a, R2DColor8 dst_col, uint32_t dst_a,
                         uint32_t& out_alpha) noexcept {
        uint32_t alpha = r2d_fpmul(src_a, 255 - dst_a);
        uint32_t alpha_rcp = r2d_alpharcp(alpha);
        uint32_t src = r2d_rgb_alphamult(src_col, alpha);
        out_alpha = alpha;
        return r2d_rgb_alphamult(src, alpha_rcp);
    }
};

struct R2DBlendSrcCopy {
    R2DColor8 operator()(R2DColor8 src_col, uint32_t src_a, R2DColor8 dst_col, uint32_t dst_a,
                         uint32_t& out_alpha) noexcept {
        out_alpha = src_a;
        return 0;
    }
};

struct R2DBlendDstOver {
    R2DColor8 operator()(R2DColor8 src_col, uint32_t src_a, R2DColor8 dst_col, uint32_t dst_a,
                         uint32_t& out_alpha) noexcept {
        uint32_t src_alpha_factor = r2d_fpmul(dst_a, 255 - src_a);
        uint32_t dst = r2d_rgb_alphamult(dst_col, dst_a);
        uint32_t src = r2d_rgb_alphamult(src_col, src_a);
        uint32_t alpha = src_alpha_factor + dst_a;
        // uint32_t alpha_rcp = r2d_alpharcp(ao);
        out_alpha = alpha;
        return r2d_rgb_alphadiv(src + dst, alpha);
    }
};

R2D_FORCEINLINE
static R2DColor8 r2d_blend_src_over(R2DColor8 src_col, uint32_t src_a, R2DColor8 dst_col,
                                    uint32_t dst_a, uint32_t& out_alpha) noexcept {
    uint32_t dst_alpha_factor = r2d_fpmul(dst_a, 255 - src_a);
    uint32_t dst = r2d_rgb_alphamult(dst_col, dst_alpha_factor);
    uint32_t src = r2d_rgb_alphamult(src_col, src_a);
    uint32_t alpha = src_a + dst_alpha_factor;
    // uint32_t alpha_rcp = r2d_alpharcp(ao);
    out_alpha = alpha;
    return r2d_rgb_alphadiv(src + dst, alpha);
}

R2D_FORCEINLINE
static R2DColor8 r2d_blend_src_atop(R2DColor8 src_col, uint32_t src_a, R2DColor8 dst_col,
                                    uint32_t dst_a, uint32_t& out_alpha) noexcept {
    uint32_t src_alpha_factor = r2d_fpmul(src_a, dst_a);
    uint32_t dst_alpha_factor = r2d_fpmul(dst_a, 255 - src_a);
    uint32_t src = r2d_rgb_alphamult(src_col, src_alpha_factor);
    uint32_t dst = r2d_rgb_alphamult(dst_col, dst_alpha_factor);
    uint32_t alpha = src_alpha_factor + dst_alpha_factor;
    out_alpha = alpha;
    return r2d_rgb_alphadiv(src + dst, alpha);
}

R2D_FORCEINLINE
static R2DColor8 r2d_blend_src_in(R2DColor8 src_col, uint32_t src_a, R2DColor8 dst_col,
                                  uint32_t dst_a, uint32_t& out_alpha) noexcept {
    return 0;
}

R2D_FORCEINLINE
static R2DColor8 r2d_blend_src_out(R2DColor8 src_col, uint32_t src_a, R2DColor8 dst_col,
                                   uint32_t dst_a, uint32_t& out_alpha) noexcept {
    return 0;
}

R2D_FORCEINLINE
static R2DColor8 r2d_blend_src_copy(R2DColor8 src_col, uint32_t src_a, R2DColor8 dst_col,
                                    uint32_t dst_a, uint32_t& out_alpha) noexcept {
    return 0;
}

R2D_FORCEINLINE
static R2DColor8 r2d_blend_dst_over(R2DColor8 src_col, uint32_t src_a, R2DColor8 dst_col,
                                    uint32_t dst_a, uint32_t& out_alpha) noexcept {
    return 0;
}