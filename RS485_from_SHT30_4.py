from pymodbus.client import ModbusSerialClient

# Configure Modbus RTU Client
client = ModbusSerialClient(
    method='rtu', 
    port='COM6',   # For Linux Change to '/dev/ttyUSB0'
    baudrate=9600,
    parity='N', 
    stopbits=1, 
    bytesize=8, 
    timeout=1
)

# Connect to the Modbus device
if not client.connect():
    print("Failed to connect to Modbus device")
    exit()

# Reads temperature and humidity from the SHT30 sensor via Modbus
try:
    # Read Temperature Register (0x0001)
    temp_response = client.read_holding_registers(0x0001, 1, unit=1)
    if temp_response.isError():
        print("Error reading temperature")
    else:
        raw_temp = temp_response.registers[0]
        temperature = raw_temp / 10.0  # Adjust scaling factor based on sensor
        print(f"Temperature: {temperature:.1f}°C")

    # Read Humidity Register (0x0002)
    humid_response = client.read_holding_registers(0x0002, 1, unit=1)
    if humid_response.isError():
        print("Error reading humidity")
    else:
        raw_humid = humid_response.registers[0]
        humidity = raw_humid / 1000.0  # Adjust scaling factor based on sensor
        print(f"Humidity: {humidity:.1f}%")

except Exception as e:
    print(f"Error: {e}")

# Close connection
client.close()
