`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2026/05/07 19:22:13
// Design Name: 
// Module Name: regFile
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


module regFile(
    input clk,
    input reset,
    input regWriteEn,
    input [31:0] regWriteData,
    input [4:0] RsAddr,
    input [4:0] RtAddr,
    input [4:0] regWriteAddr,
    
    output [31:0] RsData,
    output [31:0] RtData
    );
    
    reg [31:0]  regs [0:31];
    assign RsData = (RsAddr == 5'b0) ? 32'b0 : regs[RsAddr];
    assign RtData = (RtAddr == 5'b0) ? 32'b0 : regs[RtAddr];

    integer reg_i;

    always @(negedge clk) begin     // ÔÚÊ±ÖÓÏÂ½µÑØÍê³É¼Ä´æÆ÷Ğ´Èë
        if (!reset) begin
            for (reg_i = 0; reg_i < 32; reg_i = reg_i + 1) begin
                regs[reg_i] <= reg_i*4; // ³õÊ¼»¯¼Ä´æÆ÷Îª¼Ä´æÆ÷±àºÅ³Ë4
            end
        end else if (regWriteEn && regWriteAddr != 5'b0) begin
            regs[regWriteAddr] <= regWriteData;
        end
    end

endmodule
