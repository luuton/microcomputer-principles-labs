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
    output reg [31:0] inst
    );

    reg [31:0] instMem [0:31];
    always @(posedge clk) begin
        inst = instMem[addr];
    end
    
    //assign inst = instMem[addr];
    initial begin
        $readmemh("test.coe", instMem, 0, 11);// 预加载指令到指令存储器, 0-11行共12条指令
    end
endmodule
