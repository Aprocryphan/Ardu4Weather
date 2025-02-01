import socket
import time

# --- Configuration ---
ARDUINO_IP = '192.168.1.106'  # IP address of the Arduino
PORT = 8081  # Port to connect to
FILE_PATH = "HistoricalWeatherData.csv"  # Path to the CSV file
END_MARKER = "<END>"  # End marker to signal the end of data
# ---------------------

def request_data():
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
            client_socket.settimeout(10)  # Set a timeout for the connection
            client_socket.connect((ARDUINO_IP, PORT))
            print(f"\033[0;92mConnected to Arduino at {ARDUINO_IP}:{PORT}")
            
            # Send a request to the Arduino
            client_socket.sendall(b"REQUEST_DATA\n")
            print("Request sent to Arduino.")
            
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
            
            if data.startswith("§"):
                truncated_data = data[1:]  # Remove the delimiter
                with open(FILE_PATH, "a", newline='') as file:
                    file.write(f"{truncated_data}\n")
                    file.flush()
                print(f"\033[0;92mData saved: {truncated_data}")
            
            # Close the connection explicitly
            client_socket.close()
            print("\033[0;93mConnection closed.\n")
    except ConnectionResetError as e:
        print(f"\033[0;31mConnection was reset: {e}")
    except Exception as e:
        print(f"\033[0;31mAn error occurred: {e}")

if __name__ == "__main__":
    while True:
        request_data()
        time.sleep(600) # Wait for 10 minutes before requesting data again