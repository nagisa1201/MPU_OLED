/*
 * @Author: Nagisa 2964793117@qq.com
 * @Date: 2025-01-24 23:18:57
 * @LastEditors: Nagisa 2964793117@qq.com
 * @LastEditTime: 2025-01-26 00:12:21
 * @FilePath: \MDK-ARMf:\project\git\MPU_OLED\Core\Src\maincpp.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "maincpp.h"
#include "oled.h"
#include "mpu6050.h"
#include "font.h"
#include <cstdio>


//实例化
OLED::OLED_t oled(&hi2c2);
MPU::MPU6050_t mpu6050(&hi2c1);
char buffer[20];

void InitAll()
{
    oled.OLED_Init();
    while(mpu6050.MPU6050_Init() == 1);
}
void onceloop()
{
    mpu6050.MPU6050_Read_All();
    HAL_Delay(1);
    oled.OLED_NewFrame();
    oled.OLED_PrintASCIIString(3,0,"Pitch:",&afont16x8,OLED_COLOR_NORMAL);
    snprintf(buffer, sizeof(buffer), "%f", mpu6050._MPU6050.KalmanAngleX);
    oled.OLED_PrintASCIIString(50,0,buffer,&afont16x8,OLED_COLOR_NORMAL);

    oled.OLED_PrintASCIIString(3,20,"roll:",&afont16x8,OLED_COLOR_NORMAL);
    snprintf(buffer, sizeof(buffer), "%f", mpu6050._MPU6050.KalmanAngleY);
    oled.OLED_PrintASCIIString(50,20,buffer,&afont16x8,OLED_COLOR_NORMAL);

    oled.OLED_PrintASCIIString(3,40,"yaw:",&afont16x8,OLED_COLOR_NORMAL);

    oled.OLED_ShowFrame();
}


