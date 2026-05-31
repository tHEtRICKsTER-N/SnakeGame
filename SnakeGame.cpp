#include "raylib.h"
#include <iostream>
#include <fstream>

const int CELL_SIZE = 20;
const int GRID_WIDTH = 40;
const int GRID_HEIGHT = 30;

int snakeX, snakeY;
int fruitX, fruitY;
int score;
int highScore = 0;
int tailX[400], tailY[400];
int nTail;

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
Direction snakeDir;

enum GameState { MENU, PLAYING, GAMEOVER };
GameState currentState = MENU;

float updateTimer = 0.0f;
const float UPDATE_RATE = 0.1f; // Snake moves every 0.1 seconds

void LoadHighScore() {
    std::ifstream file("highscore.txt");
    if (file.is_open()) {
        file >> highScore;
        file.close();
    }
}

void SaveHighScore() {
    std::ofstream file("highscore.txt");
    if (file.is_open()) {
        file << highScore;
        file.close();
    }
}

void InitialSetup() {
    snakeDir = STOP;
    snakeX = GRID_WIDTH / 2;
    snakeY = GRID_HEIGHT / 2;
    fruitX = GetRandomValue(0, GRID_WIDTH - 1);
    fruitY = GetRandomValue(0, GRID_HEIGHT - 1);
    score = 0;
    nTail = 0;
    updateTimer = 0.0f;
}

void GameOver() {
    currentState = GAMEOVER;
    if (score > highScore) {
        highScore = score;
        SaveHighScore();
    }
}

void UpdateGame() {
    switch (currentState) {
        case MENU:
            if (IsKeyPressed(KEY_ENTER)) {
                InitialSetup();
                currentState = PLAYING;
            }
            break;

        case PLAYING:
            // Input handling (prevent reversing direction)
            if ((IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) && snakeDir != RIGHT) snakeDir = LEFT;
            else if ((IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) && snakeDir != LEFT) snakeDir = RIGHT;
            else if ((IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) && snakeDir != DOWN) snakeDir = UP;
            else if ((IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) && snakeDir != UP) snakeDir = DOWN;

            updateTimer += GetFrameTime();
            
            // Move snake on a fixed time step
            if (updateTimer >= UPDATE_RATE) {
                updateTimer = 0.0f;

                if (snakeDir != STOP) {
                    // Update Tail
                    int prevX = tailX[0];
                    int prevY = tailY[0];
                    int prev2X, prev2Y;
                    tailX[0] = snakeX;
                    tailY[0] = snakeY;
                    
                    for (int i = 1; i < nTail; i++) {
                        prev2X = tailX[i];
                        prev2Y = tailY[i];
                        tailX[i] = prevX;
                        tailY[i] = prevY;
                        prevX = prev2X;
                        prevY = prev2Y;
                    }

                    // Update Head
                    switch (snakeDir) {
                        case LEFT:  snakeX--; break;
                        case RIGHT: snakeX++; break;
                        case UP:    snakeY--; break;
                        case DOWN:  snakeY++; break;
                        case STOP:  break;
                    }

                    // Collision with walls
                    if (snakeX < 0 || snakeX >= GRID_WIDTH || snakeY < 0 || snakeY >= GRID_HEIGHT) {
                        GameOver();
                    }

                    // Collision with tail
                    for (int i = 0; i < nTail; i++) {
                        if (tailX[i] == snakeX && tailY[i] == snakeY) {
                            GameOver();
                        }
                    }

                    // Collision with fruit
                    if (snakeX == fruitX && snakeY == fruitY) {
                        score++;
                        nTail++;
                        fruitX = GetRandomValue(0, GRID_WIDTH - 1);
                        fruitY = GetRandomValue(0, GRID_HEIGHT - 1);
                    }
                }
            }
            break;

        case GAMEOVER:
            if (IsKeyPressed(KEY_ENTER)) {
                InitialSetup();
                currentState = PLAYING;
            }
            if (IsKeyPressed(KEY_M)) {
                currentState = MENU;
            }
            break;
    }
}

void DrawGame() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    switch (currentState) {
        case MENU: {
            const char* title = "SNAKE GAME";
            int titleWidth = MeasureText(title, 40);
            DrawText(title, GRID_WIDTH * CELL_SIZE / 2 - titleWidth / 2, GRID_HEIGHT * CELL_SIZE / 2 - 60, 40, DARKGRAY);
            
            const char* hsText = TextFormat("HIGH SCORE: %i", highScore);
            int hsWidth = MeasureText(hsText, 20);
            DrawText(hsText, GRID_WIDTH * CELL_SIZE / 2 - hsWidth / 2, GRID_HEIGHT * CELL_SIZE / 2, 20, ORANGE);

            const char* startText = "Press ENTER to start";
            int startWidth = MeasureText(startText, 20);
            DrawText(startText, GRID_WIDTH * CELL_SIZE / 2 - startWidth / 2, GRID_HEIGHT * CELL_SIZE / 2 + 40, 20, GRAY);
            break;
        }
        case PLAYING: {
            // Draw Grid Lines
            for (int i = 0; i < GRID_WIDTH; i++) DrawLine(i * CELL_SIZE, 0, i * CELL_SIZE, GRID_HEIGHT * CELL_SIZE, Color{220, 220, 220, 255});
            for (int i = 0; i < GRID_HEIGHT; i++) DrawLine(0, i * CELL_SIZE, GRID_WIDTH * CELL_SIZE, i * CELL_SIZE, Color{220, 220, 220, 255});

            // Draw Fruit
            DrawRectangle(fruitX * CELL_SIZE, fruitY * CELL_SIZE, CELL_SIZE, CELL_SIZE, RED);

            // Draw Snake Tail
            for (int i = 0; i < nTail; i++) {
                DrawRectangle(tailX[i] * CELL_SIZE, tailY[i] * CELL_SIZE, CELL_SIZE, CELL_SIZE, GREEN);
            }

            // Draw Snake Head
            DrawRectangle(snakeX * CELL_SIZE, snakeY * CELL_SIZE, CELL_SIZE, CELL_SIZE, DARKGREEN);

            // Draw Scores
            DrawText(TextFormat("SCORE: %i", score), 10, 10, 20, BLACK);
            DrawText(TextFormat("HI-SCORE: %i", highScore), 10, 40, 20, DARKGRAY);
            break;
        }
        case GAMEOVER: {
            const char* overText = "GAME OVER!";
            int overWidth = MeasureText(overText, 40);
            DrawText(overText, GRID_WIDTH * CELL_SIZE / 2 - overWidth / 2, GRID_HEIGHT * CELL_SIZE / 2 - 80, 40, RED);

            const char* scoreText = TextFormat("Score: %i", score);
            int scoreWidth = MeasureText(scoreText, 20);
            DrawText(scoreText, GRID_WIDTH * CELL_SIZE / 2 - scoreWidth / 2, GRID_HEIGHT * CELL_SIZE / 2 - 20, 20, DARKGRAY);

            const char* hsText = TextFormat("High Score: %i", highScore);
            int hsWidth = MeasureText(hsText, 20);
            DrawText(hsText, GRID_WIDTH * CELL_SIZE / 2 - hsWidth / 2, GRID_HEIGHT * CELL_SIZE / 2 + 10, 20, ORANGE);

            const char* restartText = "Press ENTER to restart";
            int restartWidth = MeasureText(restartText, 20);
            DrawText(restartText, GRID_WIDTH * CELL_SIZE / 2 - restartWidth / 2, GRID_HEIGHT * CELL_SIZE / 2 + 50, 20, GRAY);

            const char* menuText = "Press M for Main Menu";
            int menuWidth = MeasureText(menuText, 20);
            DrawText(menuText, GRID_WIDTH * CELL_SIZE / 2 - menuWidth / 2, GRID_HEIGHT * CELL_SIZE / 2 + 80, 20, GRAY);
            break;
        }
    }

    EndDrawing();
}

int main() {
    InitWindow(GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE, "Snake Game - Raylib");
    SetTargetFPS(60);

    LoadHighScore(); // Load from file on startup
    currentState = MENU;

    while (!WindowShouldClose()) {
        UpdateGame();
        DrawGame();
    }

    CloseWindow();
    return 0;
}
