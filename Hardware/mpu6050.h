/*
 * @Author: Nagisa 2964793117@qq.com
 * @Date: 2025-01-23 21:35:56
 * @LastEditors: Nagisa 2964793117@qq.com
 * @LastEditTime: 2025-01-25 00:06:01
 * @FilePath: \MDK-ARMf:\project\git\MPU_OLED\Hardware\mpu6050.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/*
 * mpu6050.h
 *
 *  Created on: Nov 13, 2019
 *      Author: Bulanov Konstantin
 */

#ifndef __MPU6050_H_
#define __MPU6050_H_



#include <stdint.h>
#include "main.h"
#include <math.h>

namespace MPU
{
    // MPU6050 structure
    typedef struct
    {

        int16_t Accel_X_RAW;
        int16_t Accel_Y_RAW;
        int16_t Accel_Z_RAW;
        double Ax;
        double Ay;
        double Az;

        int16_t Gyro_X_RAW;
        int16_t Gyro_Y_RAW;
        int16_t Gyro_Z_RAW;
        double Gx;
        double Gy;
        double Gz;

        float Temperature;

        double KalmanAngleX;
        double KalmanAngleY;
    } MPU6050;

    // Kalman structure
    typedef struct
    {
        double Q_angle;
        double Q_bias;
        double R_measure;
        double angle;
        double bias;
        double P[2][2];
    } Kalman;



class MPU6050_t
    {
        public:
            MPU6050_t()=default;
            MPU6050_t(I2C_HandleTypeDef *I2Cx):_I2Cx(I2Cx){};

            ~MPU6050_t();
            uint8_t MPU6050_Init();

            void MPU6050_Read_Accel();

            void MPU6050_Read_Gyro();

            void MPU6050_Read_Temp();

            void MPU6050_Read_All();

            double Kalman_getAngle(Kalman& kalman,double newAngle, double newRate, double dt);
            

            MPU6050 _MPU6050 = {0};
            Kalman _KalmanX = {0.001f,0.003f,0.03f};
            Kalman _KalmanY = {0.001f,0.003f,0.03f};
        private:
            I2C_HandleTypeDef *_I2Cx;
    };

}
#endif /* INC_GY521_H_ */