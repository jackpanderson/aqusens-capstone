import serial
import time
import os
from datetime import datetime
from testingNOAAwaterLevelAPI import queryForWaterLevel
import csv

SERIAL_PORT = "COM3"  # Change to match your Arduino port
BAUD_RATE = 115200
READ_FILE = "AQUSENS.txt"  # File for Aqusens
WRITE_FILE = "ASRS.txt"  # File for ASRS
TEMP_CSV = "SampleTemps.csv"   # File for storing sample temperature readings
DIRECTORY_PATH = "./"  # Location of parent folder


def setup():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        time.sleep(2)  # Give Arduino time to reset
        print(f"Connected to {SERIAL_PORT}")
        return ser
    
    except serial.SerialException as e:
        print(f"Serial error: {e}")
        return None

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
                    if (len(recv) == 16):
                        break
                
                # Process ACK
                if recv[0] == "1" and recv[1:].lower() == "savetodirectory":
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
                    if (len(recv) == 10):
                        break
                
                # Process ACK
                if recv[0] == "1" and recv[1:].lower() == "startpump":
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
                    if (len(recv) == 22):
                        break
                
                # Process ACK
                if recv[0] == "1" and recv[1:].lower() == "startsamplecollection":
                    break   # Got a successful ACK
            
            # Start 5min Timer for processing sample -------------------------
            print("Starting 5 minute timer")
            # time.sleep(60 * 5)

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
                    if (len(recv) == 21):
                        break
                
                # Process ACK
                if recv[0] == "1" and recv[1:].lower() == "stopsamplecollection":
                    break   # Got a successful ACK
        
        # send done signal to Arduino
        ser.write("D\n".encode())
        input.close()
        output.close()

    except serial.SerialException as e:
        print(f"Serial error: {e}")
    except FileNotFoundError:
        print(f"File not found: {READ_FILE}")

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
                    if (len(recv) == 9):
                        break
                
                # Process ACK
                if recv[0] == "1" and recv[1:].lower() == "stoppump":
                    break   # Got a successful ACK
        
        # send done signal to Arduino
        ser.write("D\n".encode())
        input.close()
        output.close()
        
    except serial.SerialException as e:
        print(f"Serial error: {e}")
    except FileNotFoundError:
        print(f"File not found: {READ_FILE}")

def temperature(ser):
    try:
        with open(TEMP_CSV, "w", newline="") as file:
            # Get Current date and time
            now = datetime.now()
            curr_time = now.strftime("%y-%m-%d_%H:%M:%S")

            writer = csv.writer(file)
            writer.writerow(["Timestamp", "Temperature (C)"])   # Create CSV headers

            ser.write("A\n".encode()) # Send Ready to Receive ACK to Arduino

            # Wait for temperature data from Arduino
            while (1):
                if ser.in_waiting:
                    temp_data = ser.readline().decode().strip()  # Read temperature
                    if temp_data.isdigit():  # Check it's an int
                        writer.writerow([curr_time, int(temp_data)])  # Write data to CSV
                        print(f"{curr_time} - Temperature: {temp_data}°C")
                
                time.sleep(1)  # Read every second
        
        # send done signal to Arduino
        ser.write("D\n".encode())

    except serial.SerialException as e:
        print(f"Serial error: {e}")
    except FileNotFoundError:
        print(f"File not found: {TEMP_CSV}")


if __name__ == "__main__":
    ser = setup()
    while ser is None:
        print(f"Unable to set up serial connection. Retrying...")
        ser = setup()

    while(1):
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
        elif write_to == "C":
            temperature(ser)