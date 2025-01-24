/*
 * @Author: Nagisa 2964793117@qq.com
 * @Date: 2025-01-24 23:18:57
 * @LastEditors: Nagisa 2964793117@qq.com
 * @LastEditTime: 2025-01-25 00:15:45
 * @FilePath: \MDK-ARMf:\project\git\MPU_OLED\Core\Src\maincpp.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "maincpp.h"
#include "oled.h"
#include "mpu6050.h"
#include "font.h"

//实例化
OLED::OLED_t oled(&hi2c2);
MPU::MPU6050_t mpu6050(&hi2c1);


void onceloop()
{
    mpu6050.MPU6050_Read_All();
    HAL_Delay (1);
}


