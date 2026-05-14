#!/bin/sh

# misc
gcc misc/repeat/src/main.c -o misc/repeat/bin/exec
make -C misc/processing/

# RT controller
# sudo sh -c 'echo RTCdir=$PWD >> /etc/environment'
# sudo -H gedit /etc/environment
# echo RTCdir=$PWD >> ~/.bashrc
# . ~/.bashrc
export RTCdir=$PWD
make -C PIController/

# monitor
make -C monitor/container_monitor/

# prediction
make -C prediction/curve_fitting/

# analysis
make -C analysis
