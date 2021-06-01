#include "Chip8Disassembler.h"
#include "BitUtilities.h"
#include <sstream>
#include <ios>
#include <iomanip>

Chip8Disassembler::Chip8Disassembler()
{
}

std::vector<DisassembledLine> Chip8Disassembler::Disassemble(unsigned char* buffer, int bufferSize)
{
	std::vector<DisassembledLine> disassembly;
	for (int PC = 0; PC < bufferSize; PC += 2) {
		unsigned short opcode = ((unsigned short)buffer[PC] << 8) | ((unsigned short)buffer[PC + 1]);
		unsigned short maskedOpcode = 0;
		if ((opcode & 0xf000) == 0x0000 ||
			(opcode & 0xf000) == 0xe000 ||
			(opcode & 0xf000) == 0xf000) {
			maskedOpcode = opcode & 0xf0ff;
		}
		else if ((opcode & 0xf000) == 0x8000) {
			maskedOpcode = opcode & 0xf00f;
		}
		else {
			maskedOpcode = opcode & 0xf000;
		}
		DisassembledLine line;

		std::string instruction = "???";
		if (instructionLookup.find(maskedOpcode) != instructionLookup.end()) {
			instruction = instructionLookup.at(maskedOpcode);
		}
		std::string arguments = GetArguments(opcode);
		line.address = PC;
		char strbuf[100];
		snprintf(strbuf, 100, "%04X", opcode);
		line.byteString = std::string(strbuf);
		line.disassembly = instruction + " " + arguments;
		disassembly.push_back(line);
	}

	return disassembly;
}
std::string Chip8Disassembler::GetArguments(unsigned short opcode) {
	unsigned short firstNibble = GetNibble(opcode, 0);
	std::stringstream argumentsStream;
	argumentsStream << " ";
	switch (firstNibble)
	{
	case 0x0:
	{
		
	}
	break;
	case 0x1: //JUMP
	{
		auto destination = GetLast12Bits(opcode);
		argumentsStream << std::uppercase << std::hex << std::setfill('0') << std::setw(4) << destination;
	}
	break;
	case 0x2: //GOTO SUB
	{
		auto destination = GetLast12Bits(opcode);
		argumentsStream << std::uppercase << std::hex << std::setfill('0') << std::setw(4) << destination;
	}
	break;
	case 0x3:
	case 0x4:
	{
		auto registerIndex = GetNibble(opcode, 1);
		auto value = GetLastByte(opcode);
		argumentsStream << std::hex << std::uppercase << "V" << (unsigned short)registerIndex << ", " << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << (unsigned short)value;
	}
	break;
	case 0x5:
	case 0x9:
	{
		auto registerIndex = GetNibble(opcode, 1);
		auto registerIndex2 = GetNibble(opcode, 2);
		argumentsStream << std::hex << std::uppercase << "V" << (unsigned short)registerIndex << ", V" << std::uppercase << std::hex << (unsigned short)registerIndex2;
	}
	break;	
	break;
	case 0x6: //Set Register
	{
		auto registerIndex = GetNibble(opcode, 1);
		auto value = GetLastByte(opcode);
		argumentsStream << std::hex << std::uppercase << "V" << (unsigned short)registerIndex << ", " << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << (unsigned short)value;
	}
	break;
	case 0x7: //Add value to register
	{
		auto registerIndex = GetNibble(opcode, 1);
		auto value = GetLastByte(opcode);
		argumentsStream << std::hex << std::uppercase << "V" << (unsigned short)registerIndex << ", " << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << (unsigned short)value;
	}
	break;
	case 0x8:
	{				
		auto registerIndex = GetNibble(opcode, 1);
		auto registerIndex2 = GetNibble(opcode, 2);
		argumentsStream << std::hex << std::uppercase << "V" << (unsigned short)registerIndex << ", V" << std::uppercase << std::hex << (unsigned short)registerIndex2;
	}
	break;
	case 0xA: //Set index register
	{
		auto value = GetLast12Bits(opcode);
		argumentsStream << "I, " << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << (unsigned short)value;
	}
	break;
	case 0xB: //Set index register
	{
		auto value = GetLast12Bits(opcode);		
		argumentsStream << std::uppercase << std::hex << std::setfill('0') << std::setw(3) << (unsigned short)value << " + " << "V0";
	}
	break;
	case 0xC:
	{
		auto registerIndex = GetNibble(opcode, 1);
		auto andValue = GetLastByte(opcode);
		argumentsStream << std::hex << std::uppercase << "V" << (unsigned short)registerIndex << ", " << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << (unsigned short)andValue;
	}
	break;
	case 0xD: //Draw
	{
		auto spriteHeight = GetNibble(opcode, 3);
		auto xregister = GetNibble(opcode, 1);
		auto yregister = GetNibble(opcode, 2);
		argumentsStream << std::hex << std::uppercase << (unsigned short)spriteHeight << ", " << std::hex << std::uppercase << "V" << (unsigned short)xregister << ", " << std::hex << std::uppercase << "V" << (unsigned short)yregister;
	}
	break;
	case 0xE:
	{
		switch (GetLastByte(opcode)) 
		{
		case 0x9e:
		case 0xa1:
		{
			auto registerIndex = GetNibble(opcode, 1);
			argumentsStream << std::hex << std::uppercase << "V" << (unsigned short)registerIndex;
		}
		break;
		}
	}
	break;	
	case 0xF:
	{
		switch (GetLastByte(opcode))
		{
		case 0x07:
		{
			auto registerIndex = GetNibble(opcode, 1);
			argumentsStream << std::hex << std::uppercase << "V" << (unsigned short)registerIndex << ", DT";
		}
		break;
		case 0x15:
		{
			auto registerIndex = GetNibble(opcode, 1);
			argumentsStream << "DT, " << std::hex << std::uppercase << "V" << (unsigned short)registerIndex;
		}
		break;
		case 0x18:
		{
			auto registerIndex = GetNibble(opcode, 1);
			argumentsStream << "ST, " << std::hex << std::uppercase << "V" << (unsigned short)registerIndex;
		}
		break;
		case 0x0A:
		{
			auto registerIndex = GetNibble(opcode, 1);
			argumentsStream << std::hex << std::uppercase << "V" << (unsigned short)registerIndex;
		}
		break;
		case 0x1e:
		{
			auto registerIndex = GetNibble(opcode, 1);
			argumentsStream << "I, " << std::hex << std::uppercase << "V" << (unsigned short)registerIndex;
		}
		break;
		case 0x29:
		{
			auto registerIndex = GetNibble(opcode, 1);
			argumentsStream << "I, " << std::hex << std::uppercase << "V" << (unsigned short)registerIndex;
		}
		break;
		case 0x33:
		{
			auto registerIndex = GetNibble(opcode, 1);
			argumentsStream << "BCD, " << std::hex << std::uppercase << "V" << (unsigned short)registerIndex;
		}
		break;
		case 0x55:		
		{
			auto registerIndex = GetNibble(opcode, 1);
			argumentsStream << "MEM[I], " << std::hex << std::uppercase << "V" << (unsigned short)registerIndex;
		}
		break;
		case 0x65:
		{
			auto registerIndex = GetNibble(opcode, 1);
			argumentsStream << std::hex << std::uppercase << "V" << (unsigned short)registerIndex << ", MEM[I]";
		}
		break;
		}
	}
	break;
	}
	std::string argumentsString = argumentsStream.str();
	return argumentsString;
}
