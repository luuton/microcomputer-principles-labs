# 微机原理课程实验（Microcomputer Principles Labs）

本仓库整理了华中科技大学电信学院微机原理实验课程的 4 个实验，覆盖：

- MIPS 汇编程序设计
- 类 MIPS 单周期微处理器设计（Verilog）
- 并行 IO 接口实验（MicroBlaze + AXI GPIO/Timer/INTC）
- 串行 IO 接口实验（UART 通信）

适合用于课程作业提交、实验复现实操和期末复习。

## 1. 仓库结构

```text
实验1-MIPS汇编程序设计/
├── mips1.asm          # MIPS汇编程序1
├── mips1.c            # C参考实现
├── task1.asm          # 任务1汇编代码
└── test1.txt          # 导出的机器码

实验2-类MIPS单周期微处理器设计/
├── mips1.asm          # 测试程序1
├── mips2.asm          # 测试程序2
├── test.txt           # 导出的机器码
└── MIPS_cpu/          # 微处理器设计
    ├── create_project.bat    # Windows项目创建脚本
    ├── create_prj.tcl        # Vivado重建脚本
    └── MIPS_cpu.srcs/
        ├── sources_1/new/    # RTL源码
        │   ├── pc.v
        │   ├── alu.v
        │   ├── regfile.v
        │   ├── controller.v
        │   └── ...
        └── sim_1/new/        # 仿真文件
            ├── tb_cpu.v
            └── ...

实验3-并行IO接口实验/
├── 快速中断.c          # 快速中断方式代码
└── IO_exp/            # Vivado硬件工程
    ├── create_project.bat
    ├── create_prj.tcl
    ├── IO_exp.srcs/           # 硬件源码
    │   ├── constrs_1/         # 约束文件
    │   └── sources_1/         # RTL设计
    └── IO_exp.sw/             # 软件工程
        ├── demo/              # 扫描实现
        │   └── src/
        │       └── demo.c
        └── intr/              # 中断实现
            └── src/
                └── main.c

实验4-串行IO接口实验/
└── IO_exp/            # Vivado硬件工程
    ├── create_project.bat
    ├── create_prj.tcl
    ├── IO_exp.srcs/           # 硬件源码
    └── IO_exp2.sw/            # 软件工程
        └── serial_IO/
            └── main.c         # 源码
```

## 2. 开发环境

建议环境：

- Windows 10/11
- Xilinx Vivado 2021.2（仓库中的 Tcl 脚本由该版本导出）
- Vitis/SDK（用于 MicroBlaze 软件工程编译与下载）
- 可选：MARS / QtSPIM（用于实验1汇编验证）

硬件平台信息：

- 实验2：FPGA Part 为 `xc7a35tcsg324-1`
- 实验3/4：开发板目标为 Nexys4 DDR（`xc7a100tcsg324-1`）

## 3. 各实验说明与运行方法

## 3.1 实验1：MIPS 汇编程序设计

核心内容：

- 使用 C 与 MIPS 汇编分别实现 `PENO` 逻辑
- 统计数组中的：
- 正奇数之和 `SP`
- 负偶数之和 `SN`

关键文件：

- `mips1.c`：C 版本参考实现
- `mips1.asm` / `task1.asm`：MIPS 汇编实现与测试输出
- `test1.txt`：测试相关记录

建议运行：

1. 用 MARS/QtSPIM 打开 `mips1.asm` 或 `task1.asm`。
2. Assemble 后运行，观察控制台输出的 `SP` 与 `SN`。
3. 与 `mips1.c` 运行结果对照，验证一致性。

## 3.2 实验2：类 MIPS 单周期微处理器设计

核心内容：

- 基于 Verilog 实现类 MIPS 单周期 CPU 的关键模块：
- `ALU.v`
- `ctr.v`
- `Dram.v`
- `irom.v`
- `PC.v`
- `regFile.v`
- `top.v`
- 提供对应 testbench：`tb_ALU.v`、`tb_ctr.v`、`tb_top.v` 等
- 使用 `mips1.asm`、`mips2.asm` 作为指令测试样例

运行方式（推荐）：

1. 进入目录：`实验2-类MIPS单周期微处理器设计/MIPS_cpu`。
2. 双击 `create_project.bat`（或在命令行执行）。
3. Vivado 会调用 `create_prj.tcl` 重建工程。
4. 打开生成工程，运行 Simulation（可先跑 `tb_top.v`）。
5. 根据波形和 `test.txt` 检查功能正确性。

## 3.3 实验3：并行 IO 接口实验

核心内容：

- 基于 MicroBlaze + AXI GPIO + AXI Timer + AXI INTC
- 实现按键/拨码开关中断、LED 输出、数码管动态显示
- 含两种中断处理思路：
- 普通中断分发（如 `IO_exp.sw/intr/src/main.c`）
- 快速中断处理（仓库根下 `快速中断.c`）

硬件工程重建：

1. 进入目录：`实验3-并行IO接口实验/IO_exp`。
2. 执行 `create_project.bat` 重建 Vivado 工程。
3. 生成 bitstream 并导出硬件平台（XSA）。

软件工程运行：

1. 在 Vitis/SDK 导入 `IO_exp.sw` 下工程（如 `intr` / `demo`）。
2. 关联硬件平台后编译下载。
3. 打开串口终端，查看按键、开关中断打印信息。

## 3.4 实验4：串行 IO 接口实验

核心内容：

- 在并行 IO 基础上增加 UART 串行通信
- 通过中断处理 UART 收发
- 支持 16 位开关量的分字节发送/接收与重组
- 按键状态通过串口发送并驱动对端显示逻辑

关键软件入口：

- `IO_exp/IO_exp/IO_exp2.sw/serial_IO/src/main.c`

运行步骤：

1. 进入目录：`实验4-串行IO接口实验/IO_exp`。
2. 执行 `create_project.bat` 重建硬件工程。
3. 在 Vitis/SDK 导入 `serial_IO` 工程并编译下载。
4. 按实验连线要求连接串口/板卡，观察 UART 与 GPIO 联动。

## 4. 常见问题

1. 找不到 `vivado.bat`
   - 说明 Vivado 未加入系统环境变量。
   - 解决：使用 Vivado Tcl Console 执行 `source create_prj.tcl`，或将 Vivado `bin` 目录加入系统环境变量。

2. Tcl 重建工程时报缺失 IP
   - 先确认本机已安装对应 Xilinx IP Catalog 与板卡文件（Nexys4 DDR board files）。
   - 报错`ERROR: [Board 49-71] The board_part definition was not found for digilentinc.com:nexys4_ddr:part0:1.1.`
      1. 请检查你的`nexys4 ddr`板卡文件路径是否为`%APPDATA%\Xilinx\Vivado\2021.2\xhub\board_store\xilinx_board_store\XilinxBoardStore\Vivado\2021.2\boards\Digilent`
      2. 若没有请访问[Digilent/nexys4_ddr](https://github.com/Digilent/vivado-boards/tree/master/new/board_files/nexys4_ddr/C.1)下载板卡文件并放置到上述路径的`.\nexys4_ddr/C.1`文件夹下
      3. 如果你已经导入过板卡文件且记得在哪，可以打开`create_prj.tcl`脚本，查找`board_part_repo_paths`，将那条`set_property`命令的`-value`后的路径直接替换为你的板卡路径，**用双引号包裹路径、反斜杠改'\'为斜杠'/'、最后面的反斜杠'\'不要删**
      4. 删除生成的`IO_exp/`等文件/文件夹，再次运行`create_project.bat`批处理命令

3. 软件工程无法编译
   - 检查硬件平台（XSA）与 BSP 是否重新生成并关联。
   - 清理并重编译工程。

4. 中断打印异常或无输出
   - 检查串口波特率、COM 口、下载的 bitstream 与 elf 是否匹配。
   - 检查中断使能顺序：外设使能 -> INTC 使能 -> CPU 全局中断使能。

## 5. 说明

- 本仓库包含较多由 Vivado/Vitis 自动生成的工程文件，体积可能较大。
- 有问题可以提issue
