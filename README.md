<h2> Video #1 </h2><br>
Modbus_TCP.c <br>
Modbus protocol implementation for ESP32 in ESP IDF 5.0 environment  <br>
 <br>
The following example uses comertial simulators downoaded from https://www.modbus.org/tech.php  <br>
Modpoll program   - simulates master  <br>
Diagslave program - simulates slave  <br>
Exampe 1: Write-Read one field   <br>
Slave 	- Start: 				diagslave -m tcp  <br>
Master 	- Write one fied:		modpoll 127.0.0.1 999  <br>
Master 	- Read one fied:		modpoll -m tcp 127.0.0.1  <br><br>
<h2> Video #2 </h2><br>
RS485_from_SHT30.py <br>
Python file on PC which reads data from the Humidity and Temperature Sensor via RS485 in Modbus protocol
