## some modification of env. var. for propper work of utilities

DIR="$( dirname -- "${BASH_SOURCE[0]}"; )";   # Get the directory name
DIR="$( realpath -e -- "$DIR"; )";

if [ -n "$LD_LIBRARY_PATH" ]; then
    export LD_LIBRARY_PATH=${DIR}:${LD_LIBRARY_PATH};
else
    export LD_LIBRARY_PATH=${DIR};
fi


if [ -n "$ROOT_INCLUDE_PATH" ]; then
    export ROOT_INCLUDE_PATH=${GO4SYS}/include:${ROOT_INCLUDE_PATH};
else
    export ROOT_INCLUDE_PATH=${GO4SYS}/include;
fi

 export ROOT_INCLUDE_PATH=${DIR}/src:${ROOT_INCLUDE_PATH};
