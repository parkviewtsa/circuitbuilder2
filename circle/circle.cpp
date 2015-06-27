#include <SDL2/SDL.h>
#include <math.h>
#define crIndex unsigned long
SDL_Renderer* renderer;
SDL_Texture* crDrawCircle (crIndex radius, SDL_Color color)
{
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32 rmask = 0xff000000;
    Uint32 gmask = 0x00ff0000;
    Uint32 bmask = 0x0000ff00;
    Uint32 amask = 0x000000ff;
    #else
    Uint32 rmask = 0x000000ff;
    Uint32 gmask = 0x0000ff00;
    Uint32 bmask = 0x00ff0000;
    Uint32 amask = 0xff000000;
    #endif
    crIndex surfdims = (radius * 2) + 1;
    SDL_Surface* surf = SDL_CreateRGBSurface(0,surfdims,surfdims,32,rmask,gmask,bmask,amask);
    signed long r_sq = radius * radius;
    Uint8* pixel = surf->pixels;
    for (crIndex y = 0; y < surfdims; y++)
    {
        crIndex y_sq = y - (radius + 1);
        y_sq *= y_sq;
        for (crIndex x = 0; x < surfdims; x++)
        {
            crIndex x_sq = x - (radius + 1);
            x_sq *= x_sq;
            SDL_Color writecolor = {0,0,0,0};
            signed long total = y_sq + x_sq;
            if (total - r_sq < 0) if (powf(total,0.5f) - radius > -1) writecolor = color;
            *pixel = writecolor.r;
            pixel++;
            *pixel = writecolor.g;
            pixel++;
            *pixel = writecolor.b;
            pixel++;
            *pixel = writecolor.a;
            pixel++;
        };
    };
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer,surf);
    SDL_FreeSurface(surf);
    return tex;
};
#define SDL_main main
int main ()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Swag",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,640,640,0);
    renderer = SDL_CreateRenderer(window,-1,0);
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = 513;
    rect.h = 513;
    SDL_Texture* swag = crDrawCircle(256,{0,255,0,255});
    SDL_RenderCopy(renderer,swag,NULL,&rect);
    SDL_RenderPresent(renderer);
    SDL_Delay(1000);
};
