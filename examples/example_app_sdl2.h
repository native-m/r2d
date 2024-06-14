#pragma once

#define R2D_SURFACE_SDL2

#include <Windows.h>
#include <r2d.hpp>
#include <r2d_surface.hpp>
#include <SDL2/SDL.h>

#undef main

struct R2DExampleApp {
    SDL_Window* window_{};
    const char* name_ = "Example App";
    int width_{};
    int height_{};
    bool running_{};

    R2DExampleApp(const char* name, int w = 640, int h = 480) :
        name_(name), width_(w), height_(h) {}

    int run() {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            return -1;
        }

        SDL_Window* window = SDL_CreateWindow(name_, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                              width_, height_, 0);

        if (!window) {
            SDL_Quit();
            return -1;
        }

        SDL_Surface* surface = SDL_GetWindowSurface(window);

        SDL_LockSurface(surface);
        auto pixels = (R2DColor8*)surface->pixels;
        for (int i = 0; i < surface->w * surface->h; i++)
            pixels[i] = 0xFFFFFFFF;
        SDL_UnlockSurface(surface);

        r2d_sdl2_render_blit_image(surface, true);
        SDL_UpdateWindowSurface(window);

        running_ = true;
        SDL_Event event{};
        while (running_) {
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_QUIT:
                        running_ = false;
                        break;
                }
            }
        }

        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    virtual void on_init() {}

    virtual void on_draw() {}
};