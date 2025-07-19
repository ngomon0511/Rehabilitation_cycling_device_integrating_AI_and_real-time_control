#ifndef MPU6050_H
#define MPU6050_H

#include <Arduino.h>
#include <Wire.h>

#define SDA_PIN 21 
#define SCL_PIN 22

#define AD00 0x68 // left MPU6050
#define AD01 0x69 // right MPU6050

#define DLPF_CONFIG_REG 0x1A

// #define DLPF_CFG_260HZ 0x00
// #define DLPF_CFG_184HZ 0x01
#define DLPF_CFG_94HZ 0x02
// #define DLPF_CFG_44HZ 0x03
// #define DLPF_CFG_21HZ 0x04
// #define DLPF_CFG_10HZ 0x05
// #define DLPF_CFG_5HZ 0x06

#define ACCEL_CONFIG_REG 0x1C

// #define ACCEL_FS_2G 0x00
// #define ACCEL_FS_4G 0x08
// #define ACCEL_FS_8G 0x10
#define ACCEL_FS_16G 0x18

#if defined(ACCEL_FS_2G)
#define ACCEL_SENSITIVITY 16384.0
#elif defined(ACCEL_FS_4G)
#define ACCEL_SENSITIVITY 8192.0
#elif defined(ACCEL_FS_8G)
#define ACCEL_SENSITIVITY 4096.0
#elif defined(ACCEL_FS_16G)
#define ACCEL_SENSITIVITY 2048.0
#endif

#define G_CONSTANT 9.81

#define GYRO_CONFIG_REG 0x1B

// #define GYRO_FS_250DPS 0x00
// #define GYRO_FS_500DPS 0x08
// #define GYRO_FS_1000DPS 0x10
#define GYRO_FS_2000DPS 0x18

#if defined(GYRO_FS_250DPS)
#define GYRO_SENSITIVITY 131.0
#elif defined(GYRO_FS_500DPS)
#define GYRO_SENSITIVITY 65.5
#elif defined(GYRO_FS_1000DPS)
#define GYRO_SENSITIVITY 32.8
#elif defined(GYRO_FS_2000DPS)
#define GYRO_SENSITIVITY 16.4
#endif

class MPU6050 {
public:
  float accelX, accelY, accelZ; // (m/s^2)
  float gyroX, gyroY, gyroZ; // (degree/s)

  MPU6050(uint8_t address);
  void begin();
  void readRawData(uint8_t startReg, int16_t &x, int16_t &y, int16_t &z);
  void readAccel();
  void readGyro();
  bool writeRegister(uint8_t reg, uint8_t data);

private:
  uint8_t MPU_ADDR;
  float CALIB_ACCEL_X, CALIB_ACCEL_Y, CALIB_ACCEL_Z;
  float CALIB_GYRO_X, CALIB_GYRO_Y, CALIB_GYRO_Z;
};

#endif
