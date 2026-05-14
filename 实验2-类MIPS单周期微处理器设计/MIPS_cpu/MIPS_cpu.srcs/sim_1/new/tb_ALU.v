`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2026/05/07 20:08:20
// Design Name: 
// Module Name: tb_ALU
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


module tb_ALU(
    output [31:0] aluRes,
    output zero
    );
    reg [31:0] input1;
    reg [31:0] input2;
    reg [3:0] aluCtr;
    ALU uut (
        .input1(input1),
        .input2(input2),
        .aluCtr(aluCtr),
        .aluRes(aluRes),
        .zero(zero)
    );

    initial begin
        input1 = 32'h0000_0005;
        input2 = 32'h0000_0003;
        aluCtr = 4'b0001; // +
        #20;
        aluCtr = 4'b0010; // -
        #20;
        aluCtr = 4'b0011; // &
        #20;
        aluCtr = 4'b0100; // |
        #20;
        aluCtr = 4'b0101; // slt
        #20;
        // ≤‚ ‘zero–≈∫≈
        input1 = 32'h0000_0003;
        input2 = 32'h0000_0005;
        aluCtr = 4'b0010; 
        #20 $finish;

    end

endmodule
