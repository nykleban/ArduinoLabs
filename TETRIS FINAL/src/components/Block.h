#pragma once
#include <Arduino.h>
#include "Shapes.h"

struct Block {
    const int (*shape)[2]; // Вказівник на поточну форму
    int x, y;
    int rotation;
    char type;

    void spawnRandom(int startX) {
        int r = random(7);
        x = startX;
        y = 0;
        rotation = 0;
        
        switch (r) {
            case 0: type = 'I'; shape = I_SHAPE[0]; break;
            case 1: type = 'O'; shape = O_SHAPE[0]; break;
            case 2: type = 'T'; shape = T_SHAPE[0]; break;
            case 3: type = 'L'; shape = L_SHAPE[0]; break;
            case 4: type = 'J'; shape = J_SHAPE[0]; break;
            case 5: type = 'S'; shape = S_SHAPE[0]; break;
            case 6: type = 'Z'; shape = Z_SHAPE[0]; break;
            
        }
    }
};