#ifndef DISPLAY_H
#define DISPLAY_H

#include "SDL.h"
#include <stdlib.h>

class Display
{
public:
    Display(const char *title, uint32_t width, uint32_t height)
    : width(width)
    , height(height)
    {
        if(!SDL_WasInit(SDL_INIT_EVERYTHING))
        {
            SDL_Init(SDL_INIT_EVERYTHING);
        }
        window   = SDL_CreateWindow(title, 0, 0, width, height, 0);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        texture  = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
        SDL_RenderSetLogicalSize(renderer, width, height);
        SDL_ShowCursor(SDL_DISABLE);
    }

    void HandleEvents(void)
    {
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    exit(0);

                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            exit(0);
                    }
                case SDL_KEYUP:
                    break;
            }
        }
    }

    void Draw(uint32_t *framebuffer)
    {
        SDL_UpdateTexture(texture, NULL, framebuffer, sizeof(uint32_t) * width);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

private:
    uint32_t  width;
    uint32_t  height;
    SDL_Window   *window;
    SDL_Renderer *renderer;
    SDL_Texture  *texture;
    SDL_Surface  *surface;
    SDL_Event     event;
};

#endif
