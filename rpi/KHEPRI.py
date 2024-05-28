import serial
import time
import pandas as pd
import numpy as np

data_list = []

# Set up the serial connection (adjust the port as necessary)
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
time.sleep(2)  # Wait for the serial connection to initialize

# Send the "START" command to the Arduino
start_command = "RPI_READY\n"
ser.write(start_command.encode('utf-8'))
print("Sent: START")

# Read the Arduino response until we confirm it started
while True:
    if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8').rstrip()
        print(f"Received: {line}")
        if "ARDUINO_READY" in line:
            break

print("Arduino is ready, waiting for data format")
while True:
    if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8').rstrip()
        print(f"Received: {line}")
        
        columns = line.split(',')
        break
        
            


# Now you can continue to communicate with the Arduino
try:
    while True:
        time.sleep(2)
        while ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').rstrip()
            print(f"Received: {line}")
            data_list.append(line.split(','))

except KeyboardInterrupt:
    print("Program interrupted by user")

finally:
    ser.close()
    print("Serial connection closed")
    df = pd.DataFrame(data_list, columns=columns)
    df.to_csv('data.csv', index=False)
    print("Data saved to data.csv")
