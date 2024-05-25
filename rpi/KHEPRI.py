import serial
import time

# Set up the serial connection (adjust the port as necessary)
ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)
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

print("Arduino has started the loop")

# Now you can continue to communicate with the Arduino
try:
    while True:
        # Example: Send a message to the Arduino every 2 seconds
        ser.write(b'Hello, Arduino!\n')
        print("Sent: Hello, Arduino!")
        
        # Read and print any incoming messages from the Arduino
        time.sleep(2)
        while ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').rstrip()
            print(f"Received: {line}")

except KeyboardInterrupt:
    print("Program interrupted by user")

finally:
    ser.close()
    print("Serial connection closed")
