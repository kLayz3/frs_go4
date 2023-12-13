#!/bin/bash
### $1 should be filename
fullpath=$1
#fullpath=/d/land4/sec/s533/s533_61_gC_6mm_0187.lmd
filename_ext="${fullpath##*/}"
filename_without_ext="${filename_ext%.*}"
output_filename="sort_${filename_without_ext}.root"
echo "path for lmd file: " $fullpath
echo "lmd file name    : " $filename_ext
echo "output file name : " $output_filename
#go4analysis -file $fullpath -step Sort -store rootfile/$output_filename -number 5000
go4analysis -file $fullpath -step Sort -store rootfile/$output_filename 

