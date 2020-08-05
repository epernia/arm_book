/*
Modules:
--------

System:

 - Smart home system: Coordinate hole sub-systems process

Sub-systems:

 - Fire alarm: Control alarm siren activation using gas and temperature sensors, and deactivation using code sub-system
 - User interface: Update user interface (incorrectCodeLed, systemBlockedLed and matrix keypad)
 - Code: alarm deactivation code contol
 - Event log: Log the events of the system
 - PC Serial communication: Update communication with a PC via serial interface (UART)
 - Smartphone BLE communication: Update communication with a Smartphone via BLE interface

Drivers:

 - Siren: Alarm siren LED driver
 - Gas sensor: MQ-2 gas sensor driver
 - Temperature sensor: LM35 temperature sensor driver
 - Matrix keypad: 4x4 Matrix keypad driver

Utilities:

 - Date and time: use internal RTC