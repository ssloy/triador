#include <iostream>
#include <chrono>
#include <thread>
#include <SDL.h>
#include "triador.h"

SDL_Rect paddle_dest_rect(int paddle, int left) {
    int top = 362+(paddle*58*4)/13-48;
    if (paddle>0)
        return {left, top, 16, std::min(594-top, 96)};
    return {left, std::max(top, 130), 16, 96-std::max(130-top, 0)};
}

int main() {
    Triador triador;
    triador.load_program("../prog/pong.txt");

    if (SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window   *window   = nullptr;
    SDL_Renderer *renderer = nullptr;

    if (SDL_CreateWindowAndRenderer(1024, 768, SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS, &window, &renderer)) {
        std::cerr << "Failed to create window and renderer: " << SDL_GetError() << std::endl;
        return -1;
    }
    SDL_SetWindowTitle(window, "Pong on a ternary computer!");

    SDL_Surface *scope_surface = SDL_LoadBMP("../resources/oscilloscope.bmp");
    if (!scope_surface) {
        std::cerr << "Error in SDL_LoadBMP: " << SDL_GetError() << std::endl;
        return -1;
    }
    SDL_Texture *scope_texture = SDL_CreateTextureFromSurface(renderer, scope_surface);

    SDL_Surface *dot_surface = SDL_LoadBMP("../resources/dot.bmp");
    if (!dot_surface) {
        std::cerr << "Error in SDL_LoadBMP: " << SDL_GetError() << std::endl;
        return -1;
    }
    SDL_Texture *dot_texture = SDL_CreateTextureFromSurface(renderer, dot_surface);

    SDL_Surface *paddle_surface = SDL_LoadBMP("../resources/paddle.bmp");
    if (!paddle_surface) {
        std::cerr << "Error in SDL_LoadBMP: " << SDL_GetError() << std::endl;
        return -1;
    }
    SDL_Texture *paddle_texture = SDL_CreateTextureFromSurface(renderer, paddle_surface);

    SDL_Surface *score_surface = SDL_LoadBMP("../resources/score.bmp");
    if (!score_surface) {
        std::cerr << "Error in SDL_LoadBMP: " << SDL_GetError() << std::endl;
        return -1;
    }
    SDL_Texture *score_texture = SDL_CreateTextureFromSurface(renderer, score_surface);

    auto t1 = std::chrono::high_resolution_clock::now();
    while (1) {
        { // poll events and update player's state (walk/turn flags); TODO: move this block to a more appropriate place
            SDL_Event event;
            if (SDL_PollEvent(&event)) {
                if (SDL_QUIT==event.type || (SDL_KEYDOWN==event.type && SDLK_ESCAPE==event.key.keysym.sym)) break;
                if (SDL_KEYUP==event.type) {
                    if (      's'==event.key.keysym.sym ||     'w'==event.key.keysym.sym) triador.program[26].second = 0;
                    if (SDLK_DOWN==event.key.keysym.sym || SDLK_UP==event.key.keysym.sym) triador.program[19].second = 0;
                }
                if (SDL_KEYDOWN==event.type) {
                    if ('s'==event.key.keysym.sym) triador.program[26].second =  1;
                    if ('w'==event.key.keysym.sym) triador.program[26].second = -1;
                    if (SDLK_DOWN==event.key.keysym.sym) triador.program[19].second =  1;
                    if (SDLK_UP  ==event.key.keysym.sym) triador.program[19].second = -1;
                }
            }
        }

        { // sleep if less than 20 ms since last re-rendering
            auto t2 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> fp_ms = t2 - t1;
            if (fp_ms.count()<150) {
                std::this_thread::sleep_for(std::chrono::milliseconds(3));
                continue;
            }
            t1 = t2;
        }

        do {
            triador.cycle();
        } while (triador.PC!=-13*27+3);

        int dot_x = triador.R[10];
        int dot_y = triador.R[11];
        int paddle_l = triador.R[6];
        int paddle_r = triador.R[7];
        int score = triador.R[5];

        SDL_Rect dot_rect = {435-8+(dot_x*58*5)/14, 362-8+(dot_y*58*4)/13, 16, 16};
        SDL_RenderCopy(renderer, scope_texture, NULL, NULL);
        SDL_RenderCopy(renderer, dot_texture, NULL, &dot_rect);
        SDL_Rect paddle_l_rect = paddle_dest_rect(paddle_l, 435-8-58*5);
        SDL_Rect paddle_r_rect = paddle_dest_rect(paddle_r, 435-8+58*5);
        SDL_RenderCopy(renderer, paddle_texture, NULL, &paddle_l_rect);
        SDL_RenderCopy(renderer, paddle_texture, NULL, &paddle_r_rect);

        if (score>=0) {
            SDL_Rect src1 = {0,0,8,16};
            SDL_Rect dst1 = {435-8, 594-8, 8, 16};
            SDL_RenderCopy(renderer, dot_texture, &src1, &dst1);
            SDL_Rect src2 = {8,0,8,16};
            SDL_Rect dst2 = {435+score*22, 594-8, 8, 16};
            SDL_RenderCopy(renderer, dot_texture, &src2, &dst2);
        } else {
            SDL_Rect src1 = {0,0,8,16};
            SDL_Rect dst1 = {435-8+score*22, 594-8, 8, 16};
            SDL_RenderCopy(renderer, dot_texture, &src1, &dst1);
            SDL_Rect src2 = {8,0,8,16};
            SDL_Rect dst2 = {435, 594-8, 8, 16};
            SDL_RenderCopy(renderer, dot_texture, &src2, &dst2);
        }

        for (int i=0; i<abs(score); i++) {
            int left = 435 + (score>0?i:-i-1)*22;
            int top = 594-8;
            SDL_Rect score_rect = {left, top, 22, 16};
            SDL_RenderCopy(renderer, score_texture, NULL, &score_rect);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(score_texture);
    SDL_FreeSurface(score_surface);
    SDL_DestroyTexture(paddle_texture);
    SDL_FreeSurface(paddle_surface);
    SDL_DestroyTexture(dot_texture);
    SDL_FreeSurface(dot_surface);
    SDL_DestroyTexture(scope_texture);
    SDL_FreeSurface(scope_surface);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

