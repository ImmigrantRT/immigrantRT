#!/bin/sh

# misc
rm misc/repeat/bin/exec
make -C misc/processing/ clean

# RT Controller
make -C PIController clean

# monitor
make -C monitor/container_monitor/ clean

# prediction
make -C prediction/curve_fitting/ clean

# analysis
make -C analysis clean
