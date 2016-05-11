#!/bin/bash

#LD_LIBRARY_PATH=/mnt/storage/home/vpoleynikov/root_5_34_34/lib/root
program_path=/mnt/storage/home/vpoleynikov/Qt/Parser/Parser
files_path=/mnt/scratch/ws/vpoleynikov/201603291814ws1/KETEK/290K/29_37V/
th_amp=0.0060
th_der=0.00014
N_job_max=1000

Start_global=1
End_global=1000
step=3

for start in `seq $Start_global $step $End_global` ; do
   	let "end = $start + $step - 1"
   	
	cmd="qsub -q bl2x220q -l select=1:ncpus=1:mem=300m,walltime=0:20:0 -v LD_LIBRARY_PATH="/mnt/storage/home/vpoleynikov/root_5_34_34/lib/root" -- $program_path $files_path $th_amp $th_der $start $end"
	#cmd="qsub -l select=1:ncpus=1:mem=300m,walltime=0:20:0 -v LD_LIBRARY_PATH="/mnt/storage/home/vpoleynikov/root_5_34_34/lib/root" -- $program_path $files_path $th_amp $th_der $start $end"
    

	echo $cmd
   	$cmd
done