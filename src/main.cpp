#include <iostream>
#include <pthread.h>
#include <vector>
#include "SDL.h"
#include "aabb.h"
#include "camera.h"
#include "colour.h"
#include "config.h"
#include "display.h"
#include "intersection.h"
#include "light.h"
#include "mesh.h"
#include "sphere.h"
#include "kdtree.h"

#define RGBA(r, g, b, a) ((r) << 24 | (g) << 16 | (b) << 8 | (a))

#define TRACE_PIXEL   1
#define TRACE_PIXEL_X (640 / 2 - 10)
#define TRACE_PIXEL_Y (480 / 2)

struct render_input_t
{
    uint32_t        width;
    uint32_t        height;
    uint32_t        xstart;
    uint32_t        xend;
    uint32_t        ystart;
    uint32_t        yend;
    uint32_t       *framebuffer;
    pthread_cond_t  &cond;
    pthread_mutex_t &mutex;

    render_input_t(uint32_t width,
                   uint32_t height,
                   uint32_t xstart,
                   uint32_t xend,
                   uint32_t ystart,
                   uint32_t yend,
                   uint32_t *framebuffer,
                   pthread_cond_t &cond,
                   pthread_mutex_t &mutex)
    : width(width)
    , height(height)
    , xstart(xstart)
    , xend(xend)
    , ystart(ystart)
    , yend(yend)
    , framebuffer(framebuffer)
    , cond(cond)
    , mutex(mutex)
    {}
};

void *render(void *input)
{
    uint32_t width = ((render_input_t*)input)->width;
    uint32_t height = ((render_input_t*)input)->height;
    uint32_t xstart = ((render_input_t*)input)->xstart;
    uint32_t ystart = ((render_input_t*)input)->ystart;
    uint32_t xend   = ((render_input_t*)input)->xend;
    uint32_t yend   = ((render_input_t*)input)->yend;
    uint32_t *framebuffer = ((render_input_t*)input)->framebuffer;

    pthread_mutex_lock(&((render_input_t*)input)->mutex);
    pthread_cond_signal(&((render_input_t*)input)->cond);
    pthread_mutex_unlock(&((render_input_t*)input)->mutex);

    glm::vec3 pos     = {0, 0, 3};
    glm::vec3 forward = {0, 0, -1};
    glm::vec3 right   = {1, 0,  0};
    float cam_width   = 1.0f;
    float cam_height  = 1.0f;

    Sphere sphere(glm::vec3(0, 0, -1), 0.5);

    Camera *camera = (Camera*) new PerspectiveCamera(pos, forward, right, 60.0f);
    std::vector<Object*> objects;
    std::vector<Light*> lights;
    glm::vec3 light_data[] = {
        {1, 2, 0},
        {1, 1, 1}
    };

    PointLight *light = new PointLight(light_data[0], light_data[1]);

#if 1
    std::string filename = PLY_DIRECTORY "/isosphere.ply";
#elif 0
    std::string filename = PLY_DIRECTORY "/plane.ply";
#else
    std::string filename = PLY_DIRECTORY "/bunny.ply";
#endif

#if 1
    PlyMesh temp(filename);
    KDMesh  kdmesh(filename, 10, 1);
    std::cout << kdmesh.node.find(idx_vec3(585, 586, 587)) << std::endl;
#else
    KDMesh  temp(filename, 10, 1);
#endif

    objects.push_back(&temp);

    Intersections::Intersection info;
#if TRACE_PIXEL
    auto i = TRACE_PIXEL_Y;
    auto j = TRACE_PIXEL_X;
    Ray initial_ray = camera->Emit(j / (float)width, (height - i) / (float)height);
    for(auto object : objects)
    {
        if(object->Intersects(initial_ray, info))
        {
            std::cout << info.triangle.x << " " << info.triangle.y << " " << info.triangle.z << std::endl;
            framebuffer[i * width + j] = 0xffffffff;
        }
        else
        {
            framebuffer[i * width + j] = 0x0;
        }
    }
#else


    for(auto i = ystart; i < yend; i++)
    {
        for(auto j = xstart; j < xend; j++)
        {
            Ray initial_ray = camera->Emit(j / (float)width, (height - i) / (float)height);
            for(auto object : objects)
            {
                if(object->Intersects(initial_ray, info))
                {
#if 0
                    glm::vec3 dir = light->origin - info.point;
                    dir = glm::normalize(dir);
                    glm::vec3 origin = initial_ray.origin + (info.t - 0.001f) * initial_ray.direction;
                    Ray shadow_ray = Ray(origin, dir);

                    Colour colour(info.normal);
                    for(auto occluder : objects)
                    {
                        if(occluder->Intersects(shadow_ray, info) && info.t > 1e-6)
                        {
                            colour = Colour(glm::vec3(0.0));
                        }
                    }
                    framebuffer[i * width +j] = colour;
#else
                    framebuffer[i * width + j] = 0xffffffff;
#endif
                }
                else
                {
                    framebuffer[i * width + j] = 0x999999ff;
                }
            }
        }
        static uint32_t count = 0;
    }
#endif
    return NULL;
}

int main(int argc, char **argv)
{
    const uint32_t width  = 640;
    const uint32_t height = 480;
    uint32_t framebuffer[height * width];
    memset(framebuffer, 0x00, sizeof(framebuffer));
    Display display = Display("", width, height);

#if TRACE_PIXEL
    auto num_threads          = 1;
#else
    auto num_threads          = 9;
#endif
    pthread_mutex_t mutex     = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t  condition = PTHREAD_COND_INITIALIZER;
    for(auto i = 0; i < num_threads; i++)
    {

        pthread_t render_thread;
        uint32_t xstart = 0;
        uint32_t xend   = width;
        uint32_t ystart = i * (height / num_threads);
        uint32_t yend   = (i + 1) * (height / num_threads);
        if(i == num_threads - 1)
        {
            yend += height % num_threads;
        }
        render_input_t input(width, height, xstart, xend, ystart, yend, framebuffer, condition, mutex);
        pthread_mutex_lock(&mutex);
        pthread_create(&render_thread, NULL, render, &input);
        pthread_cond_wait(&condition, &mutex);
        pthread_mutex_unlock(&mutex);
    }

    while(1)
    {
        display.Draw(framebuffer);
        display.HandleEvents();
    }
    return 0;
}
