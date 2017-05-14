/*
  power-monitor.ino - Device for monitoring the power consumption using Particle Photon
  Created by Anastas Manolevski
  GNU General Public License v3.0
*/

#include <EmonLib.h> //https://github.com/openenergymonitor/EmonLib

EnergyMonitor emon1;
EnergyMonitor emon2;

int currentPin1 = A0;
int currentPin2 = A1;
int voltagePin = A4;
int type = 2;

const int numReadings = 60;
int readIndex = 0;              // the index of the current reading
double total = 0;               // the running total
double average = 0;              // the average

unsigned long start;            //start time of calculations
unsigned long stop;             //stop time of calculations

double PowerTotal, PowerFactorTotal, VrmsTotal, IrmsTotal;

void setup() {
    Particle.variable("type", type);
    Particle.variable("voltage", VrmsTotal);
    Particle.variable("current", IrmsTotal);
    Particle.variable("power", PowerTotal);
    Particle.variable("factor", PowerFactorTotal);
    
    emon1.current(currentPin1, 90.9);         // Current: input pin, calibration.
    emon1.voltage(voltagePin, 253, -1.7);  // Voltage: input pin, calibration, phase_shift
    
    emon2.current(currentPin2, 90.9);         // Current: input pin, calibration.
    emon2.voltage(voltagePin, 253, -1.7);  // Voltage: input pin, calibration, phase_shift
}

void loop() {
    start = millis();
    emon1.calcVI(60,2000); // Calculate all. No.of half wavelengths (crossings), time-out
    emon2.calcVI(60,2000); // Calculate all. No.of half wavelengths (crossings), time-out
    
    //set variables
    VrmsTotal = emon1.Vrms;
    IrmsTotal = emon1.Irms + emon2.Irms;
    PowerTotal = emon1.realPower + emon2.realPower;
    PowerFactorTotal = (emon1.powerFactor + emon2.powerFactor)/2;
    
    total = total + emon1.realPower + emon2.realPower;
    readIndex = readIndex + 1;

    if (readIndex >= numReadings) {
        average = total / numReadings;
        
        Particle.publish("power", String(average, 2), PRIVATE);
        readIndex = 0;
        total = 0;
    }
    stop = millis();
    
    if((stop - start) < 1000)
        delay(1000 - (stop - start));
}