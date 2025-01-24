/*
 * @Author: Nagisa 2964793117@qq.com
 * @Date: 2025-01-23 21:35:30
 * @LastEditors: Nagisa 2964793117@qq.com
 * @LastEditTime: 2025-01-24 23:44:07
 * @FilePath: \MDK-ARMf:\project\git\MPU_OLED\Hardware\oled.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __OLED_H__
#define __OLED_H__

#include "font.h"
#include "main.h"
#include "string.h"
#include <math.h>
#include <stdlib.h>

typedef enum {
  OLED_COLOR_NORMAL = 0, // 正常模式 黑底白字
  OLED_COLOR_REVERSED    // 反色模式 白底黑字
} OLED_ColorMode;

namespace OLED
{
  class OLEDbase_t
    {
      public:
        OLEDbase_t()=default;
        OLEDbase_t(I2C_HandleTypeDef *hi2c):_hi2c(hi2c)
        {
          OLED_Init();
        };
        ~OLEDbase_t();
        void Send(uint8_t *data, uint8_t len);
        void OLED_SendCmd(uint8_t data);
        void OLED_Init();
        void OLED_SetColorMode(OLED_ColorMode mode);
        void OLED_DisPlay_On();
        void OLED_DisPlay_Off();
        void OLED_NewFrame();
        void OLED_ShowFrame();
        I2C_HandleTypeDef *_hi2c;
    };

    class OLED_t :public OLEDbase_t
    {
      public:
        OLED_t()=default;
        OLED_t(I2C_HandleTypeDef *hi2c):OLEDbase_t(hi2c){};
        ~OLED_t();
   
        void OLED_SetPixel(uint8_t x, uint8_t y, OLED_ColorMode color);

        void OLED_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, OLED_ColorMode color);
        void OLED_DrawRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, OLED_ColorMode color);
        void OLED_DrawFilledRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, OLED_ColorMode color);
        void OLED_DrawCircle(uint8_t x, uint8_t y, uint8_t r, OLED_ColorMode color);
        void OLED_DrawFilledCircle(uint8_t x, uint8_t y, uint8_t r, OLED_ColorMode color);
        void OLED_DrawImage(uint8_t x, uint8_t y, const Image *img, OLED_ColorMode color);

        void OLED_PrintASCIIChar(uint8_t x, uint8_t y, char ch, const ASCIIFont *font, OLED_ColorMode color);
        void OLED_PrintASCIIString(uint8_t x, uint8_t y, char *str, const ASCIIFont *font, OLED_ColorMode color);
        void OLED_PrintString(uint8_t x, uint8_t y, char *str, const Font *font, OLED_ColorMode color);
    };
}
#endif