#!/bin/bash

nohup ./darknet detect cfg/yolo.cfg yolo.weights  -out /mnt/fullImgFile/detect/image_aa/ &> /mnt/fullImgFile/detect/log.aa &
