`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2026/05/07 19:40:13
// Design Name: 
// Module Name: tb_regFile
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


module tb_regFile(
    output [31:0] RsData,
    output [31:0] RtData
    );
    reg clk;
    reg reset;
    reg regWriteEn;
    reg [31:0] regWriteData;
    reg [4:0] RsAddr;
    reg [4:0] RtAddr;
    reg [4:0] regWriteAddr;
    
    regFile uut (
        .clk(clk),
        .reset(reset),
        .regWriteEn(regWriteEn),
        .regWriteData(regWriteData),
        .RsAddr(RsAddr),
        .RtAddr(RtAddr),
        .regWriteAddr(regWriteAddr),
        .RsData(RsData),
        .RtData(RtData)
    );

    parameter period = 10;
    always #(period/2) clk = ~clk;

    initial begin
        // RstAddr&RtAddr = 0
        clk = 0;
        reset = 1;
        RsAddr = 0;
        RtAddr = 0;
        #20;
        reset = 0;
        #15;
        //向寄存器02写入0xab3c
        regWriteEn = 1;
        regWriteData = 32'hab3c;
        regWriteAddr = 5'h02;
        #20;
        //读出寄存器02的值
        regWriteEn = 0;
        RsAddr = 5'h02;
        RtAddr = 5'h02;
        #20 $finish;
    end

endmodule
