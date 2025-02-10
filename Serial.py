import serial
import time 
import datetime

# --- Configuration ---
PORT = "COM5"  # Replace with your Arduino's serial port
BAUD_RATE = 57600  # Match your Arduino's baud rate
# ---------------------

def get_timestamp():
    now = datetime.datetime.now()
    return now.strftime("%Y-%m-%d %H:%M:%S")

try:
    ser = serial.Serial(PORT, BAUD_RATE, timeout=1)
    print(f"Connected to Arduino on {PORT}")
    while True:
        line = ser.readline().decode("utf-8").rstrip()  # Read a line, decode, and remove whitespace
        if line:  # Check if the line is not empty
            timestamp = get_timestamp()
            print(f"{timestamp}: {line}") # Print to console with timestamp

except serial.SerialException as e:
    print(f"Error: Could not connect to Arduino on {PORT}: {e}")
except Exception as e:
    print(f"An error occurred: {e}")
finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()
        print("Serial connection closed.")