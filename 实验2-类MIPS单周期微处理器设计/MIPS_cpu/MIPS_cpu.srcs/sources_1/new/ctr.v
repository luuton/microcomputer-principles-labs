`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2026/05/07 21:02:28
// Design Name: 
// Module Name: ctr
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


module ctr(
    input [31:0] instr,
    output reg regDst,
    output reg aluSrc,
    output reg memToReg,
    output reg regWriteEn,
    output reg memReadEn,
    output reg memWriteEn,
    output reg [3:0] aluCtr,
    output reg branch,
    output reg jump,
    output [11:0] ctr_varl  //for debug
    );
    wire [5:0] opCode, funct;
    //wire [11:0] ctr_varl;

    assign funct = instr[5:0];
    assign opCode = instr[31:26];
    assign ctr_varl = {opCode, funct}; 
    
    always @(ctr_varl) begin
        // casex (ctr_varl)
        //     12'b000000_xxxxxx: // R-type
        case (opCode)
        // R-type÷∏¡Ó
            6'b000000: 
            begin
                regDst = 1;
                aluSrc = 0;
                memToReg = 0;
                regWriteEn = 1;
                memReadEn = 0;
                memWriteEn = 0;
                branch = 0;
                jump = 0;
                case (funct)
                    6'b100000:
                        aluCtr = 4'b0001; // add
                    6'b100010:
                        aluCtr = 4'b0010; // sub
                    6'b100100:
                        aluCtr = 4'b0011; // and
                    6'b100101:
                        aluCtr = 4'b0100; // or
                    6'b101010:
                        aluCtr = 4'b0101; // slt
                    default: 
                        aluCtr = 4'b0000; 
                endcase
            end
        // J-type÷∏¡Ó
            6'b000010: // j
            begin
                regDst = 1'bx; 
                aluSrc = 1'bx; 
                memToReg = 1'bx; 
                regWriteEn = 0;
                memReadEn = 0;
                memWriteEn = 0;
                aluCtr = 4'bx; 
                branch = 0;
                jump = 1;
            end
        // I-type÷∏¡Ó
            6'b100011: // lw
            begin
                regDst = 0;
                aluSrc = 1;
                memToReg = 1;
                regWriteEn = 1;
                memReadEn = 1;
                memWriteEn = 0;
                aluCtr = 4'b0001; // +
                branch = 0;
                jump = 0;
            end
            6'b101011: // sw
            begin
                regDst = 1'bx;
                aluSrc = 1;
                memToReg = 1'bx;
                regWriteEn = 0;
                memReadEn = 0;
                memWriteEn = 1;
                aluCtr = 4'b0001; // +
                branch = 0;
                jump = 0;
            end
            6'b000100: // beq
            begin
                regDst = 1'bx;
                aluSrc = 0;
                memToReg = 1'bx;
                regWriteEn = 0;
                memReadEn = 0;
                memWriteEn = 0;
                aluCtr = 4'b0010; // -
                branch = 1;
                jump = 0;
            end
            6'b001000: // addi
            begin
                regDst = 0;
                aluSrc = 1;
                memToReg = 0;
                regWriteEn = 1;
                memReadEn = 0;
                memWriteEn = 0;
                aluCtr = 4'b0001; // +
                branch = 0;
                jump = 0;
            end
            6'b001101: // ori
            begin
                regDst = 0;
                aluSrc = 1;
                memToReg = 0;
                regWriteEn = 1;
                memReadEn = 0;
                memWriteEn = 0;
                aluCtr = 4'b0100; // or
                branch = 0;
                jump = 0;
            end
            default:
            begin
                regDst = 1'bx; 
                aluSrc = 1'bx; 
                memToReg = 1'bx; 
                regWriteEn = 1'bx; 
                memReadEn = 1'bx; 
                memWriteEn = 1'bx; 
                aluCtr = 4'bx; 
                branch = 1'bx; 
                jump = 1'bx; 
            end
        endcase
    end
endmodule
