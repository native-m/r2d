#include "example_app_sdl2.h"
#include <iostream>

R2DExampleApp::R2DExampleApp(const char* name, int w, int h) : name_(name), width_(w), height_(h) {
}

int R2DExampleApp::run() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return -1;
    }

    SDL_Window* window =
        SDL_CreateWindow(name_, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width_, height_, 0);

    if (!window) {
        SDL_Quit();
        return -1;
    }

    on_init();

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    R2DImage image;
    image.init(width_, height_, R2DPixelFormat::RGBA8);
    image.clear(R2DColor(0, 0, 0));
    on_resize(width_, height_);

    running_ = true;
    SDL_Event event{};
    while (running_) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running_ = false;
                    break;
                case SDL_MOUSEMOTION: {
                    on_mouse_move(event.motion.x, event.motion.y);
                    break;
                }
                case SDL_MOUSEBUTTONDOWN: {
                    on_mouse_down(event.button.x, event.button.y, event.button.button);
                    break;
                }
                case SDL_MOUSEBUTTONUP: {
                    on_mouse_up(event.button.x, event.button.y, event.button.button);
                    break;
                }
                case SDL_WINDOWEVENT: {
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_RESIZED:
                            on_resize(width_, height_);
                            break;
                    }
                    break;
                }
            }
        }

        on_draw(image);
        SDL_LockSurface(surface);
        r2d_sdl2_render_blit_image(surface, image, true);
        SDL_UnlockSurface(surface);
        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
