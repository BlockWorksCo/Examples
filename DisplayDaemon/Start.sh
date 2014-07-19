#!/bin/sh


python2 /root/Apps/Analyser.py 1 &
python2 /root/Apps/Clock.py 2 &
python2 /root/Apps/TextDemo.py 3 &

sleep 1

python2 /root/DisplayDaemon.py 1 2 3






