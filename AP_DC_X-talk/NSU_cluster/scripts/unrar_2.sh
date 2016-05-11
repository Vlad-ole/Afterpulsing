#!/bin/bash

unrar_path=/usr/bin/unrar
path=/mnt/scratch/ws/vpoleynikov/201603291814ws1/KETEK/290K/29_37V
rar_path=$path/raw/rar/run_
out_path=$path/raw/binary/

Start_global=1
End_global=1000
step=2


for start in `seq $Start_global $step $End_global` ; do
   	   	
	qsub -q bl2x220q -l select=1:ncpus=1:mem=50m,walltime=0:5:0 -- /bin/bash -c "$unrar_path x ${rar_path}${start}.rar ${out_path}; $unrar_path x ${rar_path}$(($start+1)).rar ${out_path}"
done

