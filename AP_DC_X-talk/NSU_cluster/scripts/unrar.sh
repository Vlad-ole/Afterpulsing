#!/bin/bash

unrar_path=/usr/bin/unrar
path=/mnt/scratch/ws/vpoleynikov/201601261856ws1/MPPC_S10362-11-100C/275K/69_10V
rar_path=$path/raw/rar/run_
out_path=$path/raw/binary/

Start_global=801
End_global=1000
step=1


for start in `seq $Start_global $step $End_global` ; do
   	   	
	#cmd="unrar x ${rar_path}${start}.rar ${out_path}"

	cmd="qsub -q bl2x220g6q -l select=1:ncpus=1:mem=50m,walltime=0:5:0 -- $unrar_path x ${rar_path}${start}.rar ${out_path}"
	echo $cmd
	$cmd


	#cmd="qsub -q bl2x220g6q -l select=1:ncpus=1:mem=50m,walltime=0:5:0 -- $unrar_path x ${rar_path}$(($start+1)).rar ${out_path}"


done