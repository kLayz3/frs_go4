#!/bin/bash
script_path="$(dirname -- "$(readlink -f -- $0)")"

def_file=$script_path/__foot_defs.hh
ptr_file=$script_path/__foot_ptr_assign.hh
clr_file=$script_path/__foot_clear_all.hh
files="$def_file $ptr_file $clr_file"

cmn=$script_path/foot_common.hh

echo "#pragma once" | tee $files &>/dev/null

MAX_ID=$(grep '#define MAX_FOOT_I' $cmn | awk '{print $3;}')
for f in $files; do
	echo "/* This is autogenerated file. Do not edit it. */" >> $f
	echo >> $f
done

echo "#define FOOT_UNPACK_STRUCTURES_DECL \\" >> $def_file
echo "#define FOOT_PTR_ASSIGN \\" >> $ptr_file
echo "#define CLEAR_FOOT \\" >> $clr_file

for (( i=1; i <= $MAX_ID; ++i )); do
	echo "    __FOOT_RAW__($i) \\" >> $def_file
	echo "    __ASSIGN_FOOT_PTR__($i) \\" >> $ptr_file
	echo "    __CLEAR_FOOT__($i) \\" >> $clr_file
done

echo "    TFootPtr foot[MAX_FOOT_I + 1]; \\" >> $def_file

# Add last newline so #define's don't jump line into EOF
echo | tee -a $files &>/dev/null
