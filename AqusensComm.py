import serial
import time
import os
from datetime import datetime
import csv
import signal
import sys
import requests

SERIAL_PORT = "COM4"
BAUD_RATE = 115200
READ_FILE = "response_file.txt" #"C:/Aqusens/Aqusens_CalPoly_CPE/response_file.txt"  # File for Aqusens
WRITE_FILE = "command_file.txt" #"C:/Aqusens/Aqusens_CalPoly_CPE/command_file.txt"  # File for ASRS
TEMP_CSV = "SampleTemps.csv"   # File for storing sample temperature readings
DIRECTORY_PATH = "./" #"D:/Data/Raw/test/"  # Location of parent folder for Aqusens captures


def sigint_handler(signum, frame):
    if ser and ser.is_open:
        ser.close()
        print("Serial connection closed.")
    sys.exit(0)

signal.signal(signal.SIGINT, sigint_handler)

def setup():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        time.sleep(2)  # Give Arduino time to reset
        print(f"Connected to {SERIAL_PORT}")
        return ser
    
    except serial.SerialException as e:
        print(f"Serial error: {e}")
        return None

def queryForWaterLevel():
    url = "https://api.tidesandcurrents.noaa.gov/api/prod/datagetter?date=latest&station=9412110&product=water_level&datum=MLLW&time_zone=lst&units=metric&format=json"
    # Queries for the current water level from Port San Luis pier, updates every 6 minutes.

    # Tries to get NOAA data from url
    # If connectivity issues or data not found, fallback to SD card information (return -100)
    try:
        response = requests.get(url)
        if response.status_code == 200:
            data = response.json()

            water_level = data['data'][0]['v']
            return water_level
        else:
            return -1000
    except requests.exceptions.RequestException as e:
        return -1000

def readCommand(ser):
    # Read arduino response for gather Tide data or writing to Aqusens
    if ser.in_waiting:
        response = ser.readline().decode().strip()
        # print(f"Arduino: {response}")
        return response
    # If nothing in serial return None
    return None

def communicate(ser):
    try:
        with open(READ_FILE, "r") as input, open(WRITE_FILE, "w") as output:

            # Create Sample Folder
            now = datetime.now()
            curr_time = now.strftime("%y%m%d_%H%M%S")

            directory = DIRECTORY_PATH + curr_time # Folder to put data within
            # print("Directory Location" , directory)
            os.makedirs(directory, exist_ok=True)

            # Sampling Process ------------------------------------------
            while (1):
                output.seek(0)

                # Set directory for data
                save_dir = "SaveToDirectory(" + directory + ")"
                output.write(save_dir)
                output.flush()

                # Wait for ACK
                while (1):
                    input.seek(0)
                    
                    recv = input.read()
                    if (len(recv) >= 16):
                        break
                
                # Process ACK
                if recv[0] == "0" and recv[1:16].lower() == "savetodirectory":
                    break   # Got a successful ACK

                # If error remake directory and try to save there
                os.makedirs(directory, exist_ok=True)

            # Starting Pump ------------------------------------------------
            while (1):
                output.close()
                output = open(WRITE_FILE, "w")

                start_pump = "StartPump()"
                output.write(start_pump)
                output.flush()

                # Wait for ACK
                while (1):
                    input.seek(0)

                    recv = input.read()
                    if (len(recv) >= 10):
                        break
                
                # Process ACK
                if recv[0] == "0" and recv[1:10].lower() == "startpump":
                    break   # Got a successful ACK

            # Start 2min Timer for sample to Aqusens ------------------------
            print("Starting 2 minute timer")
            # time.sleep(60 * 2)

            # Start Sample Collection ---------------------------------------
            frame_number = "1"
            while (1):
                output.close()
                output = open(WRITE_FILE, "w")

                start_sample = "StartSampleCollection(" + frame_number + ")"
                output.write(start_sample)
                output.flush()

                # Wait for ACK
                while (1):
                    input.seek(0)

                    recv = input.read()
                    if (len(recv) >= 22):
                        break
                
                # Process ACK
                if recv[0] == "0" and recv[1:22].lower() == "startsamplecollection":
                    break   # Got a successful ACK
            
            # Start 5min Timer for processing sample -------------------------
            print("Starting 5 minute timer")

            # create CSV file
            csv_file = open(TEMP_CSV, "a", newline="\n")
            writer = csv.writer(csv_file)
            # Create CSV header if empty
            if os.stat(TEMP_CSV).st_size == 0:
                writer.writerow(["Timestamp", "Min Temp(C)", "Max Temp(C)", "Avg Temp(C)"])

            temperatures = []

            # Each 15 seconds request temperature -- 20 total requests
            for i in range (20):
                time.sleep(15)
                ser.write("T\n".encode())
                # Get Current date and time
                now = datetime.now()
                curr_time = now.strftime("%y-%m-%d_%H:%M:%S")

                while (1):
                    if ser.in_waiting:
                        temp_data = ser.readline().decode().strip()  # Read temperature
                        if temp_data.isdigit():  # Check it's an int
                            print(f"{curr_time} - Temperature: {temp_data}°C")
                            temperatures.append((int)(temp_data))
                            break
                    
                    time.sleep(0.5)  # try every 0.5 seconds

            min_temp = temperatures[0]
            max_temp = temperatures[0]
            total_temp = 0
            for i in range(20):
                if temperatures[i] < min_temp:
                    min_temp = temperatures[i]
                if temperatures[i] > max_temp:
                    max_temp = temperatures[i]
                total_temp += temperatures[i]
            average_temp = total_temp / 20
            print(f"Min temp: {min_temp}, Max temp: {max_temp}, Average temp: {average_temp}")
            writer.writerow([curr_time, min_temp, max_temp, average_temp])  # Write data to CSV
            csv_file.close()

            # Stop Sample Collection -----------------------------------------
            while (1):
                output.close()
                output = open(WRITE_FILE, "w")

                stop_sample = "StopSampleCollection()"
                output.write(stop_sample)
                output.flush()

                # Wait for ACK
                while (1):
                    input.seek(0)

                    recv = input.read()
                    if (len(recv) >= 21):
                        break
                
                # Process ACK
                if recv[0] == "0" and recv[1:21].lower() == "stopsamplecollection":
                    break   # Got a successful ACK
        
        # send done signal to Arduino
        ser.write("D\n".encode())
        input.close()
        output.close()

    except serial.SerialException as e:
        print(f"Serial error: {e}")
        if ser and ser.is_open:
            ser.close()
    except FileNotFoundError:
        print(f"File not found: {READ_FILE}")
        if ser and ser.is_open:
            ser.close()

def flush(ser):
    try:
        with open(READ_FILE, "r") as input, open(WRITE_FILE, "w") as output:
            # Stop Sample Collection -----------------------------------------
            while (1):
                output.close()
                output = open(WRITE_FILE, "w")

                stop_sample = "StopPump()"
                output.write(stop_sample)
                output.flush()

                # Wait for ACK
                while (1):
                    input.seek(0)

                    recv = input.read()
                    if (len(recv) >= 9):
                        break
                
                # Process ACK
                if recv[0] == "0" and recv[1:9].lower() == "stoppump":
                    break   # Got a successful ACK
        
        # send done signal to Arduino
        ser.write("D\n".encode())
        input.close()
        output.close()
        
    except serial.SerialException as e:
        print(f"Serial error: {e}")
        if ser and ser.is_open:
            ser.close()
    except FileNotFoundError:
        print(f"File not found: {READ_FILE}")
        if ser and ser.is_open:
            ser.close()

if __name__ == "__main__":
    ser = setup()
    while ser is None:
        print(f"Unable to set up serial connection. Retrying...")
        ser = setup()

    while(1):
        if ser is None or not ser.is_open:
            print(f"Unable to set up serial connection. Retrying...")
            ser = setup()
            continue
        write_to = readCommand(ser)
        if write_to is None:
            continue
        if write_to == "T":
            tide_level = queryForWaterLevel()
            print(f"Tide level is {tide_level}")
            ser.write((str(tide_level) + "\n").encode())    # Send tide data over
        elif write_to == "S":
            communicate(ser)
        elif write_to == "F":
            flush(ser)