#ifndef SCREEN2VIEW_HPP
#define SCREEN2VIEW_HPP

#include <gui_generated/screen2_screen/Screen2ViewBase.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <touchgfx/widgets/Box.hpp>

class Screen2View : public Screen2ViewBase
{
public:
    Screen2View();
    virtual ~Screen2View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

    void moveLeftPressed();
    void moveRightPressed();
    void moveDownPressed();
    void moveDownReleased();
    void rotatePiecePressed();

    virtual void replayGame();

protected:
    touchgfx::Box fallingPieceBlocks[4];
    int pieceX, pieceY;
    int currentPieceIndex;
    int tickCounter;
    int dropSpeedTicks;
    bool isGameOver;
    uint8_t board[16][12];
    int currentScore;
    touchgfx::Box dynamicBlocks[16][12];

    // Mảng cấu hình chuẩn (Không có const)
    int tetrominoTemplates[7][4][2] = {
        { {0,0}, {1,0}, {2,0}, {3,0} }, // 0. I
        { {0,0}, {1,0}, {0,1}, {1,1} }, // 1. O
        { {1,0}, {0,1}, {1,1}, {2,1} }, // 2. T
        { {0,1}, {1,1}, {1,0}, {2,0} }, // 3. Z
        { {0,0}, {1,0}, {1,1}, {2,1} }, // 4. S
        { {0,0}, {1,0}, {2,0}, {2,1} }, // 5. L
        { {0,1}, {1,1}, {2,1}, {2,0} }  // 6. J
    };

    // Mảng động RAM để xoay gạch
    int currentPieceShape[4][2];

    const uint32_t pieceColors[7] = {
        0x00FFFF, 0xFFFF00, 0x800080, 0xFF0000, 0x00FF00, 0xFFA500, 0x0000FF
    };

    bool checkCollision(int nextX, int nextY);
    void lockPiece();
    void spawnNewPiece();
    void checkAndClearLines();
    void drawBoard();
    void updateFallingPieceInPixels();
    int buzzerMode; // 0: Im lặng, 1: Tinh tinh (Start), 2: Tình tình (GameOver)
	int buzzerTick;
};

#endif // SCREEN2VIEW_HPP
