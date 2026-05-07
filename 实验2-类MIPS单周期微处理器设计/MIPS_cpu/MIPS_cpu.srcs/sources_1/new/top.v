`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2026/05/07 21:26:28
// Design Name: 
// Module Name: top
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


module top(
    input clk,
    input reset
    );

    wire [31:0] instr;
    wire [31:0] pAddr;
    wire [4:0] RdAddr;
    wire [31:0] RsData;
    wire [31:0] RtData;
    wire regWriteEn;
    wire [31:0] regWriteData;
    wire zero;
    wire ctr_regDst, ctr_aluSrc, ctr_memToReg, ctr_memReadEn, ctr_memWriteEn, ctr_branch, ctr_jump;
    wire [3:0] ctr_aluCtr;
    wire [31:0] memReadData;
    irom instMem(
        .clk(clk),
        .addr(pAddr[6:0]),
        .inst(instr)
    );

    assign RdAddr = ctr_regDst ? instr[15:11] : instr[20:16]; // R-type指令写入Rd，I-type指令写入Rt
    // MUX_2to1 #(.WIDTH(5)) regDstMux(
    //     .sel(ctr_regDst),
    //     .in0(instr[20:16]), // Rt
    //     .in1(instr[15:11]), // Rd
    //     .out(RdAddr)
    // );

    regFile regFile(
        .clk(clk),
        .reset(reset),
        .regWriteEn(regWriteEn),
        .regWriteData(regWriteData),
        .RsAddr(instr[25:21]),
        .RtAddr(instr[20:16]),
        .regWriteAddr(RdAddr),
        .RsData(RsData),
        .RtData(RtData)
    );

    PC pc(
        .clk(clk),
        .reset(reset),
        .jump(ctr_jump),
        .branch(ctr_branch),
        .zero(zero),
        .instr(instr),
        .PAddr(pAddr)
    );

    ctr controlUnit(
        .instr(instr),
        .regDst(ctr_regDst),
        .aluSrc(ctr_aluSrc),
        .memToReg(ctr_memToReg),
        .regWriteEn(regWriteEn),
        .memReadEn(ctr_memReadEn),
        .memWriteEn(ctr_memWriteEn),
        .aluCtr(ctr_aluCtr),
        .branch(ctr_branch),
        .jump(ctr_jump)
    );
    wire [31:0] alu_in2;
    assign alu_in2 = ctr_aluSrc ? (instr[15] ? {16'hFFFF, instr[15:0]} : {16'h0000, instr[15:0]}) : RtData; // 立即数需要符号扩展
    // MUX_2to1 #(.WIDTH(32)) aluSrcMux(
    //     .sel(ctr_aluSrc),
    //     .in0(RtData),
    //     .in1(instr[15:15] ? {16'hffff,instr[15:0]} : {16'h0000,instr[15:0]}), // 立即数需要符号扩展
    //     .out(alu_in2)
    // );
    wire [31:0] aluRes;
    ALU alu(
        .input1(RsData),
        .input2(alu_in2),
        .aluCtr(ctr_aluCtr),
        .aluRes(aluRes),
        .zero(zero)
    );
    wire [31:0] inData;
    Dram dataMem(
        .clk(clk),
        .memWriteEn(ctr_memWriteEn),
        .WriteData(RtData),
        .Addr(aluRes[7:2]),
        .ReadData(inData)
    );
    assign regWriteData = ctr_memToReg ? inData : aluRes; // lw指令从内存读取数据写回寄存器，其他指令写回ALU结果
    // MUX_2to1 #(.WIDTH(32)) memToRegMux(
    //     .sel(ctr_memToReg),
    //     .in0(inData),
    //     .in1(aluRes),
    //     .out(regWriteData)
    // );

endmodule
