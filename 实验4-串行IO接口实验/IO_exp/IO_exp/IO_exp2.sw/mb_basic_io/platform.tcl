# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct E:\XilinxProject\IO_exp\IO_exp2.sw\mb_basic_io\platform.tcl
# 
# OR launch xsct and run below command.
# source E:\XilinxProject\IO_exp\IO_exp2.sw\mb_basic_io\platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {mb_basic_io}\
-hw {E:\XilinxProject\IO_exp\design_1_wrapper.xsa}\
-proc {microblaze_0} -os {standalone} -out {E:/XilinxProject/IO_exp/IO_exp2.sw}

platform write
platform generate -domains 
platform active {mb_basic_io}
platform generate
platform active {mb_basic_io}
platform generate -domains 
platform generate -domains 
platform config -updatehw {E:/XilinxProject/IO_exp/design_1_wrapper.xsa}
platform clean
platform generate
platform clean
platform generate
platform active {mb_basic_io}
platform generate -domains 
platform generate -domains standalone_domain 
platform clean
platform generate
platform clean
platform clean
platform clean
