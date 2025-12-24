#pragma once
#include "Arduino.h"
#include <LedControl.h>

// картинка кінця
const uint64_t IMAGES[] = {
  0xff0ff9f0f0f90fff,
  0xff1ef3e1e1f31eff,
  0xff3ce7c3c3e73cff,
  0xff78cf8787cf78ff,
  0xfff09f0f0f9ff0ff,
  0xffe13f1e1e3fe1ff,
  0xffc37e3c3c7ec3ff,
  0xff87fc7878fc87ff
};
const int IMAGES_LEN = sizeof(IMAGES) / 8;

// букви
static const uint8_t SPACE[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static const uint8_t G[8] = {0x3E, 0x63, 0x60, 0x6F, 0x66, 0x63, 0x3E, 0x00};
static const uint8_t A[8] = {0x1C, 0x36, 0x63, 0x63, 0x7F, 0x63, 0x63, 0x00};
static const uint8_t M[8] = {0x63, 0x77, 0x7F, 0x6B, 0x63, 0x63, 0x63, 0x00};
static const uint8_t E[8] = {0x7F, 0x60, 0x60, 0x7E, 0x60, 0x60, 0x7F, 0x00};
static const uint8_t O[8] = {0x3E, 0x63, 0x63, 0x63, 0x63, 0x63, 0x3E, 0x00};
static const uint8_t V[8] = {0x63, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x08, 0x00};
static const uint8_t R[8] = {0x7E, 0x63, 0x63, 0x7E, 0x78, 0x6C, 0x66, 0x00};

class TetrisDisplay
{
private:
    LedControl& lc;
    const int numModules;
    const int screenWidth;
    const uint8_t *charToLedLetter(char c);

public:
    TetrisDisplay(LedControl &lc, int numModules, int screenWidth);
    ~TetrisDisplay();

    void setup();
    void clearDisplay();
    void showStartupScreen(); 
    void playExplosionAnimation();
    void gameOverAnimation();
    void PrintText(const char *w1);
};

TetrisDisplay::TetrisDisplay(LedControl &lc, int numModules,  int screenWidth)
    : lc(lc), numModules(numModules), screenWidth(screenWidth)
{
}

TetrisDisplay::~TetrisDisplay()
{
}

void TetrisDisplay::setup()
{
    for (int i = 0; i < numModules; i++)
    {
        lc.shutdown(i, false);
        lc.setIntensity(i, 1);
        lc.clearDisplay(i);
    }
}

void TetrisDisplay::clearDisplay()
{
    for (int i = 0; i < numModules; i++)
    {
        lc.clearDisplay(i);
    }
}

void TetrisDisplay::showStartupScreen()
{
    PrintText(" GO "); 
    delay(2000); 
    clearDisplay();
}

const uint8_t* TetrisDisplay::charToLedLetter(char c)
{
  switch (c)
  {
  case ' ': return SPACE;
  case 'G': return G;
  case 'A': return A;
  case 'M': return M;
  case 'E': return E;
  case 'O': return O;
  case 'V': return V;
  case 'R': return R;
  
  default:  return SPACE;
  }
}

void TetrisDisplay::PrintText(const char *w1)
{
  for (int m = 0; m < numModules; m++)
  {
    const uint8_t *letter = charToLedLetter(w1[m]);
    uint8_t rotatedLetter[screenWidth] = {0};
    
    // Поворот букви на 90 градусів (для правильного відображення)
    for (int y = 0; y < screenWidth; y++)
    {
      for (int x = 0; x < screenWidth; x++)
      {
        if (bitRead(letter[y], x))
        {
          int newX = 7 - y;
          int newY = x;
          bitSet(rotatedLetter[newY], newX);
        }
      }
    }
    int module = numModules - 1 - m;
    for (int row = 0; row < screenWidth; row++)
    {
      lc.setRow(module, row, rotatedLetter[row]);
    }
  }
}

void TetrisDisplay::playExplosionAnimation() {
    for(int frame = 0; frame < IMAGES_LEN; frame++) {
        uint64_t img = IMAGES[frame];
        for(int r = 0; r < 8; r++) {
            byte rowData = (img >> (r * 8)) & 0xFF;
            for(int m = 0; m < numModules; m++) {
                lc.setRow(m, r, rowData);
            }
        }
        delay(150);
    }
    clearDisplay();
}

void TetrisDisplay::gameOverAnimation()
{
  playExplosionAnimation();
  delay(300);

  clearDisplay();
  delay(100);

  const char *w1 = "GAME";
  PrintText(w1);
  delay(1500); 

  const char *w2 = "OVER";
  PrintText(w2);
  delay(2000); 
}