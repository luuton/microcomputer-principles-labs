`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2026/05/07 20:19:44
// Design Name: 
// Module Name: irom
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


module irom(
    input clk,
    input [6:0] addr,
    output [31:0] inst
    );

    reg [31:0] instMem [0:31];
    integer i;
    assign inst = instMem[addr];
    
    initial begin
        // addr+4为下一条指令
        $readmemh("C:/Users/yv236/Desktop/test.txt", instMem, 0, 12);// 预加载指令到指令存储器, 0-11行共12条指令
        $display("Instruction Memory Initialized:");
        for (i = 0; i < 12; i = i + 1) begin
            $display("Address %0d: %h", i, instMem[i]);
        end
    end
endmodule
