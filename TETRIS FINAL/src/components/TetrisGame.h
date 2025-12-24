#pragma once
#include "Block.h"
#include "GameField.h"
#include "InputSystem.h"
#include "SoundSystem.h"
#include "Display.h"
#include "GameRenderer.h"

class TetrisGame {
private:
    // Компоненти
    TetrisDisplay& display;
    SoundSystem& sound;
    InputSystem& input;
    GameField field;
    GameRenderer renderer;
    Block currentBlock;

    // Таймери
    unsigned long lastMove = 0;
    unsigned long lastDrop = 0;
    unsigned long lastRefresh = 0;
    const unsigned long moveInterval = 200;
    const unsigned long refreshInterval = 33;
    unsigned long dropInterval = 500;

public:
    TetrisGame(TetrisDisplay& d, SoundSystem& s, InputSystem& i, LedControl& lc)
        : display(d), sound(s), input(i), field(&s), renderer(d, lc) {}

    void setup() {
        display.setup();
        display.showStartupScreen();
        input.init();
        restart();
    }

    void restart() {
        field.reset();
        display.clearDisplay();
        sound.playIntro();
        currentBlock.spawnRandom(SCREEN_W / 2 - 2);
        lastDrop = millis();
    }

    void update() {
        unsigned long now = millis();

        if (now - lastMove > moveInterval) {
            int dx = input.getHorizontalMove();
            if (dx != 0) {
                if (!field.checkCollision(currentBlock, currentBlock.x + dx, currentBlock.y, currentBlock.shape)) {
                    currentBlock.x += dx;
                    sound.playMove();
                }
                lastMove = now;
            }
        }

        if (input.isRotatePressed()) {
            tryRotate();
        }

        dropInterval = input.getDropSpeed();
        if (now - lastDrop > dropInterval) {
            processDrop();
            lastDrop = now;
        }

        if (now - lastRefresh >= refreshInterval) {
            renderer.render(field, currentBlock);
            lastRefresh = now;
        }
    }

private:
    void processDrop() {
        // Якщо немає колізії знизу - падаємо
        if (!field.checkCollision(currentBlock, currentBlock.x, currentBlock.y + 1, currentBlock.shape)) {
            currentBlock.y++;
        } else {
            // Перевірка на Game Over (якщо вперлися у верхній край)
            bool hitTop = false;
            for (int i = 0; i < 4; i++) {
                if (currentBlock.y + currentBlock.shape[i][1] <= 0) hitTop = true;
            }

            if (hitTop) {
                sound.playGameOver();
                display.gameOverAnimation();
                input.waitForClick();
                restart();
            } else {
                // Фіксуємо блок і створюємо новий
                field.lockBlock(currentBlock);
                currentBlock.spawnRandom(SCREEN_W / 2 - 2);
            }
        }
    }

    void tryRotate() {
        int nextRot = currentBlock.rotation + 1;
        // Ліміти поворотів для різних фігур
        int limit = 4;
        if (currentBlock.type == 'O') limit = 1;
        else if (currentBlock.type == 'I' || currentBlock.type == 'S' || currentBlock.type == 'Z') limit = 2;
        
        if (nextRot >= limit) nextRot = 0;

        // Вибір правильної форми
        const int (*nextShape)[2] = nullptr;
        switch (currentBlock.type) {
            case 'I': nextShape = I_SHAPE[nextRot]; break;
            case 'O': nextShape = O_SHAPE[0]; break;
            case 'T': nextShape = T_SHAPE[nextRot]; break;
            case 'L': nextShape = L_SHAPE[nextRot]; break;
            case 'J': nextShape = J_SHAPE[nextRot]; break;
            case 'S': nextShape = S_SHAPE[nextRot]; break;
            case 'Z': nextShape = Z_SHAPE[nextRot]; break;
        }

        if (!field.checkCollision(currentBlock, currentBlock.x, currentBlock.y, nextShape)) {
            currentBlock.rotation = nextRot;
            currentBlock.shape = nextShape;
            sound.playMove();
        }
    }
};