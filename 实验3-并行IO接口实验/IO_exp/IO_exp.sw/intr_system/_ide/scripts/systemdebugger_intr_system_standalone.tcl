# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: E:\XilinxProject\IO_exp\IO_exp.sw\intr_system\_ide\scripts\systemdebugger_intr_system_standalone.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source E:\XilinxProject\IO_exp\IO_exp.sw\intr_system\_ide\scripts\systemdebugger_intr_system_standalone.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -filter {jtag_cable_name =~ "Digilent Nexys4DDR 210292A4BE34A" && level==0 && jtag_device_ctx=="jsn-Nexys4DDR-210292A4BE34A-13631093-0"}
fpga -file E:/XilinxProject/IO_exp/IO_exp.sw/intr/_ide/bitstream/design_1_wrapper.bit
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
loadhw -hw E:/XilinxProject/IO_exp/IO_exp.sw/mb_basic_io/export/mb_basic_io/hw/design_1_wrapper.xsa -regs
configparams mdm-detect-bscan-mask 2
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
rst -system
after 3000
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
dow E:/XilinxProject/IO_exp/IO_exp.sw/intr/Debug/intr.elf
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
con
