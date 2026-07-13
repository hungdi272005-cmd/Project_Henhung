#include <gui/screen2_screen/Screen2View.hpp>
#include <stdlib.h>
#include <touchgfx/Color.hpp>
#include "stm32f4xx_hal.h" // Bắt buộc phải có để gọi thư viện HAL Flash
#include "main.h"
#define FLASH_SCORE_ADDR   0x080E0000   // Địa chỉ an toàn tại Sector 11
// Hàm đọc điểm cao nhất từ Flash
uint32_t readBestScore() {
    uint32_t val = *(__IO uint32_t*)FLASH_SCORE_ADDR;
    // Nếu Flash chưa từng được ghi (trắng), giá trị mặc định của nó là 0xFFFFFFFF
    if (val == 0xFFFFFFFF) return 0;
    return val;
}

// Hàm ghi điểm mới vào Flash
void saveBestScore(uint32_t newScore) {
    HAL_FLASH_Unlock(); // Mở khóa kho lưu trữ

    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError = 0;

    // Xóa sạch vùng nhớ cũ trước khi ghi (Bắt buộc với Flash)
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    EraseInitStruct.Sector = FLASH_SECTOR_11;
    EraseInitStruct.NbSectors = 1;

    HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);

    // Ghi điểm mới vào
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_SCORE_ADDR, newScore);

    HAL_FLASH_Lock(); // Khóa lại cho an toàn
}

Screen2View::Screen2View() :
    currentPieceIndex(-1),
    tickCounter(0),
    dropSpeedTicks(40)
{
}

void Screen2View::setupScreen()
{
	Screen2ViewBase::setupScreen();
	// 🔥 LẤY MILI-GIÂY BẠN BẤM NÚT START ĐỂ TẠO SỰ NGẪU NHIÊN THẬT
	srand(HAL_GetTick());
	// 1. Tắt cờ game over và ẩn bảng Popup đi
	isGameOver = false;
	GameOverContainer.setVisible(false);
	GameOverContainer.invalidate();


//    saveBestScore(0);
    for (int r = 0; r < 16; r++) {
        for (int c = 0; c < 12; c++) {
            board[r][c] = 0;
        }
    }
    // 3. Reset điểm hiện tại
    currentScore = 0;
    Unicode::snprintf(ScoretextBuffer, SCORETEXT_SIZE, "%d", currentScore);
    Scoretext.invalidate();

    // 4. LOAD KỶ LỤC TỪ FLASH LÊN CHỮ BEST (BẠN ĐANG THIẾU ĐOẠN NÀY ĐÓ)
	uint32_t bestScore = readBestScore();
	Unicode::snprintf(BesttextBuffer, BESTTEXT_SIZE, "%d", bestScore);
	// ^ (Lưu ý: Chữ BesttextBuffer có thể khác tùy tên bạn đặt trong TouchGFX, thường là BesttextBuffer)
        Besttext.invalidate();
    int startOffset_X = 0;
    // 5. Khởi tạo mảng gạch nền
    for (int r = 0; r < 16; r++) {
        for (int c = 0; c < 12; c++) {
            dynamicBlocks[r][c].setPosition(startOffset_X + (c * 20), r * 20, 20, 20);
            dynamicBlocks[r][c].setColor(touchgfx::Color::getColorFromRGB(230, 230, 0));
            dynamicBlocks[r][c].setVisible(false);
            add(dynamicBlocks[r][c]);
        }
    }
    // 6. Khởi tạo khối gạch đang rơi
    for (int i = 0; i < 4; i++) {
        fallingPieceBlocks[i].setPosition(0, 0, 20, 20);
        fallingPieceBlocks[i].setVisible(false);
        add(fallingPieceBlocks[i]);
    }
    // Đưa GameOverContainer lên lớp trên cùng đè lên toàn bộ gạch
    remove(GameOverContainer); // Xóa khỏi vị trí cũ ở lớp đáy
	add(GameOverContainer);    // Thêm lại vào cuối cùng để hiện đè lên toàn bộ gạch
	// Khởi động còi báo hiệu START
	buzzerMode = 1; // Bật chế độ Tinh tinh
	buzzerTick = 0; // Reset bộ đếm nhịp
    spawnNewPiece();
}

void Screen2View::tearDownScreen()
{
    // Lấy kỷ lục cũ trong máy ra
    uint32_t bestScore = readBestScore();

    // Nếu điểm ván này cao hơn kỷ lục cũ -> Phá kỷ lục! Ghi đè vào mạch.
    if (currentScore > bestScore) {
        saveBestScore(currentScore);
    }

    Screen2ViewBase::tearDownScreen();
}

void Screen2View::spawnNewPiece()
{
    currentPieceIndex = rand() % 7;
    pieceX = 4;
    pieceY = 0;

    for (int i = 0; i < 4; i++) {
        currentPieceShape[i][0] = tetrominoTemplates[currentPieceIndex][i][0];
        currentPieceShape[i][1] = tetrominoTemplates[currentPieceIndex][i][1];
    }

    uint8_t r = (pieceColors[currentPieceIndex] >> 16) & 0xFF;
    uint8_t g = (pieceColors[currentPieceIndex] >> 8) & 0xFF;
    uint8_t b = pieceColors[currentPieceIndex] & 0xFF;

    for (int i = 0; i < 4; i++) {
        fallingPieceBlocks[i].setColor(touchgfx::Color::getColorFromRGB(r, g, b));
        fallingPieceBlocks[i].setVisible(true);
    }

    updateFallingPieceInPixels();

    if (checkCollision(pieceX, pieceY)) {
    	// 1. Kiểm tra và lưu kỷ lục vào Flash luôn
		uint32_t bestScore = readBestScore();
		if (currentScore > bestScore) {
			saveBestScore(currentScore);
		}

		// 2. Bật cờ Game Over để đóng băng game
		isGameOver = true;

		// 3. Hiển thị bảng Game Over lên
		GameOverContainer.setVisible(true);
		GameOverContainer.invalidate();
		// THÊM 2 DÒNG NÀY ĐỂ KÍCH HOẠT CÒI GAME OVER:
		buzzerMode = 2; // Bật chế độ Tình tình
		buzzerTick = 0;
    }
}

void Screen2View::updateFallingPieceInPixels()
{
    int startOffset_X = 0;
    for (int i = 0; i < 4; i++) {
        int colOffset = currentPieceShape[i][0];
        int rowOffset = currentPieceShape[i][1];

        int16_t xPixel = startOffset_X + ((pieceX + colOffset) * 20);
        int16_t yPixel = (pieceY + rowOffset) * 20;

        fallingPieceBlocks[i].moveTo(xPixel, yPixel);
        fallingPieceBlocks[i].invalidate();
    }
}

void Screen2View::handleTickEvent()
{
	// ============================================================
	// LOGIC ĐIỀU KHIỂN CÒI BẰNG CHÂN PA1 (60 Ticks = 1 Giây)
	// ============================================================
	if (buzzerMode > 0) {
		buzzerTick++;

		if (buzzerMode == 1) {
			// HIỆU ỨNG: TINH TINH (Kéo dài thời gian ra)
			if (buzzerTick == 1)       HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);
			else if (buzzerTick == 15)  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
			else if (buzzerTick == 25) HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);
			else if (buzzerTick == 40) {
				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
				buzzerMode = 0;
			}
		}
		else if (buzzerMode == 2) {
			// HIỆU ỨNG: TÌNH TÌNH
			if (buzzerTick == 1)       HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);
			else if (buzzerTick == 30) HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
			else if (buzzerTick == 45) HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);
			else if (buzzerTick == 90) {
				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
				buzzerMode = 0;
			}
		}
	}
	// ============================================================
	// Nếu Game Over rồi thì đóng băng toàn bộ, không cho phím nào hay gạch nào chạy nữa
	    if (isGameOver) {
	        return;
	    }
    // Đồng bộ xử lý cả 4 nút ngay trong chu kỳ vẽ đồ họa
    if (presenter->isLeftPressed()) {
        presenter->clearLeftFlag();
        moveLeftPressed();
    }
    if (presenter->isRightPressed()) {
        presenter->clearRightFlag();
        moveRightPressed();
    }
    if (presenter->isRotatePressed()) {
        presenter->clearRotateFlag();
        rotatePiecePressed();
    }
    if (presenter->isDownPressed()) {
        presenter->clearDownFlag();
        moveDownPressed();
    }

    tickCounter++;
    if (tickCounter >= dropSpeedTicks)
    {
        tickCounter = 0;

        if (!checkCollision(pieceX, pieceY + 1)) {
            pieceY++;
            updateFallingPieceInPixels();
        } else {
            lockPiece();
            spawnNewPiece();
        }
        dropSpeedTicks = 40;
    }
}

bool Screen2View::checkCollision(int nextX, int nextY)
{
    for (int i = 0; i < 4; i++) {
        int col = nextX + currentPieceShape[i][0];
        int row = nextY + currentPieceShape[i][1];

        if (col < 0 || col >= 12) return true;
        if (row >= 16) return true;
        if (row >= 0 && board[row][col] != 0) return true;
    }
    return false;
}

void Screen2View::lockPiece()
{
    for (int i = 0; i < 4; i++) {
        int col = pieceX + currentPieceShape[i][0];
        int row = pieceY + currentPieceShape[i][1];

        if (row >= 0 && row < 16 && col >= 0 && col < 12) {
            board[row][col] = 1;
        }
    }

    for (int i = 0; i < 4; i++) {
        fallingPieceBlocks[i].setVisible(false);
        fallingPieceBlocks[i].invalidate();
    }

    checkAndClearLines();
    drawBoard();
    dropSpeedTicks = 40;
}

void Screen2View::checkAndClearLines()
{
    bool needRedraw = false;
    for (int r = 15; r >= 0; r--)
    {
        bool isLineFull = true;
        for (int c = 0; c < 12; c++) {
            if (board[r][c] == 0) {
                isLineFull = false;
                break;
            }
        }

        if (isLineFull)
        {
            currentScore += 10;
            Unicode::snprintf(ScoretextBuffer, SCORETEXT_SIZE, "%d", currentScore);
            Scoretext.invalidate();

            for (int rowToMove = r; rowToMove > 0; rowToMove--) {
                for (int col = 0; col < 12; col++) {
                    board[rowToMove][col] = board[rowToMove - 1][col];
                }
            }
            for (int col = 0; col < 12; col++) {
                board[0][col] = 0;
            }

            needRedraw = true;
            r++;
        }
    }

    if (needRedraw)
    {
        drawBoard();
    }
}

void Screen2View::drawBoard()
{
    for (int r = 0; r < 16; r++)
    {
        for (int c = 0; c < 12; c++)
        {
            if (board[r][c] == 1)
            {
                if (!dynamicBlocks[r][c].isVisible())
                {
                    dynamicBlocks[r][c].setVisible(true);
                    dynamicBlocks[r][c].invalidate();
                }
            }
            else
            {
                if (dynamicBlocks[r][c].isVisible())
                {
                    dynamicBlocks[r][c].setVisible(false);
                    dynamicBlocks[r][c].invalidate();
                }
            }
        }
    }
}

void Screen2View::moveLeftPressed()
{
    if (!checkCollision(pieceX - 1, pieceY))
    {
        pieceX--;
        updateFallingPieceInPixels();
    }
}

void Screen2View::moveRightPressed()
{
    if (!checkCollision(pieceX + 1, pieceY))
    {
        pieceX++;
        updateFallingPieceInPixels();
    }
}

void Screen2View::moveDownPressed()
{
	// --- ĐOẠN CODE DEBUG: Cứ nhận được tín hiệu Rơi nhanh là cộng 10 điểm ---
//	currentScore += 10;
//	Unicode::snprintf(ScoretextBuffer, SCORETEXT_SIZE, "%d", currentScore);
//	Scoretext.invalidate();
//	// ------------------------------------------------------------------
    if (!checkCollision(pieceX, pieceY + 1))
    {
        pieceY++;
        updateFallingPieceInPixels();
    }
    dropSpeedTicks = 5;
}

void Screen2View::moveDownReleased()
{
//    if (!checkCollision(pieceX, pieceY + 1))
//    {
//        pieceY++;
//        updateFallingPieceInPixels();
//    }
    dropSpeedTicks = 40;
}

void Screen2View::rotatePiecePressed()
{
    if (currentPieceIndex == 1) return; // Khối O không xoay

    int tempTemplates[4][2];
    int centerX = currentPieceShape[1][0];
    int centerY = currentPieceShape[1][1];

    for (int i = 0; i < 4; i++)
    {
        int oldX = currentPieceShape[i][0];
        int oldY = currentPieceShape[i][1];

        tempTemplates[i][0] = centerX - (oldY - centerY);
        tempTemplates[i][1] = centerY + (oldX - centerX);
    }

    bool canRotate = true;
    for (int i = 0; i < 4; i++)
    {
        int nextCol = pieceX + tempTemplates[i][0];
        int nextRow = pieceY + tempTemplates[i][1];

        if (nextCol < 0 || nextCol >= 12 || nextRow >= 16)
        {
            canRotate = false;
            break;
        }
        if (nextRow >= 0 && board[nextRow][nextCol] != 0)
        {
            canRotate = false;
            break;
        }
    }

    if (canRotate)
    {
        for (int i = 0; i < 4; i++)
        {
            currentPieceShape[i][0] = tempTemplates[i][0];
            currentPieceShape[i][1] = tempTemplates[i][1];
        }
        updateFallingPieceInPixels();
    }
}
void Screen2View::replayGame()
{
    // 1. Tắt cờ Game Over và ẩn bảng thông báo đi
    isGameOver = false;
    GameOverContainer.setVisible(false);
    GameOverContainer.invalidate();

    // 2. Quét sạch ma trận bàn cờ (đưa tất cả về 0)
    for (int r = 0; r < 16; r++) {
        for (int c = 0; c < 12; c++) {
            board[r][c] = 0;
        }
    }

    // 3. Reset điểm số về 0 và cập nhật lên màn hình
    currentScore = 0;
    Unicode::snprintf(ScoretextBuffer, SCORETEXT_SIZE, "%d", currentScore);
    Scoretext.invalidate();
    // 🎯 CẬP NHẬT ĐIỂM BEST MỚI NHẤT NGAY LẬP TỨC KHI ẤN REPLAY
        // ======================================================
	uint32_t bestScore = readBestScore();
	Unicode::snprintf(BesttextBuffer, BESTTEXT_SIZE, "%d", bestScore);
	Besttext.invalidate();

    // 4. Xóa sạch các khối gạch cũ đang hiển thị trên đồ họa
    drawBoard(); // Hàm này duyệt ma trận (đang toàn 0) và tự động ẩn hết gạch đi

    // 5. Thả một khối gạch mới xuống để bắt đầu ván mới!
    spawnNewPiece();
}
