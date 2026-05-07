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


module PC(
    input clk,
    input reset,
    input jump,
    input branch,
    input zero,
    input [31:0] instr,
    output reg [31:0] PAddr
    );
    wire [31:0] offset = instr[15:15] ? {16'hFFFF, instr} : {16'h0000, instr};

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
