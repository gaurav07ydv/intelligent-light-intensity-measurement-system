# Intelligent Light Intensity Measurement System with Auto-Calibration Using Machine Learning

## Team Name
LuxSense

## Team Members
- Gaurav Yadav
- Radheshyam E.P

## Guide
Dr. A.N. Gnana Jeevan

## Project Overview
This project presents an ESP32-based intelligent light intensity measurement system that improves the accuracy of a low-cost LDR sensor using machine learning-based calibration. The system uses TSL2561 as the reference sensor, BH1750 as a secondary sensor for validation, and DHT22 for temperature monitoring. The calibrated lux value is displayed on an OLED display and used for automatic lighting control in a smart home prototype.

## Objectives
- Measure indoor light intensity using low-cost sensors
- Improve LDR accuracy through machine learning-based auto-calibration
- Compare readings using TSL2561 and BH1750
- Display real-time readings on OLED
- Control LED lighting automatically based on calibrated lux
- Demonstrate a smart home lighting prototype

## Hardware Components
- ESP32
- LDR sensor
- TSL2561
- BH1750
- DHT22
- OLED SSD1306
- IRLZ44N MOSFET
- 12V LED strip

## Software Used
- Arduino IDE
- Python
- pandas
- numpy
- matplotlib
- scikit-learn
- xgboost

## Repository Structure
- `hardware/` : hardware list and circuit connections
- `firmware/` : ESP32 Arduino code
- `ml_model/` : training code and model results
- `data/` : sample dataset
- `docs/` : project summary and supporting documents

## System Workflow
1. LDR senses raw light intensity
2. DHT22 measures temperature
3. TSL2561 provides reference lux values
4. BH1750 provides secondary comparison
5. Machine learning model predicts calibrated lux
6. OLED displays readings and calibrated lux
7. LED turns ON/OFF based on calibrated lux threshold

## Applications
- Smart home lighting
- Indoor automation
- Low-cost light monitoring
- Future street lighting systems

## Future Scope
- IoT dashboard integration
- Cloud data logging
- Mobile app monitoring
- Better embedded ML deployment
