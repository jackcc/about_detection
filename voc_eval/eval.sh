#!/bin/bash

test_file=`grep valid ../darknet/vehicle/vehicle.data | cut -d '=' -f2`
dir=../darknet/results/
for i in `ls $dir`
do
    classname=`echo $i | cut -d'.' -f1 | cut -d'_' -f4`
    echo $classname: 
    python2 voc_eval.py $dir/$i $test_file $classname
done
#python2 voc_eval.py ../darknet/results/comp4_det_test_bus.txt 2007_test.txt bus
#python2 voc_eval.py ../darknet/results/comp4_det_test_bicycle.txt 2007_test.txt bicycle
#python2 voc_eval.py ../darknet/results/comp4_det_test_car.txt 2007_test.txt car
#python2 voc_eval.py ../darknet/results/comp4_det_test_motorbike.txt 2007_test.txt motorbike
