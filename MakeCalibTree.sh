#!/bin/bash
### $1 should be filename
fullpath=$1
filename_ext="${fullpath##*/}"
filename_without_ext="${filename_ext%.*}"
output_filename="calib_${filename_without_ext}.root"
echo "path for lmd file: " $1
echo "lmd file name    : " $filename_ext
echo "output file name : " $output_filename
go4analysis -file $1 -step Calibr -store rootfile/$output_filename
