# gameport_flightstick
An overengineered gameport compatible flightstick for Retro PCs

It's becoming more and more hard to find good and relyable gameport joysticks/flightsticks
to play those old games using original hardware.
This project started as a try to make my own gameport joystick using hall effect sensors
instead of those unreliable 100K potentiometers.
The circuit make use of MCP41100 ICs to replace the 100K potentiometers and make the device
as reliable as possible.
The microcontroler (Arduino Nano v3) code have noise reduction and "cubic curve" axis smoothing
![alt text](https://github.com/luizopiloto/gameport_flightstick/blob/main/flightstick_schem.png?raw=true)
