#!/bin/bash
sudo podman rm $(sudo podman stop $(sudo podman ps -a -q  --filter ancestor=docker.io/kaobinchou/rtapps-image:latest --format="{{.ID}}"))
