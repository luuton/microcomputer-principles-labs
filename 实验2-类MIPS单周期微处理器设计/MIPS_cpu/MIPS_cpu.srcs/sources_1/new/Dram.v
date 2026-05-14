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
    input memReadEn,
    input [31:0] WriteData,
    input [5:0] Addr,
    output [31:0] ReadData
    );
    reg [31:0] dataMem [0:31];
    // dataMem初始化为地址取值
    integer i;
    initial begin
        for (i = 0; i < 32; i = i + 1) begin
            dataMem[i] = i;
        end
    end
    assign ReadData = dataMem[Addr];

    always @(negedge clk) begin     // 在时钟下降沿完成数据写入
        if(memWriteEn) begin
            dataMem[Addr] <= WriteData;
        end
    end
    
endmodule
