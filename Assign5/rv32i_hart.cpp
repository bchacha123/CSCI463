//********************************************************************************************************************************
//
//  Brayan Chacha Gonzalez
//  z1861700
//  CSCI 463, Professir John Winans 
//
//  I certify that this is my own work and where appropriate an extension 
//  of the starter code provided for the assignment.
//
//  File: rv32i_hart.h                                         							    Revised Final Copy Tue, 23 Mar 2021 20:09:15 +0000
//
//********************************************************************************************************************************
#include "hex.h"
#include "memory.h"
#include "rv32i_hart.h"
#include "registerfile.h"	 

#include <vector>
#include <iomanip>      
#include <cassert>
#include <sstream>
#include <fstream>
#include <iostream>     

typedef hex H;

using std::cout;
using std::setw;
using std::string;

/**
 * @note This is a software decoder. The switch statement below has every instruction necessary needed  
 *		   for this assignment and the decoder based on the rv32i. 
 *
 * @param insn Which instruction will be executed
 * @param pos  any possible instruction.
 *
 * @return Will return the render/execution instruction based on the instruction that was fetched. 
 ********************************************************************************************************************************/
void rv32i_hart::exec(uint32_t insn, std::ostream *pos)
{

	switch(get_opcode(insn))
	{
    // Load Upper Immediate 
		case opcode_lui:   exec_lui(insn,pos);   return;

		// Add Upper Immediate to PC
		case opcode_auipc: exec_auipc(insn,pos); return;

		// Jump and Link
		case opcode_jal:   exec_jal(insn,pos);   return;

		// Jump and Link Register
    case opcode_jalr:  exec_jalr(insn,pos);  return;

		// This case/switch statement has the system calls
    case opcode_system:
			switch(get_funct3(insn))
			{
				case funct3_beq:
					switch(insn)
          {
						case insn_ecall:      exec_ecall(pos);             return;
						case insn_ebreak:     exec_ebreak(pos);            return;
						default:	            exec_illegal_insn(insn,pos); return;
				  } break;

        case funct3_csrrs:        exec_csrrs(insn,pos);        return;
				default:	        		    exec_illegal_insn(insn,pos); return;

			} break;


		// This case/switch statement is based on the B-Type instruction placing the appropiate funct3 returing its insruction execution. 
		// B-Type Instruction:  Are used to branch instructions that require an even immediate 
		// 						          value that is used to determine the branch target addr as on offset
		// 						          from the current instruction's addr. 
		//
		//		   B-Type Insts:  beq, bne, blt, bge, bltu, bgeu.
		case opcode_btype:
			switch(get_funct3(insn))
			{
				case funct3_beq:	  exec_beq(insn, pos);         return;
				case funct3_bne:    exec_bne(insn, pos);         return;
				case funct3_blt:	  exec_blt(insn, pos);         return;
				case funct3_bge:	  exec_bge(insn, pos);         return;
				case funct3_bltu: 	exec_bltu(insn, pos);        return;
				case funct3_bgeu:	  exec_bgeu(insn, pos);        return;
				default:            exec_illegal_insn(insn,pos); return;
			} break;


		// This case/switch statement is based on the load instructions referencing the I-Type instructions, 
		// taking the appropiate funct3 returing its insruction execution.
		// I-Type Load Insts: lb, lh, lw, lbu, lhu
		case opcode_load_imm:
			switch(get_funct3(insn))
			{
				case funct3_lb:	    exec_lb(insn,pos);           return;
				case funct3_lh:     exec_lh(insn,pos);           return;
				case funct3_lw:	    exec_lw(insn,pos);           return;
				case funct3_lbu:	  exec_lbu(insn,pos);          return;
				case funct3_lhu: 	  exec_lhu(insn,pos);          return;
				default:            exec_illegal_insn(insn,pos); return;
			} break;


		// This case/switch statement will specifically execute s-type instruction and base on func3 of the given instruction, returing its execution. 
		// S-Type Instruction:  Are used to encode instructions with a signed 12-bit immediate operand with a range [-2048:2047], (rs1 & rs2).
		// 		   S-Type Insts:  sb, sh, sw	
		case opcode_stype:
			switch(get_funct3(insn))
			{
				case funct3_sb:	    exec_sb(insn,pos); return;
				case funct3_sh:     exec_sh(insn,pos); return;
				case funct3_sw:	    exec_sw(insn,pos); return;
				default: exec_illegal_insn(insn,pos);  return;
			} break;


		// This case/switch statement will be used for the i-type instruction and base on func3 of the given instruction, it will return its insruction execution.
		// I-Type Instruction:  Are used to encode instructions with a signed 12-bit immediatre operand with a range [-2048:2047], (rs1 & rs2).
		// 	  	 I-Type Insts:  addi, slti, sltiu, xori, ori, slli, andi.
		case opcode_alu_imm:
			switch(get_funct3(insn))
			{
				case funct3_add:	    exec_addi(insn, pos);  return;
				case funct3_slt:	    exec_slti(insn, pos);  return;
				case funct3_sltu:	    exec_sltiu(insn, pos); return;
				case funct3_xor:      exec_xori(insn, pos);  return;
				case funct3_or:	    	exec_ori(insn, pos);   return;
				case funct3_sll:	    exec_slli(insn, pos);  return;
				case funct3_and:	    exec_andi(insn, pos);  return;
				case funct3_srx:

					// This case/switch statement will specifically execute the i-type instruction and base on func7 of the given instruction, returing its insruction execution.
					// I-Type Insts: srli, srai.
					switch(get_funct7(insn))
					{
						case funct7_srl: exec_srli(insn,pos); return;
						case funct7_sra: exec_srai(insn,pos); return;
						default: exec_illegal_insn(insn,pos); return;
					} break;

				default: exec_illegal_insn(insn,pos); return;

			} break;


		// This case/switch statement will specifically execute the r-type instruction base on func3 of the given instruction, returning its instrctuion execution.
		// R-Type Instruction:	Are used for operations that set destination register rd to the result of an arithmeric, logical or shift
		//						          operation applied to source register rs1 & rs2.
		//		   I-Type Insts:  add, sub, sll, slt, sltu, xor, and, or 
		case opcode_rtype:
			switch(get_funct3(insn))
			{
				case funct3_add:	    
					switch(get_funct7(insn))
					{
						case funct7_add: exec_add(insn,pos);      return;
						case funct7_sub: exec_sub(insn,pos);      return;
						default: 		 exec_illegal_insn(insn,pos); return;
					} break;

				case funct3_sll:	    exec_sll(insn, pos);  return;
				case funct3_slt:      exec_slt(insn, pos);  return;
				case funct3_sltu:	    exec_sltu(insn, pos); return;
				case funct3_xor:	    exec_xor(insn, pos);  return;
				case funct3_and:	    exec_and(insn, pos);  return;
				case funct3_or:	      exec_or(insn, pos);   return;
				case funct3_srx:	    

					// This case/switch statement will specifically load the R-Type instruction and base on func7 of the given instruction. 
					switch(get_funct7(insn))
					{
						case funct7_srl: exec_srl(insn, pos); return;
						case funct7_sra: exec_sra(insn, pos); return;
						default:         exec_illegal_insn(insn,pos); return;
					} break;

				default: exec_illegal_insn(insn,pos); return;
			}

      break;
	}

}//End void rv32i_hart::exec cases statements

/**
 * @brief     This function tells the simulator to execute the given instrucitons. 
 *
 * @param hdr This paramater is needed to be print results on the left hand-side.
 *
 * @return    Will return the instruction being called with the appropiate formatting.
 *************************************************************************************/
void rv32i_hart::tick (const string &hdr) 
{
    // Incremening the counter
    insn_counter += 4;

    // Dumping the state of the hart 
    if(show_registers) regs.dump(""); 
    
    if(show_registers) std::cout << " pc " << H::to_hex32(pc) << std::endl; 

    uint32_t insn = mem.get32(pc);

    // Fetching the instructions from the memory at the address in the pc register 
    if(show_instructions)
    {
        std::cout << hdr << setw(8) << std::setfill('0') << std::right << H::to_hex32(pc) << ": " << H::to_hex32(insn) << "  ";
    
        exec(insn, &cout);

    }
    else
    {
        exec(insn, nullptr);
    }

}// End tick()


/**
 * @brief     This function will dump the registers. 
 *
 * @param hdr This paramater is a string that must be printed at the begging of the output lines.
 *
 * @return    Will return the the dump of the given memory.
 ************************************************************************************************/
void rv32i_hart::dump ( const string &hdr) const 
{
    regs.dump(hdr); 

    std::cout << " pc " << H::to_hex32(pc) << std::endl; 

}// End dump()


/**
 * @brief     This function will reset the RV32i object and the registerfile.
 ******************************************************************************/
void rv32i_hart::reset ()
{
    regs.reset();  

    // Setting pc to zero
    pc = 0;      

    // Setting insn_counter to zero
    insn_counter = 0;    

    // Setting its flag
    halt = false;  

}// End reset()


/**
 * @brief     This function will execute the ecall. 
 *
 * @param pos ostream* parameter 
 *****************************************************/
void rv32i_hart::exec_ecall(std::ostream* pos)
{
    if(pos)
    {
      //std::string s = render_ecall(0);
      std::string s = render_ecall(0);

      //s.resize(instruction_width, ' ');  

      //*pos << s << "// ECALL" << std::endl;
      *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s << "// ECALL" << std::endl;

    }
    halt = true;  

}// End exec_ecall()



/**
 * @brief     This function will execute the ebreak. 
 *
 * @param pos ostream* parameter 
 ****************************************************/
void rv32i_hart::exec_ebreak(std::ostream* pos)
{
  if (pos)
  {
    std::string s = render_ebreak(0);
    //std::string s = render_ebreak(0);

    *pos << std :: setw ( instruction_width ) << std :: setfill (' ') << std::left << s;

    *pos << "// HALT " << std::endl;

   //*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << std::setfill (' ') << std::left << s << "// HALT " << std::endl;
  
  }
  halt = true ;
  halt_reason = " EBREAK instruction ";

}// End ebreak()


/**
 * @brief     This function will execute the an illegal instruction. 
 *
 * @param pos ostream* parameter, output stream 
 *********************************************************************/
void rv32i_hart::exec_illegal_insn(uint32_t insn, std::ostream* pos)
{
  halt = true;

  halt_reason = " Illegal instruction ";       
 
  if(pos != nullptr)
  {
    std::string s = render_illegal_insn(insn);

    s.resize(instruction_width, ' ');     

    *pos << s << ": " << H::to_hex0x32(insn);
  }

}// End illegal_insn()



/**
 * @brief      This function will execute the lui instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 ********************************************************************************/
void rv32i_hart::exec_lui(uint32_t insn, std::ostream* pos)
{
    // Initializing rd and imm_u
    uint32_t rd = get_rd(insn);

    int32_t imm_u = get_imm_u(insn);

    // Assigning the register 
    regs.set(rd, imm_u);  

    // Render the details of lui
    if(pos)
    {
      std::string s = render_lui(insn);

      s.resize(instruction_width, ' ');  

      *pos << s << "// x" << rd << " = " << H::to_hex0x32(imm_u) << std::endl;
    }

    pc += 4; 

}// End lui()



/**
 * @brief      This function will execute the auipc instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory. 
 * @param pos  ostream* parameter, output stream 
 **************************************************************************************************/
void rv32i_hart::exec_auipc(uint32_t insn, std::ostream* pos)
{
  
   // Initializing rd and imm_u
   uint32_t rd = get_rd(insn);

   int32_t imm_u = get_imm_u(insn);

  // Assigning the register 
   regs.set(rd, pc + imm_u);    

  // Render the details of auipc
   if(pos)
   {
     std::string s = render_auipc(insn);

     s.resize(instruction_width, ' ');  

     *pos << s << "// x" << rd << " = " << H::to_hex0x32(pc) << " + " << H::to_hex0x32(imm_u) 
          << " = " << H::to_hex0x32(imm_u + pc) << std::endl;
   }

   pc += 4;    

}// End auipc()


/**
 * @brief      This function will execute the jal instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 ******************************************************************************************************/
void rv32i_hart::exec_jal(uint32_t insn, std::ostream* pos)
{
    // Initializing rd and imm_u
    uint32_t rd = get_rd(insn);

    int32_t imm_j = get_imm_j(insn);

    // Assigning the register 
    regs.set(rd, pc + 4);      

    // Render the details of jal
    if(pos)
    {
      std::string s = render_jal(pc,insn);

      s.resize(instruction_width, ' ');   

      *pos << s << "// x" << rd << " = " << H::to_hex0x32(pc + 4) << ",  pc = " << H::to_hex0x32(pc) 
           << " + " << H::to_hex0x32(imm_j) << " = " << H::to_hex0x32(pc + imm_j) << std::endl;
    }

    pc = pc + imm_j;   
}


/**
 * @brief      This function will execute the jalr instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 ******************************************************************************************************************/
void rv32i_hart::exec_jalr(uint32_t insn, std::ostream* pos)
{
    // Initializing rd, rs1, imm_i and val
    uint32_t rd = get_rd(insn);

    uint32_t rs1 = get_rs1(insn);

    int32_t imm_i = get_imm_i(insn);

    uint32_t val = (regs.get(rs1) + imm_i) & ~1;  
    
    // Render the details of jalr
    if(pos)
    {
      std::string s = render_jalr(insn);

      s.resize(instruction_width, ' ');   

      *pos << s << "// x" << rd << " = " << H::to_hex0x32(pc + 4)  << ",  pc = (" << H::to_hex0x32(imm_i) << " + " 
           << H::to_hex0x32(regs.get(rs1)) << ") & " << "0xfffffffe" << " = " << H::to_hex0x32(val) << std::endl;
    }

    // Assigning the register 
    regs.set(rd, pc + 4);    

    pc = val;       

}// End jalr()


/**
 * @brief      This function will execute the beq instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **********************************************************************************************************************/
void rv32i_hart::exec_beq(uint32_t insn, std::ostream* pos)
{
    // Initializing r1, rs2, imm_b and val
    uint32_t rs1 = get_rs1(insn);

    uint32_t rs2 = get_rs2(insn);

    int32_t imm_b = get_imm_b(insn);

    uint32_t val = (regs.get(rs1) == regs.get(rs2))? imm_b : 4;  

    // Render the details of beq
    if(pos)
    {
      std::string s = render_btype(pc, insn, "beq");

      s.resize(instruction_width, ' ');  

      *pos << s << "// pc += (" << H::to_hex0x32(regs.get(rs1)) << " == " <<  H::to_hex0x32(regs.get(rs2)) << " ? " 
           << H::to_hex0x32(imm_b) << " : 4) = " << H::to_hex0x32(pc + val) << std::endl;
    }

    pc += val;    

} // End beq()


/**
 * @brief      This function will execute the bne instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **********************************************************************************************************************/
void rv32i_hart::exec_bne(uint32_t insn, std::ostream* pos)
{
    // Initializing r1, rs2, imm_b and val
    uint32_t rs1 = get_rs1(insn);

    uint32_t rs2 = get_rs2(insn);

    int32_t imm_b = get_imm_b(insn);

    uint32_t val = (regs.get(rs1) != regs.get(rs2))? imm_b : 4;
    
    // Render the details of bne
    if(pos)
    {
      std::string s = render_btype(pc, insn, "bne");

      s.resize(instruction_width, ' ');  

      *pos << s << "// pc += (" << H::to_hex0x32(regs.get(rs1)) << " != " <<  H::to_hex0x32(regs.get(rs2)) << " ? " 
           << H::to_hex0x32(imm_b) << " : 4) = " << H::to_hex0x32(pc + val) << std::endl;
    }

    pc += val;     

}// End bne()


/**
 * @brief      This function will execute the blt instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **********************************************************************************************************************/
void rv32i_hart::exec_blt(uint32_t insn, std::ostream* pos)
{
    // Initializing r1, rs2, imm_b and val
    uint32_t rs1 = get_rs1(insn);

    uint32_t rs2 = get_rs2(insn);

    int32_t imm_b = get_imm_b(insn);

    uint32_t val = (regs.get(rs1) < regs.get(rs2))? imm_b : 4;

    // Render the details of blt
    if(pos)
    {
      std::string s = render_btype(pc, insn, "blt");

      s.resize(instruction_width, ' ');   

      *pos << s << "// pc += (" << H::to_hex0x32(regs.get(rs1)) << " < " <<  H::to_hex0x32(regs.get(rs2)) << " ? "
           << H::to_hex0x32(imm_b) << " : 4) = " << H::to_hex0x32(pc + val) << std::endl;
    }

    pc += val;      

}// End blt()


/**
 * @brief      This function will execute the bge instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **********************************************************************************************************************/
void rv32i_hart::exec_bge(uint32_t insn, std::ostream* pos)
{
    // Initializing r1, rs2, imm_b and val
    uint32_t rs1 = get_rs1(insn);

    uint32_t rs2 = get_rs2(insn);

    int32_t imm_b = get_imm_b(insn);

    uint32_t val = (regs.get(rs1) >= regs.get(rs2))? imm_b : 4;

    // Render the details of bge
    if(pos){
      std::string s = render_btype(pc, insn, "bge");

      s.resize(instruction_width, ' ');    

      *pos << s << "// pc += (" << H::to_hex0x32(regs.get(rs1)) << " >= " <<  H::to_hex0x32(regs.get(rs2)) << " ? "
           << H::to_hex0x32(imm_b)<< " : 4) = " << H::to_hex0x32(pc + val) << std::endl;
    }

    pc += val;     

} // End bge()



/**
 * @brief      This function will execute the bltu instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **********************************************************************************************************************/
void rv32i_hart::exec_bltu(uint32_t insn, std::ostream* pos)
{

    // Initializing r1, rs2, imm_b and val
    uint32_t rs1 = get_rs1(insn);

    uint32_t rs2 = get_rs2(insn);

    int32_t imm_b = get_imm_b(insn);

    uint32_t val = ((uint32_t) regs.get(rs1) < (uint32_t) regs.get(rs2))? imm_b : 4;;

    // Render the details of bltu
    if(pos)
    {
      std::string s = render_btype(pc, insn, "bltu");

      s.resize(instruction_width, ' ');   

      *pos << s << "// pc += (" << H::to_hex0x32(regs.get(rs1)) << " <U " <<  H::to_hex0x32(regs.get(rs2)) << " ? " 
           << H::to_hex0x32(imm_b)<< " : 4) = " << H::to_hex0x32(pc + val) << std::endl;

     }

     pc += val;   

} // End bltu()


/**
 * @brief      This function will execute the bgeu instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **********************************************************************************************************************/
void rv32i_hart::exec_bgeu(uint32_t insn, std::ostream* pos)
{
    // Initializing r1, rs2, imm_b and val
    uint32_t rs1 = get_rs1(insn);

    uint32_t rs2 = get_rs2(insn);

    int32_t imm_b = get_imm_b(insn);

    uint32_t val = ((uint32_t) regs.get(rs1) >= (uint32_t) regs.get(rs2))? imm_b : 4;

    // Render the details of bltu
    if(pos)
    {
      std::string s = render_btype(pc, insn, "bgeu");

      s.resize(instruction_width, ' ');  

      *pos << s << "// pc += (" << H::to_hex0x32(regs.get(rs1)) << " >=U " <<  H::to_hex0x32(regs.get(rs2)) << " ? " 
           << H::to_hex0x32(imm_b) << " : 4) = " << H::to_hex0x32(pc + val) << std::endl;
    }

    pc += val;    

} // End bgeu


/**
 * @brief      This function will execute the add instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **************************************************************************************************************************/
void rv32i_hart::exec_add(uint32_t insn, std::ostream* pos)
{
    // Initializing rd, rs1, rs2 and val
    uint32_t rd = get_rd(insn);

    uint32_t rs1 = get_rs1(insn);

    uint32_t rs2 = get_rs2(insn);

    int32_t val = regs.get(rs1)+regs.get(rs2);

    // Render the details of add
    if(pos)
    {
      std::string s = render_rtype(insn, "add");

      s.resize(instruction_width, ' '); 

      *pos << s << "// x" << rd << " = " << H::to_hex0x32(regs.get(rs1)) << " + " << H::to_hex0x32(regs.get(rs2)) << " = " 
           << H::to_hex0x32(val) << std::endl;
    }

    // Assigning the register 
    regs.set(rd, val);  

    pc += 4;   
     
}// End add()


/**
 * @brief      This function will execute the sub instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **************************************************************************************************************************/
void rv32i_hart::exec_sub(uint32_t insn, std::ostream* pos)
{
    // Initializing rd, rs1 and rs2 
    uint32_t rd = get_rd(insn);

    uint32_t rs1 = get_rs1(insn);

    uint32_t rs2 = get_rs2(insn);

    // Render the details of sub
    if(pos)
    {
      std::string s = render_rtype(insn, "sub");

      s.resize(instruction_width, ' ');    

      *pos << s << "// x" << rd << " = " << H::to_hex0x32(regs.get(rs1)) << " - " << H::to_hex0x32(regs.get(rs2)) << " = " 
           << H::to_hex0x32(regs.get(rs1) - regs.get(rs2)) << std::endl;
    }

    // Setting the register 
    regs.set(rd, regs.get(rs1) - regs.get(rs2));   

    pc += 4;   

}// End sub()


/**
 * @brief      This function will execute the srl instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **************************************************************************************************************************/
void rv32i_hart::exec_srl(uint32_t insn, std::ostream* pos)
{
    // Initializing rd, rs1, rs2 and val
    uint32_t rd = get_rd(insn);

    uint32_t rs1 = get_rs1(insn);

    uint32_t rs2 = get_rs2(insn);

    uint32_t val = (uint32_t)regs.get(rs1)>>(regs.get(rs2) & 0x01f);
    
    // Render the details of srl
    if(pos)
    {
      std::string s = render_rtype(insn, "srl");

      s.resize(instruction_width, ' ');  

      *pos << s << "// x" << rd << " = " << H::to_hex0x32(regs.get(rs1)) << " >> " << (uint32_t)regs.get(rs2)%32 << " = " 
           << H::to_hex0x32(val) << std::endl;
    }

    // Setting the register 
    regs.set(rd, val); 

    pc += 4;    
    
}// End srl()


/**
 * @brief      This function will execute the sra instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **************************************************************************************************************************/
void rv32i_hart::exec_sra(uint32_t insn, std::ostream* pos)
{
    // Initializing rd, rs1, rs2 and val
    uint32_t rd = get_rd(insn);

    uint32_t rs1 = get_rs1(insn);

    uint32_t rs2 = get_rs2(insn);

    uint32_t val = regs.get(rs1)>>((regs.get(rs2) & 0x01f));
   
    // Render the details of sra
    if(pos)
    {
      std::string s = render_rtype(insn, "sra");

      s.resize(instruction_width, ' ');    

      *pos << s << "// x" << rd << " = " << H::to_hex0x32(regs.get(rs1)) << " >> " << ((uint32_t)regs.get(rs2))%32 << " = " 
           << H::to_hex0x32(val) << std::endl;
    }

    // Setting the register 
    regs.set(rd, val);  

    pc += 4;     
}// End sra()


/**
 * @brief      This function will execute the sll instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **************************************************************************************************************************/
void rv32i_hart::exec_sll(uint32_t insn, std::ostream* pos)
{
    // Initializing rd, rs1, rs2 and val
    uint32_t rd = get_rd(insn);

    uint32_t rs1 = get_rs1(insn);

    uint32_t rs2 = get_rs2(insn);

    uint32_t val = regs.get(rs1)<<((regs.get(rs2)& 0x01f));

    // Render the details of sll
    if(pos)
    {
      std::string s = render_rtype(insn, "sll");

      s.resize(instruction_width, ' ');  

      *pos << s << "// x" << rd << " = " << H::to_hex0x32(regs.get(rs1)) << " << " << (uint32_t) regs.get(rs2)%32 << " = " 
           << H::to_hex0x32(val) << std::endl;
    } 

    // Setting the register 
    regs.set(rd, val);   

    pc += 4;    
}// End sll()


/**
 * @brief      This function will execute the slt instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **************************************************************************************************************************/
void rv32i_hart::exec_slt(uint32_t insn, std::ostream* pos)
{
    // Initializing rd, rs1, rs2 and val
    uint32_t rd = get_rd(insn);

    uint32_t rs1 = get_rs1(insn);

    uint32_t rs2 = get_rs2(insn);

    uint32_t val = (regs.get(rs1)<regs.get(rs2) ? 1 : 0);

    // Rendering the details of slt
    if(pos)
    {
      std::string s = render_rtype(insn, "slt");

      s.resize(instruction_width, ' ');    

      *pos << s << "// x" << rd << " = (" << H::to_hex0x32(regs.get(rs1)) << " < " << H::to_hex0x32(regs.get(rs2)) 
           << ") ? " << "1 " << ": " << "0 " << "= " << H::to_hex0x32(val) << std::endl; 
    }

    // Setting the register 
    regs.set(rd, val);   

    pc += 4;    
}// End slt()


/**
 * @brief      This function will execute the sltu instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **************************************************************************************************************************/
void rv32i_hart::exec_sltu(uint32_t insn, std::ostream* pos)
{
    // Initializing rd, rs1, rs2 and val
    uint32_t rd = get_rd(insn);

    uint32_t rs1 = get_rs1(insn);

    uint32_t rs2 = get_rs2(insn);

    uint32_t val = ((uint32_t)regs.get(rs1)<(uint32_t)regs.get(rs2) ? 1 : 0);

    // Rendering the details of sltu
    if(pos)
    {
      std::string s = render_rtype(insn, "sltu");  

      s.resize(instruction_width, ' ');
 
      *pos << s << "// x" << rd << " = (" << H::to_hex0x32(regs.get(rs1)) << " <U " << H::to_hex0x32(regs.get(rs2)) 
           << ") ? " << "1 " << ": " << "0" << " = " << H::to_hex0x32(val) << std::endl;
    }

    // Setting the register
    regs.set(rd, val);  

    pc += 4;    

}// End sltu


/**
 * @brief      This function will execute the xor instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **************************************************************************************************************************/
void rv32i_hart::exec_xor(uint32_t insn, std::ostream* pos)
{
    // Initializing rd, rs1, rs2 and val
    uint32_t rd = get_rd(insn);

    uint32_t rs1 = get_rs1(insn);

    uint32_t rs2 = get_rs2(insn);

    uint32_t val = ((regs.get(rs1)^regs.get(rs2)));

    // Rendering the details of xor
    if(pos)
    {
      std::string s = render_rtype(insn, "xor");

      s.resize(instruction_width, ' ');  

      *pos << s << "// x" << rd << " = " <<  H::to_hex0x32(regs.get(rs1)) << " ^ " << H::to_hex0x32(regs.get(rs2)) << " = " 
           << H::to_hex0x32(val) << std::endl;
    }

    // Setting the register
    regs.set(rd, val);  

    pc += 4;   

}// End xor()


/**
 * @brief      This function will execute the or instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **************************************************************************************************************************/
void rv32i_hart::exec_or(uint32_t insn, std::ostream* pos)
{
    // Initializing rd, rs1, rs2 and val
    uint32_t rd = get_rd(insn);

    uint32_t rs1 = get_rs1(insn);

    uint32_t rs2 = get_rs2(insn);

    uint32_t val = regs.get(rs1) | regs.get(rs2);

    // Rendering the details of or
    if(pos)
    {
      std::string s = render_rtype(insn, "or");

      s.resize(instruction_width, ' ');    

      *pos << s << "// x" << rd << " = " << H::to_hex0x32(regs.get(rs1)) << " | " << H::to_hex0x32(regs.get(rs2)) << " = " 
           << H::to_hex0x32(val) << std::endl;
    }

    // Setting the register
    regs.set(rd,val);  

    pc += 4;     
}// End or()


/**
 * @brief      This function will execute the and instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **************************************************************************************************************************/
void rv32i_hart::exec_and(uint32_t insn, std::ostream* pos)
{
    // Initializing rd, rs1, rs2 and val
    uint32_t rd = get_rd(insn);

    uint32_t rs1 = get_rs1(insn);

    uint32_t rs2 = get_rs2(insn);

    uint32_t val = regs.get(rs1) & regs.get(rs2);
  
    // Rendering the details of and
    if(pos)
    {
      std::string s = render_rtype(insn, "and");

      s.resize(instruction_width, ' ');    

      *pos << s << "// x" << rd << " = " << H::to_hex0x32(regs.get(rs1)) << " & " << H::to_hex0x32(regs.get(rs2)) << " = " 
           << H::to_hex0x32(val) << std::endl;
    }

    // Setting the register
    regs.set(rd,val);   

    pc += 4;  

}// End and()


/**
 * @brief      This function will execute the sb instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **************************************************************************************************************************/
void rv32i_hart::exec_sb(uint32_t insn, std::ostream* pos)
{
    // Initializing rs1, rs2 and imm_s
    uint32_t rs1 = get_rs1(insn);

    uint32_t rs2 = get_rs2(insn);

    int32_t imm_s = get_imm_s(insn);

    mem.set8(regs.get(rs1) + imm_s, regs.get(rs2)&0x000000ff);

    // Rendering the details of sb
    if(pos)
    {
      std::string s = render_stype(insn, "sb");

      s.resize(instruction_width, ' ');   

      *pos << s << "// m8(" << H::to_hex0x32(regs.get(rs1)) << " + " << H::to_hex0x32(imm_s) << ") = " 
           << H::to_hex0x32(mem.get8(regs.get(rs1) + imm_s)) << std::endl;
    }

    pc += 4;   

}// End sb()


/**
 * @brief      This function will execute the sh instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **************************************************************************************************************************/
void rv32i_hart::exec_sh(uint32_t insn, std::ostream* pos)
{
    // Initializing rs1, rs2 and imm_s
    uint32_t rs1 = get_rs1(insn);

    uint32_t rs2 = get_rs2(insn);

    int32_t imm_s = get_imm_s(insn);

    mem.set16(regs.get(rs1) + imm_s, regs.get(rs2)&0x0000ffff);

    // Rendering the details of sh
    if(pos)
    {
      std::string s = render_stype(insn, "sh");

      s.resize(instruction_width, ' ');  

      *pos << s << "// m16(" << H::to_hex0x32(regs.get(rs1)) << " + " << H::to_hex0x32(imm_s) << ") = " 
           << H::to_hex0x32(mem.get16(regs.get(rs1) + imm_s)) << std::endl;
    }

    pc += 4;   

}// End sh()


/**
 * @brief      This function will execute the sw instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **************************************************************************************************************************/
void rv32i_hart::exec_sw(uint32_t insn, std::ostream* pos)
{
    // Initializing rs1, rs2 and imm_s
    uint32_t rs1 = get_rs1(insn);

    uint32_t rs2 = get_rs2(insn);

    int32_t imm_s = get_imm_s(insn);

    mem.set32(regs.get(rs1) + imm_s, regs.get(rs2));

    // Rendering the details of sw
    if(pos)
    {
      std::string s = render_stype(insn, "sw");

      s.resize(instruction_width, ' ');   
 
      *pos << s << "// m32(" << H::to_hex0x32(regs.get(rs1)) << " + " << H::to_hex0x32(imm_s) << ") = " 
           << H::to_hex0x32(mem.get32(regs.get(rs1) + imm_s)) << std::endl;    
    }

    pc += 4;   

}// End sw()


/**
 * @brief      This function will execute the lb instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **************************************************************************************************************************/
void rv32i_hart::exec_lb(uint32_t insn, std::ostream* pos)
{
    // Initializing rd, rs1, imm_i and val
    uint32_t rd = get_rd(insn);

    uint32_t rs1 = get_rs1(insn);

    int32_t imm_i = get_imm_i(insn);

    int8_t val = mem.get8(imm_i + regs.get(rs1));

    // Rendering the details of lb
    if(pos)
    {
      std::string s = render_itype_load(insn, "lb");

      s.resize(instruction_width, ' ');  

      *pos << s << "// x" << rd << " = sx(m8(" << H::to_hex0x32(regs.get(rs1)) << " + " << H::to_hex0x32(imm_i) 
           << "))" << " = " << H::to_hex0x32(val) << std::endl;
    }

    // Setting the register
    regs.set(rd, val);   

    pc += 4;   

}// End lb()


/**
 * @brief      This function will execute the lh instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **************************************************************************************************************************/
void rv32i_hart::exec_lh(uint32_t insn, std::ostream* pos)
{
    // Initializing rd, rs1, imm_i and val
    uint32_t rd = get_rd(insn);

    uint32_t rs1 = get_rs1(insn);

    int32_t imm_i = get_imm_i(insn);

    int16_t val = mem.get16(regs.get(rs1) + imm_i);

    // Rendering the details of lh
    if(pos)
    {
      std::string s = render_itype_load(insn, "lh");

      s.resize(instruction_width, ' ');    

      *pos << s << "// x" << rd << " = sx(m16(" << H::to_hex0x32(regs.get(rs1)) << " + " << H::to_hex0x32(imm_i) << "))" 
           << " = " << H::to_hex0x32(val) << std::endl;
    }

    // Setting the register
    regs.set(rd, val);   

    pc += 4;   
}// End lh()


/**
 * @brief      This function will execute the lw instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **************************************************************************************************************************/
void rv32i_hart::exec_lw(uint32_t insn, std::ostream* pos)
{
    // Initializing rd, rs1, imm_i and val
    uint32_t rd = get_rd(insn);

    uint32_t rs1 = get_rs1(insn);

    int32_t imm_i = get_imm_i(insn);

    uint32_t val = mem.get32(regs.get(rs1) + imm_i);

    // Rendering the details of lw
    if(pos)
    {
      std::string s = render_itype_load(insn, "lw");

      s.resize(instruction_width, ' ');    

      *pos << s << "// x" << rd << " = sx(m32(" << H::to_hex0x32(regs.get(rs1)) << " + " << H::to_hex0x32(imm_i) << ")) = " 
           << H::to_hex0x32(val) << std::endl;
    }

    // Setting the register
    regs.set(rd, val);  
    
    pc += 4;    

}// End lw()


/**
 * @brief      This function will execute the lbu instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **************************************************************************************************************************/
void rv32i_hart::exec_lbu(uint32_t insn, std::ostream* pos)
{
    // Initializing rd, rs1, imm_i and val
    uint32_t rd = get_rd(insn);

    uint32_t rs1 = get_rs1(insn);

    uint32_t imm_i = get_imm_i(insn);

    uint8_t val = mem.get8(regs.get(rs1) + imm_i)&0x000000ff;

    // Rendering the details of lbu
    if(pos)
    {
      std::string s = render_itype_load(insn, "lbu");

      s.resize(instruction_width, ' ');    

      *pos << s << "// x" << rd << " = zx(m8(" << H::to_hex0x32(regs.get(rs1)) << " + " << H::to_hex0x32(imm_i) << "))" 
           << " = " << H::to_hex0x32(val) << std::endl;
    }

    // Setting the register
    regs.set(rd, val);  

    pc += 4;   

}// End lbu()


/**
 * @brief      This function will execute the lhu instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **************************************************************************************************************************/
void rv32i_hart::exec_lhu(uint32_t insn, std::ostream* pos)
{
    // Initializing rd, rs1, imm_i and val
    uint32_t rd = get_rd(insn);

    uint32_t rs1 = get_rs1(insn);

    uint32_t imm_i = get_imm_i(insn);

    uint16_t val = mem.get16(regs.get(rs1) + imm_i)&0x0000ffff;

    // Rendering the details of lhu
    if(pos)
    {
      std::string s = render_itype_load(insn, "lhu");

      s.resize(instruction_width, ' ');    

      *pos << s << "// x" << rd << " = zx(m16(" << H::to_hex0x32(regs.get(rs1)) << " + " << H::to_hex0x32(imm_i) << "))" 
           << " = " << H::to_hex0x32(val) << std::endl;
    }

    // Setting the register
    regs.set(rd, val);   

    pc += 4;   

}// End lhu()


/**
 * @brief      This function will execute the addi instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **************************************************************************************************************************/
void rv32i_hart::exec_addi(uint32_t insn, std::ostream* pos)
{
    // Initializing rd, rs1, and imm_i 
    uint32_t rd = get_rd(insn);

    uint32_t rs1 = get_rs1(insn);

    int32_t imm_i = get_imm_i(insn);

    // Rendering the details of addi
    if(pos)
    {
      std::string s = render_itype_alu(insn, "addi", imm_i);

      s.resize(instruction_width, ' ');    

      *pos << s << "// x" << rd << " = " << H::to_hex0x32(regs.get(rs1)) << " + " << H::to_hex0x32(imm_i) << " = " 
           << H::to_hex0x32(regs.get(rs1) + imm_i) << std::endl;
    }

    // Setting the register
    regs.set(rd, regs.get(rs1) + imm_i);   

    pc += 4;    

}// End addi()


/**
 * @brief      This function will execute the slti instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **************************************************************************************************************************/
void rv32i_hart::exec_slti(uint32_t insn, std::ostream* pos)
{
    // Initializing rd, rs1, imm_i and val
    uint32_t rd = get_rd(insn);

    uint32_t rs1 = get_rs1(insn);

    int32_t imm_i = get_imm_i(insn);

    uint32_t val = (regs.get(rs1) < imm_i)? 1 : 0;

    // Rendering the details of slti
    if(pos)
    {
      std::string s = render_itype_alu(insn, "slti", imm_i);

      s.resize(instruction_width, ' ');    

      *pos << s << "// x" << rd << " = (" << H::to_hex0x32(regs.get(rs1)) << " < " << imm_i << ") ? " << "1 " << ": " 
           << "0" << " = " << H::to_hex0x32(val) << std::endl;
    }

    // Setting the register
    regs.set(rd, val);   

    pc += 4;      
}// End slti()


/**
 * @brief      This function will execute the sltiu instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **************************************************************************************************************************/
void rv32i_hart::exec_sltiu(uint32_t insn, std::ostream* pos)
{
    // Initializing rd, rs1, imm_i and val
    uint32_t rd = get_rd(insn);

    uint32_t rs1 = get_rs1(insn);

    uint32_t imm_i = get_imm_i(insn);

    uint32_t val = ((uint32_t) regs.get(rs1) <  imm_i)? 1 :0;

    // Rendering the details of sltiu
    if(pos)
    {
      std::string s = render_itype_alu(insn, "sltiu", imm_i);

      s.resize(instruction_width, ' ');  

      *pos << s << "// x" << rd << " = (" <<  H::to_hex0x32(regs.get(rs1)) << " <U " << imm_i << ") ? " << "1 " << ": " 
           << "0" << " = " << H::to_hex0x32(val) << std::endl;
    }

    // Setting the register
    regs.set(rd, val);   

    pc += 4;    

}// End sltiu()


/**
 * @brief      This function will execute the xori instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 ******************************************************************************************************************/
void rv32i_hart::exec_xori(uint32_t insn, std::ostream* pos)
{
    // Initializing rd, rs1, imm_i and val
    uint32_t rd = get_rd(insn);

    uint32_t rs1 = get_rs1(insn);

    int32_t imm_i = get_imm_i(insn);

    uint32_t val = ((regs.get(rs1)^imm_i));

    // Rendering the details of xori
    if(pos)
    {
      std::string s = render_itype_alu(insn, "xori", imm_i);

      s.resize(instruction_width, ' ');  

      *pos << s << "// x" << rd << " = " << H::to_hex0x32(regs.get(rs1)) << " ^ " << H::to_hex0x32(imm_i) << " = " 
           << H::to_hex0x32(val) << std::endl;
    }

    // Setting the register
    regs.set(rd, val);   

    pc += 4;    

}// End xori()


/**
 * @brief      This function will execute the ori instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 ********************************************************************************************************************/
void rv32i_hart::exec_ori(uint32_t insn, std::ostream* pos)
{
    // Initializing rd, rs1, imm_i and val
    uint32_t rd = get_rd(insn);

    uint32_t rs1 = get_rs1(insn);

    int32_t imm_i = get_imm_i(insn);

    uint32_t val =  ((regs.get(rs1) | imm_i));

    // Rendering the details of xori
    if(pos)
    {
      std::string s = render_itype_alu(insn, "ori", imm_i);

      s.resize(instruction_width, ' ');    

      *pos << s << "// x" << rd << " = " << H::to_hex0x32(regs.get(rs1)) << " | " << H::to_hex0x32(imm_i) << " = " 
           << H::to_hex0x32(val) << std::endl;
    }
  
    // Setting the register
    regs.set(rd, val);
      
    pc += 4;   

}// End ori()


/**
 * @brief      This function will execute the andi instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 ******************************************************************************************************************/
void rv32i_hart::exec_andi(uint32_t insn, std::ostream* pos)
{
    // Initializing rd, rs1, imm_i and val
    uint32_t rd = get_rd(insn);

    uint32_t rs1 = get_rs1(insn);

    int32_t imm_i = get_imm_i(insn);

    uint32_t val = ((regs.get(rs1) & imm_i));
 
    // Rendering the details of andi
    if(pos)
    {
      std::string s = render_itype_alu(insn, "andi", imm_i);

      s.resize(instruction_width, ' ');    
 
      *pos << s << "// x" << rd << " = " << H::to_hex0x32(regs.get(rs1)) << " & " << H::to_hex0x32(imm_i) << " = " 
           << H::to_hex0x32(val) << std::endl;
    }

    // Setting the register
    regs.set(rd, val);   

    pc += 4;    

}// End andi()


/**
 * @brief      This function will execute the slli instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 *****************************************************************************************************/
void rv32i_hart::exec_slli(uint32_t insn, std::ostream* pos)
{
    // Initializing rd, rs1, imm_i and val
    uint32_t rd = get_rd(insn);

    uint32_t rs1 = get_rs1(insn);

    int32_t imm_i = get_imm_i(insn);

    uint32_t val = regs.get(rs1) << imm_i;

    // Rendering the details of slli
    if (pos)
    {
        std::string s = render_itype_alu(insn, "slli", imm_i % XLEN);

        s.resize(instruction_width, ' ');

        *pos << s << "// x" << rd << " = " << H::to_hex0x32(regs.get(rs1)) << " << " << imm_i << " = " 
            << H::to_hex0x32(val) << std::endl;
    }

    // Setting the register
    regs.set(rd, val);

    pc += 4;

}// End slli()


/**
 * @brief      This function will execute the srli instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 *****************************************************************************************************/
void rv32i_hart::exec_srli(uint32_t insn, std::ostream* pos)
{
    // Initializing rd, rs1, imm_i and val
    uint32_t rd = get_rd(insn);

    uint32_t rs1 = get_rs1(insn);

    int32_t imm_i = get_imm_i(insn);

    uint32_t val = (uint32_t)regs.get(rs1) >> imm_i;

    // Rendering the details of srli
    if(pos)
    {
      std::string s = render_itype_alu(insn, "srli", imm_i%XLEN);

      s.resize(instruction_width, ' ');    
 
      *pos << s << "// x" << rd << " = " << H::to_hex0x32(regs.get(rs1)) << " >> " << imm_i << " = " 
           << H::to_hex0x32(val) << std::endl;
    }

    // Setting the register
    regs.set(rd, val);   

    pc += 4;     

}// End srli()


/**
 * @brief      This function will execute the srai instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **************************************************************************************************************************/
void rv32i_hart::exec_srai(uint32_t insn, std::ostream* pos)
{
    // Initializing rd, rs1, imm_i, shamt and val
    uint32_t shamt = (insn & 0x01ffffff);

    int32_t imm_i = get_imm_i(shamt);

    uint32_t rd = get_rd(insn);

    int32_t rs1 = regs.get(get_rs1(insn));

    uint32_t val = rs1 >> imm_i;

    // Rendering the details of srai
    if(pos)
    {
      std::string s = render_itype_alu(insn, "srai", imm_i%XLEN);
      s.resize(instruction_width, ' ');    
 
      *pos << s << "// x" << rd << " = " << H::to_hex0x32(rs1) << " >> " << imm_i << " = " 
           << H::to_hex0x32(val) << std::endl;
    }

    // Setting the register
    regs.set(rd, val);   

    pc += 4;     

}// End srai()


/**
 * @brief      This function will execute the csrrs instruction with the appropiate 
 *             spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param pos  ostream* parameter, output stream 
 **************************************************************************************************************************/
void rv32i_hart::exec_csrrs(uint32_t insn, std::ostream* pos)
{

  // Initializing rd, rs1, and csr
  uint32_t rd = get_rd(insn);

  uint32_t rs1 = get_rs1(insn);

  int32_t csr = get_imm_i(insn) & 0x00000fff;

  if (csr != 0xf14 || rs1 != 0)
  {
      halt = true;

      halt_reason = "Illegal CSR in CRRSS instruction";

  }
  if (pos)
  {
      std::string s = render_csrrx(insn, "csrrs");

      s.resize(instruction_width, ' ');

      *pos << s << "// " << render_reg(rd) << " = " << std::dec << mhartid << std::endl;
  }
  if (!halt)
  {
      regs.set(rd, mhartid);

      pc += 4;
  }

}// End csrrs()