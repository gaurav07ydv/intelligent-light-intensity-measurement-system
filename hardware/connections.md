# Hardware Connections

## Sensor Connections
- LDR -> ESP32 analog pin
- DHT22 -> ESP32 digital pin
- TSL2561 -> I2C communication
- BH1750 -> I2C communication
- OLED -> I2C communication

## LED Control
- ESP32 output pin -> IRLZ44N gate
- IRLZ44N drain -> LED strip negative
- LED strip positive -> 12V supply
- Common ground shared between ESP32 and 12V supply
