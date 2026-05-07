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
    output reg jump
    );
    wire [5:0] opCode, funct;
    wire [11:0] ctr_varl;

    assign opCode = instr[31:26];
    assign funct = instr[5:0];
    assign ctr_varl = {opCode, funct};
    
    always @(ctr_varl) begin
        case (ctr_varl)
        // R-type÷∏¡Ó
            12'b000000_xxxxxx: 
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
                    100000:
                        aluCtr = 4'b0001; // add
                    100010:
                        aluCtr = 4'b0010; // sub
                    100100:
                        aluCtr = 4'b0011; // and
                    100101:
                        aluCtr = 4'b0100; // or
                    101010:
                        aluCtr = 4'b0101; // slt
                    default: 
                        aluCtr = 4'b0000; 
                endcase
            end
        // J-type÷∏¡Ó
            12'b000010_xxxxxx: // j
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
            12'b100011_xxxxxx: // lw
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
            12'b101011_xxxxxx: // sw
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
            12'b000100_xxxxxx: // beq
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
            12'b001000_xxxxxx: // addi
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
            12'b001101_xxxxxx: // ori
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
