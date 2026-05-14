`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2026/05/14 13:11:07
// Design Name: 
// Module Name: tb_ctr
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


module tb_ctr(
    output [3:0] aluCtr,
    output regDst,
    output aluSrc,
    output memToReg,
    output regWriteEn,
    output memReadEn,
    output memWriteEn,
    output branch,
    output jump,
    output [11:0] ctr_varl
    );
    reg [31:0] instr;

    initial begin
        // R-type add
        instr = 32'b000000_00001_00010_00011_00000_100000; // add $3, $1, $2
        #10;
        // R-type sub
        instr = 32'b000000_00001_00010_00100_00000_100010; // sub $4, $1, $2
        #10;
        // R-type and
        instr = 32'b000000_00001_00010_00101_00000_100100; // and $5, $1, $2
        #10;
        // R-type or
        instr = 32'b000000_00001_00010_00110_00000_100101; // or $6, $1, $2
        #10;
        // R-type slt
        instr = 32'b000000_00001_00010_00111_00000_101010; // slt $7, $1, $2
        #10;
        // I-type lw
        instr = 32'b100011_00001_00010_0000000000000100; // lw $2, 4($1)
        #10;
        // I-type sw
        instr = 32'b101011_00001_00010_0000000000000100; // sw $2, 4($1)
        #10;
        // I-type beq
        instr = 32'b000100_00001_00010_0000000000000100; // beq $1, $2, 4
        #10;
        // J-type jump
        instr = 32'b000010_00000000000000000000000010; // j 2
        #10 $finish;
    end

    ctr control_unit (
        .instr(instr),
        .regDst(regDst),
        .aluSrc(aluSrc),
        .memToReg(memToReg),
        .regWriteEn(regWriteEn),
        .memReadEn(memReadEn),
        .memWriteEn(memWriteEn),
        .aluCtr(aluCtr),
        .branch(branch),
        .jump(jump),
        .ctr_varl(ctr_varl)
    );
endmodule
