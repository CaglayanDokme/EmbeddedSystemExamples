# Brief		: Main Tickle file for regenerating the XADC sample project on Zedboard
# Author 	: Caglayan DOKME
# Date		: September 24, 2021

# Starts the graphical user interface
start_gui

# Detect the current working path
set script_path [file dirname [file normalize [info script]]]

# Set Project name
set project_name ZedboardXadc

# Create a project 
create_project $project_name $script_path -part xc7z020clg484-1

# Set the part that will be used in project
set_property board_part em.avnet.com:zed:part0:1.3 [current_project]

# Invoke the block design script
# The block design script file can be generated automatically in Vivado
source $script_path/mainBd.tcl

# Create and add an HDL wrapper for the block design
make_wrapper -files [get_files $script_path/${project_name}.srcs/sources_1/bd/mainBd/mainBd.bd] -top
add_files -norecurse $script_path/${project_name}.srcs/sources_1/bd/mainBd/hdl/mainBd_wrapper.v

# Import the constraint file to the project
add_files -fileset constrs_1 -norecurse $script_path/Zedboard.xdc
import_files -fileset constrs_1 $script_path/Zedboard.xdc
