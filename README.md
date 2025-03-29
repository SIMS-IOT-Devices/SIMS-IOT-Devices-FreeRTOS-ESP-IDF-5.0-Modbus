# SIMS-IOT-Devices-FreeRTOS-ESP-IDF-5.0-Modbus <br>
<h2> Video #1 </h2><br>
Modbus protocol for ESP32 in ESP IDF environment  <br>
 <br>
The following example uses comertial simulators downoaded from https://www.modbus.org/tech.php  <br> <br>
Modpoll program   - simulates master  <br>
Diagslave program - simulates slave  <br> <br>
Exampe 1: Write-Read one field   <br>
Slave 	- Start: 				diagslave -m tcp  <br>
Master 	- Write one fied:		modpoll 127.0.0.1 999  <br>
Master 	- Read one fied:		modpoll -m tcp 127.0.0.1  <br><br>
Video #2 <br>
RS485_from_SHT30.py - Python file which reads data from the Humidity and Temperature Sensor via RS485 in Modbus protocol
