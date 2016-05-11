#!/bin/bash

#PBS -l select=2:ncpus=1:mem=300m,place=scatter
#PBS -l walltime=01:00:00
#PBS -m n

cd $PBS_O_WORKDIR
echo "I run on node: `uname -n`"
echo "My working directory is: $PBS_O_WORKDIR"
echo "Assigned to me nodes are:"
cat $PBS_NODEFILE
#sleep 10

echo Parser script
LD_LIBRARY_PATH="/mnt/storage/home/vpoleynikov/root_5_34_34/lib/root"
program_path="/mnt/storage/home/vpoleynikov/Qt/Parser/Parser"
files_path="/mnt/scratch/ws/vpoleynikov/201601261856ws1/MPPC_S10362-11-100C/285K/69_65V/"
th_amp=0.008
th_der=0.0003

$program_path $files_path $th_amp $th_der 1 1 &
$program_path $files_path $th_amp $th_der 2 2 






