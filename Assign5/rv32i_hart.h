//********************************************************************************************************************************
//
//  Brayan Chacha Gonzalez
//  z1861700
//  CSCI 463, Professir John Winans 
//
//  I certify that this is my own work and where appropriate an extension 
//  of the starter code provided for the assignment.
//
//  File: rv32i_hart.h                                         				   Revised Final Copy Tue, 23 Mar 2021 16:58:10 +0000
//********************************************************************************************************************************
#ifndef RV32I_HART_H
#define RV32I_HART_H

#include <vector>
#include <fstream>
#include <iostream>
#include <string>

#include "rv32i_decode.h"
#include "registerfile.h"

/**
 * @note This class is a subclass of rv32i_decode to represent the execution unit of RV32i hart 
 ***********************************************************************************************/
class rv32i_hart : public rv32i_decode
{
    public: 
        rv32i_hart (memory &m) : mem (m)            { }
        void set_show_instructions (bool b)         { show_instructions = b ; }
        void set_show_registers (bool b)            { show_registers = b; }
        bool is_halted () const                     { return halt; }
        const std::string & get_halt_reason() const { return halt_reason; }
        uint64_t get_insn_counter() const           { return insn_counter; }
        void set_mhartid(int i)                     { mhartid = i; }

        void tick (const std::string &hdr="");
        void dump (const std::string &hdr="") const;
        void reset ();
    
    private:
        static constexpr int instruction_width              = 35;

	    ///@parm functions that will be implemented in rv32i_hart.cpp
        void exec(uint32_t insn , std::ostream *);
        void exec_illegal_insn(uint32_t insn , std :: ostream *);
        void exec_ecall (std::ostream* pos);
        void exec_ebreak(std::ostream* pos);

        void exec_lui       (uint32_t insn, std::ostream* pos);
        void exec_auipc     (uint32_t insn, std::ostream* pos);
        void exec_jal       (uint32_t insn, std::ostream* pos);
        void exec_jalr      (uint32_t insn, std::ostream* pos);

        void exec_csrrs     (uint32_t insn, std::ostream* pos);

        void exec_beq       (uint32_t insn, std::ostream* pos);
        void exec_bne       (uint32_t insn, std::ostream* pos);
        void exec_blt       (uint32_t insn, std::ostream* pos);
        void exec_bge       (uint32_t insn, std::ostream* pos);
        void exec_bltu      (uint32_t insn, std::ostream* pos);
        void exec_bgeu      (uint32_t insn, std::ostream* pos);

        void exec_lb        (uint32_t insn, std::ostream* pos);
        void exec_lh        (uint32_t insn, std::ostream* pos);
        void exec_lw        (uint32_t insn, std::ostream* pos);
        void exec_lbu       (uint32_t insn, std::ostream* pos);
        void exec_lhu       (uint32_t insn, std::ostream* pos);

        void exec_sb        (uint32_t insn, std::ostream* pos);
        void exec_sh        (uint32_t insn, std::ostream* pos);
        void exec_sw        (uint32_t insn, std::ostream* pos);

        void exec_addi      (uint32_t insn, std::ostream* pos);
        void exec_slti      (uint32_t insn, std::ostream* pos);
        void exec_sltiu     (uint32_t insn, std::ostream* pos);
        void exec_xori      (uint32_t insn, std::ostream* pos);
        void exec_ori       (uint32_t insn, std::ostream* pos);
        void exec_slli      (uint32_t insn, std::ostream* pos);
        void exec_andi      (uint32_t insn, std::ostream* pos);

        void exec_srli      (uint32_t insn, std::ostream* pos);
        void exec_srai      (uint32_t insn, std::ostream* pos);

        void exec_add       (uint32_t insn, std::ostream* pos);
        void exec_sub       (uint32_t insn, std::ostream* pos);

        void exec_sll       (uint32_t insn, std::ostream* pos);
        void exec_slt       (uint32_t insn, std::ostream* pos);
        void exec_sltu      (uint32_t insn, std::ostream* pos);
        void exec_xor       (uint32_t insn, std::ostream* pos);
        void exec_and       (uint32_t insn, std::ostream* pos);
        void exec_or        (uint32_t insn, std::ostream* pos);

        void exec_srl       (uint32_t insn, std::ostream* pos);
        void exec_sra       (uint32_t insn, std::ostream* pos);
  
    protected:
        registerfile regs;
        memory &mem;

        bool halt = { false };
        std::string halt_reason = { " none " };

        bool show_registers = { false };
        bool show_instructions = { false };

        uint32_t pc = { 0 };
        uint32_t mhartid = { 0 };
        uint64_t insn_counter = { 0 };

};// End rv32i_hart 
#endif