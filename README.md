# SIMS-IOT-Devices-FreeRTOS-ESP-IDF-5.0-Modbus
Modbus protocol for ESP32 in ESP IDF environment

The following example uses comertial simulators downoaded from https://www.modbus.org/tech.php
Modpoll program simulates master
Diagslave program simulates slave
Exampe 1: Write-Read one field 
Slave 	- Start: 				diagslave -m tcp
Master 	- Write one fied:		modpoll 127.0.0.1 999
Master 	- Read one fied:		modpoll -m tcp 127.0.0.1
