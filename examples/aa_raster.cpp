#include "example_app_sdl2.h"
#include <optional>
#include <vector>

struct AARasterExample : public R2DExampleApp {
    R2DContext context{};
    R2DRaster raster{};
    R2DRaster raster_sm{};
    R2DImage image_sm{};
    std::vector<R2DPoint> points{};
    std::optional<uint32_t> point_idx;
    static constexpr uint32_t pixel_size = 16;
    static constexpr uint32_t mul_pixel_size = 1;

    AARasterExample() : R2DExampleApp("AA Raster") {}

    void on_init() override {
        points.resize(3);
        points[0] = {1.0f * mul_pixel_size, 5.0f * mul_pixel_size};
        points[1] = {5.0f * mul_pixel_size, 5.0f * mul_pixel_size};
        points[2] = {5.0f * mul_pixel_size, 10.0f * mul_pixel_size};
    }

    void on_resize(int w, int h) override {
        raster.init(w, h);
        image_sm.init(w / pixel_size, h / pixel_size, R2DPixelFormat::RGBA8);
        raster_sm.init(w / pixel_size, h / pixel_size);
    }

    void on_mouse_move(int x, int y) override {
        if (point_idx) {
            R2DPoint mouse_pos{(float)x, (float)y};
            mouse_pos.x = mouse_pos.x;
            mouse_pos.y = mouse_pos.y;
            points[*point_idx].x = (float)x / (float)pixel_size;
            points[*point_idx].y = (float)y / (float)pixel_size;
        }
    }

    void on_mouse_down(int x, int y, int button) override {
        static constexpr float pixel_size_f = (float)pixel_size;
        if (button == 1) {
            R2DPoint mouse_pos{(float)x, (float)y};
            for (int i = 0; i < points.size(); i++) {
                if ((mouse_pos - points[i] * pixel_size_f).length_sq() < 25.0f) {
                    point_idx = i;
                }
            }
        }
        if (button == 3) {
            points.push_back({(float)x / pixel_size, (float)y / pixel_size});
        }
    }

    void on_mouse_up(int x, int y, int button) override {
        if (button == 1) {
            point_idx.reset();
        }
    }

    void on_draw(R2DImage& out_image) override {
        static constexpr float pixel_size_f = (float)pixel_size;
        R2DSource source;
        source.type = R2DSourceType::Solid;
        source.solid = R2DColor(255, 255, 255).to_rgba8();

        R2DSource source2;
        source2.type = R2DSourceType::Solid;
        source2.solid = 0xFF277FFF; // R2DColor(255, 0, 255).to_rgba8();

        static const R2DPoint line_points[] = {
            R2DPoint{30.0f, 200.0f - 40.0f},
            R2DPoint{150.0f, 20.0f - 40.0f},
            R2DPoint{200.0f, 50.0f - 40.0f},
            R2DPoint{160.0f, 60.0f - 40.0f},
        };

        R2DRect image_rect = image_sm.rect();
        context.set_render_target(&image_sm);
        context.set_raster(&raster_sm);
        context.set_clip_rect(&image_rect);
        context.set_source(&source);
        context.clear_render_target(R2DColor(0.0f, 0.0f, 0.0f));
        // context.draw_triangle_filled(points[0], points[1], points[2]);
        context.draw_polygon(points.data(), points.size(), 0);

        // context.set_source(&source);
        R2DRect out_image_rect = out_image.rect();
        context.set_render_target(&out_image);
        context.set_clip_rect(&out_image_rect);
        context.set_raster(&raster);
        context.set_source(&source);

        R2DColor8* pixel_data = (R2DColor8*)image_sm.raw_data();
        for (uint32_t y = 0; y < image_sm.height(); y++) {
            for (uint32_t x = 0; x < image_sm.width(); x++) {
                source.solid = pixel_data[y * image_sm.width() + x];
                context.draw_rect_filled(x * pixel_size_f, y * pixel_size_f, pixel_size_f,
                                         pixel_size_f);
            }
        }

        context.set_source(&source2);
        context.set_line_thickness(1.0f);
        float x0 = points[0].x * pixel_size_f;
        float y0 = points[0].y * pixel_size_f;
        for (size_t i = 1; i < points.size(); i++) {
            float x1 = points[i].x * pixel_size_f;

            float y1 = points[i].y * pixel_size_f;
            context.add_line(R2DPoint{x0, y0}, R2DPoint{x1, y1});
            x0 = x1;
            y0 = y1;
        }
        context.add_line(R2DPoint{x0, y0}, points[0] * pixel_size_f);
        context.render_raster();
        context.discard_raster();

        for (auto& point : points) {
            context.draw_rect_filled(point.x * pixel_size_f - 2.5f,
                                     point.y * pixel_size_f - 2.5f, 5.0f, 5.0f);
        }
    }
};

int main() {
    AARasterExample raster;
    return raster.run();
}