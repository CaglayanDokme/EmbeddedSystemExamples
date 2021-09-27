# Brief     : Constraint file for Zynq Triple Timer Counter example project, based on Zedboard.
# Author    : Caglayan DOKME, caglayandokme@gmail.com
# Date      : September 27, 2021 -> Created

set_property PACKAGE_PIN Y11 [get_ports TTC0_WAVE0_OUT]
set_property PACKAGE_PIN AA11 [get_ports TTC0_WAVE1_OUT]
set_property PACKAGE_PIN Y10 [get_ports TTC0_WAVE2_OUT]
set_property PACKAGE_PIN AA9 [get_ports WDT_RST_OUT]
set_property IOSTANDARD LVCMOS33 [get_ports TTC0_WAVE0_OUT]
set_property IOSTANDARD LVCMOS33 [get_ports TTC0_WAVE1_OUT]
set_property IOSTANDARD LVCMOS33 [get_ports TTC0_WAVE2_OUT]
set_property IOSTANDARD LVCMOS33 [get_ports WDT_RST_OUT]
