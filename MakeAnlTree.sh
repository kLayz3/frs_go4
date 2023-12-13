#!/bin/bash
### $1 should be filename
#name=/d/land4/sec/s526/sc41sc22rr_tac_10ns__0033.lmd
name=$1
fullpath=$name
filename_ext="/u/profi/NewGo4/data_engi_2023/sci41_sc43_pos_0013.lmd"
filename_without_ext="${filename_ext%.*}"
output_filename="analysis_029_sci41trig_0006.root"
echo "path for lmd file: " $name
echo "lmd file name    : " $filename_ext
echo "output file name : " $output_filename
go4analysis -file /u/profi/NewGo4/data_engi_2023/engrun_029_sc41trig_0006.lmd -step Analysis -store rootfile/$output_filename
