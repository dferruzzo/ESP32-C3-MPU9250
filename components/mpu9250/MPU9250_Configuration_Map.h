// MPU-9250 Register Map and Descriptions
// Reference: https://invensense.tdk.com/wp-content/uploads/2015/02/PS-MPU-9250A-01-v1.1.pdf

#ifndef MPU9250_CONFIGURATION_MAP_H
#define MPU9250_CONFIGURATION_MAP_H

#define MPU9250_GYRO_FS_SEL_250     0x00 // 250 degrees/sec 
#define MPU9250_GYRO_FS_SEL_500     0x01 // 500 degrees/sec
#define MPU9250_GYRO_FS_SEL_1000    0x02 // 1000 degrees/sec
#define MPU9250_GYRO_FS_SEL_2000    0x03 // 2000 degrees/sec

// FCHOICE_B 0 0 : DLPF enabled
// FCHOICE_B X 1 : DLPF bypassed
// FCHOICE_B 1 X : DLPF bypassed
#define MPU9250_FCHOICE_B_0     0x00 // FCHOICE_B bit 0
#define MPU9250_FCHOICE_B_1     0x00 // FCHOICE_B bit 1

#define MPU9250_GYRO_DLPF_CFG_250HZ  0x00 // DLPF configuration for 250Hz
#define MPU9250_GYRO_DLPF_CFG_184HZ  0x01 // DLPF configuration for 184Hz
#define MPU9250_GYRO_DLPF_CFG_92HZ   0x02 // DLPF configuration for 92Hz
#define MPU9250_GYRO_DLPF_CFG_41HZ   0x03 // DLPF configuration for 41Hz
#define MPU9250_GYRO_DLPF_CFG_20HZ   0x04 // DLPF configuration for 20Hz
#define MPU9250_GYRO_DLPF_CFG_10HZ   0x05 // DLPF configuration for 10Hz
#define MPU9250_GYRO_DLPF_CFG_5HZ    0x06 // DLPF configuration for 5Hz
#define MPU9250_GYRO_DLPF_CFG_3600HZ 0x07 // DLPF configuration for 3600Hz

#define MPU9250_GYRO_FS_SEL_250_VALUE     ((uint16_t)250) // 250 degrees/sec 
#define MPU9250_GYRO_FS_SEL_500_VALUE     ((uint16_t)500) // 500 degrees/sec
#define MPU9250_GYRO_FS_SEL_1000_VALUE    ((uint16_t)1000) // 1000 degrees/sec
#define MPU9250_GYRO_FS_SEL_2000_VALUE    ((uint16_t)2000) // 2000 degrees/sec

#define MPU9250_ACCEL_FS_SEL_2     0x00 // +/- 2g
#define MPU9250_ACCEL_FS_SEL_4     0x01 // +/- 4g   
#define MPU9250_ACCEL_FS_SEL_8     0x02 // +/- 8g
#define MPU9250_ACCEL_FS_SEL_16    0x03 // +/- 16g

#define MPU9250_ACCEL_FS_SEL_2_VALUE     ((uint16_t)2) // +/- 2g
#define MPU9250_ACCEL_FS_SEL_4_VALUE     ((uint16_t)4) // +/- 4g
#define MPU9250_ACCEL_FS_SEL_8_VALUE     ((uint16_t)8) // +/- 8g
#define MPU9250_ACCEL_FS_SEL_16_VALUE    ((uint16_t)16) // +/- 16g
#define MPU9250_ACCEL_CONFIG_FS_SEL_MASK 0x18 // Mask for FS_SEL bits in the accelerometer configuration register
#define MPU9250_ACCEL_CONFIG_FS_SEL_POS  3 // Position of FS_SEL bits in the accelerometer configuration register

#endif // MPU9250_CONFIGURATION_MAP_H
//
