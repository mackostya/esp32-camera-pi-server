# ESP32-cam
This is the project to create data transfer from ESP-32 camera to raspberry pi 3 using Arduino Uno and Arduino IDE.

The guide for this project is taken from the following [link](https://randomnerdtutorials.com/esp32-cam-post-image-photo-server/).


## Apache server on Raspberry Pi

Install apache2 if not already installed

```
sudo apt-get update
sudo apt-get install apache2 -y
```

Run the following command to start the apache server

```
sudo systemctl enable apache2
sudo systemctl start apache2
```
### php
If the php is not visualied accordingly install following packages

```
sudo apt-get install php libapache2-mod-php
```
And start

```
sudo a2enmod php*
sudo systemctl restart apache2
```

You can verify the apache server by opening the browser and typing the following address

```
http://<raspberry_pi_ip>/
```

or if you created file in `/var/www/html` directory, then you can access it by typing

```
http://<raspberry_pi_ip>/file.php
```

Consider changing the permission of the `/var/www/html` directory to allow the user to write in it.

```
sudo chown -R pi:pi /var/www/html
chmod -R 777 /var/www/html/
```

## Arduino Connection diagram

Consider different USB-Buses. For some the programming of ESP can simply fail, even after the established connection.

| **ESP32-CAM Pin**                 | **Arduino Uno Pin**             |
|-----------------------------------|---------------------------------|
| GND                               | GND                             |
| 5V                                | 5V                              |
| U0R (RX)                          | Pin 0 (RX)                      |
| U0T (TX)                          | Pin 1 (TX)                      |
| IO0<->GND *(for programming mode)*|                                 |
|                                   | RESET<->GND                     |

## Useful links

- [ESP32-CAM youtube guide](https://www.youtube.com/watch?v=7-3piBHV1W0&t=73s)

- [ESP32-CAM data transfer](https://randomnerdtutorials.com/esp32-how-to-log-data/)

- ESP32-CAM data transfer with [Raspberry Pi](https://randomnerdtutorials.com/raspberry-pi-apache-mysql-php-lamp-server/) and [MySQL](https://randomnerdtutorials.com/esp32-esp8266-raspberry-pi-lamp-server/)

## Bugfixes

### ESP32 board not visible in Arduino IDE
Consider downgrading the esp32 board (by Espressif Systems) to version 2.0.0, if getting the following error:

```
app_httpd.cpp:22:10: fatal error: fd_forward.h: No such file or directory
   22 | #include "fd_forward.h"
```
For this read following [issue](https://github.com/easytarget/esp32-cam-webserver/issues/191), `fd_forward.h` was removed from `v2.0.1+`.


### Python PATH fix

- Python [Error fix](https://stackoverflow.com/questions/71479069/exec-python-executable-file-not-found-in-path) while flashing with Arduino IDE

- For python Fix consider opening Arduino from Terminal, if python is normally visible there.

Check python path with

```
which python
```

If python is visible, then open Arduino from terminal with

```
open /Applications/Arduino.app
```