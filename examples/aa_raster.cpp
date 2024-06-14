#include "example_app_sdl2.h"

struct AARaster : public R2DExampleApp {
    R2DContext context;

    AARaster() : R2DExampleApp("AA Raster") {}

    void on_init() {
        
    }
};

int main() {
    AARaster raster;
    return raster.run();
}