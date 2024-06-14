#include "r2d.hpp"
#include "r2d_surface.hpp"
#include "stb_image_write.h"
#include <chrono>
#include <cstdio>
#include <iostream>
#include <numeric>
#include <random>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

int main() {
    R2DMatrix mat;
    mat(0, 0) = 0.0f;

    R2DImage image;
    image.init(640, 480, R2DPixelFormat::RGBA8);

    R2DRaster raster;
    raster.init(640, 480);

    if (image)
        std::cout << "Test" << std::endl;

    /*std::cin.get();

    double avg = 0.0;
    for (int i = 0; i < 100; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        image.clear_raw(0xFF000000);
        auto end = std::chrono::high_resolution_clock::now();
        double time = std::chrono::duration<double, std::milli>(end - start).count();
        avg += time;
        std::printf("%.16f\n", time);
    }
    std::printf("Avg: %.16f", avg / 100.0);*/
    // std::cout << std:: << std::chrono::duration<double>(end - start).count() << std::endl;

    R2DSource source;
    source.type = R2DSourceType::Solid;
    source.solid = R2DColor(255, 220, 1).to_rgba8();

    R2DSource source2;
    source2.type = R2DSourceType::Solid;
    source2.solid = R2DColor(104, 199, 232, 255).to_rgba8();

    R2DPoint verts[] = {
        {50.0f, 50.0f},
        {120.3f, 65.7f},
        {150.0f, 100.0f},
        {70.0f, 140.0f},
    };

    std::vector<R2DPoint> circle_points;
    
    /*for (int i = 0; i < 10; i++) {
        static constexpr float pi2 = 3.14159265359f * 2.0f;
        float angle = (float)i / 10.0f * pi2;
        float pos_x = std::cos(angle) * 100.0f + 200.15f;
        float pos_y = std::sin(angle) * 100.0f + 200.15f;
        circle_points.push_back(R2DPoint{pos_x, pos_y});
    }*/

    /*std::random_device rd;
    std::uniform_real_distribution<float> rnd_x(2.0f, 1918.0f);
    std::uniform_real_distribution<float> rnd_y(2.0f, 1078.0f);

    for (int i = 0; i < 100; i++) {
        float pos_x = rnd_x(rd);
        float pos_y = rnd_y(rd);
        circle_points.push_back(R2DPoint{pos_x, pos_y});
    }*/

    R2DContext context;
    context.set_render_target(&image);
    context.set_raster(&raster);
    context.set_source(&source);
    context.clear_render_target(0, 0, 0, 255);
    // context.add_edge(169.098282f, 295.105652f, 119.098297f, 258.778503f);
    // context.add_edge(circle_points[3].x, circle_points[3].y, circle_points[4].x,
    //                  circle_points[4].y);

    // context.add_polygon(circle_points.data(), (uint32_t)circle_points.size());

    // context.add_edge(-10.0f, 1.0f, 10.0f, 5.0f);
    // context.add_edge(1.0f, 1.0f, 10.f, 10.0f);
    /*context.plot_move_to(-10.0f, 1.0f);
    context.plot_line_to(10.0f, 5.0f);
    context.plot_line_to(-10.0f, 20.0f);

    context.plot_move_to(10.0f, 30.0f);
    context.plot_line_to(-10.0f, 40.0f);
    context.plot_line_to(10.0f, 50.0f);
    context.plot_line_to(10.0f, 30.0f);*/

    /*(29.84, -19.44), (-4.94, -21.06), (26.01, -8.69), (18.75, -6.37), (-15.23, -11.68),
        (-17.90, -31.99),
        (-3.91, -34.49)*/

    /*context.plot_move_to(29.84, 19.44);
    context.plot_line_to(-4.94, 21.06);
    context.plot_line_to(26.01, 8.69);
    context.plot_line_to(18.75, 6.37);
    context.plot_line_to(-15.23, 11.68);
    context.plot_line_to(-17.90, 31.99);
    context.plot_line_to(-3.91, 34.49);
    context.plot_line_to(29.84, 19.44);
    context.plot_close();*/

    /*(-6.14, -26.41), (-4.94, -21.06), (29.00, -16.65), (18.75, -6.37), (-15.23, -11.68),
        (-12.07, -39.81),
        (29.94, -27.52)*/

    /*context.plot_move_to(-6.14, 26.41);
    context.plot_line_to(-4.94, 21.06);
    context.plot_line_to(29.00, 16.65);
    context.plot_line_to(18.75, 6.37);
    context.plot_line_to(-15.23, 11.68);
    context.plot_line_to(-12.07, 39.81);
    context.plot_line_to(29.94, 27.52);
    context.plot_line_to(-6.14, 26.41);
    context.plot_close();*/

    /*(-2.80, -11.67), (13.95, -5.89), (-10.54, -6.06), (-9.90, -23.81), (14.94, -22.89),
        (-3.36, -18.71), (14.12, -15.71),
        (15.93, -11.98)*/

    /*context.plot_move_to(-2.80, 11.67);
    context.plot_line_to(13.95, 5.89);
    context.plot_line_to(-10.54, 6.06);
    context.plot_line_to(-9.90, 23.81);
    context.plot_line_to(14.94, 22.89);
    context.plot_line_to(-3.36, 18.71);
    context.plot_line_to(14.12, 15.71);
    context.plot_line_to(15.93, 11.98);
    context.plot_line_to(-2.80, 11.67);
    context.plot_close();*/

    /*(11.44, -9.07), (10.48, -4.40), (-10.54, -6.06), (-9.90, -23.81), (10.85, -28.83),
        (16.20, -24.28), (14.37, -18.56),
        (14.07, -13.84)*/

    /*context.plot_move_to(11.44, 9.07);
    context.plot_line_to(10.48, 4.40);
    context.plot_line_to(-10.54, 6.06);
    context.plot_line_to(-9.90, 23.81);
    context.plot_line_to(10.85, 28.83);
    context.plot_line_to(16.20, 24.28);
    context.plot_line_to(14.37, 18.56);
    context.plot_line_to(14.07, 13.84);
    context.plot_line_to(11.44, 9.07);
    context.plot_close();*/

    /*context.plot_move_to(650.0f, 00.0f);
    context.plot_line_to(600.0f, 20.0f);
    context.plot_line_to(670.0f, 25.0f);
    context.plot_line_to(650.0f, 00.0f);
    context.plot_close();
    context.render_raster();
    context.discard_raster();*/

    context.set_blend_mode(R2DBlendMode::SrcOver);
    context.draw_rect_filled(0.0f, 0.0f, 100.0f, 100.0f);
    context.set_source(&source2);
    context.draw_rect_filled(50.0f, 50.0f, 100.0f, 100.0f);
    context.draw_triangle_filled(R2DPoint{200.0f, 200.0f}, R2DPoint{300.0f, 280.0f},
                                 R2DPoint{150.0f, 250.0f});
    // context.draw_line(R2DPoint{30.0f, 200.0f}, R2DPoint{150.0f, 20.0f});

    const R2DPoint line_points[] = {
        R2DPoint{30.0f, 200.0f},
        R2DPoint{150.0f, 20.0f},
        R2DPoint{200.0f, 50.0f},
        R2DPoint{160.0f, 60.0f},
    };

    context.set_source(&source);
    context.set_line_thickness(5.0f);
    context.set_line_join(R2DLineJoin::Miter);
    context.draw_polyline(line_points, 4, 0);

    context.set_source(&source2);
    context.set_line_thickness(1.0f);
    context.draw_polyline(line_points, 4, 0);

    std::vector<R2DPoint> spiral;

    for (int i = 0; i < 100; i++) {
        static constexpr float pi2 = 3.14159265359f * 10.0f;
        //float period = 100.0f / 2.0f;
        float angle = (float)i / 100.0f * pi2;
        float pos_x = std::cos(angle) * (float)i + 300.15f;
        float pos_y = std::sin(angle) * (float)i + 300.15f;
        spiral.push_back(R2DPoint{pos_x, pos_y});
    }

    context.set_source(&source);
    context.set_line_thickness(3.0f);
    context.set_line_join(R2DLineJoin::Miter);
    context.draw_polyline(spiral.data(), spiral.size());

    // context.set_blend_mode(R2DBlendMode::SrcOut);
    /*context.draw_triangle_filled(R2DPoint{10.0f, 10.0f}, R2DPoint{50.0f, 15.0f},
                                 R2DPoint{35.0f, 20.0f});*/

    stbi_write_bmp("test.bmp", image.width(), image.height(), 4, image.raw_data());
}