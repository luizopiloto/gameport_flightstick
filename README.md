# gameport_flightstick
An overengineered gameport compatible flightstick for Retro PCs.

As it become more and more hard to find good and reliable gameport 
joysticks/flightsticks to play those old games using original hardware, 
I tried to make my own gameport joystick that use hall effect sensors 
instead of those unreliable 100K potentiometers.

The circuit make use of MCP41100 ICs to replace the 100K potentiometers. 

The microcontroler (Arduino Nano v3) code have noise reduction,
"cubic curve" axis smoothing and calibration routine, that saves the
Min/Max sensors range into EEPROM.

To activate calibration mode, press and hold all the 4 buttons untill
the joystick input starts to flicker, something around 12 seconds, then
release the buttons and move the 4 axis to maximun and minimun positions.
To save the calibration press button 3 + button 4.
To cancel press button 1 + button 2.
<p align="center">
  <img src="https://github.com/luizopiloto/gameport_flightstick/blob/main/flightstick_schem.png?raw=true" width="350" title="Circuit diagram">
</p>
