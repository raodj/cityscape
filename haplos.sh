#!/bin/bash

#PBS -N HAPLOS
#PBS -l walltime=48:00:00
#PBS -l mem=24GB
#PBS -l nodes=1:ppn=1

cd /home/schmidee/haplos/haplos
/usr/bin/time -v ./haplos > output.txt