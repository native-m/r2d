#pragma once

#include "r2d_core.hpp"
#include <cassert>
#include <cmath>
#include <d2d1helper.h>

enum class R2DPixelFormat {
    Unknown,
    RGBA8,
    ARGB8,
    BGRA8,
    RGBX8,
    BGRX8
};

enum class R2DBlendMode {
    SrcOver,
    SrcAtop,
    SrcIn,
    SrcOut,
    SrcCopy,
    DstOver,
    DstAtop,
    DstIn,
    DstOut,
    DstCopy,
    Clear,
    Xor,
};

enum class R2DSourceType {
    Solid,
    Linear
};

enum class R2DFillMode {
    NonZero,
    EvenOdd,
};

enum class R2DLineJoin {
    None,
    Miter,
    Bevel,
    Rounded,
};

enum class R2DPathCommand {
    MoveTo,
    LineTo,
    QuadTo,
    CubicTo,
};

enum class R2DContextFlags {
    Blending, // Enable/disable color blending between render target and the incoming source color
              // values. (default: enabled)
    AntiAliasing,          // Enable/disable anti-aliasing. (default: enabled)
    LazyClear,             // Enable/disable render target lazy clear. (default: disabled)
    PremultipliedDstAlpha, // Render target color is alpha-premultiplied. (default:
                           // disabled)
};

struct R2DPoint {
    float x;
    float y;
};

struct R2DRect {
    float x;
    float y;
    float w;
    float h;
};

struct R2DColor {
    float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;

    R2D_FORCEINLINE
    constexpr R2DColor() noexcept {}

    R2D_FORCEINLINE
    constexpr R2DColor(int r, int g, int b, int a = 255) noexcept {
        constexpr float div = 1.f / 255.f;
        this->r = (float)r * div;
        this->g = (float)g * div;
        this->b = (float)b * div;
        this->a = (float)a * div;
    }

    R2D_FORCEINLINE
    constexpr R2DColor(float r, float g, float b, float a = 1.0f) noexcept :
        r(r), g(g), b(b), a(a) {}

    R2D_FORCEINLINE
    constexpr R2DColor premultiply() const noexcept { return R2DColor(r * a, g * a, b * a); }

    R2D_FORCEINLINE
    constexpr R2DColor blend(const R2DColor& other, float factor) const noexcept {
        float inv = (1.0f - factor);
        return R2DColor(inv * r + factor * other.r, inv * g + factor * other.g,
                        inv * b + factor * other.b, inv * a + factor * other.a);
    }

    R2D_FORCEINLINE
    constexpr R2DColor blend(const R2DColor& other, const R2DColor& factor) const noexcept {
        return R2DColor(
            (1.0f - factor.r) * r + factor.r * other.r, (1.0f - factor.g) * g + factor.g * other.g,
            (1.0f - factor.b) * b + factor.b * other.b, (1.0f - factor.a) * a + factor.a * other.a);
    }

    R2D_FORCEINLINE
    R2DColor8 to_bytes(R2DPixelFormat fmt) const noexcept {
        uint32_t u_r = (uint32_t)std::round(r * 255.0f);
        uint32_t u_g = (uint32_t)std::round(g * 255.0f);
        uint32_t u_b = (uint32_t)std::round(b * 255.0f);
        uint32_t u_a = (uint32_t)std::round(a * 255.0f);

        switch (fmt) {
            case R2DPixelFormat::RGBA8:
                return R2D_COLOR_CHANNEL_4(u_r, u_g, u_b, u_a);
            case R2DPixelFormat::ARGB8:
                return R2D_COLOR_CHANNEL_4(u_a, u_r, u_g, u_b);
            case R2DPixelFormat::BGRA8:
                return R2D_COLOR_CHANNEL_4(u_b, u_g, u_r, u_a);
            case R2DPixelFormat::RGBX8:
                return R2D_COLOR_CHANNEL_4(u_r, u_g, u_b, 255);
            case R2DPixelFormat::BGRX8:
                return R2D_COLOR_CHANNEL_4(u_b, u_g, u_r, 255);
            default:
                R2D_UNREACHABLE();
        }

        return 0;
    }

    R2D_FORCEINLINE
    R2DColor8 to_rgba8() const noexcept { return to_bytes(R2DPixelFormat::RGBA8); }
};

struct R2DMatrix {
    float m[9]{};

    inline R2DMatrix() noexcept {}

    inline R2DMatrix(float m00, float m01, float m02, float m10, float m11, float m12, float m20,
                     float m21, float m22) noexcept :
        m{m00, m01, m02, m10, m11, m12, m20, m21, m22} {}

    inline R2DMatrix(const R2DMatrix&) noexcept = default;

    inline R2DMatrix(R2DMatrix&&) = delete;

#define AT(row, col) (row * 3 + col)

    R2D_FORCEINLINE
    float& at(uint32_t row, uint32_t col) {
        R2D_CHECK(row < 3 && col < 3);
        return m[AT(row, col)];
    }

    R2D_FORCEINLINE
    float at(uint32_t row, uint32_t col) const {
        R2D_CHECK(row < 3 && col < 3);
        return m[AT(row, col)];
    }

    R2D_FORCEINLINE
    float& operator()(uint32_t row, uint32_t col) {
        assert(row < 3 && col < 3);
        return m[AT(row, col)];
    }

    R2D_FORCEINLINE
    float operator()(uint32_t row, uint32_t col) const {
        assert(row < 3 && col < 3);
        return m[AT(row, col)];
    }

#if __has_cpp_attribute(__cpp_multidimensional_subscript)
    R2D_FORCEINLINE
    float& operator[](uint32_t row, uint32_t col) {
        assert(row < 3 && col < 3);
        return m[AT(row, col)];
    }

    R2D_FORCEINLINE
    float operator[](uint32_t row, uint32_t col) const {
        assert(row < 3 && col < 3);
        return m[AT(row, col)];
    }
#endif

    inline R2DMatrix& operator+=(const R2DMatrix& other) noexcept {
        m[0] += other.m[0];
        m[1] += other.m[1];
        m[2] += other.m[2];
        m[3] += other.m[3];
        m[4] += other.m[4];
        m[5] += other.m[5];
        m[6] += other.m[6];
        m[7] += other.m[7];
        m[8] += other.m[8];
        return *this;
    }

    inline R2DMatrix& operator-=(const R2DMatrix& other) noexcept {
        m[0] -= other.m[0];
        m[1] -= other.m[1];
        m[2] -= other.m[2];
        m[3] -= other.m[3];
        m[4] -= other.m[4];
        m[5] -= other.m[5];
        m[6] -= other.m[6];
        m[7] -= other.m[7];
        m[8] -= other.m[8];
        return *this;
    }

    inline R2DMatrix& operator*=(const R2DMatrix& other) noexcept {
        float m00 = m[AT(0, 0)] * other.m[AT(0, 0)] + m[AT(0, 1)] * other.m[AT(1, 0)] +
                    m[AT(0, 2)] * other.m[AT(2, 0)];
        float m01 = m[AT(0, 0)] * other.m[AT(0, 1)] + m[AT(0, 1)] * other.m[AT(1, 1)] +
                    m[AT(0, 2)] * other.m[AT(2, 1)];
        float m02 = m[AT(0, 0)] * other.m[AT(0, 2)] + m[AT(0, 1)] * other.m[AT(1, 2)] +
                    m[AT(0, 2)] * other.m[AT(2, 2)];

        float m10 = m[AT(1, 0)] * other.m[AT(0, 0)] + m[AT(1, 1)] * other.m[AT(1, 0)] +
                    m[AT(1, 2)] * other.m[AT(2, 0)];
        float m11 = m[AT(1, 0)] * other.m[AT(0, 1)] + m[AT(1, 1)] * other.m[AT(1, 1)] +
                    m[AT(1, 2)] * other.m[AT(2, 1)];
        float m12 = m[AT(1, 0)] * other.m[AT(0, 2)] + m[AT(1, 1)] * other.m[AT(1, 2)] +
                    m[AT(1, 2)] * other.m[AT(2, 2)];

        float m20 = m[AT(2, 0)] * other.m[AT(0, 0)] + m[AT(2, 1)] * other.m[AT(1, 0)] +
                    m[AT(2, 2)] * other.m[AT(2, 0)];
        float m21 = m[AT(2, 0)] * other.m[AT(0, 1)] + m[AT(2, 1)] * other.m[AT(1, 1)] +
                    m[AT(2, 2)] * other.m[AT(2, 1)];
        float m22 = m[AT(2, 0)] * other.m[AT(0, 2)] + m[AT(2, 1)] * other.m[AT(1, 2)] +
                    m[AT(2, 2)] * other.m[AT(2, 2)];

        m[0] = m00;
        m[1] = m01;
        m[2] = m02;
        m[3] = m10;
        m[4] = m11;
        m[5] = m12;
        m[6] = m20;
        m[7] = m21;
        m[8] = m22;
        return *this;
    }

    inline R2DMatrix& operator*=(float x) noexcept {
        m[0] *= x;
        m[1] *= x;
        m[2] *= x;
        m[3] *= x;
        m[4] *= x;
        m[5] *= x;
        m[6] *= x;
        m[7] *= x;
        m[8] *= x;
        return *this;
    }

    inline R2DMatrix operator+(const R2DMatrix& other) const noexcept {
        return R2DMatrix(m[0] + other.m[0], m[1] + other.m[1], m[2] + other.m[2], m[3] + other.m[3],
                         m[4] + other.m[4], m[5] + other.m[5], m[6] + other.m[6], m[7] + other.m[7],
                         m[8] + other.m[8]);
    }

    inline R2DMatrix operator-(const R2DMatrix& other) const noexcept {
        return R2DMatrix(m[0] - other.m[0], m[1] - other.m[1], m[2] - other.m[2], m[3] - other.m[3],
                         m[4] - other.m[4], m[5] - other.m[5], m[6] - other.m[6], m[7] - other.m[7],
                         m[8] - other.m[8]);
    }

    inline R2DMatrix operator*(const R2DMatrix& other) const noexcept {
        float m00 = m[AT(0, 0)] * other.m[AT(0, 0)] + m[AT(0, 1)] * other.m[AT(1, 0)] +
                    m[AT(0, 2)] * other.m[AT(2, 0)];
        float m01 = m[AT(0, 0)] * other.m[AT(0, 1)] + m[AT(0, 1)] * other.m[AT(1, 1)] +
                    m[AT(0, 2)] * other.m[AT(2, 1)];
        float m02 = m[AT(0, 0)] * other.m[AT(0, 2)] + m[AT(0, 1)] * other.m[AT(1, 2)] +
                    m[AT(0, 2)] * other.m[AT(2, 2)];
        float m10 = m[AT(1, 0)] * other.m[AT(0, 0)] + m[AT(1, 1)] * other.m[AT(1, 0)] +
                    m[AT(1, 2)] * other.m[AT(2, 0)];
        float m11 = m[AT(1, 0)] * other.m[AT(0, 1)] + m[AT(1, 1)] * other.m[AT(1, 1)] +
                    m[AT(1, 2)] * other.m[AT(2, 1)];
        float m12 = m[AT(1, 0)] * other.m[AT(0, 2)] + m[AT(1, 1)] * other.m[AT(1, 2)] +
                    m[AT(1, 2)] * other.m[AT(2, 2)];
        float m20 = m[AT(2, 0)] * other.m[AT(0, 0)] + m[AT(2, 1)] * other.m[AT(1, 0)] +
                    m[AT(2, 2)] * other.m[AT(2, 0)];
        float m21 = m[AT(2, 0)] * other.m[AT(0, 1)] + m[AT(2, 1)] * other.m[AT(1, 1)] +
                    m[AT(2, 2)] * other.m[AT(2, 1)];
        float m22 = m[AT(2, 0)] * other.m[AT(0, 2)] + m[AT(2, 1)] * other.m[AT(1, 2)] +
                    m[AT(2, 2)] * other.m[AT(2, 2)];
        return R2DMatrix(m00, m01, m02, m10, m11, m12, m20, m21, m22);
    }

    inline R2DMatrix operator*(float x) noexcept {
        return R2DMatrix(m[0] * x, m[1] * x, m[2] * x, m[3] * x, m[4] * x, m[5] * x, m[6] * x,
                         m[7] * x, m[8] * x);
    }

    inline R2DMatrix transposed() noexcept {
        return R2DMatrix(m[AT(0, 0)], m[AT(1, 0)], m[AT(2, 0)], m[AT(0, 1)], m[AT(1, 1)],
                         m[AT(1, 2)], m[AT(2, 0)], m[AT(2, 1)], m[AT(2, 2)]);
    }

    inline static R2DMatrix identity() noexcept {
        return R2DMatrix(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    inline static R2DMatrix translate(float x, float y) noexcept {
        return R2DMatrix(1.0f, 0.0f, x, 0.0f, 1.0f, y, 0.0f, 0.0f, 1.0f);
    }

    inline static R2DMatrix translate(R2DPoint point) noexcept {
        return R2DMatrix(1.0f, 0.0f, point.x, 0.0f, 1.0f, point.y, 0.0f, 0.0f, 1.0f);
    }

    inline static R2DMatrix scale(float s) noexcept {
        return R2DMatrix(s, 0.0f, 0.0f, 0.0f, s, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    inline static R2DMatrix scale(float sx, float sy) noexcept {
        return R2DMatrix(sx, 0.0f, 0.0f, 0.0f, sy, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    inline static R2DMatrix scale_x(float sx) noexcept {
        return R2DMatrix(sx, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    inline static R2DMatrix scale_y(float sy) noexcept {
        return R2DMatrix(1.0f, 0.0f, 0.0f, 0.0f, sy, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    inline static R2DMatrix scale_at_point(float sx, float sy, R2DPoint center) noexcept {
        float cx = center.x - sx * center.x;
        float cy = center.y - sy * center.y;
        return R2DMatrix(sx, 0.0f, cx, 0.0f, sy, cy, 0.0f, 0.0f, 1.0f);
    }

    inline static R2DMatrix rotate(float rad_angle, float dir = 1.0f) noexcept {
        float s = std::sin(rad_angle);
        float c = std::cos(rad_angle) * dir;
        return R2DMatrix(c, -s, 0.0f, s, c, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    inline static R2DMatrix shear(float x, float y) noexcept {
        return R2DMatrix(1.0f, x, 0.0f, y, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    template <typename... Matrices>
    inline static R2DMatrix compose(Matrices&&... matrices) {
        return (matrices * ...);
    }

#undef AT
};

struct R2DSourceLinear {};

struct R2DSource {
    R2DSourceType type;
    union {
        R2DColor8 solid;
        R2DSourceLinear linear;
    };
};

struct R2DImage {
    void* data_{};
    R2DPixelFormat format_{};
    uint32_t width_{};
    uint32_t height_{};
    bool unmanaged_{};

    R2DImage() noexcept {}

    R2DImage(R2DImage&& image) noexcept :
        data_(image.data_), format_(image.format_), width_(image.width_), height_(image.height_) {
        std::memset(&image, 0, sizeof(R2DImage));
    }

    ~R2DImage() {
        if (data_)
            std::free(data_);
        std::memset(this, 0, sizeof(R2DImage));
    }

    R2DImage& operator=(R2DImage&& image) noexcept {
        data_ = std::exchange(image.data_, nullptr);
        format_ = std::exchange(image.format_, {});
        width_ = std::exchange(image.width_, 0);
        height_ = std::exchange(image.height_, 0);
        return *this;
    }

    bool init(uint32_t width, uint32_t height, R2DPixelFormat format) {
        assert(width != 0);
        assert(height != 0);
        size_t size = width * height * sizeof(R2DPixel);
        void* new_data = std::malloc(size);
        if (!new_data)
            return false;
        if (data_)
            std::free(data_);
        std::memset(new_data, 0, size);
        data_ = new_data;
        format_ = format;
        width_ = width;
        height_ = height;
        return true;
    }

    inline void clear(const R2DColor& color) noexcept { clear_raw(color.to_bytes(format_)); }

    void clear_raw(R2DColor8 color) noexcept {
        r2d_clear_image((R2DColor8*)data_, width_, height_, color);
    }

    R2DImage clone() const {
        R2DImage new_image;
        new_image.init(width_, height_, format_);
        R2D_CHECK(std::memcpy(new_image.data_, data_, width_ * height_ * sizeof(R2DPixel)));
        return new_image;
    }

    R2DPixelFormat format() const { return format_; }
    uint32_t width() const { return width_; }
    uint32_t height() const { return height_; }
    void* raw_data() const { return data_; }
    operator bool() const noexcept { return data_ != nullptr; }
};

struct R2DCell {
    uint32_t generation;
    int32_t cover;
    int32_t area;
};

struct R2DRaster {
    R2DCell* cells_{};
    uint32_t width_{};
    uint32_t height_{};
    uint32_t stride_{};

    // The generation counter optimizes how often the R2DRaster will be discarded.
    // R2DRaster will be discarded/cleared only if the generation counter overflowed.
    // This prevents unnecessary memory writes and saves memory bandwidth.
    uint32_t prev_gen_{};
    uint32_t current_gen_{};

    R2DRaster() {}

    R2DRaster(R2DRaster&& other) noexcept :
        cells_(std::exchange(other.cells_, nullptr)),
        width_(std::exchange(other.width_, 0)),
        height_(std::exchange(other.height_, 0)),
        stride_(std::exchange(other.stride_, 0)),
        prev_gen_(std::exchange(other.prev_gen_, 0)),
        current_gen_(std::exchange(other.current_gen_, 0)) {}

    ~R2DRaster() {
        if (cells_)
            std::free(cells_);
        width_ = 0;
        height_ = 0;
    }

    R2DRaster& operator=(R2DRaster&& other) noexcept {
        cells_ = std::exchange(other.cells_, nullptr);
        width_ = std::exchange(other.width_, 0);
        height_ = std::exchange(other.height_, 0);
        stride_ = std::exchange(other.stride_, 0);
        prev_gen_ = std::exchange(other.prev_gen_, 0);
        current_gen_ = std::exchange(other.current_gen_, 0);
        return *this;
    }

    bool init(uint32_t width, uint32_t height) noexcept {
        assert(width != 0);
        assert(height != 0);
        uint32_t stride = width + 1;
        size_t size = stride * height * sizeof(R2DCell);
        R2DCell* new_cells = (R2DCell*)std::malloc(size);
        if (!new_cells)
            return false;
        if (cells_)
            std::free(cells_);
        std::memset(new_cells, 0, size);
        cells_ = new_cells;
        stride_ = stride;
        width_ = width;
        height_ = height;
        return true;
    }

    void clear() {
        std::memset(cells_, 0, stride_ * height_ * sizeof(R2DCell));
        current_gen_ = 0;
        prev_gen_ = 0;
    }

    R2DRaster clone() {
        R2DRaster raster;
        if (!raster.init(width_, height_))
            return raster;
        std::memcpy(raster.cells_, cells_, stride_ * height_ * sizeof(R2DCell));
        return raster;
    }

    uint32_t width() const noexcept { return width_; }
    uint32_t stride() const noexcept { return stride_; }
    uint32_t height() const noexcept { return height_; }
    operator bool() const noexcept { return cells_ != nullptr; }
};

struct R2DPath {
    R2DPathCommand* commands;
    R2DPoint* points;
    uint32_t num_commands;
    uint32_t num_points;

    R2D_FORCEINLINE void move_to(float x, float y) {}

    R2D_FORCEINLINE void line_to(float x, float y) {}

    R2D_FORCEINLINE void reserve(uint32_t num_commands) {}

    R2D_FORCEINLINE bool full() const noexcept { return false; }

    R2D_FORCEINLINE bool empty() const noexcept { return num_commands > 0; }

    // Unchecked operations for more control
    R2D_FORCEINLINE void umove_to(float x, float y) {}

    R2D_FORCEINLINE void uline_to(float x, float y) {}
};

struct R2DContext {
    R2DImage* rt_{};
    R2DColorBitPos rt_bitpos_{};
    R2DRaster* raster_{};
    const R2DSource* source_{};
    R2DBlendMode blend_mode_{};
    R2DLineJoin line_join_{};
    float miter_limit_{};
    R2DRect clip_rect_{};
    R2DBox clip_box_{};
    float line_thickness_{0.5f};

    float px0, py0;
    uint32_t p0_clip{};
    bool p0_inside = false;

    R2DPoint clip_stack[3]{};
    uint32_t clip_stack_pos{};

    R2DVector<R2DPoint> tmp_line_normals_;
    R2DPath imm_path_{};

    R2DContext() {}
    R2DContext(const R2DContext&) = delete;
    ~R2DContext() {}

    void set_render_target(R2DImage* image) noexcept {
        rt_ = image;
        rt_bitpos_ = color_bitpos_(image->format_);
    }

    void set_raster(R2DRaster* raster) noexcept {
        if (!raster_) {
            clip_box_.x0 = 0;
            clip_box_.y0 = 0;
            clip_box_.x1 = (float)(raster->width_);
            clip_box_.y1 = (float)(raster->height_);
        }
        raster_ = raster;
    }

    void set_clip_rect(const R2DRect* rect) noexcept {
        if (!rect) {
            clip_box_.x0 = 0;
            clip_box_.y0 = 0;
            clip_box_.x1 = (float)(raster_->width_);
            clip_box_.y1 = (float)(raster_->height_);
            return;
        }
        clip_box_.x0 = rect->x;
        clip_box_.y0 = rect->y;
        clip_box_.x1 = rect->x + rect->w;
        clip_box_.y1 = rect->y + rect->h;
    }

    void set_source(const R2DSource* source) noexcept { source_ = source; }

    void set_blend_mode(R2DBlendMode blend_mode) noexcept { blend_mode_ = blend_mode; }

    void set_line_thickness(float thickness) noexcept { line_thickness_ = thickness * 0.5f; }

    void set_line_join(R2DLineJoin line_join) noexcept { line_join_ = line_join; }

    void set_fill_mode(R2DFillMode fill_mode) noexcept {}

    void set_pre_transform_matrix(const R2DMatrix& matrix) noexcept {}

    void set_post_transform_matrix(const R2DMatrix& matrix) noexcept {}

    void enable(R2DContextFlags flag) noexcept {}

    void disable(R2DContextFlags flag) noexcept {}

    void add_path(const R2DPath& path) {}

    void add_path_filled() {}

    inline void add_line(const R2DPoint& v0, const R2DPoint& v1) {
        float dx = -(v1.y - v0.y);
        float dy = v1.x - v0.x;
        float inv_len = line_thickness_ / r2d_sqrt(dy * dy + dx * dx);
        float nx = dx * inv_len;
        float ny = dy * inv_len;
        plot_move_to(v0.x - nx, v0.y - ny);
        plot_line_to(v1.x - nx, v1.y - ny);
        plot_line_to(v1.x + nx, v1.y + ny);
        plot_line_to(v0.x + nx, v0.y + ny);
        plot_line_to(v0.x - nx, v0.y - ny);
        plot_close();
    }

    void add_polygon(const R2DPoint* verts, size_t count, size_t first_vertex = 0) {
        if (count < 3)
            return;
        verts += first_vertex;
        double first_x = (double)verts[0].x * 256.0;
        double first_y = (double)verts[0].y * 256.0;
        R2DFixed32 x0 = r2d_iround(first_x);
        R2DFixed32 y0 = r2d_iround(first_y);
        for (size_t i = 1; i < count; i++) {
            R2DFixed32 x1 = r2d_iround((double)verts[i].x * 256.0);
            R2DFixed32 y1 = r2d_iround((double)verts[i].y * 256.0);
            add_edge(x0, y0, x1, y1);
            x0 = x1;
            y0 = y1;
        }
        add_edge(x0, y0, (int)first_x, (int)first_y);
    }

    void add_polygon_indexed(const R2DPoint* verts, const uint32_t* index, size_t index_count,
                             size_t first_index = 0, size_t vertex_offset = 0) {
        if (index_count < 3)
            return;
        index += first_index;
        double first_x = (double)verts[index[0]].x * 256.0;
        double first_y = (double)verts[index[0]].y * 256.0;
        R2DFixed32 x0 = r2d_iround(first_x);
        R2DFixed32 y0 = r2d_iround(first_y);
        for (uint32_t i = 1; i < index_count; i++) {
            R2DFixed32 x1 = r2d_iround((double)verts[index[i]].x * 256.0);
            R2DFixed32 y1 = r2d_iround((double)verts[index[i]].y * 256.0);
            add_edge(x0, y0, x1, y1);
            x0 = x1;
            y0 = y1;
        }
        add_edge(x0, y0, (int)first_x, (int)first_y);
    }

    void add_polyline(const R2DPoint* verts, size_t count, size_t first_vertex = 0,
                      bool close = false) {
        if (count == 0)
            return;
        if (count < 3) {
            add_line(verts[0], verts[1]);
            return;
        }
        verts += first_vertex;
        switch (line_join_) {
            case R2DLineJoin::None: {
                float x0 = verts[0].x;
                float y0 = verts[0].y;
                for (size_t i = 1; i < count; i++) {
                    float x1 = verts[i].x;
                    float y1 = verts[i].y;
                    add_line(R2DPoint{x0, y0}, R2DPoint{x1, y1});
                    x0 = x1;
                    y0 = y1;
                }
                break;
            }
            case R2DLineJoin::Miter: {
                // size_t join_count = ((size_t)count * 2ull) / 3ull;
                float x0 = verts[0].x;
                float y0 = verts[0].y;
                float x1 = verts[1].x;
                float y1 = verts[1].y;
                float x2 = verts[2].x;
                float y2 = verts[2].y;
                float tx0 = x1 - x0;
                float ty0 = y1 - y0;
                float tx1 = x2 - x1;
                float ty1 = y2 - y1;
                float inv_len0 = line_thickness_ / r2d_sqrt(tx0 * tx0 + ty0 * ty0);
                float inv_len1 = line_thickness_ / r2d_sqrt(tx1 * tx1 + ty1 * ty1);
                tx0 *= inv_len0;
                ty0 *= inv_len0;
                tx1 *= inv_len1;
                ty1 *= inv_len1;
                float nx0 = ty0;
                float ny0 = -tx0;
                float nx1 = ty1;
                float ny1 = -tx1;
                float dir = tx0 * nx1 + ty0 * ny1;
                float miter_x = nx1 + nx0;
                float miter_y = ny1 + ny0;
                float miter_invlen = 1.0f / r2d_sqrt(miter_x * miter_x + miter_y * miter_y);
                float miter_nx = miter_x * miter_invlen;
                float miter_ny = miter_y * miter_invlen;
                float miter_angle_inv =
                    line_thickness_ / ((miter_nx * nx1 + miter_ny * ny1) / line_thickness_);

                if (dir < 0.0f) {
                    miter_angle_inv = -miter_angle_inv;
                }

                float offset_x = miter_nx * miter_angle_inv;
                float offset_y = miter_ny * miter_angle_inv;
                plot_move_to(x0 + nx0, y0 + ny0);
                plot_line_to(x1 + offset_x, y1 + offset_y);
                x0 = x1;
                y0 = y1;
                x1 = x2;
                y1 = y2;

                for (size_t i = 1; i < count - 2; i++) {
                    x2 = verts[i + 2].x;
                    y2 = verts[i + 2].y;
                    tx0 = x1 - x0;
                    ty0 = y1 - y0;
                    tx1 = x2 - x1;
                    ty1 = y2 - y1;
                    inv_len0 = line_thickness_ / r2d_sqrt(tx0 * tx0 + ty0 * ty0);
                    inv_len1 = line_thickness_ / r2d_sqrt(tx1 * tx1 + ty1 * ty1);
                    tx0 *= inv_len0;
                    ty0 *= inv_len0;
                    tx1 *= inv_len1;
                    ty1 *= inv_len1;
                    nx0 = ty0;
                    ny0 = -tx0;
                    nx1 = ty1;
                    ny1 = -tx1;
                    dir = tx0 * nx1 + ty0 * ny1;
                    miter_x = nx1 + nx0;
                    miter_y = ny1 + ny0;
                    miter_invlen = 1.0f / r2d_sqrt(miter_x * miter_x + miter_y * miter_y);
                    miter_nx = miter_x * miter_invlen;
                    miter_ny = miter_y * miter_invlen;
                    miter_angle_inv =
                        line_thickness_ / ((miter_nx * nx1 + miter_ny * ny1) / line_thickness_);

                    if (dir < 0.0f) {
                        miter_angle_inv = -miter_angle_inv;
                    }

                    offset_x = miter_nx * miter_angle_inv;
                    offset_y = miter_ny * miter_angle_inv;
                    plot_line_to(x1 + offset_x, y1 + offset_y);
                    x0 = x1;
                    y0 = y1;
                    x1 = x2;
                    y1 = y2;
                }

                plot_line_to(x1 + nx1, y1 + ny1);
                plot_line_to(x1 - nx1, y1 - ny1);

                x0 = verts[count - 1].x;
                y0 = verts[count - 1].y;
                x1 = verts[count - 2].x;
                y1 = verts[count - 2].y;
                x2 = verts[count - 3].x;
                y2 = verts[count - 3].y;
                tx0 = x1 - x0;
                ty0 = y1 - y0;
                tx1 = x2 - x1;
                ty1 = y2 - y1;
                inv_len0 = line_thickness_ / r2d_sqrt(tx0 * tx0 + ty0 * ty0);
                inv_len1 = line_thickness_ / r2d_sqrt(tx1 * tx1 + ty1 * ty1);
                tx0 *= inv_len0;
                ty0 *= inv_len0;
                tx1 *= inv_len1;
                ty1 *= inv_len1;
                nx0 = ty0;
                ny0 = -tx0;
                nx1 = ty1;
                ny1 = -tx1;
                dir = tx0 * nx1 + ty0 * ny1;
                miter_x = nx1 + nx0;
                miter_y = ny1 + ny0;
                miter_invlen = 1.0f / r2d_sqrt(miter_x * miter_x + miter_y * miter_y);
                miter_nx = miter_x * miter_invlen;
                miter_ny = miter_y * miter_invlen;
                miter_angle_inv =
                    line_thickness_ / ((miter_nx * nx1 + miter_ny * ny1) / line_thickness_);

                if (dir < 0.0f) {
                    miter_angle_inv = -miter_angle_inv;
                }

                offset_x = miter_nx * miter_angle_inv;
                offset_y = miter_ny * miter_angle_inv;
                plot_line_to(x1 - offset_x, y1 - offset_y);
                x0 = x1;
                y0 = y1;
                x1 = x2;
                y1 = y2;

                for (size_t i = count - 1; i > 2; i--) {
                    x2 = verts[i - 3].x;
                    y2 = verts[i - 3].y;
                    tx0 = x1 - x0;
                    ty0 = y1 - y0;
                    tx1 = x2 - x1;
                    ty1 = y2 - y1;
                    inv_len0 = line_thickness_ / r2d_sqrt(tx0 * tx0 + ty0 * ty0);
                    inv_len1 = line_thickness_ / r2d_sqrt(tx1 * tx1 + ty1 * ty1);
                    tx0 *= inv_len0;
                    ty0 *= inv_len0;
                    tx1 *= inv_len1;
                    ty1 *= inv_len1;
                    nx0 = ty0;
                    ny0 = -tx0;
                    nx1 = ty1;
                    ny1 = -tx1;
                    dir = tx0 * nx1 + ty0 * ny1;
                    miter_x = nx1 + nx0;
                    miter_y = ny1 + ny0;
                    miter_invlen = 1.0f / r2d_sqrt(miter_x * miter_x + miter_y * miter_y);
                    miter_nx = miter_x * miter_invlen;
                    miter_ny = miter_y * miter_invlen;
                    miter_angle_inv =
                        line_thickness_ / ((miter_nx * nx1 + miter_ny * ny1) / line_thickness_);

                    if (dir < 0.0f) {
                        miter_angle_inv = -miter_angle_inv;
                    }

                    offset_x = miter_nx * miter_angle_inv;
                    offset_y = miter_ny * miter_angle_inv;
                    plot_line_to(x1 - offset_x, y1 - offset_y);
                    x0 = x1;
                    y0 = y1;
                    x1 = x2;
                    y1 = y2;
                }
                plot_line_to(x1 + nx1, y1 + ny1);
                plot_line_to(x1 - nx1, y1 - ny1);
                plot_close();
                break;
            }
            case R2DLineJoin::Bevel: {

                break;
            }
            case R2DLineJoin::Rounded: {
                break;
            }
            default:
                break;
        }
    }

    R2D_FORCEINLINE void plot_move_to(float x, float y) noexcept {
        px0 = x;
        py0 = y;
        p0_clip = r2d_clipping_flag(x, y, clip_box_);
        p0_inside = p0_clip == 0;
        clip_stack_pos = 0;
    }

    bool plot_line_to(float x, float y) noexcept {
        uint32_t p1_clip = r2d_clipping_flag(x, y, clip_box_);
        bool p1_inside = p1_clip == 0;

        if (!(p0_clip || p1_clip)) {
            add_edge(px0, py0, x, y);
            px0 = x;
            py0 = y;
            p0_clip = p1_clip;
            p0_inside = p1_inside;
            return false;
        }

        if (p0_clip & p1_clip & 1) {
            px0 = x;
            py0 = y;
            p0_clip = p1_clip;
            p0_inside = p1_inside;
            return true;
        }

        float dx = px0 - x;
        float dy = py0 - y;
        float slope = (py0 - y) / (px0 - x);

        if (p0_clip & R2D_CLIP_X0) {
            float intersect_x = clip_box_.x0;
            float intersect_y = (clip_box_.x0 - px0) * slope + py0;

            /*if (clip_stack_pos > 0 && p0_inside && !p1_inside) {
                R2DPoint& last_intersect = clip_stack[--clip_stack_pos];
                add_edge(last_intersect.x, last_intersect.y, intersect_x, intersect_y);
            }*/

            add_edge(intersect_x, intersect_y, x, y);

            R2DPoint& intersect = clip_stack[clip_stack_pos++];
            intersect.x = intersect_x;
            intersect.y = intersect_y;
        } else if (p1_clip & R2D_CLIP_X0) {
            float intersect_x = clip_box_.x0;
            float intersect_y = (clip_box_.x0 - px0) * slope + py0;

            add_edge(px0, py0, intersect_x, intersect_y);

            if (clip_stack_pos > 0) {
                R2DPoint& last_intersect = clip_stack[--clip_stack_pos];
                add_edge(intersect_x, intersect_y, last_intersect.x, last_intersect.y);
            } else {
                R2DPoint& intersect = clip_stack[clip_stack_pos++];
                intersect.x = intersect_x;
                intersect.y = intersect_y;
            }
        } else if (p0_clip & R2D_CLIP_X1) {
            float intersect_x = clip_box_.x1;
            float intersect_y = (clip_box_.x1 - px0) * slope + py0;

            add_edge(intersect_x, intersect_y, x, y);

            R2DPoint& intersect = clip_stack[clip_stack_pos++];
            intersect.x = intersect_x;
            intersect.y = intersect_y;
        } else if (p1_clip & R2D_CLIP_X1) {
            float intersect_x = clip_box_.x1;
            float intersect_y = (clip_box_.x1 - px0) * slope + py0;

            add_edge(px0, py0, intersect_x, intersect_y);

            if (clip_stack_pos > 0) {
                R2DPoint& last_intersect = clip_stack[--clip_stack_pos];
                add_edge(intersect_x, intersect_y, last_intersect.x, last_intersect.y);
            } else {
                R2DPoint& intersect = clip_stack[clip_stack_pos++];
                intersect.x = intersect_x;
                intersect.y = intersect_y;
            }
        } else if ((p0_clip & R2D_CLIP_X0) && (p1_clip & R2D_CLIP_X1)) {
            float intersect_x0 = clip_box_.x0;
            float intersect_x1 = clip_box_.x1;
            float intersect_y0 = (clip_box_.x0 - px0) * slope + py0;
            float intersect_y1 = (clip_box_.x1 - px0) * slope + py0;

            add_edge(intersect_x0, intersect_y0, intersect_x1, intersect_y1);

            R2DPoint& intersect = clip_stack[clip_stack_pos++];
            intersect.x = intersect_x0;
            intersect.y = intersect_y0;

            intersect = clip_stack[clip_stack_pos++];
            intersect.x = intersect_x1;
            intersect.y = intersect_y1;
        } else if ((p0_clip & R2D_CLIP_X1) && (p1_clip & R2D_CLIP_X0)) {
            float intersect_x0 = clip_box_.x0;
            float intersect_x1 = clip_box_.x1;
            float intersect_y0 = (clip_box_.x0 - px0) * slope + py0;
            float intersect_y1 = (clip_box_.x1 - px0) * slope + py0;

            add_edge(intersect_x0, intersect_y0, intersect_x1, intersect_y1);

            switch (clip_stack_pos) {
                case 1: {
                    R2DPoint& last_intersect = clip_stack[--clip_stack_pos];
                    add_edge(last_intersect.x, last_intersect.y, intersect_x0, intersect_y0);
                    break;
                }
                case 2: {
                    R2DPoint& last_intersect = clip_stack[--clip_stack_pos];
                    add_edge(last_intersect.x, last_intersect.y, intersect_x0, intersect_y0);
                    last_intersect = clip_stack[--clip_stack_pos];
                    add_edge(intersect_x1, intersect_y1, last_intersect.x, last_intersect.y);
                    break;
                }
                default: {
                    R2DPoint& intersect = clip_stack[clip_stack_pos++];
                    intersect.x = intersect_x0;
                    intersect.y = intersect_y0;
                    intersect = clip_stack[clip_stack_pos++];
                    intersect.x = intersect_x1;
                    intersect.y = intersect_y1;
                }
            }
        }

        px0 = x;
        py0 = y;
        p0_clip = p1_clip;
        p0_inside = p1_inside;

        return true;
    }

    inline void plot_close() noexcept {
        if (clip_stack_pos > 0) {
            R2DPoint& last_intersect0 = clip_stack[--clip_stack_pos];
            R2DPoint& last_intersect1 = clip_stack[--clip_stack_pos];
            add_edge(last_intersect1.x, last_intersect1.y, last_intersect0.x, last_intersect0.y);
        }
    }

    // Add an edge to the raster
    inline void add_edge(float x0, float y0, float x1, float y1) noexcept {
        R2DFixed32 ix0 = r2d_iround((double)x0 * 256.0);
        R2DFixed32 iy0 = r2d_iround((double)y0 * 256.0);
        R2DFixed32 ix1 = r2d_iround((double)x1 * 256.0);
        R2DFixed32 iy1 = r2d_iround((double)y1 * 256.0);
        add_edge(ix0, iy0, ix1, iy1);
    }

    void add_edge(R2DFixed32 x0, R2DFixed32 y0, R2DFixed32 x1, R2DFixed32 y1) noexcept {
        // This routine is mostly referenced from freetype/AGG rasterization code
        // The preparation code is based on: https://github.com/kobalicek/b2drefras
        static constexpr int aa_shift = 8;
        static constexpr int aa_scale = 1 << aa_shift;
        static constexpr int aa_mask = aa_scale - 1;
        static constexpr int area_shift = aa_shift + 1;

        R2DCell* cells = raster_->cells_;
        uint32_t stride = raster_->stride_;
        uint32_t generation = raster_->current_gen_;
        int dx = x1 - x0;
        int dy = y1 - y0;

        // Skip flat horizontal edge
        if (dy == 0)
            return;

        if (dx < 0)
            dx = -dx;
        if (dy < 0)
            dy = -dy;

        int inc_y = 1;
        int sign = 1;

        if (x0 > x1) {
            int tmp_x = x1;
            int tmp_y = y1;
            x1 = x0;
            x0 = tmp_x;
            y1 = y0;
            y0 = tmp_y;
            sign = -sign;
        }

        if (y0 > y1) {
            y0 ^= aa_mask;
            y0 += (y0 & aa_mask) == aa_mask ? 1 - aa_scale * 2 : 1;
            y1 = y0 + dy;
            inc_y = -1;
            sign = -sign;
        }

        int ix0 = x0 >> aa_shift;
        int ix1 = x1 >> aa_shift;
        int iy0 = y0 >> aa_shift;
        int iy1 = y1 >> aa_shift;

        int fx0 = x0 & aa_mask;
        int fx1 = x1 & aa_mask;
        int fy0 = y0 & aa_mask;
        int fy1 = y1 & aa_mask;

        int scanline_count = iy1 - iy0;
        int cover;
        int area;
        bool add_cell;

        if (scanline_count == 0 && ix0 == ix1) {
            R2DCell* cell = cells + iy0 * stride + ix0;
            // dy *= sign;
            cover = dy * sign;
            area = ((fx0 + fx1) * cover) >> area_shift;
            add_cell = (cell->generation == generation);
            cell->generation = generation;
            cell->cover = cell->cover * add_cell + cover;
            cell->area = cell->area * add_cell + area;
            return;
        }

        if (dx == 0) {
            int two_fx = fx0 + fx0;

            R2DCell* cell = &cells[iy0 * stride + ix0];
            cover = (aa_scale - fy0) * sign;
            area = (two_fx * cover) >> area_shift;
            add_cell = (cell->generation == generation);
            cell->generation = generation;
            cell->cover = cell->cover * add_cell + cover;
            cell->area = cell->area * add_cell + area;

            iy0 += inc_y;
            cover = aa_scale * sign;
            area = (two_fx * cover) >> area_shift;

            while (--scanline_count) {
                cell = &cells[iy0 * stride + ix0];
                add_cell = (cell->generation == generation);
                cell->generation = generation;
                cell->cover = cell->cover * add_cell + cover;
                cell->area = cell->area * add_cell + area;
                iy0 += inc_y;
            }

            if (fy1 != 0) {
                cell = &cells[iy0 * stride + ix0];
                cover = fy1 * sign;
                area = (two_fx * cover) >> area_shift;
                add_cell = (cell->generation == generation);
                cell->generation = generation;
                cell->cover = cell->cover * add_cell + cover;
                cell->area = cell->area * add_cell + area;
            }
            return;
        }

        int base_x = aa_scale * dx;
        int lift_x = base_x / dy;
        int rem_x = base_x % dy;
        int err_x = -dy / 2;

        int base_y = aa_scale * dy;
        int lift_y = base_y / dx;
        int rem_y = base_y % dx;
        int err_y = -dx / 2;

        int offset_x = (aa_scale - fy0) * dx;
        int delta_x = offset_x / dy;
        err_x += offset_x % dy;

        int offset_y = (aa_scale - fx0) * dy;
        int delta_y = offset_y / dx;
        err_y += offset_y % dx;

        int acc_fx = fx0;
        int acc_y = y0 + delta_y;

        fy1 = aa_scale;

        if (dx > dy) {
            // Split the edge into multiple horizontal edge spans for each vertical scanlines.
            do {
                R2DCell* scanline = &cells[iy0 * stride];

                if (scanline_count == 0) {
                    delta_x = x1 - ((ix0 << aa_shift) + acc_fx);
                    fy1 = y1 & aa_mask;
                    if (delta_x == 0)
                        continue;
                }

                int acc_fy = acc_y & aa_mask;
                int next_x = acc_fx + delta_x;
                int next_ix = ix0 + (next_x >> aa_shift);
                int has_err;

                if (next_x <= 256) {
                    R2DCell* cell = &scanline[ix0];
                    cover = (fy1 - fy0) * sign;
                    area = ((acc_fx + next_x) * cover) >> area_shift;

                    if (cell->generation == generation) {
                        cover += cell->cover;
                        area += cell->area;
                    }

                    cell->generation = generation;
                    cell->cover = cover;
                    cell->area = area;

                    if (next_x == 256) {
                        acc_y += lift_y;
                        err_y += rem_y;
                        has_err = (err_y >= 0);
                        err_y -= dx * has_err;
                        acc_y += has_err;
                    }

                    delta_x = lift_x;
                    err_x += rem_x;
                    has_err = (err_x >= 0);
                    err_x -= dy * has_err;
                    delta_x += has_err;

                    fy0 = 0;
                    acc_fx = next_x & aa_mask;
                    ix0 = next_ix;
                    iy0 += inc_y;
                    continue;
                }

                R2DCell* cell = &scanline[ix0];
                cover = (acc_fy - fy0) * sign;
                area = ((acc_fx + aa_scale) * cover) >> area_shift;

                if (cell->generation == generation) {
                    cover += cell->cover;
                    area += cell->area;
                }

                cell->generation = generation;
                cell->cover = cover;
                cell->area = area;

                ix0++;
                while (ix0 != next_ix) {
                    delta_y = lift_y;
                    err_y += rem_y;
                    has_err = (err_y >= 0);
                    err_y -= dx * has_err;
                    delta_y += has_err;
                    acc_y += delta_y;

                    cell = &scanline[ix0];
                    cover = delta_y * sign;
                    area = (aa_scale * cover) >> area_shift;

                    if (cell->generation == generation) {
                        cover += cell->cover;
                        area += cell->area;
                    }

                    cell->generation = generation;
                    cell->cover = cover;
                    cell->area = area;
                    ix0++;
                }

                acc_fx = next_x & aa_mask;
                acc_fy = acc_y & aa_mask;

                if (acc_fy != 0 || scanline_count == 0) {
                    cell = &scanline[ix0];
                    cover = (fy1 - acc_fy) * sign;
                    area = (acc_fx * cover) >> area_shift;

                    if (cell->generation == generation) {
                        cover += cell->cover;
                        area += cell->area;
                    }

                    cell->generation = generation;
                    cell->cover = cover;
                    cell->area = area;
                }

                err_y += rem_y;
                has_err = (err_y >= 0);
                err_y -= dx * has_err;
                acc_y += has_err;
                acc_y += lift_y;

                delta_x = lift_x;
                err_x += rem_x;
                has_err = (err_x >= 0);
                err_x -= dy * has_err;
                delta_x += has_err;

                fy0 = 0;
                iy0 += inc_y;
            } while (scanline_count--);
        } else {
            do {
                R2DCell* scanline = &cells[iy0 * stride];

                if (scanline_count == 0) {
                    delta_x = x1 - ((ix0 << aa_shift) + acc_fx);
                    fy1 = y1 & aa_mask;
                }

                int next_fx = acc_fx + delta_x;
                int has_err;

                if (next_fx <= 256) {
                    R2DCell* cell = &scanline[ix0];
                    cover = (fy1 - fy0) * sign;
                    area = ((acc_fx + next_fx) * cover) >> area_shift;

                    if (cell->generation == generation) {
                        cover += cell->cover;
                        area += cell->area;
                    }

                    cell->generation = generation;
                    cell->cover = cover;
                    cell->area = area;

                    if (next_fx == 256) {
                        acc_y += lift_y;
                        err_y += rem_y;
                        has_err = (err_y >= 0);
                        err_y -= dx * has_err;
                        acc_y += has_err;
                    }

                    delta_x = lift_x;
                    err_x += rem_x;
                    has_err = (err_x >= 0);
                    err_x -= dy * has_err;
                    delta_x += has_err;

                    fy0 = 0;
                    fy1 = aa_scale;
                    acc_fx = next_fx & aa_mask;
                    ix0 += next_fx >> aa_shift;
                    iy0 += inc_y;
                    continue;
                }

                acc_y &= aa_mask;

                R2DCell* cell = &scanline[ix0];
                cover = (acc_y - fy0) * sign;
                area = ((acc_fx + aa_scale) * cover) >> area_shift;

                if (cell->generation == generation) {
                    cover += cell->cover;
                    area += cell->area;
                }

                cell->generation = generation;
                cell->cover = cover;
                cell->area = area;

                ix0++;
                acc_fx = next_fx & aa_mask;

                cell = &scanline[ix0];
                cover = (fy1 - acc_y) * sign;
                area = (acc_fx * cover) >> area_shift;

                if (cell->generation == generation) {
                    cover += cell->cover;
                    area += cell->area;
                }

                cell->generation = generation;
                cell->cover = cover;
                cell->area = area;

                acc_y += lift_y;
                err_y += rem_y;
                has_err = (err_y >= 0);
                err_y -= dx * has_err;
                acc_y += has_err;

                delta_x = lift_x;
                err_x += rem_x;
                has_err = (err_x >= 0);
                err_x -= dy * has_err;
                delta_x += has_err;

                fy0 = 0;
                iy0 += inc_y;
            } while (scanline_count--);
        }
    }

    inline void clear_render_target(float r, float g, float b, float a = 1.0f) {
        clear_render_target(R2DColor(r, g, b, a));
    }

    inline void clear_render_target(int r, int g, int b, int a = 255) {
        rt_->clear_raw(r << rt_bitpos_.r | g << rt_bitpos_.g | b << rt_bitpos_.b |
                       a << rt_bitpos_.a);
    }

    inline void clear_render_target(const R2DColor& color) {
        rt_->clear_raw(color.to_bytes(rt_->format_));
    }

    inline void clear_render_target(R2DColor8 color,
                                    R2DPixelFormat format = R2DPixelFormat::RGBA8) {
        if (format == rt_->format_) {
            rt_->clear_raw(color);
            return;
        }
        R2DColorBitPos rgba_bitpos = color_bitpos_(format);
        uint32_t r = (color >> rgba_bitpos.r) & 0xFF;
        uint32_t g = (color >> rgba_bitpos.g) & 0xFF;
        uint32_t b = (color >> rgba_bitpos.b) & 0xFF;
        uint32_t a = (color >> rgba_bitpos.a) & 0xFF;
        rt_->clear_raw(r << rt_bitpos_.r | g << rt_bitpos_.g | b << rt_bitpos_.b |
                       a << rt_bitpos_.a);
    }

    inline void render_raster() {
        switch (blend_mode_) {
            case R2DBlendMode::SrcOver:
                render_raster_solid<R2DBlendSrcOver>();
                break;
            case R2DBlendMode::SrcAtop:
                render_raster_solid<R2DBlendSrcAtop>();
                break;
            case R2DBlendMode::SrcIn:
                render_raster_solid<R2DBlendSrcIn>();
                break;
            case R2DBlendMode::SrcOut:
                render_raster_solid<R2DBlendSrcOut>();
                break;
            case R2DBlendMode::SrcCopy:
                break;
            default:
                R2D_UNREACHABLE();
        }
    }

    // Discard content in the raster. Should be used after drawing.
    inline void discard_raster() {
        if (!raster_)
            return;
        if (++raster_->current_gen_ == 0)
            raster_->clear();
    }

    template <typename BlendFnT>
    void render_raster_solid() {
        BlendFnT blend_fn{};
        R2DColor8 src = source_->solid;
        uint32_t src_color = 0xFFFFFF & src;
        uint32_t src_alpha = (0xFF000000 & src) >> 24;
        uint32_t rt_width = rt_->width_;
        uint32_t raster_stride = raster_->stride_;
        R2DPixel* image_data = (R2DPixel*)rt_->data_;
        R2DCell* cells = raster_->cells_;
        uint32_t current_raster_gen = raster_->current_gen_;
        uint32_t render_width = r2d_min(rt_width, raster_->width_);
        uint32_t render_height = r2d_min(rt_->height_, raster_->height_);
        uint32_t bitpos_r = rt_bitpos_.r;
        uint32_t bitpos_g = rt_bitpos_.g;
        uint32_t bitpos_b = rt_bitpos_.b;
        uint32_t bitpos_a = rt_bitpos_.a;

        for (uint32_t y = 0; y < render_height; y++) {
            R2DColor8* image_row = &image_data[y * rt_width];
            R2DCell* raster_row = &cells[y * raster_stride];
            int effective_cover = 0;

            for (uint32_t x = 0; x < render_width; x++) {
                R2DCell* cell = raster_row + x;
                R2DColor8 dst = image_row[x];
                int cover = 0;
                int area = 0;

                if (cell->generation >= current_raster_gen) {
                    cover = cell->cover;
                    area = cell->area;
                }

                effective_cover += cover;
                int raster_mask = effective_cover - area;
                if (raster_mask < 0)
                    raster_mask = -raster_mask;
                if (raster_mask > 255)
                    raster_mask = 255;

                uint32_t msk_alpha = r2d_fpmul(raster_mask, src_alpha);

                // Un-swizzle color from their destination format to RGBA
                uint32_t dst_r = (dst >> bitpos_r) & 0xFF;
                uint32_t dst_g = (dst >> bitpos_g) & 0xFF;
                uint32_t dst_b = (dst >> bitpos_b) & 0xFF;
                uint32_t dst_a = (dst >> bitpos_a) & 0xFF;
                R2DColor8 dst_color = dst_r | (dst_g << 8) | (dst_b << 16);

                uint32_t out_alpha;
                R2DColor8 out_color = blend_fn(src_color, msk_alpha, dst_color, dst_a, out_alpha);

                // Swizzle the blending result back to their destination format
                uint32_t out_r = (out_color >> 0) & 0xFF;
                uint32_t out_g = (out_color >> 8) & 0xFF;
                uint32_t out_b = (out_color >> 16) & 0xFF;

                image_row[x] = (out_r << bitpos_r) | (out_g << bitpos_g) | (out_b << bitpos_b) |
                               (out_alpha << bitpos_a);
            }
        }
    }

    void draw_rect() noexcept {}

    void draw_rect_filled(float x, float y, float w, float h) noexcept {
        float x1 = x + w;
        float y1 = y + h;
        plot_move_to(x, y);
        plot_line_to(x1, y);
        plot_line_to(x1, y1);
        plot_line_to(x, y1);
        plot_line_to(x, y);
        plot_close();
        render_raster();
        discard_raster();
    }

    void draw_triangle_filled(const R2DPoint& v0, const R2DPoint& v1, const R2DPoint& v2) noexcept {
        plot_move_to(v0.x, v0.y);
        plot_line_to(v1.x, v1.y);
        plot_line_to(v2.x, v2.y);
        plot_line_to(v0.x, v0.y);
        plot_close();
        render_raster();
        discard_raster();
    }

    void draw_circle(float cx, float cy, float radius) noexcept {}

    inline void draw_line(const R2DPoint& v0, const R2DPoint& v1) noexcept {
        add_line(v0, v1);
        render_raster();
        discard_raster();
    }

    void draw_path(const R2DPath& path) noexcept {}

    void draw_path_filled(const R2DPath& path) noexcept {
        // TODO
        render_raster();
        discard_raster();
    }

    void draw_polygon(const R2DPoint* verts, size_t count, size_t first_vertex = 0) noexcept {
        add_polygon(verts, count, first_vertex);
        render_raster();
        discard_raster();
    }

    void draw_polyline(const R2DPoint* verts, size_t count, size_t first_vertex = 0,
                       bool close = true) noexcept {
        add_polyline(verts, count, first_vertex, close);
        render_raster();
        discard_raster();
    }

    R2D_FORCEINLINE
    constexpr R2DColorBitPos color_bitpos_(R2DPixelFormat format) noexcept {
        // Color bit position table for format conversion
        switch (format) {
            case R2DPixelFormat::RGBA8:
                return {0, 8, 16, 24};
            case R2DPixelFormat::ARGB8:
                return {8, 16, 24, 0};
            case R2DPixelFormat::BGRA8:
                return {8, 16, 24, 0};
            default:
                R2D_UNREACHABLE();
        }
        return {};
    }
};