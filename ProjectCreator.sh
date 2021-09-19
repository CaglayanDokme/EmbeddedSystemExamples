#!/bin/bash
# Brief     : This is a shell script file for invoking the Vivado and initially run a tickle file in it
# Date      : September 19, 2020
# Note      : User must specify a valid tickle file name
# Author    : Caglayan DOKME, caglayan.dokme@gmail.com
# Usage     : User can either modify the script file to invoke the Vivado with a custom TCL file
#           : or give the TCL file as an input argument(Better with full path)
#           : The Vivado application version and installiation path can vary for hos devices
#           : Therefore, it can also be given as an input or modified inside the script file

# User specified variables(Can be modified using the command line input arguments)
tclfile=System.tcl                          # Main tickle file name to be used initially in Vivado
vivadoPath=/tools/Xilinx/Vivado/2017.4      # Path of the Vivado in the host device

# Check for command line input arguments
# First input argument represents the tickle file name(can be full path)
if [ $# -gt 0 ]; then
    tclfile=$1

    # Second input argument represents the full path to Vivado
    if [ $# -gt 1 ]; then
        vivadoPath=$2

        # Cannot accept more than 2 input arguments
        if [ $# -gt 2 ]; then
            echo "ERROR: Too many($#) input arguments!"

            exit
        fi
    fi
fi

# Check if the given tickle file exists
if [ -e "$tclfile" ]; then
    echo "Given tickle file $tclfile exists."
else
	echo "ERROR: Given tickle file $tclfile doesn't exist!"

    exit
fi

# Check if the given Vivado directory exists
if [ -d "$vivadoPath" ]; then
    echo "Given Vivado path $vivadoPath is valid."
else
	echo "ERROR: Given Vivado path $vivadoPath is invalid!"

    exit
fi

# Check if the given Vivado directory includes the corresponding shell script
if ! [ -e "${vivadoPath}/settings64.sh" ]; then
	echo "ERROR: Couldn't find the settings64.sh script under given Vivado path $vivadoPath!"

    exit
fi

# Operation was successfull
echo "Vivado invoked, the program will run the given TCL file $tclfile"
echo "Caglayan DOKME salutes you :)"

# Invoke the Vivado using the given TCL script initially
# Ampersand(&) used in order not to wait for the Vivado to launch
source ${vivadoPath}/settings64.sh
vivado  -mode tcl   \
        -nojournal  \
        -nolog      \
        -notrace    \
        -source $tclfile &

# Child process(Vivado) has been disowned for the purpose of
# exiting the terminal safely without forcing them to close
disown

# Script ended
exit 0
