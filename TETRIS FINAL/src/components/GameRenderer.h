#pragma once
#include <Arduino.h>
#include "Display.h"
#include "GameField.h"
#include "Block.h"

class GameRenderer {
private:
    TetrisDisplay& display;
    LedControl& lc; 

public:
    GameRenderer(TetrisDisplay& disp, LedControl& ledCtrl) : display(disp), lc(ledCtrl) {}

    void render(GameField& field, const Block& current) {
        // Локальний буфер для кадру
        uint8_t buf[MODULES][SCREEN_W] = {};

        for (int y = 0; y < SCREEN_H; y++) {
            uint8_t row = field.getRow(y);
            if (!row) continue;

            int mod = MODULES - 1 - (y / SCREEN_W);
            int vOffset = (y % SCREEN_W);
            int bitPos = (7 - vOffset);

            for (int col = 0; col < SCREEN_W; col++) {
                if (bit_is_set(row, col)) {
                    bitSet(buf[mod][col], bitPos);
                }
            }
        }

        for (int i = 0; i < 4; i++) {
            int fx = current.x + current.shape[i][0];
            int fy = current.y + current.shape[i][1];

            if (fx >= 0 && fx < SCREEN_W && fy >= 0 && fy < SCREEN_H) {
                int mod = MODULES - 1 - (fy / SCREEN_W);
                int bitPos = (7 - (fy % SCREEN_W));
                bitSet(buf[mod][fx], bitPos);
            }
        }

        for (int m = 0; m < MODULES; m++) {
            for (int r = 0; r < SCREEN_W; r++) {
                lc.setRow(m, r, buf[m][r]);
            }
        }
    }
};