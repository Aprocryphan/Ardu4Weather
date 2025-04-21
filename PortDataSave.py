import socket
import time
from datetime import datetime
import ctypes, sys

# --- Configuration ---
ARDUINO_IP = '192.168.1.106'  # IP address of the Arduino
PORT = 8081  # Port to connect to
FILE_PATH = "HistoricalWeatherData.csv"  # Path to the CSV file
END_MARKER = "<END>"  # End marker to signal the end of data
# ---------------------

network = 0

def config_network():
    global network
    network = (network + 1) % 4
    if network == 0:
        global ARDUINO_IP
        ARDUINO_IP = '192.168.1.106'
    elif network == 1:
        ARDUINO_IP = '10.25.126.63'
    elif network == 2:
        ARDUINO_IP = input("Enter the IP address of the Arduino: ")

def request_data():
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
            client_socket.settimeout(10)  # Set a timeout for the connection
            client_socket.connect((ARDUINO_IP, PORT))
            print(f"\033[0;92mConnected to Arduino at {ARDUINO_IP}:{PORT}")
            # Send a request to the Arduino
            client_socket.sendall(b"REQUEST_DATA\n")
            print(f"Request sent to Arduino. {datetime.now().strftime('%H:%M:%S')}")
            # Receive the data from the Arduino
            data = ""
            while True:
                try:
                    part = client_socket.recv(1024).decode('utf-8')
                    if not part:
                        break
                    data += part
                    print(f"\033[0;94mReceived part: {part}")  # Debug statement
                    if END_MARKER in data:
                        data = data.replace(END_MARKER, "")
                        break
                except socket.timeout:
                    print("\033[0;31mSocket timeout, no more data.")
                    break
            data = data.strip()
            print(f"\033[0;94mReceived data: {data}")  # Debug statement
            if data.startswith("<START>"):
                truncated_data = data[7:]  # Remove the delimiter
                with open(FILE_PATH, "a", newline='') as file:
                    file.write(f"{truncated_data}\n")
                    file.flush()
                print(f"\033[0;92mData saved: {truncated_data}")
            # Close the connection explicitly
            client_socket.close()
            print("\033[0;93mConnection closed.\n")
    except ConnectionResetError as e:
        print(f"\033[0;31mConnection was reset: {e}, Check arduino code.")
    except Exception as e:
        print(f"\033[0;31mAn error occurred: {e}")
        config_network()

#https://stackoverflow.com/questions/130763/request-uac-elevation-from-within-a-python-script
def is_admin():
    try:
        return ctypes.windll.shell32.IsUserAnAdmin()
    except:
        print("\033[0;31mError checking admin permissions.")
        return False

if is_admin():
    if __name__ == "__main__":
        while True:
            current_time = datetime.now()
            if current_time.minute % 1 == 0: # Save data every 10 minutes
                request_data()
                time.sleep(61)  # Wait for 1 minute to avoid multiple requests
else:
    # Re-run the program with admin rights
    ctypes.windll.shell32.ShellExecuteW(None, "runas", sys.executable, " ".join(sys.argv), None, 1)