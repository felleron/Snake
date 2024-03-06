#include "snake.h"
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <time.h>
#include "glyphs.c"

#define WINDOW_X 0
#define WINDOW_Y 0
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080


#define GRID_SIZE 20
#define GRID_DIM 1000

typedef struct color {
    int red;
    int green;
    int blue;
    int alpha;
}Color;

Color snake_color = {52, 211, 80, 255};
Color grid_color = {0x00, 0xfb, 0xff, 255};
Color score_color = {255,255,255,255};
Color score_box = {223,85,207,170};
enum {
    SNAKE_UP,
    SNAKE_DOWN,
    SNAKE_LEFT,
    SNAKE_RIGHT,
};

struct snake {
    int x;
    int y;
    int dir;

    struct snake *next;
};
typedef struct snake Snake;

Snake *head;
Snake *tail;

typedef struct {
    int x;
    int y;
    int count;
} apple;

apple Apple;

void init_snake()
{
    Snake *new = malloc(sizeof(Snake));
    new->x = rand() % (GRID_SIZE / 2) + (GRID_SIZE / 4);
    new->y = rand() % (GRID_SIZE / 2) + (GRID_SIZE / 4);
    new->dir = SNAKE_UP;
    new->next = NULL;

    head = new;
    tail = new;

    return;
}

void increase_snake()
{
    Snake *new = malloc(sizeof(Snake));

    switch(tail->dir) {
        case SNAKE_UP:
            new->x = tail->x;
            new->y = tail->y +1;
            break;
        case SNAKE_DOWN:
            new->x = tail->x;
            new->y = tail->y - 1;
            break;
        case SNAKE_RIGHT:
            new->x = tail->x - 1;
            new->y = tail->y;
            break;
        case SNAKE_LEFT:
            new->x = tail->x + 1 ;
            new->y = tail->y;
            break;

    }
    new->next = NULL;
    tail->next = new;
    tail = new;
}

void move_snake()
{
    int prev_x = head->x;
    int prev_y = head->y;
    int prev_dir = head->dir;
    switch (head->dir)
    {
    case SNAKE_UP:
        head->y--;
        break;
    case SNAKE_DOWN:
        head->y++;
        break;
    case SNAKE_LEFT:
        head->x--;
        break;
    case SNAKE_RIGHT:
        head->x++;
        break;
    }

    Snake *track = head;

    if(track->next != NULL)
    {
        track = track->next;
    }
    while(track != NULL)
    {
        int save_x = track->x;
        int save_y = track->y;
        int save_dir = track->dir;

        track->x = prev_x;
        track->y = prev_y;
        track->dir = prev_dir;

        track = track->next;

        prev_x = save_x;
        prev_y = save_y;
        prev_dir = save_dir;
    }

    return;
}

void reset_snake()
{
    Snake *track = head;
    Snake *temp;

    while(track != NULL) {
        temp = track;
        track = track->next;
        free(temp);
    }

    init_snake();
    increase_snake();
    increase_snake();
    increase_snake();
    Apple.count = 0;
    return;
}
void SDL_RenderFillCircle(SDL_Renderer *renderer, int x, int y, int radius, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int w = 0; w < radius * 2; w++)
    {
        for (int h = 0; h < radius * 2; h++)
        {
            int dx = radius - w;
            int dy = radius - h; 
            if ((dx*dx + dy*dy) <= (radius * radius))
            {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}
void render_snake(SDL_Renderer *renderer, int x, int y)
{
    SDL_SetRenderDrawColor(renderer, snake_color.red, snake_color.green, snake_color.blue, snake_color.alpha);

    int seg_size = GRID_DIM / GRID_SIZE;
    SDL_Rect seg;
    seg.w = seg_size;
    seg.h = seg_size;
    
    Snake *track = head;

    while(track != NULL){
        seg.x = x + track->x * seg_size;
        seg.y = y + track->y * seg_size;

        SDL_RenderFillRect(renderer, &seg);
        track = track->next;
    }


    return;
}

void render_grid(SDL_Renderer *renderer,int x, int y)
{
    SDL_SetRenderDrawColor(renderer, grid_color.red, grid_color.green, grid_color.blue, grid_color.alpha);
    SDL_Rect outline;
    outline.x = x;
    outline.y = y;
    outline.w = GRID_DIM;
    outline.h = GRID_DIM;

    SDL_RenderDrawRect(renderer, &outline);
    return;    
}

void gen_apple()
{
    bool in_snake;
    do {
        in_snake = false;
        Apple.x = rand() % GRID_SIZE;
        Apple.y = rand() % GRID_SIZE;

        Snake *track = head;

        while(track != NULL) {
            if(track->x == Apple.x && track->y == Apple.y)
            {
                in_snake = true;
            }
            track = track->next;
        }
    }
    while(in_snake);

    Apple.count++;

}

void render_apple(SDL_Renderer *renderer, int x, int y)
{
    SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 255);
    int apple_size = GRID_DIM / GRID_SIZE;
    int circle_x = x + (apple_size * Apple.x) + ( apple_size / 2 );
    int circle_y = y + (apple_size * Apple.y) + ( apple_size / 2 );

    SDL_Color c = {0xff, 0x00, 0x00, 255}; 

    SDL_RenderFillCircle(renderer, circle_x, circle_y, apple_size / 2, c);
    return;
}

void detect_apple()
{
    if(head->x == Apple.x && head->y == Apple.y)
    {
        gen_apple();
        increase_snake();
    }
    return;
}

void detect_crash()
{
    if(head->x < 0 || head->x >= GRID_SIZE  || head->y < 0 || head->y >= GRID_SIZE )
    {
        gen_apple();
        reset_snake();
    }

    Snake *track = head;
    if(track->next != NULL)
    {
        track = track->next;
    }
    while(track != NULL) {
        if(track->x == head->x && track->y == head->y)
        {
            gen_apple();
            reset_snake();
        }
        track = track->next;
    }

    return;
}
void render_score(SDL_Renderer *renderer, int x, int y)
{
    int score_x = x + GRID_DIM / 2 + 500;
    int score_y = y + 50;

    SDL_Rect score;
    score.w = 200;
    score.h = 100;
    score.x = score_x + 100;
    score.y = score_y;
    SDL_SetRenderDrawColor(renderer, score_box.red, score_box.green, score_box.blue, score_box.alpha);
    
    SDL_RenderDrawRect(renderer, &score);

    SDL_SetRenderDrawColor(renderer, score_color.red, score_color.green, score_color.blue, score_color.alpha);

    int cell_width = 10;

    SDL_Rect cell;
    cell.w = cell_width;
    cell.h = cell_width;

    char buffer[100];
    snprintf(buffer, sizeof(buffer), "%3d", Apple.count);
    
    int x_step = 100;

    for(int k = 0; k < 3; k++) {

        for(int i = 0; i < 9; i++) {
            for(int j = 0; j < 10; j++) {
                cell.x = score_x + i * cell_width + (k * x_step); 
                cell.y = score_y + j * cell_width; 
                if(glyphs[(int)buffer[k]][j][i] == 1) {
                    SDL_RenderFillRect(renderer, &cell);
                }
            }
        }
    }



    return;
}
int main()
{
    srand(time(NULL));
    init_snake();
    increase_snake();
    increase_snake();
    increase_snake();

    gen_apple();
    Apple.count = 0;

    SDL_Window *window;
    SDL_Renderer *renderer;

    if(SDL_INIT_VIDEO < 0 )
    {
        fprintf(stderr, "ERROR: SDL_INIT_VIDEO");
    }
    window = SDL_CreateWindow(
        "Snake",
        WINDOW_X,
        WINDOW_Y,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS
    );

    if(!window)
    {
        fprintf(stderr, "ERROR: !window.");
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer)
    {
        fprintf(stderr, "ERROR: !renderer.");
    }
    SDL_SetRenderDrawColor(renderer, 0x11, 0x11, 0x11, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    //grid to be centered no matter what
    int grid_x = (WINDOW_WIDTH / 2) - (GRID_DIM / 2);
    int grid_y = (WINDOW_HEIGHT / 2) - (GRID_DIM / 2);

    bool quit = false;
    bool moved = false;

    SDL_Event event;

    while(!quit) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYUP:
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                        quit = true;
                        break;
                        case SDLK_UP:
                            if(head->dir != SNAKE_DOWN && moved != true)
                            {
                                head->dir = SNAKE_UP;
                                moved = true;
                            }
                            break;
                        case SDLK_DOWN:
                            if(head->dir != SNAKE_UP && moved != true)
                            {
                                head->dir = SNAKE_DOWN;
                                moved = true;
                            }
                            break;
                        case SDLK_LEFT:
                            if(head->dir != SNAKE_RIGHT && moved != true)
                            {
                                head->dir = SNAKE_LEFT;
                                moved = true;
                            }
                            break;
                        case SDLK_RIGHT:
                            if(head->dir != SNAKE_LEFT && moved != true)
                            {
                                head->dir = SNAKE_RIGHT;
                                moved = true;
                            }
                            break;
                    }
                    break;
            }
        }
    SDL_RenderClear(renderer);
    detect_apple();
    
    render_grid(renderer, grid_x, grid_y);
    render_snake(renderer, grid_x, grid_y);
    render_apple(renderer, grid_x, grid_y);
    render_score(renderer, grid_x, grid_y);
    move_snake();
    moved = false;
    detect_crash();


    SDL_SetRenderDrawColor(renderer, 0x11, 0x11, 0x11, 255); //color of background here
    SDL_RenderPresent(renderer);
    SDL_Delay(100);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}