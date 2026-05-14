#!/bin/bash
rm apps/rpi/RT-apps/RT-apps.o
make -C apps/rpi/RT-apps/
sudo chmod 777 apps/rpi/RT-apps/RT-apps.o
sudo podman image rm rtapps-image-rpi
sudo podman build -f apps/rpi/RT-apps/Dockerfile --tag rtapps-image-rpi
