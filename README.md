# ESP32 Ball and Beam Control System

An autonomous ball-and-beam stabilization system designed, manufactured, and programmed completely from scratch. This project demonstrates the practical application of control theory, embedded systems programming, and rapid prototyping.

## Key Features
- **Control Feedback Loop:** Implemented a tuned **PID controller** for real-time ball position stabilization.
- **Signal Processing:** Integrated an **EMA (Exponential Moving Average) filter** to denoise the distance sensor data without introducing critical phase lag.
- **Hardware Platform:** Powered by a **ESP32** microcontroller driving a high-torque servo motor.
- **Mechanical Design:** Custom structural components designed from scratch in **Autodesk Inventor** and manufactured via 3D printing.

## 🛠️ Engineering Challenges & Solutions

### 1. Optical Sensing Geometry
The Time-of-Flight (ToF) laser sensor features a conical field of view. During initial testing, this led to signal loss and false readings. 
* **Solution:** Iteratively optimized the sensor's mounting angle, extended the physical length of the beam, and adjusted the ball diameter to maximize the reflected light returning to the receiver.

### 2. Sensor Noise Filtering
Raw data from the ToF sensor suffered from high-frequency noise, which caused the servo to jitter and destabilized the PID loop.
* **Solution:** Developed and deployed a digital EMA filter. This effectively smoothed out the noise while maintaining low latency, ensuring smooth and responsive control behavior.

## 📂 Repository Structure
- `/src` - Source code for ESP32 (`.ino` file)
- `/3DModels` - CAD models and `.stl` files for 3D printing

---
*Created as a personal project to bridge university theoretical knowledge with practical, hands-on engineering.*
<img width="3200" height="1232" alt="Bez tytułu" src="https://github.com/user-attachments/assets/ac141f98-83b3-46d3-a76b-e1d1895595d9" />
