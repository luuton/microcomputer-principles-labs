`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2026/05/14 13:12:38
// Design Name: 
// Module Name: tb_pc
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module tb_pc(
    output [31:0] Paddr
    );
    reg clk;
    reg reset;
    reg jump;
    reg branch;
    reg zero;
    reg [31:0] instr;
    PC uut (
        .clk(clk),
        .reset(reset),
        .jump(jump),
        .branch(branch),
        .zero(zero),
        .instr(instr),
        .PAddr(Paddr)
    );
    parameter period = 10;
    always #(period/2) clk = ~clk;

    initial begin
        clk = 0;
        reset = 0;
        jump = 0;
        branch = 0;
        zero = 0;
        instr = 32'b0;
        
        #10; // 等待一个时钟周期
        reset = 1; // 释放复位信号，PC开始工作
        
        // 模拟一些指令输入
        #10; // 等待一个时钟周期
        instr = 32'h00000000; // NOP指令，PC应该加4
        // 输入一条普通指令，PC应该加4
        
        #10;

        #10; // 等待一个时钟周期
        instr = 32'h08000005; // J指令，跳转到地址20（4*5）
        jump = 1; // 设置jump信号，PC应该跳转到地址20

        #10; // 等待一个时钟周期
        jump = 0; // 取消jump信号，PC应该继续正常加4
        instr = 32'h10000002; // BEQ指令，offset为8（2*4），如果zero为1则跳转到地址24
        
        #10; // 等待一个时钟周期
        zero = 1; // 设置zero信号为1
        branch = 1; // 设置branch信号，PC应该根据BEQ指令跳转到地址24
        
        #10; // 等待一个时钟周期
        zero = 0; // 取消zero信号，PC应该继续正常加4
        branch = 0; // 取消branch信号，PC应该继续正常加4
        instr = 32'h00000000; // PC应该加4
        
        #10; // 等待一个时钟周期
        $finish; // 结束仿真
    end
endmodule
