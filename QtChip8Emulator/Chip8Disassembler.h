#pragma once
#include <vector>
#include <string>
#include <map>
#include "DisassembledLine.h"

class Chip8Disassembler
{
private:
	const std::map<unsigned short, std::string> instructionLookup
	{ // based on ASankaran/CHIP-Disassembler table
		{ 0x00E0 , "CLS "},
		{ 0x00EE , "RET "},
		{ 0x0000 , "SYS "},
		{ 0x1000 , "JP  "},
		{ 0x2000 , "CALL"},
		{ 0x3000 , "SE  "},
		{ 0x4000 , "SNE "},
		{ 0x5000 , "SE  "},
		{ 0x6000 , "LD  "},
		{ 0x7000 , "ADD "},
		{ 0x8000 , "LD  "},
		{ 0x8001 , "OR  "},
		{ 0x8002 , "AND "},
		{ 0x8003 , "XOR "},
		{ 0x8004 , "ADD "},
		{ 0x8005 , "SUB "},
		{ 0x8006 , "SHR "},
		{ 0x8007 , "SUBN"},
		{ 0x800E , "SHL "},
		{ 0x9000 , "SNE "},
		{ 0xA000 , "LD  "},
		{ 0xB000 , "JP  "},
		{ 0xC000 , "RND "},
		{ 0xD000 , "DRW "},
		{ 0xE09E , "SKP "},
		{ 0xE0A1 , "SKNP"},
		{ 0xF007 , "LD  "},
		{ 0xF00A , "LD  "},
		{ 0xF015 , "LD  "},
		{ 0xF018 , "LD  "},
		{ 0xF01E , "ADD "},
		{ 0xF029 , "LDF  "},
		{ 0xF033 , "LD  "},
		{ 0xF055 , "LD  "},
		{ 0xF065 , "LD  "}
	};
public:
	Chip8Disassembler();
	std::vector<DisassembledLine> Disassemble(unsigned char* buffer, int bufferSize);
	std::string GetArguments(unsigned short opcode);
};

