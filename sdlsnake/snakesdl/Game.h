#pragma once
#include <vector>
#include "SDL/SDL_image.h"

class Game
{

public:

    Game();
    void Initialize();
    int GetScore();
    int GetSize();

private:

    bool mIsRunning = false;
    bool alive = false;
    int fps = 0;

    static const int FRAME_RATE = 1000 / 60;
    static const int SCREEN_WIDTH = 640;
    static const int SCREEN_HEIGHT = 640;
    static const int GRID_WIDTH = 32;
    static const int GRID_HEIGHT = 32;

    SDL_Window* window = nullptr;
    SDL_Renderer* mRenderer = nullptr;

    enum class snakey { 
        head, body, food, empty 
    };
    enum class Move
    { 
        up, down, left, right 
    };

    Move last_dir = Move::up;
    Move dir = Move::up;

    struct
    { 
        float x = GRID_WIDTH / 2, y = GRID_HEIGHT / 2; 
    } pos;

    SDL_Point head = { static_cast<int>(pos.x), static_cast<int>(pos.y) };
    SDL_Point food;
    std::vector<SDL_Point> body;

    snakey grid[GRID_WIDTH][GRID_HEIGHT];

    float speed = 0.25f;
    int growing = 0;
    int score = 0;
    int size = 1;

    void ReplaceFood();
    void GrowBody(int quantity);
    void UpdateWindowTitle();
    void GameLoop();
    void Render();
    void Update();
    void PollEvents();
    void Close();

};