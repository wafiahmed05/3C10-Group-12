# Arduino Oscilloscope
Using the wave generator function on an oscilloscope, a sine wave is generated and used as input into an MP6042 op-amp.
The op-amp is configured as a buffer and is the analog input to the arduino uno r3.
The uno r3 quantizes the sine wave inputs and outputs to the D0 pin of the SSD1306 OLED. 
Using the SSD1306 arduino library, the sine wave is then plotted on the OLED screen. 