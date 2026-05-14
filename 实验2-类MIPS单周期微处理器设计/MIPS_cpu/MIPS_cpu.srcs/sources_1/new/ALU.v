`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2026/05/07 19:39:16
// Design Name: 
// Module Name: ALU
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


module ALU(
    input [31:0] input1,
    input [31:0] input2,
    input [3:0] aluCtr,

    output reg [31:0] aluRes,
    output reg zero
    );

    always @(input1 or input2 or aluCtr) begin
        case (aluCtr)
        // +
            4'b0001:
            begin
                aluRes = input1 + input2;
                zero = 0;
            end
        // -
            4'b0010:
            begin
                aluRes = input1 - input2;
                if (aluRes == 0) begin
                    zero = 1;
                end else begin
                    zero = 0;
                end
            end
        // &
            4'b0011:
            begin
                aluRes = input1 & input2;
                zero = 0;
            end
        // |
            4'b0100:
            begin
                aluRes = input1 | input2;
                zero = 0;
            end
        // slt
            4'b0101:
            begin
                aluRes = (input1 < input2) ? 1 : 0;
                zero = 0;
            end
            default: 
            begin
                aluRes = 0;
                zero = 0;
            end
        endcase
    end
endmodule
