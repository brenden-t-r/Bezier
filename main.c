#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<SDL.h>
#include <time.h>

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int skip = 0;
uint32_t delta;
uint32_t b_ticks;
int bezier_follow_index = 0;
float bezier_follow_percent = 0;
int selected_anchor_index = 0;
int num_of_points = 1;

struct point {
    int x;
    int y;
};

struct point points[100] = {
        {200, 300},
        {250, 0},
        {500, 300},
        {550, 0},
        {625, 425},
};

int getPt(int n1, int n2, float perc)
{
    int diff = n2 - n1;
    return n1 + (diff * perc);
}

void drawPixel(int x, int y) {
    SDL_RenderDrawPoint(renderer, x, y);
}

void bezier(int x1, int y1, int x2, int y2, int x3, int y3) {
    for (float i = 0; i < 1; i += 0.01) {
        int xa = getPt(x1, x2, i);
        int ya = getPt(y1, y2, i);
        int xb = getPt(x2, x3, i);
        int yb = getPt(y2, y3, i);
        int x = getPt(xa, xb, i);
        int y = getPt(ya, yb, i);

        drawPixel(x, y);
    }
}

void bezier1(struct point points[], int num_points) {
    for (int i = 0; i < num_points; i++) {
        if (num_points >= i + 2) {
            int x1 = points[i].x;
            int y1 = points[i].y;
            int x2 = points[i + 1].x;
            int y2 = points[i + 1].y;
            int x3 = points[i + 2].x;
            int y3 = points[i + 2].y;
            bezier(x1, y1, x2, y2, x3, y3);
            i += 1;
        } else if (num_points == i + 1) {
            int x1 = points[i].x;
            int y1 = points[i].y;
            int x3 = points[0].x;
            int y3 = points[0].y;
            int x2 = getPt(x1, x3, 0.5f);
            int y2 = getPt(y1, y3, 0.5f);
            bezier(x1, y1, x2, y2, x3, y3);
        }
    }
}

void bezierFollow(struct point points[], int num_points) {
    if (bezier_follow_index + 2 >= num_points) {
        bezier_follow_index = 0;
        bezier_follow_percent = 0;
        return;
    }

    int x1 = points[bezier_follow_index].x;
    int y1 = points[bezier_follow_index].y;
    int x2 = points[bezier_follow_index + 1].x;
    int y2 = points[bezier_follow_index + 1].y;
    int x3 = points[bezier_follow_index + 2].x;
    int y3 = points[bezier_follow_index + 2].y;
    int xa = getPt(x1, x2, bezier_follow_percent);
    int ya = getPt(y1, y2, bezier_follow_percent);
    int xb = getPt(x2, x3, bezier_follow_percent);
    int yb = getPt(y2, y3, bezier_follow_percent);
    int x = getPt(xa, xb, bezier_follow_percent);
    int y = getPt(ya, yb, bezier_follow_percent);
    SDL_RenderDrawLine(renderer, x, y, x, y + 20);

    bezier_follow_percent += 0.01f;
    if (bezier_follow_percent >= 1) {
        bezier_follow_percent = 0;
        bezier_follow_index += 2;
    }
}

void bezierAnchors(struct point points[], int num_of_points) {
    for (int i = 0; i < num_of_points; i++) {
        // Draw point and have click action
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_Rect rect = {
                points[i].x - 5, points[i].y - 5, 10, 10
        };
        if (i % 2 == 1) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        }
        SDL_RenderDrawRect(renderer, &rect);
        if (i != selected_anchor_index) {
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

void update() {
    uint32_t a = SDL_GetTicks();
    delta = a - b_ticks;
    if (delta > 1000/120.0)
    {
        printf("%u", 1000/delta);
        b_ticks = a;
    } else {
        return;
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    bezierAnchors(points, num_of_points);
    bezier1(points, num_of_points);
    bezierFollow(points, num_of_points);
}

int main(int argc, char* args []) {
    window = SDL_CreateWindow("Bézier", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    int running = 1;
    SDL_Event event;

    while (running) {
        // Close window with any input
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
                break;
            }
            if (event.type == SDL_KEYUP) {
                if (event.key.keysym.sym == SDLK_s) {
                    printf("hi");
                }
            }
            if (event.type == SDL_MOUSEBUTTONUP) {
                // Anchor click selection
                for (int i = 0; i < num_of_points; i++) {
                    if ((points[i].x-5 < event.button.x && points[i].x + 10 > event.button.x)
                        && (points[i].y-5 < event.button.y && points[i].y + 10 > event.button.y)) {
                        // clicked an anchor
                        if (selected_anchor_index == i) {
                            selected_anchor_index = -1;
                        } else {
                            selected_anchor_index = i;
                        }
                        skip = 1;
                        continue;
                    }
                }

                // Move anchor
                if (selected_anchor_index != -1) {
                    points[selected_anchor_index].x = event.button.x;
                    points[selected_anchor_index].y = event.button.y;
                    continue;
                }

                // New anchor/points
                if (selected_anchor_index == -1 && !skip) {
                    struct point new_point = {
                            event.button.x, event.button.y
                    };
                    points[num_of_points] = new_point;
                    num_of_points += 1;
                }

                if (skip == 1) {
                    skip = 0;
                }
            }
        }

        update();

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}