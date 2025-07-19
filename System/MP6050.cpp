#include "MPU6050.h"

MPU6050::MPU6050(uint8_t address) {
  MPU_ADDR = address;

  accelX = 0;
  accelY = 0;
  accelZ = 0;
  if (MPU_ADDR == AD00) {  
    CALIB_ACCEL_X = -0.21;
    CALIB_ACCEL_Y = -0.05;
    CALIB_ACCEL_Z = -0.85;
  } else {  
    CALIB_ACCEL_X = -0.16;
    CALIB_ACCEL_Y = -0.03;
    CALIB_ACCEL_Z = 0.61;
  }

  gyroX = 0;
  gyroY = 0;
  gyroZ = 0;
  if (MPU_ADDR == AD00) {
    CALIB_GYRO_X = 1.57;
    CALIB_GYRO_Y = 0.81;
    CALIB_GYRO_Z = -0.58;
  } else {
    CALIB_GYRO_X = 2.67;
    CALIB_GYRO_Y = -0.72;
    CALIB_GYRO_Z = 0.57;
  }
}

void MPU6050::begin() {
  Wire.begin();

  // Awake MPU6050
  writeRegister(0x6B, 0x00);

  /* ACCEL range */
#if defined(ACCEL_FS_2G)
  writeRegister(ACCEL_CONFIG_REG, ACCEL_FS_2G);
#elif defined(ACCEL_FS_4G)
  writeRegister(ACCEL_CONFIG_REG, ACCEL_FS_4G);
#elif defined(ACCEL_FS_8G)
  writeRegister(ACCEL_CONFIG_REG, ACCEL_FS_8G);
#elif defined(ACCEL_FS_16G)
  writeRegister(ACCEL_CONFIG_REG, ACCEL_FS_16G);
#endif


  /* GYRO range */
#if defined(GYRO_FS_250DPS)
  writeRegister(GYRO_CONFIG_REG, GYRO_FS_250DPS);
#elif defined(GYRO_FS_500DPS)
  writeRegister(GYRO_CONFIG_REG, GYRO_FS_500DPS);
#elif defined(GYRO_FS_1000DPS)
  writeRegister(GYRO_CONFIG_REG, GYRO_FS_1000DPS);
#elif defined(GYRO_FS_2000DPS)
  writeRegister(GYRO_CONFIG_REG, GYRO_FS_2000DPS);
#endif

  /* Digital low pass filter */
#if defined(DLPF_CFG_260HZ)
  writeRegister(DLPF_CONFIG_REG, DLPF_CFG_260HZ);
#elif defined(DLPF_CFG_184HZ)
  writeRegister(DLPF_CONFIG_REG, DLPF_CFG_184HZ);
#elif defined(DLPF_CFG_94HZ)
  writeRegister(DLPF_CONFIG_REG, DLPF_CFG_94HZ);
#elif defined(DLPF_CFG_44HZ)
  writeRegister(DLPF_CONFIG_REG, DLPF_CFG_44HZ);
#elif defined(DLPF_CFG_21HZ)
  writeRegister(DLPF_CONFIG_REG, DLPF_CFG_21HZ);
#elif defined(DLPF_CFG_10HZ)
  writeRegister(DLPF_CONFIG_REG, DLPF_CFG_10HZ);
#elif defined(DLPF_CFG_5HZ)
  writeRegister(DLPF_CONFIG_REG, DLPF_CFG_5HZ);
#endif
}

void MPU6050::readAccel() {
  int16_t rawX, rawY, rawZ;
  readRawData(0x3B, rawX, rawY, rawZ);

  // accelX = (float)rawX / ACCEL_SENSITIVITY * G_CONSTANT + CALIB_ACCEL_X;
  accelY = (float)rawY / ACCEL_SENSITIVITY * G_CONSTANT + CALIB_ACCEL_Y;
  accelZ = (float)rawZ / ACCEL_SENSITIVITY * G_CONSTANT + CALIB_ACCEL_Z;
}

void MPU6050::readGyro() {
  int16_t rawX, rawY, rawZ;
  readRawData(0x43, rawX, rawY, rawZ);

  gyroX = (float)rawX / GYRO_SENSITIVITY + CALIB_GYRO_X;
  // gyroY = (float)rawY / GYRO_SENSITIVITY + CALIB_GYRO_Y;
  // gyroZ = (float)rawZ / GYRO_SENSITIVITY + CALIB_GYRO_Z;
}

void MPU6050::readRawData(uint8_t startReg, int16_t &x, int16_t &y, int16_t &z) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(startReg);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, (uint8_t)6);

  unsigned long start = millis();
  while (Wire.available() < 6) {
    if (millis() - start > 100) {  
      x = y = z = 0;
      return;
    }
  }

  x = (Wire.read() << 8) | Wire.read();
  y = (Wire.read() << 8) | Wire.read();
  z = (Wire.read() << 8) | Wire.read();
}

bool MPU6050::writeRegister(uint8_t reg, uint8_t data) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(data);
  return (Wire.endTransmission() == 0);
}
