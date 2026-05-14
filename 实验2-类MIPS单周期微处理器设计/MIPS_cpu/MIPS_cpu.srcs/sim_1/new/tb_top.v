`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2026/05/14 20:15:39
// Design Name: 
// Module Name: tb_top
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


module tb_top(

    );

    reg clk;
    reg reset;

    top top(
        .clk(clk),
        .reset(reset)
    );

    parameter period = 10;
    always #(period/2) clk = ~clk;


    initial begin
        clk = 0;
        reset = 0;
        
        #10; // 等待一个时钟周期
        reset = 1; // 释放复位信号，CPU开始工作
        #200; $finish;
        
    end
endmodule
