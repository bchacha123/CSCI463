//********************************************************************************************************************************
//
//  Brayan Chacha Gonzalez
//  z1861700
//  CSCI 463, Professir John Winans 
//
//  I certify that this is my own work and where appropriate an extension 
//  of the starter code provided for the assignment.
//
//  File: rv32i_decode.cpp                                           								   Revised Final Copy 3/2/2021
//
//********************************************************************************************************************************
#include "hex.h"
#include "memory.h"
#include "rv32i_decode.h"

#include <iostream> 
#include <cassert>
#include <iomanip>
#include <sstream>
#include <string>

using std::string;
using std::cout;
using std::endl;
using std::ostringstream;
using std::dec;
using std::setw;
using std::setfill;
using std::left;
using std::right;

typedef hex HEXX;

/**
 * @note This is a software decoder. The switch statement below has every instruction necessary needed  
 *		 for this assignment and the decoder based on the rv32i. 
 *
 * @param addr memory address, from which instruction will be fetched.
 * @param insn any possible value.
 *
 * @return Will return the render instruction based on the instruction that was fetched. 
 ********************************************************************************************************************************/
string rv32i_decode::decode(uint32_t addr, uint32_t insn)
{
	switch(get_opcode(insn))
	{
		// Load Upper Immediate 
		case opcode_lui:  				    return render_lui(insn); 

		// Add Upper Immediate to PC
		case opcode_auipc: 				    return render_auipc(insn);

		// Jump and Link
		case opcode_jal:   					return render_jal(addr,insn);

		// Jump and Link Register 
        case opcode_jalr:  					return render_jalr(insn);

		// This case/switch statement has the system calls
        case opcode_system:
			switch(get_funct3(insn))
			{
				case funct3_beq:
					switch(insn)
					{
						case insn_ecall:    return render_ecall(insn);
						case insn_ebreak:   return render_ebreak(insn);
						default:	        return render_illegal_insn(insn);
					}

				case funct3_csrrw:  		return render_csrrx(insn,"csrrw");
				case funct3_csrrs:  		return render_csrrx(insn,"csrrs");
				case funct3_csrrc:  		return render_csrrx(insn,"csrrc");
				case funct3_csrrwi: 		return render_csrrxi(insn,"csrrwi");
				case funct3_csrrsi: 		return render_csrrxi(insn,"csrrsi");
				case funct3_csrrci: 		return render_csrrxi(insn,"csrrci");
				default:	        		return render_illegal_insn(insn);
			}
			assert(0 && "unrecognized system call");


		// This case/switch statement is based on the B-Type instruction placing the appropiate funct3 returing its insruction rendering. 
		// B-Type Instruction:  Are used to branch instruction that require an even immediate 
		// 						value that is used to determine the branch target addr as on offset
		// 						from the current instruction's addr. 
		//
		//		 B-Type Insts:  beq, bne, blt, bge, bltu, bgeu.
		case opcode_btype:
			switch(get_funct3(insn))
			{
				case funct3_beq:			return render_btype(addr, insn, "beq");
				case funct3_bne:    		return render_btype(addr, insn, "bne");
				case funct3_blt:			return render_btype(addr, insn, "blt");
				case funct3_bge:			return render_btype(addr, insn, "bge");
				case funct3_bltu: 			return render_btype(addr, insn, "bltu");
				case funct3_bgeu:			return render_btype(addr, insn, "bgeu"); 
				default: 					return render_illegal_insn(insn);
			}
			assert(0 && "unrecognized funct3");


		// This case/switch statement is based on the load instructions referencing the I-Type instructions, 
		// taking the appropiate funct3 returing its insruction rendering.
		// I-Type Load Insts: lb, lh, lw, lbu, lhu
		case opcode_load_imm:
			switch(get_funct3(insn))
			{
				case funct3_lb:	    		return render_itype_load(insn, "lb");
				case funct3_lh:    			return render_itype_load(insn, "lh");
				case funct3_lw:	    		return render_itype_load(insn, "lw");
				case funct3_lbu:			return render_itype_load(insn, "lbu");
				case funct3_lhu: 			return render_itype_load(insn, "lhu");
				default: 					return render_illegal_insn(insn);

			}
			assert(0 && "unrecognized funct3");


		// This case/switch statement will specifically call the s-type instruction and base on func3 of the given instruction, returing its insruction rendering. 
		// S-Type Instruction:  Are used to encode instructions with a signed 12-bit immediate operand with a range [-2048:2047], (rs1 & rs2).
		// 		 S-Type Insts:  sb, sh, sw					
		case opcode_stype:
			switch(get_funct3(insn))
			{
				case funct3_sb:	    		return render_stype(insn, "sb");
				case funct3_sh:     		return render_stype(insn, "sh");
				case funct3_sw:	    		return render_stype(insn, "sw");
				default: 					return render_illegal_insn(insn);
			}
			assert(0 && "unrecognized funct3");


		// This case/switch statement will be used for the i-type instruction and base on func3 of the given instruction, returing its insruction rendering.
		// I-Type Instruction:  Are used to encode instructions with a signed 12-bit immediatre operand with a range [-2048:2047], (rs1 & rs2).
		// 		 I-Type Insts:  addi, slti, sltiu, xori, ori, slli, andi.
		case opcode_alu_imm:
			switch(get_funct3(insn))
			{
				case funct3_add:	    	return render_itype_alu(insn, "addi" , get_imm_i(insn));
				case funct3_slt:	   		return render_itype_alu(insn, "slti" , get_imm_i(insn));
				case funct3_sltu:	    	return render_itype_alu(insn, "sltiu" , get_imm_i(insn));
				case funct3_xor:       		return render_itype_alu(insn, "xori" , get_imm_i(insn));
				case funct3_or:	    		return render_itype_alu(insn, "ori" ,  get_imm_i(insn));
				case funct3_sll:	    	return render_itype_alu(insn, "slli" , get_imm_i(insn));
				case funct3_and:	    	return render_itype_alu(insn, "andi" , get_imm_i(insn));
				case funct3_srx:

					// This case/switch statement will specifically load the i-type instruction and base on func7 of the given instruction, returing its insruction rendering.
					// I-Type Insts: srli, srai.
					switch(get_funct7(insn))
					{
						case funct7_srl: 	return render_itype_alu(insn, "srli" , get_imm_i(insn));
						case funct7_sra: 	return render_itype_alu(insn, "srai" , get_rs2(insn));
						default: 			return render_illegal_insn(insn);
					}
					assert(0 && "unrecognized funct7");

				default: return render_illegal_insn(insn);
			}
			assert(0 && "unrecognized funct3");


		// This case/switch statement will specifically load the r-type instruction base on func3 of the given instruction, returning its instrctuion rendering.
		// R-Type Instruction:	Are used for operations that set destination register rd to the result of an arithmeric, logical or shift
		//						operation applied to source register rs1 & rs2.
		//		 I-Type Insts:  add, sub, sll, slt, sltu, xor, and, or 
		case opcode_rtype:
			switch(get_funct3(insn))
			{
				case funct3_add:	    
					switch(get_funct7(insn))
					{
						case funct7_add: 	return render_rtype(insn, "add");
						case funct7_sub: 	return render_rtype(insn, "sub");
						default: 			return render_illegal_insn(insn);
					}
					assert(0 && "unrecognized funct7");

				case funct3_sll:	    	return render_rtype(insn, "sll");
				case funct3_slt:       		return render_rtype(insn, "slt");
				case funct3_sltu:	    	return render_rtype(insn, "sltu");
				case funct3_xor:	    	return render_rtype(insn, "xor");
				case funct3_and:	    	return render_rtype(insn, "and");
				case funct3_or:	        	return render_rtype(insn, "or");
				case funct3_srx:	    

					// This case/switch statement will specifically load the R-Type instruction and base on func7 of the given instruction. 
					switch(get_funct7(insn))
					{
						case funct7_srl: 	return render_rtype(insn, "srl");
						case funct7_sra: 	return render_rtype(insn, "sra");
						default:         	return render_illegal_insn(insn);
					}

					assert(0 && "unrecognized funct7");

				default: return render_illegal_insn(insn);
			}

			assert(0 && "unrecognized funct3");

		default: return render_illegal_insn(insn);

	}// End switch(get_opcode(insn))

	assert(0 && "unrecognized opcode"); // It should be impossible to ever get here!

}// End decode 



/**
 * @brief   This function is used to render the registers. 
 *
 * @param r This parameter will return the register it gets assigned, following an x. 
 *
 * @return  Will return a std::string (Format: x[Register #]).
 ********************************************************************************************************************************/
string rv32i_decode::render_reg(int r)
{
	ostringstream os;

	os << std::right << "x" << r;

	return os.str();
}


/**
 * @brief       This function is used to render the operands for those instructions that have an addressing mode.
 *
 * @param base  imm value. 
 * @param disp  displacement. 
 * 
 * @return      Will return a std::string with the base and disp.
 ********************************************************************************************************************************/
string rv32i_decode::render_base_disp(uint32_t base, int32_t disp){

	ostringstream os;

	// Static_cast is needed, for the conversion of the negative numbers. 
	os << dec << static_cast<int>(base) << "(x" << disp << ")";

	return os.str();

}


/**
 * @brief    This function is used to render the given instruction mnemonic.
 *
 * @param m  Passing any mnemonic instruction
 *
 * @return   Will return the appropriate formating of the mnemonic.
 ********************************************************************************************************************************/
string rv32i_decode::render_mnemonic(const string &m)
{
	ostringstream os;

	os << left << setw(mnemonic_width) << m;

	return os.str();
}



/**
 * @brief      This function will extract the opcode from any given instruction. 
 *
 * @param insn Address passed of the simulated memory.
 *
 * @return     Will return the opcode fiel using LE. bits [6:0]
 ********************************************************************************************************************************/
uint32_t rv32i_decode::get_opcode(uint32_t insn)
{
	// Shifting the bits to extract the correct field. opcode -> (bits [6:0]) 
	return (insn & 0x0000007f);
}

/**
 * @brief      This function will extract the rd from any given instruction.
 *
 * @param insn Address passed of the simulated memory.
 *
 * @return     Will return the appropriate rd. bits [11:7]
 ********************************************************************************************************************************/
uint32_t rv32i_decode::get_rd(uint32_t insn)
{
	// Shifting the bits to extract the correct field. rd -> (bits [11:7]) 
	return (insn & 0x00000f80) >> 7;
}

/**
 * @brief      This function will extract funct3 from any given instruction.
 *
 * @param insn Address passed of the simulated memory.
 *
 * @return     Will return the appropriate funct3. bits [14:12]
 ********************************************************************************************************************************/
uint32_t rv32i_decode::get_funct3(uint32_t insn)
{
	// Shifting the bits to extract the correct field. funct3 -> (bits [14:12]) 
	return (insn & 0x00007000) >> 12; 
}

/**
 * @brief      This function will extract the rs1 fiel from any given instruction.
 *
 * @param insn Address passed of the simulated memory.
 *
 * @return     Will return the appropriate rs1. bits [19:15]
 ********************************************************************************************************************************/
uint32_t rv32i_decode::get_rs1(uint32_t insn)
{
	// Shifting the bits to extract the correct field. rs1 -> (bits [19:15])
	return (insn & 0x000f8000) >> 15; 

}

/**
 * @brief      This function will extract the rs2 fiel from any given instruction.
 *
 * @param insn Address passed of the simulated memory.
 *
 * @return     Will return the appropriate rs2. bits [20:24]
 ********************************************************************************************************************************/
uint32_t rv32i_decode::get_rs2(uint32_t insn)
{
	// Shifting the bits to extract the correct field. rs2 -> (bits [20:24])
	return (insn & 0x01f00000) >> 20; 

}

/**
 * @brief      This function will extract funct7 from any given instruction.
 *
 * @param insn Address passed of the simulated memory.
 *
 * @return     Will return the appropriate funct7. bits [31:25]
 ********************************************************************************************************************************/
uint32_t rv32i_decode::get_funct7(uint32_t insn)
{
	// Shifting the bits to extract the correct field. funct7. -> (bits [31:25])
	return (insn & 0x7f000000) >> 25; 

}

/**
 * @brief      This function will extract imm_i from the given instruction.
 *
 * @param insn Address passed of the simulated memory.
 *
 * @return     Will return the appropriate imm_i field
 ********************************************************************************************************************************/
int32_t rv32i_decode::get_imm_i(uint32_t insn)
{
	// Shifting the correct i-type field [31:20]
	int32_t imm_i = (insn & 0xfff00000) >> (20);   

	// Sign bit [31]
	if (insn & 0x80000000)

		// Placing sign bits to [31:20] and zero out [11:0]
		imm_i |= 0xfffff000; 

	return imm_i;
}

/**
 * @brief      This function will extract imm_u from the given instruction.
 *
 * @param insn Address passed of the simulated memory.
 *
 * @return     Will return the appropriate imm_u field
 ********************************************************************************************************************************/
int32_t rv32i_decode::get_imm_u(uint32_t insn)
{
	// Assigning corresponding bits [31:12], rest of the bits are zero out [11:0]
	return (insn & 0xfffff000);
}

/**
 * @brief      This function will extract imm_b from the given instruction.
 *
 * @param insn Address passed of the simulated memory.
 *
 * @return     Will return the appropriate imm_b field
 ********************************************************************************************************************************/
int32_t rv32i_decode::get_imm_b(uint32_t insn)
{
	// Assigning corresponding bits to imm_b 
	int32_t imm_b = (insn & 0x80000000) >> (19); 

	// Shifting bits [30:25] to [10:5]
    imm_b |= (insn & 0x7e000000) >> (20); 

	// Shifting bit [7] to bit [11]
    imm_b |= (insn & 0x80) << (4); 
	
	// Shifting bits [11:7] to [4:1]
    imm_b |= (insn & 0xf00) >> (7); 

	// Sign bit [31]
	if (insn & 0x80000000)

		// Placing sign bit to [31:13] 
		imm_b |= 0xffffe000; 

    return imm_b;
}

/**
 * @brief      This function will extract imm_s from the given instruction.
 *
 * @param insn Address passed of the simulated memory.
 *
 * @return     Will return the appropriate imm_s field
 ********************************************************************************************************************************/
int32_t rv32i_decode::get_imm_s(uint32_t insn)
{

	// Assigning corresponding bits to imm_s
	int32_t imm_s = (insn & 0xfe000000) >> (20);

	// Shifting bits [11:7] to [4:0]
	imm_s |= (insn & 0x00000f80) >> (7);

	// Sign bit [31]
	if (insn & 0x80000000)

		// Placing sign bit to [31:12] 
		imm_s |= 0xfffff000; 

	return imm_s;
}

/**
 * @brief      This function will extract imm_j from the given instruction.
 *
 * @param insn Address passed of the simulated memory.
 *
 * @return     Will return the appropriate imm_j field
 ********************************************************************************************************************************/
int32_t rv32i_decode::get_imm_j(uint32_t insn)
{
	int32_t imm_j = (insn & 0x80000000) >> (11); 

	// Shifting bits [19:12]
    imm_j |= (insn & 0xff000); 

	// Shifting bit [20] to bit [11]
	imm_j |= (insn & 0x100000) >> (9); 

	// Shifting bit [30:21] to bit [10:1]
    imm_j |= (insn & 0x7fe00000) >> (20); 

	// Sign bit [31]
	if (insn & 0x80000000)

		// Placing sign bit to [31:21] 
		imm_j |= 0xffe00000; 

	return imm_j;
}

/**
 * @brief      This function will render invalid instructions.
 *
 * @param insn Address passed of the simulated memory.
 *
 * @return     Will return Error: message
 ****************************************************************************************************************************************/
string rv32i_decode::render_illegal_insn(uint32_t insn)
{
	return "ERROR: UNIMPLEMENTED INSTRUCTION";
}

/**
 * @brief      This function will render the lui instruction with the appropiate spacing calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 *
 * @return     Will print the lui
 ****************************************************************************************************************************************/
string rv32i_decode::render_lui(uint32_t insn)
{

	ostringstream os;

	// Set the lui mnemonic, also the register to the imm_u value 
	os << render_mnemonic("lui") << render_reg(get_rd(insn)) << "," << hex::to_hex0x20((get_imm_u(insn) >> 12) & 0x0fffff);

	return os.str();
}

/**
 * @brief      This function will render the auipc instruction with the appropiate format calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 *
 * @return     Will print auipc
 ****************************************************************************************************************************************/
string rv32i_decode::render_auipc(uint32_t insn)
{
	ostringstream os;

	// Setting the auipc mnemonic, also the register to the imm_u value
	os << render_mnemonic("auipc") << render_reg(get_rd(insn)) << "," << hex::to_hex0x20((get_imm_u(insn) >> 12) & 0x0fffff);

	return os.str();
}

/**
 * @brief      This function will render the jal instruction with the appropiate format calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 * @param addr Instruction address
 *
 * @return     Will print jal
 ****************************************************************************************************************************************/
string rv32i_decode::render_jal(uint32_t addr, uint32_t insn)
{
	ostringstream os;

	// Set the jal mnemonic, also the register to the imm_j value that determines the jump target address
	os << render_mnemonic("jal") << render_reg(get_rd(insn)) << "," << hex::to_hex0x32(get_imm_j(insn) + addr);

	return os.str();
}

/**
 * @brief      This function will render the jalr instruction with the appropiate format calling the appropiate functions needed.
 *
 * @param insn Address passed of the simulated memory.
 *
 * @return     Will print jalr
 ****************************************************************************************************************************************/
string rv32i_decode::render_jalr(uint32_t insn)
{
	ostringstream os;

	// Set the jalr mnemonic, also setting the regiser to the addres of the next inst, then jumping to an adress given by the sum of rs1 and imm_i
	os << render_mnemonic("jalr") << render_reg(get_rd(insn)) << "," << render_base_disp(get_imm_i(insn), get_rs1(insn));

	return os.str();
}

/**
 * @brief          This function will render the btype instruction with the appropiate format calling the appropiate functions needed.
 *
 * @param addr     Address passed of the simulated memory
 * @param insn 	   Address of the instruction
 * @param mnemonic mnemonic 
 *
 * @return         Will print the given btype
 ************************************************************************************************************************************************************/
string rv32i_decode::render_btype(uint32_t addr, uint32_t insn, const char *mnemonic)
{

	uint32_t rs1 =   get_rs1(insn);
    uint32_t rs2 =   get_rs2(insn);
    int32_t  imm_b = get_imm_b(insn);

    ostringstream os;

    os << render_mnemonic(mnemonic) <<  "x" << dec << rs1 << ",x" << rs2 << ",0x" << setfill('0') << setw(8) << std:: right << std::hex << (imm_b + addr);

    return os.str();  
}

/**
 * @brief          This function will render the itype load instruction with the appropiate format calling the appropiate functions needed.
 *
 * @param insn 	   Address of the instruction
 * @param mnemonic mnemonic 
 *
 * @return         Will print the given itype load insturuction 
 ************************************************************************************************************************************************************/
string rv32i_decode::render_itype_load(uint32_t insn, const char *mnemonic)
{    

	ostringstream os;

	os << render_mnemonic(mnemonic) << render_reg(get_rd(insn)) << "," << render_base_disp(get_imm_i(insn), get_rs1(insn));

	return os.str();
}

/**
 * @brief          This function will render the stype load instruction with the appropiate format calling the appropiate functions needed.
 *
 * @param insn 	   Address of the instruction
 * @param mnemonic mnemonic 
 *
 * @return         Will print the given stype load insturuction 
 ************************************************************************************************************************************************************/
string rv32i_decode::render_stype(uint32_t insn, const char *mnemonic)
{
	ostringstream os;

	os << render_mnemonic(mnemonic) << render_reg(get_rs2(insn)) << "," << render_base_disp(get_imm_s(insn), get_rs1(insn));

	return os.str();
}

/**
 * @brief          This function will render the itype alu instruction with the appropiate format calling the appropiate functions needed.
 *
 * @param addr     Address passed of the simulated memory
 * @param insn 	   Address of the instruction
 * @param mnemonic mnemonic 
 *
 * @return         Will print the given itype alu
 ************************************************************************************************************************************************************/
string rv32i_decode::render_itype_alu(uint32_t insn, const char *mnemonic, int32_t imm_i)
{
	ostringstream os;

	os << render_mnemonic(mnemonic) << render_reg(get_rd(insn)) << "," << render_reg(get_rs1(insn)) << "," << imm_i;

	return os.str();
}

/**
 * @brief          This function will render the rtype load instruction with the appropiate format calling the appropiate functions needed.
 *
 * @param insn 	   Address of the instruction
 * @param mnemonic mnemonic 
 *
 * @return         Will print the given rtype load insturuction 
 ************************************************************************************************************************************************************/
string rv32i_decode::render_rtype(uint32_t insn, const char *mnemonic)
{

	ostringstream os;

	os << render_mnemonic(mnemonic) << render_reg(get_rd(insn)) << "," << render_reg(get_rs1(insn)) << "," << render_reg(get_rs2(insn));

	return os.str();
}

/**
 * @brief          This function will render the ecall calling the mnenomic 
 *
 * @param insn 	   Address of the instruction
 *
 * @return         Will print the ecall mnemonic 
 ************************************************************************************************************************************************************/
string rv32i_decode::render_ecall(uint32_t insn)
{
	return render_mnemonic("ecall");
}

/**
 * @brief          This function will render the ebreak calling the mnenomic 
 *
 * @param insn 	   Address of the instruction
 *
 * @return         Will print the ebreak mnemonic 
 ************************************************************************************************************************************************************/
string rv32i_decode::render_ebreak(uint32_t insn)
{

	return render_mnemonic("ebreak");
}

/**
 * @brief          This function will render the csrrx load instruction with the appropiate format calling the appropiate functions needed.
 *
 * @param insn 	   Address of the instruction
 * @param mnemonic mnemonic 
 *
 * @return         Will print the given csrrx load insturuction 
 ******************************************************************************************************************************************************************************************/
string rv32i_decode::render_csrrx(uint32_t insn, const char *mnemonic)
{
	ostringstream os;

	os << render_mnemonic(mnemonic) << render_reg(get_rd(insn)) << "," << HEXX::to_hex0x12(get_imm_i(insn)) << "," << render_reg(get_rs1(insn));

	return os.str();
}

/**
 * @brief          This function will render the csrxxi load instruction with the appropiate format calling the appropiate functions needed.
 *
 * @param insn 	   Address of the instruction
 * @param mnemonic mnemonic 
 *
 * @return         Will print the given csrxxi load insturuction 
 ******************************************************************************************************************************************************************************************/
string rv32i_decode::render_csrrxi(uint32_t insn, const char *mnemonic)
{
	ostringstream os;

	os << render_mnemonic(mnemonic) << render_reg(get_rd(insn)) << "," << "0x" << HEXX::to_hex0x12(get_imm_i(insn)) << "," << get_rs1(insn);

	return os.str();
}