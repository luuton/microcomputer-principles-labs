`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2026/05/07 20:24:46
// Design Name: 
// Module Name: Dram
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


module Dram(
    input clk,
    input memWriteEn,
    input [31:0] WriteData,
    input [5:0] Addr,
    output [31:0] ReadData
    );
    reg [31:0] dataMem [0:31];
    assign ReadData = dataMem[Addr];

    always @(negedge clk) begin     // 在时钟下降沿完成数据写入
        if(memWriteEn) begin
            dataMem[Addr] <= WriteData;
        end
    end
    
endmodule
