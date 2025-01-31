import serial
import time 
import datetime

# --- Configuration ---
PORT = "COM5"  # Replace with your Arduino's serial port
BAUD_RATE = 57600  # Match your Arduino's baud rate
FILE_PATH = "HistoricalWeatherData.csv"  # Path to the text file
# ---------------------

def get_timestamp():
    now = datetime.datetime.now()
    return now.strftime("%Y-%m-%d %H:%M:%S")

try:
    ser = serial.Serial(PORT, BAUD_RATE, timeout=1)
    print(f"Connected to Arduino on {PORT}")
    with open(FILE_PATH, "a", newline='') as file:  # Open in append mode ("a")
        print(f"Writing data to {FILE_PATH}")
        while True:
            line = ser.readline().decode("utf-8").rstrip()  # Read a line, decode, and remove whitespace
            if line:  # Check if the line is not empty
                if "§" in line:  # Check if the line contains the delimiter
                    truncatedline = line[1:]  # Remove the delimiter
                    file.write(f"{truncatedline}\n") #Write with newline character
                    file.flush()  # Immediately write to the file (optional but recommended)
                else:  # If the line does not contain the delimiter
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