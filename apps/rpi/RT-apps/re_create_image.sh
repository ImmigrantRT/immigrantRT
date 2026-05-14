#!/bin/bash
sudo podman rm $(sudo podman stop $(sudo podman ps -a -q  --filter ancestor=docker.io/kaobinchou/rtapps-image-rpi:latest --format="{{.ID}}"))
sudo podman image rm rtapps-image-rpi
$PWD/apps/rpi/RT-apps/build_old.sh
sudo podman login -u kaobinchou
sudo podman push localhost/rtapps-image-rpi:latest docker://docker.io/kaobinchou/rtapps-image-rpi:latest
sudo podman image rm rtapps-image-rpi
$PWD/apps/rpi/RT-apps/build.sh
