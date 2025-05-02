import serial
import matplotlib.pyplot as plt

ser = serial.Serial('/dev/ttyACM0', 115200)
print("Reading data...")

plt.ion()

gyrx_data, gyry_data, gyrz_data = [], [], []
accx_data, accy_data, accz_data = [], [], []
magx_data, magy_data, magz_data = [], [], []
temp_data = []

while True:
    line = ser.readline().decode('utf-8').strip()
    print(line)
    try:
        data = line.split(',')
        if len(data) == 10:
            gyrx = float(data[0])
            gyry = float(data[1])
            gyrz = float(data[2])
            accx = float(data[3])
            accy = float(data[4])
            accz = float(data[5])
            magx = float(data[6])
            magy = float(data[7])
            magz = float(data[8])
            temp = float(data[9])
            
            gyrx_data.append(gyrx)
            gyry_data.append(gyry)
            gyrz_data.append(gyrz)
            accx_data.append(accx)
            accy_data.append(accy)
            accz_data.append(accz)
            magx_data.append(magx)
            magy_data.append(magy)
            magz_data.append(magz)
            temp_data.append(temp)
            
            plt.clf()
            plt.subplot(4, 1, 1)
            plt.plot(gyrx_data, label="GyrX")
            plt.plot(gyry_data, label="GyrY")
            plt.plot(gyrz_data, label="GyrZ")
            plt.legend()
            plt.grid()
            
            plt.subplot(4, 1, 2)
            plt.plot(accx_data, label="AccX")
            plt.plot(accy_data, label="AccY")
            plt.plot(accz_data, label="AccZ")
            plt.legend()
            plt.grid()
            
            plt.subplot(4, 1, 3)
            plt.plot(magx_data, label="MagX")
            plt.plot(magy_data, label="MagY")
            plt.plot(magz_data, label="MagZ")
            plt.legend()
            plt.grid()
            
            plt.subplot(4, 1, 4)
            plt.plot(temp_data, label="Temp")
            plt.legend()
            plt.grid()
            
            plt.pause(0.01)
    except ValueError:
        print("Invalid data received, skipping...")

ser.close()
# Note: Make sure to adjust the serial port and baud rate as per your setup.