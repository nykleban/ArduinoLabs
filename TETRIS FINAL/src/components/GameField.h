#pragma once
#include <Arduino.h>
#include "Block.h"
#include "SoundSystem.h"

#define SCREEN_W 8
#define MODULES 4
#define SCREEN_H (SCREEN_W * MODULES)

class GameField {
private:
    uint8_t field[SCREEN_H];
    SoundSystem* sound;

public:
    GameField(SoundSystem* s) : sound(s) {
        reset();
    }

    void reset() {
        memset(field, 0, sizeof(field));
    }

    // Перевірка колізій
    bool checkCollision(const Block& block, int newX, int newY, const int (*newShape)[2]) {
        for (int i = 0; i < 4; i++) {
            int fx = newX + newShape[i][0];
            int fy = newY + newShape[i][1];

            // Вихід за межі
            if (fx < 0 || fx >= SCREEN_W || fy >= SCREEN_H) return true;
            
            // Зіткнення з існуючими блоками
            if (fy >= 0 && bit_is_set(field[fy], fx)) return true;
        }
        return false;
    }


    void lockBlock(const Block& b) {
        sound->playLock();
        for (int i = 0; i < 4; i++) {
            int xx = b.x + b.shape[i][0];
            int yy = b.y + b.shape[i][1];
            if (yy >= 0 && yy < SCREEN_H) {
                bitSet(field[yy], xx);
            }
        }
        clearFullRows();
    }

    void clearFullRows() {
        bool cleared = false;
        for (int y = 0; y < SCREEN_H; y++) {
            if (field[y] == 0xFF) { // Рядок повний
                cleared = true;
                for (int j = y; j > 0; j--) {
                    field[j] = field[j - 1];
                }
                field[0] = 0;
            }
        }
        if (cleared) sound->playLineClear();
    }

    uint8_t getRow(int y) {
        return field[y];
    }
};