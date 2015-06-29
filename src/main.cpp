#include <iostream>
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
#include "ply.h"
#include "scene.h"

#define RGBA(r, g, b, a) ((r) << 24 | (g) << 16 | (b) << 8 | (a))

#define TRACE_PIXEL   0
#define TRACE_PIXEL_X (640 / 2 - 10)
#define TRACE_PIXEL_Y (480 / 2)

struct render_input_t
{
    uint32_t    width;
    uint32_t    height;
    uint32_t    xstart;
    uint32_t    xend;
    uint32_t    ystart;
    uint32_t    yend;
    uint32_t   *framebuffer;
	SDL_cond   *cond;
	SDL_mutex  *mutex;
	Scene      *scene;

    render_input_t(uint32_t    width,
                   uint32_t    height,
                   uint32_t    xstart,
                   uint32_t    xend,
                   uint32_t    ystart,
                   uint32_t    yend,
                   uint32_t   *framebuffer,
                   SDL_cond   *cond,
                   SDL_mutex  *mutex,
				   Scene      *scene)
    : width(width)
    , height(height)
    , xstart(xstart)
    , xend(xend)
    , ystart(ystart)
    , yend(yend)
    , framebuffer(framebuffer)
    , cond(cond)
    , mutex(mutex)
	, scene(scene)
    {}
};

int test_render(void *input)
{
	return 0;
}

int pixel_trace(void *input)
{
	uint32_t width        = ((render_input_t*)input)->width;
    uint32_t height       = ((render_input_t*)input)->height;
    uint32_t xstart       = ((render_input_t*)input)->xstart;
    uint32_t ystart       = ((render_input_t*)input)->ystart;
    uint32_t xend         = ((render_input_t*)input)->xend;
    uint32_t yend         = ((render_input_t*)input)->yend;
    uint32_t *framebuffer = ((render_input_t*)input)->framebuffer;
	Scene *scene          = ((render_input_t*)input)->scene;
	
	SDL_LockMutex(((render_input_t*)input)->mutex);
	throw std::exception("Pixel Trace is not implemented.");

	SDL_CondSignal(((render_input_t*)input)->cond);
	SDL_UnlockMutex(((render_input_t*)input)->mutex);
}

int render(void *input)
{
	uint32_t width        = ((render_input_t*)input)->width;
    uint32_t height       = ((render_input_t*)input)->height;
    uint32_t xstart       = ((render_input_t*)input)->xstart;
    uint32_t ystart       = ((render_input_t*)input)->ystart;
    uint32_t xend         = ((render_input_t*)input)->xend;
    uint32_t yend         = ((render_input_t*)input)->yend;
    uint32_t *framebuffer = ((render_input_t*)input)->framebuffer;
	Scene *scene          = ((render_input_t*)input)->scene;
	
	SDL_LockMutex(((render_input_t*)input)->mutex);
	SDL_CondSignal(((render_input_t*)input)->cond);
	SDL_UnlockMutex(((render_input_t*)input)->mutex);

    Intersections::Record info;
	Intersections::Record shadow_record;
    for(auto i = ystart; i < yend; i++)
    {
        for(auto j = xstart; j < xend; j++)
        {
            Ray initial_ray = scene->camera->Emit(j / (float)width, (height - i) / (float)height);
			uint32_t &pixel = framebuffer[i * width + j];
			if(scene->Intersects(initial_ray, info))
			{
				Ray shadow_ray;
				shadow_ray.origin = initial_ray.origin + initial_ray.direction * (info.t - 0.01f);
				for(const auto light : scene->lights)
				{
					if(const PointLight *p = dynamic_cast<const PointLight*>(light))
					{
						shadow_ray.direction = glm::normalize(p->origin - shadow_ray.origin);
						if(scene->Intersects(shadow_ray, shadow_record))
						{
							pixel = 0x000000ff;
						}
						else
						{
							if(info.mesh->has_normals)
							{
								auto v0 = info.mesh->normals[info.triangle[0]] * (1 -info.u - info.v);
								auto v1 = info.mesh->normals[info.triangle[1]] * info.u;
								auto v2 = info.mesh->normals[info.triangle[2]] * info.v;
								glm::vec3 interp = v0 + v1 + v2;
								interp += 1.0f;
								interp /= 2.0f;
								pixel = RGBA((uint8_t)(interp.x * 255),
											 (uint8_t)(interp.y * 255),
											 (uint8_t)(interp.z * 255),
											 0xff);
							}
							else
							{
								pixel = RGBA(0xff, 0xff, 0xff, 0xff);
							}
						}
					}
				}

			}
            else
            {
				pixel = 0x555555ff;
            }
        }
        static uint32_t count = 0;
    }
	return 0;
}
#if 0
						if(mesh->has_normals)
						{
							auto v0 = mesh->normals[info.triangle[0]];
							auto v1 = mesh->normals[info.triangle[1]];
							auto v2 = mesh->normals[info.triangle[2]];
							glm::vec3 interp = v0 * info.u + v1 * info.v + v2 * (1 - info.u - info.v);
							interp += 1.0f;
							interp /= 2.0f;
							pixel = RGBA((uint8_t)(interp.x * 255),
										 (uint8_t)(interp.y * 255),
										 (uint8_t)(interp.z * 255),
										 0xff);
						}
						else
						{
							pixel = RGBA(0xff, 0xff, 0xff, 0xff);
						}
#endif

int main(int argc, char **argv)
{

	const uint32_t scale  = 2;
    const uint32_t width  = scale * 640 / 4;
    const uint32_t height = scale * 640 / 4;
    uint32_t *framebuffer = new uint32_t[height * width];
	for(uint32_t i = 0; i < width * height; i++)
	{
		framebuffer[i] = 0x000000ff;
	}
	Scene scene(SCENE_DIRECTORY "test.scene");
    Display display = Display("", width, height);

#if TRACE_PIXEL
    auto num_threads          = 1;
#else
    auto num_threads          = 3;
#endif
	SDL_mutex *mutex = SDL_CreateMutex();
	SDL_cond  *cond  = SDL_CreateCond();
    for(auto i = 0; i < num_threads; i++)
    {
		for(auto j = 0; j < num_threads; j++)
		{
			SDL_Thread *render_thread;
			uint32_t xstart = j       * (width  / num_threads);
			uint32_t xend   = (j + 1) * (width  / num_threads);
			uint32_t ystart = i       * (height / num_threads);
			uint32_t yend   = (i + 1) * (height / num_threads);
			if(i == num_threads - 1)
			{
				yend += height % num_threads;
			}
			if(j == num_threads - 1)
			{
				xend += width % num_threads;
			}
			render_input_t input(width, height, xstart, xend, ystart, yend, framebuffer, cond, mutex, &scene);

			SDL_LockMutex(mutex);
			render_thread = SDL_CreateThread(render, "Render Thread", &input);
			SDL_CondWait(cond, mutex);
			SDL_UnlockMutex(mutex);
		}
    }

    while(1)
    {
        display.Draw(framebuffer);
        display.HandleEvents();
    }
    return 0;
}
