jfbot
=====

super-secret arduino project which noone is allowed to know about,
except the whole open-source community

This is both a hobby thing and an education vehicle. So the code will
be initially very densely commented, also including the basics of C++.

# arduino

## Getting started

+ Download and install from www.arduino.cc
+ Clone this project into ~/sketchbook/jfbot (directory name must be *jfbot*)
+ Start arduino with the board connected and check that tools/serial port is set

If tools/serial port is greyed out, it means that you don't have read/write permissions
to the serial port. The serial port is most likely `/dev/ttyUSB0` so do `ls -l /dev/ttyUSB0`
to see who has the rights. In my case (Ubuntu 14.04) it is the group *dialout*. So,
add yourself to the group `sudo gpasswd -a username dialout` and logout/login for it to take
effect. You can run the command `id` to check that the relevant group is listed. Then restart
arduino and hopefully all is OK. Some people run it under `sudo` but that is hardly a clever
thing to do. But one can try to run it under `sudo` and if the port is visible then, it means
you have a permissions problem.

## Running

Open `jfbot` from file/sketchbook. Code happily. Then file/Upload and
test. For the upload to succeed, you need to set the board under
Tools/Board. In our case this is `Nano w/ atmega328`.