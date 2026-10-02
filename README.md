# Boost Converter

## 📌 Project Overview

This project focuses on the modeling, simulation, and control of a **DC-DC Boost Converter**. The converter is designed to step up a low DC input voltage to a higher regulated DC output voltage using high-frequency switching.

The project includes the design of the power stage, analysis of continuous conduction mode (CCM) operation, and implementation of PWM-based voltage control.

## 🎯 Objectives

- Design and model a DC-DC Boost Converter.
- Analyze boost converter operation in CCM.
- Calculate the required inductor and capacitor values.
- Generate PWM switching signals for the MOSFET.
- Regulate the output voltage using feedback control.
- Analyze inductor current, switch voltage, diode current, and output voltage.
- Verify converter performance through MATLAB/Simulink simulation.

## ⚡ Operating Principle

The boost converter operates through two switching states:

### Switch ON

The MOSFET is turned ON and the inductor stores energy from the input source. The diode is reverse biased and the load is supplied mainly by the output capacitor.

### Switch OFF

The MOSFET is turned OFF and the inductor releases its stored energy through the diode to the output capacitor and load. The inductor voltage adds to the input voltage, producing an output voltage higher than the input voltage.

For an ideal boost converter operating in CCM:

\[
V_o = \frac{V_{in}}{1-D}
\]

where:

- \(V_{in}\) = Input voltage
- \(V_o\) = Output voltage
- \(D\) = Duty ratio

## 💻 Software

- MATLAB
- Simulink
- MATLAB Control System Toolbox

## 📊 Parameters

Example design parameters:

| Parameter | Value |
|---|---:|
| Input Voltage | 48 V |
| Duty Ratio | 0.52 |
| Switching Frequency | 10 kHz |
| Inductor | 6 mH |
| Output Capacitor | 500 µF |
| Load Resistance | 16 Ω |

## 📈 Simulation Results

The following waveforms are analyzed:

- Input voltage
- Output voltage
- Inductor current
- MOSFET switching voltage/current
- Diode current
- PWM gate signal
- Output voltage regulation

## 🚀 Applications

Boost converters are widely used in:

- Renewable energy systems
- PV power conversion
- Battery-powered systems
- Electric vehicles
- DC-link voltage boosting
- Power-factor correction stages
- DC microgrids

## 👨‍💻 Author

**Dhanunjay Killari**

M.Tech – Electrical Engineering  
National Institute of Technology, Warangal
