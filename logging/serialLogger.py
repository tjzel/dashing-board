import serial
import time

# Serial port settings
serial_port = '/dev/tty.usbserial-0001'
# baud_rate = 9600
# serial_port = '/dev/tty.usbmodem1101'
baud_rate = 115200

chunk_size = 1  # How many bytes to read at once

try:
    with serial.Serial(serial_port, baud_rate, timeout=1) as ser:
        # Open the file with 'write' ('w') mode
        with open("output_data.txt", "w") as file:
            print("Started logging data. Press Ctrl+C to stop.")
            while True:
                # Read a chunk of data from the serial port
                data = ser.read(chunk_size)
                if data:
                    # Decode binary data to UTF-8 text
                    decoded_data = data.decode('utf-8')
                    # Output to console
                    # print(decoded_data, end='', flush=True)
                    # Write to the file
                    file.write(decoded_data)
                    # Ensure it is written to disk
                    file.flush()
except serial.SerialTimeoutException:
    print("Data read timed out.")
except serial.SerialException as e:
    print(f"Serial error: {e}")
except Exception as e:
    print(f"An unforeseen error occurred: {e}")
finally:
    print("\nProcess completed.")
