# gameport_flightstick
An overengineered gameport compatible flightstick for Retro PCs.

As it becomes more and more hard to find good and reliable gameport 
joysticks/flightsticks to play those old games on the original hardware, 
I tried to make my own gameport joystick that uses hall effect sensors 
instead of those unreliable potentiometers.

The circuit makes use of MCP41100 ICs to replace the 100K potentiometers. 

The microcontroler (Arduino Nano v3) takes care of noise reduction
adds a "cubic curve" to smooth the input.

To activate calibration mode, press and hold all the 4 buttons untill
the joystick input starts to flicker, something around 10 seconds. 
Release the buttons and move all the 4 axis to it's maximun and minimun positions.
To save the calibration press button 3 + button 4.
To cancel press button 1 + button 2.
<p align="center">
  <img src="https://github.com/luizopiloto/gameport_flightstick/blob/main/flightstick_schem.png?raw=true" width="350" title="Circuit diagram">
</p>
