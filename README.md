# ESP32-Nagios-NRDP
Arduino-ESP32 example sketch to send device and sensor data to NagiosXI via NRDP.

# Will send 6 total checks
1. HostCheck = (UP)
2. Sens1 = Example of State (OK)
3. Sens2 = Example of State (Warning)
4. Sens3 = Example of State (Critical)
5. Sens4 = Example of State (Unknown)
6. Sens5 = Example of State determined by evaluatated thresholds

# Uses
WiFi.h
Arduino_JSON.h
HTTPClient.h
