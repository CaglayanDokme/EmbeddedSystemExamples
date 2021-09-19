# Brief 	: This is an example tickle file for automatically generating a project
#           : with various resources such as RTL modules, block design, constraints, etc.
# Author 	: Caglayan DOKME, caglayandokme@gmail.com
# Date		: September 19, 2021

####################################
########### USER DEFINES ###########
####################################
# Project name
set projectName MicroblazeDDR

# Part and Board Part name
set partName xc7a100tcsg324-1
set boardPartName digilentinc.com:arty-a7-100:part0:1.0

# IP repo path
set ipRepoPath /REPOs/ip_repo/

# RTL Modules to be imported (Multiple declaration allowed)
array set rtlModuleNames {0 RtlModule0.vhd 1 RtlModule1.vhd}

# Block Design Tickle
set blockDesigName mainBd

# Constraint file to be imported
set xdcFileName /REPOs/Constraints/constraint.xdc

####################################
### COMMANDS FOR AUTO GENERATION ###
####################################
# Starts the graphical user interface
start_gui

# Detect the current working path
set script_path [file dirname [file normalize [info script]]]

# Create a project with part
create_project $project_name $script_path -part $partName

# Set the part that will be used in project
set_property board_part $boardPartName [current_project]

# Add IP repo to repository paths
if {[info exists rtlModuleName]} {
    set_property ip_repo_paths $ipRepo [current_project]
    update_ip_catalog
}

# Import RTL resources
if {[info exists rtlModuleNames]} {
    foreach key [array names rtlModuleNames] {
        add_files -norecurse -scan_for_includes $script_path/$rtlModuleNames($key)
        import_files -norecurse $script_path/$rtlModuleNames($key)
        update_compile_order -fileset sources_1
    }
}

# Invoke the block design script
# The block design script file can be generated automatically in Vivado
if {[info exists blockDesigName]} {
    source $script_path/${blockDesigName}.tcl

    # Create and add an HDL wrapper for the block design
    make_wrapper -files [get_files $script_path/${project_name}.srcs/sources_1/bd/${blockDesigName}/${blockDesigName}.bd] -top
    add_files -norecurse $script_path/${project_name}.srcs/sources_1/bd/${blockDesigName}/hdl/${blockDesigName}_wrapper.v
}

# Import the constraint file to the project
if {[info exists xdcFileName]} {
    puts "Constraint file given, importing.."

    add_files -fileset constrs_1 -norecurse $script_path/${xdcFileName}
    import_files -fileset constrs_1 $script_path/${xdcFileName}
}
