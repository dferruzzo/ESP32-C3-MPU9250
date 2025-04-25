// MPU-9250 Register Map and Descriptions
// Reference: https://invensense.tdk.com/wp-content/uploads/2015/02/PS-MPU-9250A-01-v1.1.pdf

#ifndef MPU9250_CONFIGURATION_MAP_H
#define MPU9250_CONFIGURATION_MAP_H

#define MPU9250_GYRO_FS_SEL_250     0x00 // 250 degrees/sec 
#define MPU9250_GYRO_FS_SEL_500     0x01 // 500 degrees/sec
#define MPU9250_GYRO_FS_SEL_1000    0x02 // 1000 degrees/sec
#define MPU9250_GYRO_FS_SEL_2000    0x03 // 2000 degrees/sec

#define MPU9250_GYRO_FS_SEL_250_VALUE     ((uint16_t)250) // 250 degrees/sec 
#define MPU9250_GYRO_FS_SEL_500_VALUE     ((uint16_t)500) // 500 degrees/sec
#define MPU9250_GYRO_FS_SEL_1000_VALUE    ((uint16_t)1000) // 1000 degrees/sec
#define MPU9250_GYRO_FS_SEL_2000_VALUE    ((uint16_t)2000) // 2000 degrees/sec


#endif // MPU9250_CONFIGURATION_MAP_H
//
