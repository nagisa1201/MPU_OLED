/*
 * mpu6050.c
 *
 *  Created on: Nov 13, 2019
 *      Author: Bulanov Konstantin
 *
 *  Contact information
 *  -------------------
 *
 * e-mail   :  leech001@gmail.com
 */

/*
 * |---------------------------------------------------------------------------------
 * | Copyright (C) Bulanov Konstantin,2021
 * |
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * |
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |
 * | Kalman filter algorithm used from https://github.com/TKJElectronics/KalmanFilter
 * |---------------------------------------------------------------------------------
 */
#include "mpu6050.h"

#define RAD_TO_DEG 57.295779513082320876798154814105

#define WHO_AM_I_REG 0x75
#define PWR_MGMT_1_REG 0x6B
#define SMPLRT_DIV_REG 0x19
#define ACCEL_CONFIG_REG 0x1C
#define ACCEL_XOUT_H_REG 0x3B
#define TEMP_OUT_H_REG 0x41
#define GYRO_CONFIG_REG 0x1B
#define GYRO_XOUT_H_REG 0x43

// Setup MPU6050
#define MPU6050_ADDR 0xD0
using namespace MPU;

const uint16_t i2c_timeout = 100;
const double Accel_Z_corrector = 14418.0;

uint32_t timer;

MPU6050_t::~MPU6050_t() {}   
uint8_t MPU6050_t::MPU6050_Init()
{
    uint8_t check;
    uint8_t Data;
    // check device ID WHO_AM_I
    HAL_I2C_Mem_Read(_I2Cx, MPU6050_ADDR, WHO_AM_I_REG, 1, &check, 1, i2c_timeout);

    if (check == 104) // 0x68 will be returned by the sensor if everything goes well
    {
        // power management register 0X6B we should write all 0's to wake the sensor up
        Data = 0;
        HAL_I2C_Mem_Write(_I2Cx, MPU6050_ADDR, PWR_MGMT_1_REG, 1, &Data, 1, i2c_timeout);

        // Set DATA RATE of 1KHz by writing SMPLRT_DIV register
        Data = 0x07;
        HAL_I2C_Mem_Write(_I2Cx, MPU6050_ADDR, SMPLRT_DIV_REG, 1, &Data, 1, i2c_timeout);

        // Set accelerometer configuration in ACCEL_CONFIG Register
        // XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 . � 2g
        Data = 0x00;
        HAL_I2C_Mem_Write(_I2Cx, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &Data, 1, i2c_timeout);

        // Set Gyroscopic configuration in GYRO_CONFIG Register
        // XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=0 . � 250 �/s
        Data = 0x00;
        HAL_I2C_Mem_Write(_I2Cx, MPU6050_ADDR, GYRO_CONFIG_REG, 1, &Data, 1, i2c_timeout);
        return 0;
    }
    return 1;
}

void MPU6050_t::MPU6050_Read_Accel()
{
    uint8_t Rec_Data[6];

    // Read 6 BYTES of data starting from ACCEL_XOUT_H register

    HAL_I2C_Mem_Read(_I2Cx, MPU6050_ADDR, ACCEL_XOUT_H_REG, 1, Rec_Data, 6, i2c_timeout);

    _MPU6050.Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
    _MPU6050.Accel_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data[3]);
    _MPU6050.Accel_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data[5]);

    /*** convert the RAW values into acceleration in 'g'
         we have to divide according to the Full scale value set in FS_SEL
         I have configured FS_SEL = 0. So I am dividing by 16384.0
         for more details check ACCEL_CONFIG Register              ****/

    _MPU6050.Ax = _MPU6050.Accel_X_RAW / 16384.0;
    _MPU6050.Ay = _MPU6050.Accel_Y_RAW / 16384.0;
    _MPU6050.Az = _MPU6050.Accel_Z_RAW / Accel_Z_corrector;
}

void MPU6050_t::MPU6050_Read_Gyro()
{
    uint8_t Rec_Data[6];

    // Read 6 BYTES of data starting from GYRO_XOUT_H register

    HAL_I2C_Mem_Read(_I2Cx, MPU6050_ADDR, GYRO_XOUT_H_REG, 1, Rec_Data, 6, i2c_timeout);

    _MPU6050.Gyro_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
    _MPU6050.Gyro_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data[3]);
    _MPU6050.Gyro_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data[5]);

    /*** convert the RAW values into dps (�/s)
         we have to divide according to the Full scale value set in FS_SEL
         I have configured FS_SEL = 0. So I am dividing by 131.0
         for more details check GYRO_CONFIG Register              ****/

    _MPU6050.Gx = _MPU6050.Gyro_X_RAW / 131.0;
    _MPU6050.Gy = _MPU6050.Gyro_Y_RAW / 131.0;
    _MPU6050.Gz = _MPU6050.Gyro_Z_RAW / 131.0;
}

void MPU6050_t::MPU6050_Read_Temp()
{
    uint8_t Rec_Data[2];
    int16_t temp;

    // Read 2 BYTES of data starting from TEMP_OUT_H_REG register

    HAL_I2C_Mem_Read(_I2Cx, MPU6050_ADDR, TEMP_OUT_H_REG, 1, Rec_Data, 2, i2c_timeout);

    temp = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
    _MPU6050.Temperature = (float)((int16_t)temp / (float)340.0 + (float)36.53);
}

void MPU6050_t::MPU6050_Read_All()
{
    uint8_t Rec_Data[14];
    int16_t temp;

    // Read 14 BYTES of data starting from ACCEL_XOUT_H register

    HAL_I2C_Mem_Read(_I2Cx, MPU6050_ADDR, ACCEL_XOUT_H_REG, 1, Rec_Data, 14, i2c_timeout);

    _MPU6050.Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
    _MPU6050.Accel_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data[3]);
    _MPU6050.Accel_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data[5]);
    temp = (int16_t)(Rec_Data[6] << 8 | Rec_Data[7]);
    _MPU6050.Gyro_X_RAW = (int16_t)(Rec_Data[8] << 8 | Rec_Data[9]);
    _MPU6050.Gyro_Y_RAW = (int16_t)(Rec_Data[10] << 8 | Rec_Data[11]);
    _MPU6050.Gyro_Z_RAW = (int16_t)(Rec_Data[12] << 8 | Rec_Data[13]);

    _MPU6050.Ax = _MPU6050.Accel_X_RAW / 16384.0;
    _MPU6050.Ay = _MPU6050.Accel_Y_RAW / 16384.0;
    _MPU6050.Az = _MPU6050.Accel_Z_RAW / Accel_Z_corrector;
    _MPU6050.Temperature = (float)((int16_t)temp / (float)340.0 + (float)36.53);
    _MPU6050.Gx = _MPU6050.Gyro_X_RAW / 131.0;
    _MPU6050.Gy = _MPU6050.Gyro_Y_RAW / 131.0;
    _MPU6050.Gz = _MPU6050.Gyro_Z_RAW / 131.0;

    // Kalman angle solve
    double dt = (double)(HAL_GetTick() - timer) / 1000;
    timer = HAL_GetTick();
    double roll;
    double roll_sqrt = sqrt(
        _MPU6050.Accel_X_RAW * _MPU6050.Accel_X_RAW + _MPU6050.Accel_Z_RAW * _MPU6050.Accel_Z_RAW);
    if (roll_sqrt != 0.0)
    {
        roll = atan(_MPU6050.Accel_Y_RAW / roll_sqrt) * RAD_TO_DEG;
    }
    else
    {
        roll = 0.0;
    }
    double pitch = atan2(-_MPU6050.Accel_X_RAW, _MPU6050.Accel_Z_RAW) * RAD_TO_DEG;
    if ((pitch < -90 && _MPU6050.KalmanAngleY > 90) || (pitch > 90 && _MPU6050.KalmanAngleY < -90))
    {
        _KalmanY.angle = pitch;
        _MPU6050.KalmanAngleY = pitch;
    }
    else
    {
        _MPU6050.KalmanAngleY = Kalman_getAngle(_KalmanY, pitch, _MPU6050.Gy, dt);
    }
    if (fabs(_MPU6050.KalmanAngleY) > 90)
        _MPU6050.Gx = -_MPU6050.Gx;
    _MPU6050.KalmanAngleX = Kalman_getAngle(_KalmanX, roll, _MPU6050.Gx, dt);
}

double MPU6050_t::Kalman_getAngle(Kalman& kalman,double newAngle, double newRate, double dt)
{
    double rate = newRate - kalman.bias;
    kalman.angle += dt * rate;

    kalman.P[0][0] += dt * (dt * kalman.P[1][1] - kalman.P[0][1] - kalman.P[1][0] + kalman.Q_angle);
    kalman.P[0][1] -= dt * kalman.P[1][1];
    kalman.P[1][0] -= dt * kalman.P[1][1];
    kalman.P[1][1] += kalman.Q_bias * dt;

    double S = kalman.P[0][0] + kalman.R_measure;
    double K[2];
    K[0] = kalman.P[0][0] / S;
    K[1] = kalman.P[1][0] / S;

    double y = newAngle - kalman.angle;
    kalman.angle += K[0] * y;
    kalman.bias += K[1] * y;

    double P00_temp = kalman.P[0][0];
    double P01_temp = kalman.P[0][1];

    kalman.P[0][0] -= K[0] * P00_temp;
    kalman.P[0][1] -= K[0] * P01_temp;
    kalman.P[1][0] -= K[1] * P00_temp;
    kalman.P[1][1] -= K[1] * P01_temp;

    return kalman.angle;
};
