`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2026/05/14 13:12:38
// Design Name: 
// Module Name: tb_dram
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


module tb_dram(
    output [31:0] ReadData
    );
    reg clk;
    reg memWriteEn;
    reg memReadEn;
    reg [31:0] memWriteData;
    reg [7:0] addr;

    Dram dram(
        .clk(clk),
        .memWriteEn(memWriteEn),
        .memReadEn(memReadEn),
        .WriteData(memWriteData),
        .Addr(addr),
        .ReadData(ReadData)
    );

    parameter period = 10;
    always #(period/2) clk = ~clk;

    initial begin
        clk = 0;
        memWriteEn = 0;
        memReadEn = 0;
        addr = 0;
        #20;
        // 向地址0写入0x12345678
        memWriteEn = 1;
        memWriteData = 32'h12345678;
        addr = 0;
        #20;
        // 向地址1写入0xabcdef01
        memWriteData = 32'habcd_ef01;
        addr = 4;
        #20;
        // 读取地址0和地址1的数据
        memWriteEn = 0;
        memReadEn = 1;
        addr = 0;
        #20;
        addr = 4;
        #20 $finish;
    end
endmodule
