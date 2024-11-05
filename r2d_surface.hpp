#pragma once

#include "r2d.hpp"

#if defined(R2D_SURFACE_GDI)
#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
#endif
#elif defined(R2D_SURFACE_SDL2)
#include <SDL_surface.h>
#endif

#ifdef R2D_SURFACE_GDI
static void r2d_get_color_mask_gdi(R2DPixelFormat pixel_format, BITMAPV4HEADER* bmi) {
    switch (pixel_format) {
        case R2DPixelFormat::RGBA8:
        case R2DPixelFormat::RGBX8:
            bmi->bV4RedMask = 0x0000FF;
            bmi->bV4BlueMask = 0x00FF00;
            bmi->bV4AlphaMask = 0xFF0000;
            break;
        case R2DPixelFormat::ARGB8:
            bmi->bV4RedMask = 0x0000FF00;
            bmi->bV4BlueMask = 0x00FF0000;
            bmi->bV4AlphaMask = 0xFF000000;
            break;
        case R2DPixelFormat::BGRA8:
        case R2DPixelFormat::BGRX8:
            bmi->bV4RedMask = 0xFF0000;
            bmi->bV4BlueMask = 0x00FF00;
            bmi->bV4AlphaMask = 0x0000FF;
            break;
        default:
            R2D_UNREACHABLE();
    }
}

static void r2d_copy_image_gdi(HDC hdc, const RECT& dst, const R2DImage& src_image,
                               const RECT& src) {
    assert(hdc);

    // We use BITMAPV4HEADER so that we can have the color mask field
    BITMAPV4HEADER bmi{};
    bmi.bV4Size = sizeof(BITMAPINFOHEADER);
    bmi.bV4Width = src_image.width_;
    bmi.bV4Height = src_image.height_;
    bmi.bV4Planes = 1;
    bmi.bV4BitCount = 32;
    bmi.bV4V4Compression = BI_BITFIELDS;
    r2d_get_color_mask_gdi(src_image.format_, &bmi);

    uint32_t dst_width = dst.right - dst.left;
    uint32_t dst_height = dst.bottom - dst.top;
    uint32_t src_width = src.right - src.left;
    uint32_t src_height = src.bottom - src.top;
    SetDIBitsToDevice(hdc, dst.left, dst.top, dst_width, dst_height, src.left, src.top, src_width,
                      src_height, src_image.data_, (const BITMAPINFO*)&bmi, DIB_RGB_COLORS);
}

static void r2d_blit_image_gdi(HDC hdc, const RECT& dst, const R2DImage& src_image, const RECT& src,
                               int stretch_blt_mode = COLORONCOLOR, int rop = SRCCOPY) {
    assert(hdc);

    BITMAPV4HEADER bmi{};
    bmi.bV4Size = sizeof(BITMAPINFOHEADER);
    bmi.bV4Width = src_image.width_;
    bmi.bV4Height = src_image.height_;
    bmi.bV4Planes = 1;
    bmi.bV4BitCount = 32;
    bmi.bV4V4Compression = BI_BITFIELDS;
    r2d_get_color_mask_gdi(src_image.format_, &bmi);

    uint32_t dst_width = dst.right - dst.left;
    uint32_t dst_height = dst.bottom - dst.top;
    uint32_t src_width = src.right - src.left;
    uint32_t src_height = src.bottom - src.top;

    int old_blt_mode = GetStretchBltMode(hdc);
    SetStretchBltMode(hdc, stretch_blt_mode);
    StretchDIBits(hdc, dst.left, dst.top, dst_width, dst_height, src.left, src.top, src_width,
                  src_height, src_image.data_, (const BITMAPINFO*)&bmi, DIB_RGB_COLORS, rop);
    SetStretchBltMode(hdc, old_blt_mode);
}

static void r2d_gdi_blit_image(HDC hdc, const RECT& dst, const R2DImage& src_image,
                               int stretch_blt_mode = COLORONCOLOR, int rop = SRCCOPY) {
    RECT src = {0, 0, (LONG)src_image.width_, (LONG)src_image.height_};
    r2d_gdi_blit_image(hdc, dst, src_image, src, stretch_blt_mode, rop);
}

#endif

static void r2d_copy_image_d3d11() {
}

#ifdef R2D_SURFACE_SDL2
static void r2d_sdl2_render_blit_image(SDL_Surface* dst_surface, const R2DImage& src_image,
                                       bool update_window) {
    assert(dst_surface->w == src_image.width_);
    assert(dst_surface->h == src_image.height_);
    auto pixels = (R2DColor8*)dst_surface->pixels;
    uint32_t r_shift = dst_surface->format->Rshift;
    uint32_t g_shift = dst_surface->format->Gshift;
    uint32_t b_shift = dst_surface->format->Bshift;
    uint32_t a_shift = dst_surface->format->Ashift;
    uint32_t a_mask = dst_surface->format->Ashift;
    R2DColorBitShift src_bitshift = r2d_color_bitshift(src_image.format_);
    uint32_t src_color_shift = 24 - src_bitshift.a;
    R2DColor8* src_color = (R2DColor8*)src_image.raw_data();

    if (a_mask == 0) {
        for (int i = 0; i < dst_surface->w * dst_surface->h; i++) {
            uint32_t col = src_color[i];
            uint32_t rgb = (col >> src_color_shift) & 0x00FFFFFF;
            uint32_t premul = r2d_rgb_alphamult(rgb, (col >> src_bitshift.a) & 0xFF);
            uint32_t src_r = (premul >> src_bitshift.r) & 0xFF;
            uint32_t src_g = (premul >> src_bitshift.g) & 0xFF;
            uint32_t src_b = (premul >> src_bitshift.b) & 0xFF;
            pixels[i] = (src_r << r_shift) | (src_g << g_shift) | (src_b << b_shift);
        }
    } else {
        for (int i = 0; i < dst_surface->w * dst_surface->h; i++) {
            uint32_t col = src_color[i];
            uint32_t src_r = (col >> src_bitshift.r) & 0xFF;
            uint32_t src_g = (col >> src_bitshift.g) & 0xFF;
            uint32_t src_b = (col >> src_bitshift.b) & 0xFF;
            uint32_t src_a = (col >> src_bitshift.a) & 0xFF;
            pixels[i] =
                (src_r << r_shift) | (src_g << g_shift) | (src_b << b_shift) | (src_a << a_shift);
        }
    }
}
#endif