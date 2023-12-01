# ProtecKid
This repository contains code to run the solution for the IoT project built in 02808 IoT Prototyping -- Fall 23 -- Group 8. The prototype relies on https://github.com/jpmeijers/RN2483-Arduino-Library to connect to LoRa. To successfully run the code under /rfidsend, please install the library in your Arduino IDE.

# rfidsend
The solution will attempt to connect to Helium. Once connected, the solution will send scanned RFID unique ids as raw data to Helium. The Helium connection available transmits the raw data to Azure IoT Hub. Azure maps the data in order to be able to create the map in PowerBI.

# windowcontrol
The solution relies on the readings of two temperature sensors -- one inside and one outside. If the solution is in automatic mode, the solution will automatically open or close the window (run servo motor) once the desired temperature is read.
