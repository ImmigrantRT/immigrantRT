#!/bin/bash
rm apps/RT-apps/RT-apps.o
g++ -lrt apps/RT-apps/RT-apps.cc -o apps/RT-apps/RT-apps.o
sudo chmod 777 apps/RT-apps/RT-apps.o
sudo podman image rm rtapps-image
sudo podman build -f apps/RT-apps/Dockerfile --tag rtapps-image
