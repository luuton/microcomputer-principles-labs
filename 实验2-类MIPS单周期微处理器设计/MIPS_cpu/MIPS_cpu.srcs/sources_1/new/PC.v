`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2026/05/07 20:41:58
// Design Name: 
// Module Name: PC
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

// PC模块负责生成下一条指令的地址，根据当前指令类型（普通指令、分支指令、跳转指令）和条件（如分支是否满足）来更新PC值。
module PC(
    input clk,
    input reset,
    input jump,             // jump信号由控制单元输出，表示当前指令是否为跳转指令（如J），需要根据指令中的地址字段计算跳转地址
    input branch,           // branch信号由控制单元输出，表示当前指令是否为分支指令（如BEQ），需要根据zero信号判断是否跳转
    input zero,             // zero信号由ALU输出，表示上一次运算结果是否为零，通常用于BEQ指令的分支判断
    input [31:0] instr,     // 当前指令，用于提取offset和jump地址
    output reg [31:0] PAddr // PC寄存器，输出当前指令的地址
    );
    wire [31:0] offset = instr[15:15] ? {16'hFFFF, instr[15:0]} : {16'h0000, instr[15:0]}; // sign-extend offset，BEQ指令的offset是16位，需要根据符号位进行扩展
    // sign-extend offset，BEQ指令的offset是16位，需要根据符号位进行扩展
    // 根据instr[15]（即offset的符号位）来决定扩展的高16位是0还是1，确保offset在计算跳转地址时正确表示正负值

    always @(posedge clk) begin
        if(!reset) begin
            PAddr <= 32'b0;
        end else if (jump) begin            // J指令的跳转地址由PC的高4位和指令的低26位组成
            PAddr <= {PAddr[31:28], instr[25:0] << 2};
        end else if (branch && zero) begin  // BEQ指令的跳转地址由PC+4和指令的offset组成，offset需要左移2位（因为指令是按字对齐的）
            PAddr <= PAddr + 4 + (offset << 2);
        end else begin                      // 其他情况正常PC+4
            PAddr <= PAddr + 4;
        end
        if(PAddr >= 120) begin  // 120是指令存储器的地址上限，超过则回到0
            PAddr <= 32'b0;
        end
    end
endmodule
