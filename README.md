# ESP32-C3-MPU9250

This project integrates the ESP32-C3 microcontroller with the MPU9250 sensor to provide motion tracking and environmental sensing capabilities.

## Features

- **ESP32-C3**: Low-power microcontroller with Wi-Fi and Bluetooth capabilities.
- **MPU9250**: 9-axis motion tracking sensor (3-axis gyroscope, 3-axis accelerometer, and 3-axis magnetometer).
- Real-time data acquisition and processing.
- Example code for sensor initialization and data reading.

## Requirements

- ESP32-C3 development board.
- MPU9250 sensor module.
- USB cable for programming.
- ESP-IDF for development.

## Installation

1. Clone this repository:
    ```bash
    git clone https://github.com/dferruzzo/ESP32-C3-MPU9250.git
    cd ESP32-C3-MPU9250
    ```

2. Install dependencies:
    - For ESP-IDF: Ensure the ESP-IDF environment is set up.


## Usage

1. Open the project in your preferred development environment.
2. Flash the firmware to the ESP32-C3.
3. Monitor the serial output to view sensor data.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request.

## License

This project is licensed under the [MIT License](LICENSE).

## Acknowledgments

- Inspired by the ESP32 and MPU9250 communities.
- Special thanks to open-source contributors.

## TODO

- [x] Ler, configurar a escala do giroscópio.
- [x] Configurar a escala dop acelerômetro.
- [x] Ler e configurar a escala da temperatura.
- [x] Implementar um I2C scanner para detectar os dispositivos conectados.
- [x] Ler o magnetômetro do MPU9250.
- [x] Configurar o magnetômetro do MPU9250.
- [x] Implementar os filtros (DLPF) do Giroscópio do MPU9250.
- [x] Python script to visualize data in real-time.
- [ ] Implementar os filtros (DLPF) do Acelerômetro do MPU9250.
- [ ] Calibrar o giroscópio.
- [ ] Calibrar o acelerômetro.
- [ ] Calibrar o magnetómetro.
- [ ] Conectar o magnetômetro HMC5883L via I2C.
- [ ] Implementar o filtro de Kalman.
- [ ] Determinar a atitude em ângulos de Euler.
- [ ] Determinar a atitude em quaternions.





