connect -url tcp:127.0.0.1:3121
targets -set -filter {jtag_cable_name =~ "Digilent Arty A7-100T 210319AD2B42A" && level==0} -index 0
fpga -file /home/caglayan/Desktop/Workspaces/WS_Vivado/MicroblazeDDR/MicroblazeDDR.sdk/mainBd_wrapper_hw_platform_0/mainBd_wrapper.bit
targets -set -nocase -filter {name =~ "microblaze*#0" && bscan=="USER2"  && jtag_cable_name =~ "Digilent Arty A7-100T 210319AD2B42A"} -index 0
rst -system
after 3000
targets -set -nocase -filter {name =~ "microblaze*#0" && bscan=="USER2"  && jtag_cable_name =~ "Digilent Arty A7-100T 210319AD2B42A"} -index 0
dow /home/caglayan/Desktop/Workspaces/WS_Vivado/MicroblazeDDR/MicroblazeDDR.sdk/mainProj/Debug/mainProj.elf
bpadd -addr &main
