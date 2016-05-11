#!/bin/bash           
echo Parser script
program_path="/mnt/storage/home/vpoleynikov/Qt/Parser/Parser"
files_path="/mnt/scratch/ws/vpoleynikov/201601261856ws1/MPPC_S10362-11-100C/285K/69_65V/"
th_amp=0.008
th_der=0.0003

$program_path $files_path $th_amp $th_der 1 1 &
$program_path $files_path $th_amp $th_der 100 100