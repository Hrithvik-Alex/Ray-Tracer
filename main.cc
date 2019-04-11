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


bool scene_intersect(const Vec3f &orig, const Vec3f &dir, const std::vector<Sphere> &spheres, Vec3f &hit, Vec3f &N, Material &material) {
    float spheres_dist = std::numeric_limits<float>::max();
    for (size_t i=0; i < spheres.size(); i++) {
        float dist_i;
        if (spheres[i].ray_intersect(orig, dir, dist_i) && dist_i < spheres_dist) {
            spheres_dist = dist_i;
            hit = orig + dir*dist_i;
            N = (hit - spheres[i].center).normalize();
            material = spheres[i].material;
        }
    }
    return spheres_dist<1000;
}

Vec3f cast_ray(const Vec3f &orig, const Vec3f &dir, const std::vector<Sphere> &spheres) {
    Vec3f point, N;
    Material material;

    if (!scene_intersect(orig, dir, spheres, point, N, material)) {
        return Vec3f(0.1, 0.7, 0.9); // background color
    }
    return material.diffuse_color;
}

void render(const std::vector<Sphere> &spheres){
    
    const int HEIGHT = 768;
    const int WIDTH = 1024;
    const int FOV = M_PI/2.;
    std::vector<Vec3f> framebuffer(WIDTH*HEIGHT);
    for (size_t j = 0; j<HEIGHT; j++) {
        for (size_t i = 0; i<WIDTH; i++) {
            float x =  (2*(i + 0.5)/(float)WIDTH  - 1)*tan(FOV/2.)*WIDTH/(float)HEIGHT;
            float y = -(2*(j + 0.5)/(float)HEIGHT - 1)*tan(FOV/2.);
            Vec3f dir = Vec3f(x, y, -1).normalize();
            framebuffer[i+j*WIDTH] = cast_ray(Vec3f(0,0,0), dir, spheres);
        }
    }

    std::ofstream ofs;
    ofs.open("./spheres.ppm");
    ofs << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";
    for (size_t i = 0; i < HEIGHT*WIDTH; ++i) {
        for (size_t j = 0; j<3; j++) {
            ofs << (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
        }
    }
    ofs.close();
}



int main(){
    Material      ivory(Vec3f(0.3, 0.1, 0.3));
    Material red_rubber(Vec3f(0.4, 0.1, 0.2));

    std::vector<Sphere> spheres;
    spheres.push_back(Sphere(Vec3f(-3,    0,   -16), 2,      ivory));
    spheres.push_back(Sphere(Vec3f(-1.0, -1.5, -12), 2, red_rubber));
    spheres.push_back(Sphere(Vec3f( 1.5, -0.5, -18), 3, red_rubber));
    spheres.push_back(Sphere(Vec3f( 7,    5,   -18), 4,      ivory));

    render(spheres);
    return 0;
}
