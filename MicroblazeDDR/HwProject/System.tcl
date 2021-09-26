# Brief 	: This is a tickle file for automatically generating the Microbale DDR project
# Author 	: Caglayan DOKME
# Date		: September 19, 2021

# Starts the graphical user interface
start_gui

# Detect the current working path
set script_path [file dirname [file normalize [info script]]]

# Set Project name
set project_name MicroblazeDDR

# Create a project
create_project $project_name $script_path -part xc7a100tcsg324-1

# Set the part that will be used in project
set_property board_part digilentinc.com:arty-a7-100:part0:1.0 [current_project]

# Add IP repo to repository paths (No custom IPs in current project)
# set_property  ip_repo_paths  $ipRepo [current_project]
# update_ip_catalog

# Import RTL resources (No custom RTL modules in current project)
# add_files -norecurse -scan_for_includes $script_path/$rtlModuleName
# import_files -norecurse $script_path/$rtlModuleName
# update_compile_order -fileset sources_1

# Invoke the block design script
# The block design script file can be generated automatically in Vivado
source $script_path/mainBd.tcl

# Create and add an HDL wrapper for the block design
make_wrapper -files [get_files $script_path/${project_name}.srcs/sources_1/bd/mainBd/mainBd.bd] -top
add_files -norecurse $script_path/${project_name}.srcs/sources_1/bd/mainBd/hdl/mainBd_wrapper.v

# Import the constraint file to the project
add_files -fileset constrs_1 -norecurse $script_path/ArtyA7.xdc
import_files -fileset constrs_1 $script_path/ArtyA7.xdc
