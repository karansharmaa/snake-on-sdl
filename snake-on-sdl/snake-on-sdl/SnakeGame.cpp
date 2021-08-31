#include <iostream>
#include <string>
#include <ctime>
#include "SDL/SDL_image.h"
#include "Game.h"

using namespace std;

Game::Game()
{
    for (int i = 0; i < GRID_WIDTH; ++i)
        for (int j = 0; j < GRID_HEIGHT; ++j)
        {
            grid[i][j] = snakey::empty;
        }

    srand(static_cast<unsigned int>(time(0)));
}

void Game::Initialize()
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cerr << "SDL could not initialize. SDL_Error: " << SDL_GetError() << endl;
        exit(EXIT_FAILURE);
    }

    // Create Window
    window = SDL_CreateWindow("CMPT 1267 PROJECT 1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        640, 640, 4);

    if (window == NULL)
    {
        cout << "Window could not be created. SDL_Error: " << SDL_GetError() << endl;
        exit(EXIT_FAILURE);
    }

    // Create mRenderer
    mRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (mRenderer == NULL)
    {
        cout << "Renderer could not be created. SDL_Error: " << SDL_GetError() << endl;
        exit(EXIT_FAILURE);
    }

    alive = true;
    mIsRunning = true;
    ReplaceFood();
    GameLoop();
}

void Game::ReplaceFood()
{
    int a, b;
    while (true)
    {
        a = rand() % GRID_WIDTH;
        b = rand() % GRID_HEIGHT;

        if (grid[a][b] == snakey::empty)
        {
            grid[a][b] = snakey::food;
            food.x = a;
            food.y = b;
            break;
        }
    }
}

void Game::GameLoop()
{
    Uint32 before, second = SDL_GetTicks(), after;
    int frame_time, frames = 0;

    while (mIsRunning)
    {
        before = SDL_GetTicks();

        PollEvents();
        Update();
        Render();

        frames++;
        after = SDL_GetTicks();
        frame_time = after - before;

        if (after - second >= 100)
        {
            fps = frames;
            frames = 0;
            second = after;
            UpdateWindowTitle();
        }

        if (FRAME_RATE > frame_time)
        {
            SDL_Delay(FRAME_RATE - frame_time);
        }
    }

}

void Game::PollEvents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
        {
            mIsRunning = false;
        }
        else if (e.type == SDL_KEYDOWN)
        {
            switch (e.key.keysym.sym)
            {
            case SDLK_ESCAPE: 
                mIsRunning = false; 
                break; 

            case SDLK_UP:
                if (last_dir != Move::down || size == 1)
                    dir = Move::up;
                break;

            case SDLK_DOWN:
                if (last_dir != Move::up || size == 1)
                    dir = Move::down;
                break;

            case SDLK_LEFT:
                if (last_dir != Move::right || size == 1)
                    dir = Move::left;
                break;

            case SDLK_RIGHT:
                if (last_dir != Move::left || size == 1)
                    dir = Move::right;
                break;
            }
        }
    }
}

void Game::Update()
{
    if (!alive)
        return;

    switch (dir)
    {
    case Move::up:
        pos.y -= speed;
        pos.x = floorf(pos.x);
        break;

    case Move::down:
        pos.y += speed;
        pos.x = floorf(pos.x);
        break;

    case Move::left:
        pos.x -= speed;
        pos.y = floorf(pos.y);
        break;

    case Move::right:
        pos.x += speed;
        pos.y = floorf(pos.y);
        break;
    }

    if (pos.x < 0)
        pos.x = GRID_WIDTH - 1;
    else if (pos.x > GRID_WIDTH - 1)
        pos.x = 0;

    if (pos.y < 0)
        pos.y = GRID_HEIGHT - 1;
    else if (pos.y > GRID_HEIGHT - 1)
        pos.y = 0;

    int new_x = static_cast<int>(pos.x);
    int new_y = static_cast<int>(pos.y);

    if (new_x != head.x || new_y != head.y)
    {
        last_dir = dir;

        if (growing > 0)
        {
            size++;
            body.push_back(head);
            growing--;
            grid[head.x][head.y] = snakey::body;
        }
        else
        {

            SDL_Point free = head;
            vector<SDL_Point>::reverse_iterator rit = body.rbegin();
            for (; rit != body.rend(); ++rit)
            {
                grid[free.x][free.y] = snakey::body;
                swap(*rit, free);
            }

            grid[free.x][free.y] = snakey::empty;
        }

    }

    head.x = new_x;
    head.y = new_y;

    snakey& next = grid[head.x][head.y];
    // CHECK FOR FOOD 
    if (next == snakey::food)
    {
        score += 1;
        ReplaceFood();
        GrowBody(1);
    }
    // ALIVE OR DEAD 
    else if (next == snakey::body)
    {
        alive = false;
        cout << "Snake has died, lol. Your score was " << score << " and your size was " << size << endl;
        mIsRunning = false;

    }

    next = snakey::head;

}

void Game::GrowBody(int quantity)
{
    growing += quantity;
}

void Game::UpdateWindowTitle()
{
    string title = "Score: " + to_string(score) + " Frames: " + to_string(fps);
    SDL_SetWindowTitle(window, title.c_str());
}

void Game::Render()
{
    SDL_Rect block;
    block.w = SCREEN_WIDTH / GRID_WIDTH;
    block.h = SCREEN_WIDTH / GRID_HEIGHT;

    //the background
    SDL_SetRenderDrawColor(mRenderer, 30, 30, 255, 255);
    SDL_RenderClear(mRenderer);

    // color of the food -- also creates the food 
    SDL_SetRenderDrawColor(mRenderer, 0, 255, 0, 255);
    block.x = food.x * block.w;
    block.y = food.y * block.h;
    SDL_RenderFillRect(mRenderer, &block);

    // color of the snake -- also creates the snake 
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
    for (SDL_Point& point : body)
    {
        block.x = point.x * block.w;
        block.y = point.y * block.h;
        SDL_RenderFillRect(mRenderer, &block);
    }

    // color of the snake's head -- also creates the snake's head 
    block.x = head.x * block.w;
    block.y = head.y * block.h;
    if (alive) SDL_SetRenderDrawColor(mRenderer, 240, 154, 56, 0);
    else       SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255);
    SDL_RenderFillRect(mRenderer, &block);

    // renders everything 
    SDL_RenderPresent(mRenderer);
}

void Game::Close()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}