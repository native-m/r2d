#pragma once

#define R2D_SURFACE_SDL2

#include "../r2d_surface.hpp"
#include <SDL.h>
#include <Windows.h>
#include <r2d.hpp>

#undef main

struct R2DExampleApp {
    SDL_Window* window_{};
    const char* name_ = "Example App";
    int width_{};
    int height_{};
    bool running_{};

    R2DExampleApp(const char* name, int w = 640, int h = 480);
    int run();
    virtual void on_init() {}
    virtual void on_resize(int w, int h) {}
    virtual void on_mouse_move(int x, int y) {}
    virtual void on_mouse_down(int x, int y, int button) {}
    virtual void on_mouse_up(int x, int y, int button) {}
    virtual void on_draw(R2DImage& out_image) {}
};