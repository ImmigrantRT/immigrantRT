#!/bin/bash
sudo podman rm $(sudo podman stop $(sudo podman ps -a -q  --filter ancestor=docker.io/kaobinchou/vr_video_player:latest --format="{{.ID}}"))

rm -rf $PWD/apps/vr_video_player/run/
mkdir $PWD/apps/vr_video_player/run/

sudo podman --runtime runc run --privileged --cgroupns=private --tmpfs $PWD/apps/vr_video_player/run/ -itd vr_video_player
