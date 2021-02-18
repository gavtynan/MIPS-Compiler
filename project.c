#include "spimcore.h"

/* project.c
 * Ryan Black, Erik Schreifer, and Gavin Tynan
 * This program acts as a MIPS simulator that something something
 *
 *
 */

/* ALU */
/* 10 Points */
/* The Arithmetic Logic Unit (ALU) takes in the values two registers (A & B) and
 * and performs arithmetic and bitwise operations on these registers. */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
  // Initialize zero to zero then change it as needed.
  *Zero = 0;
  // Perform the operation based on ALUControl input.
  if(ALUControl == 0)
    *ALUresult = A + B;
  else if(ALUControl == 1)
    *ALUresult = A - B;
  else if(ALUControl == 2) {
    if((signed)A < (signed)B)
      *ALUresult = 1;
    else{
      *ALUresult = 0; // Here the result is zero so we change zero to 1.
      *Zero = 1;
    }
  }
  else if(ALUControl == 3) {
    if(A < B)
      *ALUresult = 1;
    else {
      *ALUresult = 0; //Change zero to one because the result is 1.
      *Zero = 1;
    }
  }
  else if(ALUControl == 4)
      *ALUresult = A & B;
  else if(ALUControl == 5)
      *ALUresult = A | B;
  else if(ALUControl == 6)
      *ALUresult = B << 16;
  else
       *ALUresult = !A;

}

/* instruction fetch */
/* 10 points */
/* The instruction fetch is the first stage in a MIPS processor where an instruction
 * from memory is loaded from the address pointed to by the program counter (PC). Each
 * instruction is read in as a 32-bit number. */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{

  // Checks if the instruction is word-aligned, if not a HALT condition has occurred.
  if (PC % 4 == 0) {
    /* PC is the current index of of the address in memory, and to access the next
     * instruction from memory, shift PC to the right by two bits. */
    *instruction = Mem[PC >> 2];
    return 0;
  }
  return 1;
  
}
 

/* instruction partition */
/* 10 Points */
/* After a 32-bit instruction is read from memory, each part of the instruction is partitioned
 * into its respective parts (op, r1, r2 , r3, function, offset, and jsec)  */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{

 /* op (opcode) from instruction bits 31-26
 * r1 (source operand register 1) from instruction bits 25-21
 * r2 (source operand register 2) from in 20-16
 * r3 in 15-11 (destination register) from instruction bits 15-11
 * funct (r-type instruction) from instruction bits 5-0
 * offset (offset amount) from instruction bits  15-0
 * jsec (address to jump to) from instruction bits 25-0  */
    *op = instruction >> 26;
    *r1 = instruction >> 21 & 0x1F;
    *r2 = instruction >> 16 & 0x1F;
    *r3 = instruction >> 11 & 0x1F;
    *funct = instruction & 0x3F;
    *offset = instruction & 0xFFFF;
    *jsec = instruction & 0x3FFFFFF;

}



/* instruction decode */
/* 15 Points */
/* The instruction decode function decodes an instruction based on its opcode then
 * assigns the values of the control signals based on a given operation.
 *
 * Control signals
 * ---------------
 * MemRead - reads from memory (0 disabled 1 enabled 2 don't care)
 * MemWrite - writes to memory (0 disabled 1 enabled 2 don't care)
 * RegWrite - writes to a register (0 disabled 1 enabled 2 don't care)
 * RegDst - register destination (0 for i-type / 1 for r-type)
 * Jump - path of multiplexer (0,1 or 2 dont care)
 * Branch - path of multiplexer (0,1 or 2 dont care)
 * MemtoReg -path of multiplexer (0,1 or 2 dont care)
 * ALUSrc -path of multiplexer (0,1 or 2 dont care)
 * ALUOp - Instruction sent to the Arithmetic Logic Unit (ALU)  */
int instruction_decode(unsigned op,struct_controls *controls)
{
    int halt_true = 0;

    switch(op)
    {
        // Add, Sub, And, Or, Slt, Sltu
        case 0:
            controls->ALUOp = 7;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->RegWrite = 1;
            controls->RegDst = 1;
            controls->Jump = 2;
            controls->Branch = 2;
            controls->MemtoReg = 0;
            controls->ALUSrc = 0;
            break;
        // Lw
        case 35:
            controls->ALUOp = 0;
            controls->MemRead = 1;
            controls->MemWrite = 0;
            controls->RegWrite = 1;
            controls->RegDst = 0;
            controls->Jump = 2;
            controls->Branch = 2;
            controls->MemtoReg = 1;
            controls->ALUSrc = 1;
            break;
        // Sw
        case 43:
            controls->ALUOp = 0;
            controls->MemRead = 0;
            controls->MemWrite = 1;
            controls->RegWrite = 0;
            controls->RegDst = 0;
            controls->Jump = 2;
            controls->Branch = 2;
            controls->MemtoReg = 1;
            controls->ALUSrc = 1;
            break;
        // Beq
        case 4:
            controls->ALUOp = 1;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->RegWrite = 0;
            controls->RegDst = 0;
            controls->Jump = 2;
            controls->Branch = 1;
            controls->MemtoReg = 0;
            controls->ALUSrc = 1;
            break;
        // Addi
        case 8:
            controls->ALUOp = 0;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->RegWrite = 1;
            controls->RegDst = 0;
            controls->Jump = 2;
            controls->Branch = 2;
            controls->MemtoReg = 0;
            controls->ALUSrc = 1;
            break;
        // Lui
        case 15:
            controls->ALUOp = 6;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->RegWrite = 1;
            controls->RegDst = 0;
            controls->Jump = 2;
            controls->Branch = 2;
            controls->MemtoReg = 0;
            controls->ALUSrc = 1;
            break;
        // Slti
        case 10:
            controls->ALUOp = 0;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->RegWrite = 1;
            controls->RegDst = 0;
            controls->Jump = 2;
            controls->Branch = 2;
            controls->MemtoReg = 0;
            controls->ALUSrc = 1;
            break;
        // Sltiu
        case 9:
            controls->ALUOp = 0;
            controls->MemRead = 0;
            controls->MemWrite = 0;
            controls->RegWrite = 1;
            controls->RegDst = 0;
            controls->Jump = 2;
            controls->Branch = 2;
            controls->MemtoReg = 0;
            controls->ALUSrc = 1;
            break;
        // J
        case 2:
            controls->ALUOp = 0;
            controls->MemRead = 2;
            controls->MemWrite = 2;
            controls->RegWrite = 2;
            controls->RegDst = 2;
            controls->Jump = 1;
            controls->Branch = 2;
            controls->MemtoReg = 2;
            controls->ALUSrc = 0;
            break;
        default:
            halt_true = 1;
    }

    return halt_true;
}

/* Read Register */
/* 5 Points */
/* This function reads in the registers passed in (r1 & r2) and writes their values to variables data1 and data2 respectively. */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}


/* Sign Extend */
/* Examine offset, if positive then extend normally to 32 bit, if negative then extend to 32 bit for negative */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
  unsigned Negative = offset >> 15;

    if(Negative == 1)
        *extended_value = offset | 0xffff0000;
    else
        *extended_value = offset & 0x0000ffff;
}

/* ALU operations */
/* Here, we will be looking at the ALUSrc control. This is to determine whether you need to use the extended offset or r2. If the ALUSrc is 0, you use r1. If it is 1, you use the extended offset. */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    int retval = 0;

    switch(ALUOp)
    {
        case 0:
                if(ALUSrc == 1)    //beq
                    ALU(data1, extended_value, ALUOp, ALUresult, Zero);
                else
                    ALU(data1, data2, ALUOp, ALUresult, Zero);
                return retval;
        case 1:
                if(ALUSrc == 1)
                    ALU(data1, extended_value, ALUOp, ALUresult, Zero);
                else
                    ALU(data1, data2, ALUOp, ALUresult, Zero);
                return retval;
        case 2:
                if(ALUSrc == 1)
                    ALU(data1, extended_value, ALUOp, ALUresult, Zero);
                else
                    ALU(data1, data2, ALUOp, ALUresult, Zero);
                return retval;
        case 3:
                if(ALUSrc == 1)
                    ALU(data1, extended_value, ALUOp, ALUresult, Zero);
                else
                    ALU(data1, data2, ALUOp, ALUresult, Zero);
                return retval;
        case 4:
                if(ALUSrc == 1)
                    ALU(data1, extended_value, ALUOp, ALUresult, Zero);
                else
                    ALU(data1, data2, ALUOp, ALUresult, Zero);
                return retval;
        case 5:
                if(ALUSrc == 1)
                    ALU(data1, extended_value, ALUOp, ALUresult, Zero);
                else
                    ALU(data1, data2, ALUOp, ALUresult, Zero);
                return retval;
        case 6:
                if(ALUSrc == 1)
                    ALU(data1, extended_value, ALUOp, ALUresult, Zero);
                else
                    ALU(data1, data2, ALUOp, ALUresult, Zero);
                return retval;
        case 7:
                if(funct == 32) //add
                {
                    ALUOp = 0;
                    if(ALUSrc == 1)
                        ALU(data1, extended_value, ALUOp, ALUresult, Zero);
                    else
                        ALU(data1, data2, ALUOp, ALUresult, Zero);
                    return retval;
                }
                else if(funct == 36)//and
                {
                    ALUOp = 4;
                    if(ALUSrc == 1)
                        ALU(data1, extended_value, ALUOp, ALUresult, Zero);
                    else
                        ALU(data1, data2, ALUOp, ALUresult, Zero);
                    return retval;
                }
                else if(funct == 37)//or
                {
                    ALUOp = 5;
                    if(ALUSrc == 1)
                        ALU(data1, extended_value, ALUOp, ALUresult, Zero);
                    else
                        ALU(data1, data2, ALUOp, ALUresult, Zero);
                    return retval;
                }
                else if(funct == 42)//slt
                {
                    ALUOp = 2;
                    if(ALUSrc == 1)
                        ALU(data1, extended_value, ALUOp, ALUresult, Zero);
                    else
                        ALU(data1, data2, ALUOp, ALUresult, Zero);
                    return retval;
                }
                else if (funct == 43)//sltu
                {
                    ALUOp = 3;
                    if (ALUSrc == 1)
                        ALU(data1, extended_value,ALUOp, ALUresult, Zero);
                    else
                        ALU(data1, data2, ALUOp, ALUresult, Zero);
                    return retval;
                }
                else if(funct == 34)//sub
                {
                    ALUOp = 1;
                    if(ALUSrc == 1)
                        ALU(data1, extended_value, ALUOp, ALUresult, Zero);
                    else
                        ALU(data1, data2, ALUOp, ALUresult, Zero);
                    return retval;
                }
                break;
        default:
            retval = 1;
    }

    return retval;
}

/* Read / Write Memory */
/* In this function we will check whether we will be reading or writing to memory. When you read from memory, you read the address into the register. When you write to memory, you write a value gotten from the ALU. */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    unsigned temp = ALUresult % 4;

    if (MemRead == 1)
    {
        if (temp != 0)
            return 1;
        *memdata = Mem[ALUresult / 4];
    }

    if (MemWrite == 1)
    {
        if (temp != 0)
            return 1;
        Mem[ALUresult / 4] = data2;
    }

    return 0;
}


/* Write Register */
/* Check the RegWrite control signal to see if writing to register is allowed. If so, then check whether the writing will be from memory or ALU result. If RegDst signal is 0, then writing will be to r2, if it is 1, then writing will be to r3 */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    if (RegWrite == '1')
    {
        if (MemtoReg == '0')
        {
            if (RegDst == '0')
                Reg[r2] = ALUresult;
            else if (RegDst == '1')
                Reg[r3] = ALUresult;
        }
        else if (MemtoReg == '1')
        {
            if (RegDst == '0')
                Reg[r2] = memdata;
            else if (RegDst == '1')
                Reg[r3] = memdata;
        }
    }
}

/* PC update */
/* In this section, we will check the control signals in order to see if we need to update the counter to something other than the normal 4. */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    unsigned tempPC = *PC + 4, newJsec = tempPC;

    extended_value <<= 2;
    jsec <<= 2;
    newJsec >>= 28;
    newJsec <<= 28;

    newJsec += jsec;

    if (Branch == 2 && Jump == 2)
        *PC = tempPC;
    else if (Branch == 1 && Zero == 1)
    {
        *PC += extended_value;
        
        if (Jump == 0)
            return;
    }
    else if (Branch == 0 || Zero == 0)
    {
        *PC = tempPC;
        return;
    }


    if (Jump == 1)
        *PC = newJsec;
}
