#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include "geometry.h"

struct Material {
    Material(const Vec3f &color) : diffuse_color(color) {}
    Material() : diffuse_color() {}
    Vec3f diffuse_color;
};

struct Sphere {
        Vec3f center;
        float radius;
        Material material;
        Sphere(const Vec3f &c, const float &r, const Material &m) : center(c), radius(r), material(m) {}

        bool ray_intersect(const Vec3f &orig, const Vec3f &dir, float &t0) const {
            Vec3f L = center - orig;
            float t = L*dir;
            float d2 = L*L - t*t;
            if (d2 > radius*radius) return false;
            float x = sqrtf(radius*radius - d2);
            t0       = t - x;
            float t1 = t + x;
            if (t0 < 0) t0 = t1;
            if (t0 < 0) return false;
            return true;
        }
    };


Vec3f cast_ray(const Vec3f &orig, const Vec3f &dir, const Sphere &sphere) {
    float sphere_dist = std::numeric_limits<float>::max();
    if (!sphere.ray_intersect(orig, dir, sphere_dist)) {
        return Vec3f(0.2, 0.7, 0.8); // background color
    }
    return Vec3f(0.4, 0.4, 0.3);
}

void render(const Sphere &sphere){
    
    const int HEIGHT = 768;
    const int WIDTH = 1024;
    const int FOV = M_PI/2.;
    std::vector<Vec3f> framebuffer(WIDTH*HEIGHT);
    for (size_t j = 0; j<HEIGHT; j++) {
        for (size_t i = 0; i<WIDTH; i++) {
            float x =  (2*(i + 0.5)/(float)WIDTH  - 1)*tan(FOV/2.)*WIDTH/(float)HEIGHT;
            float y = -(2*(j + 0.5)/(float)HEIGHT - 1)*tan(FOV/2.);
            Vec3f dir = Vec3f(x, y, -1).normalize();
            framebuffer[i+j*WIDTH] = cast_ray(Vec3f(0,0,0), dir, sphere);
        }
    }

    std::ofstream ofs;
    ofs.open("./out.ppm");
    ofs << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";
    for (size_t i = 0; i < HEIGHT*WIDTH; ++i) {
        for (size_t j = 0; j<3; j++) {
            ofs << (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
        }
    }
    ofs.close();
}



int main(){
    Sphere sphere(Vec3f(-3, 0, -16), 2);
    render(sphere);
    return 0;
}
