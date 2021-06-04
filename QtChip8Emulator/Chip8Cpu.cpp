#include "Chip8Cpu.h"
#include <vector>
#include <cstdlib>
#include <time.h>
#include "BitUtilities.h"
Chip8Cpu::Chip8Cpu() {
	Reset();
	currentCpuConfiguration = Chip8Configuration { false, false, false };
}
Chip8Cpu::Chip8Cpu(Chip8Configuration configuration) {
	Reset();
	currentCpuConfiguration = configuration;
}

Chip8Cpu::~Chip8Cpu() {

}
void Chip8Cpu::SetSoundCallbacks(std::function<void(void)> soundStartCb, std::function<void(void)> soundStopCb) {
	soundStart = soundStartCb;
	soundStop = soundStopCb;
}
void Chip8Cpu::LoadRom(std::vector<unsigned char> buffer) {
	Reset();		
	currentRom.assign(buffer.begin(), buffer.end());
	memcpy(&memory[0x200], &buffer[0], buffer.size());
}
void Chip8Cpu::ResetAndReloadCurrentRom() {
	Reset();
	if (currentRom.size() > 0) {
		memcpy(&memory[0x200], &currentRom[0], currentRom.size());
	}
}
void Chip8Cpu::Reset() {
	srand(time(NULL));
	memset(&memory, 0, MemorySize);
	memcpy(&memory[FontStartAddress], font, sizeof(font));
	for (auto& pixel : screen) {
		pixel = false;
	}
	PC = 0x200;
	I = 0;
	while (!stack.empty()) { stack.pop_back(); }
	delayTimer = 0;
	soundTimer = 0;
	for (auto& key : keysDown) {
		key = false;
	}
	memset(registers, 0, sizeof(registers));
	soundPlaying = false;
}
void Chip8Cpu::Run(int nr_instructions) {
	for (int i = 0; i < nr_instructions; i++) {
		unsigned short opcode = FetchInstruction();
		ExecuteInstruction(opcode);
	}
}
unsigned short Chip8Cpu::FetchInstruction() {
	unsigned short opcode = ((unsigned short)memory[PC] << 8) | ((unsigned short)memory[PC + 1]);
	PC += 2;
	return opcode;
}
void Chip8Cpu::ExecuteInstruction(unsigned short opcode) {
	unsigned short firstNibble = GetNibble(opcode, 0);
	static int instructionCounter = 0;
	instructionCounter++;
	if (instructionCounter > 8) {
		if (delayTimer > 0) {
			delayTimer--;
		}
		if (soundTimer > 0) {
			soundTimer--;
		}
		instructionCounter = 0;
	}
	switch (firstNibble)
	{
	case 0x0:
		if (GetLastByte(opcode) == 0xe0) { //clear screen
			for (int i = 0; i < ScreenWidth * ScreenHeight; i++) { screen[i] = false; }
		}
		if (GetLastByte(opcode) == 0xee) { //return from sub
			auto address = stack.back();
			stack.pop_back();
			PC = address;
		}
		break;
	case 0x1: //JUMP
	{
		auto destination = GetLast12Bits(opcode);
		PC = destination;
	}
	break;
	case 0x2: //GOTO SUB
	{
		auto destination = GetLast12Bits(opcode);
		stack.push_back(PC);
		PC = destination;
	}
	break;
	case 0x3: //Skip if equal
	{
		auto registerIndex = GetNibble(opcode,1);
		auto cmpValue = GetLastByte(opcode);
		if (registers[registerIndex] == cmpValue) {
			PC+=2;
		}
	}
	break;
	case 0x4: //Skip if not equal
	{
		auto registerIndex = GetNibble(opcode, 1);
		auto cmpValue = GetLastByte(opcode);
		if (registers[registerIndex] != cmpValue) {
			PC+=2;
		}
	}
	break;
	case 0x5: //Skip if equal registers
	{
		auto registerIndex = GetNibble(opcode, 1);
		auto registerIndex2 = GetNibble(opcode, 2);		
		if (registers[registerIndex] == registers[registerIndex2]) {
			PC+= 2;
		}
	}
	break;
	case 0x9: //Skip if not equal registers
	{
		auto registerIndex = GetNibble(opcode, 1);
		auto registerIndex2 = GetNibble(opcode, 2);
		if (registers[registerIndex] != registers[registerIndex2]) {
			PC+= 2;
		}
	}
	break;
	case 0x6: //Set Register
	{
		auto registerIndex = GetNibble(opcode, 1);
		auto value = GetLastByte(opcode);
		registers[registerIndex] = value;
	}
	break;
	case 0x7: //Add value to register
	{
		auto registerIndex = GetNibble(opcode, 1);
		auto value = GetLastByte(opcode);
		registers[registerIndex] += value;
	}
	break;
	case 0x8: //logical instructions
	{
		switch (GetNibble(opcode, 3)) {
		case 0: //VX = VY
		{			
			auto registerIndexDest = GetNibble(opcode, 1);
			auto registerIndexSrc = GetNibble(opcode, 2);
			registers[registerIndexDest] = registers[registerIndexSrc];
		}
		break;
		case 1: //VX = VX | VY
		{
			auto registerIndexDest = GetNibble(opcode, 1);
			auto registerIndexSrc = GetNibble(opcode, 2);
			registers[registerIndexDest] = registers[registerIndexSrc] | registers[registerIndexDest];
		}
		break;
		case 2: //VX = VX & VY
		{
			auto registerIndexDest = GetNibble(opcode, 1);
			auto registerIndexSrc = GetNibble(opcode, 2);
			registers[registerIndexDest] = registers[registerIndexSrc] & registers[registerIndexDest];
		}
		break;
		case 3: //VX = VX ^ VY
		{
			auto registerIndexDest = GetNibble(opcode, 1);
			auto registerIndexSrc = GetNibble(opcode, 2);
			registers[registerIndexDest] = registers[registerIndexSrc] ^ registers[registerIndexDest];
		}
		break;
		case 4: //VX = VX + VY
		{
			auto registerIndexDest = GetNibble(opcode, 1);
			auto registerIndexSrc = GetNibble(opcode, 2);
			auto overflowCheck = (int)registers[registerIndexSrc] + (int)registers[registerIndexDest];
			if (overflowCheck > 255) {
				registers[0xF] = 1;
			}
			registers[registerIndexDest] = registers[registerIndexSrc] + registers[registerIndexDest];
		}
		break;
		case 5: //VX = VX - VY
		{
			auto registerIndexMinuend = GetNibble(opcode, 1);
			auto registerIndexSubtrahend = GetNibble(opcode, 2);
			if (registers[registerIndexMinuend] >= registers[registerIndexSubtrahend]) {
				registers[0xF] = 1;
			}
			auto overflowCheck = (int)registers[registerIndexMinuend] - (int)registers[registerIndexSubtrahend];
			if (overflowCheck < 0) {
				registers[0xF] = 0;
			}
			registers[registerIndexMinuend] = registers[registerIndexMinuend] - registers[registerIndexSubtrahend];
		}
		break;
		case 7: //VX = VY - VX
		{
			auto registerIndexMinuend = GetNibble(opcode, 2);
			auto registerIndexSubtrahend = GetNibble(opcode, 1);
			if (registers[registerIndexMinuend] >= registers[registerIndexSubtrahend]) {
				registers[0xF] = 1;
			}
			auto overflowCheck = (int)registers[registerIndexMinuend] - (int)registers[registerIndexSubtrahend];
			if (overflowCheck < 0) {
				registers[0xF] = 0;
			}
			registers[registerIndexSubtrahend] = registers[registerIndexMinuend] - registers[registerIndexSubtrahend];
		}
		break;
		case 6: //VX = VY
		{
			//VX = VY
			//VX = VX >> 1
			auto registerIndexLeft = GetNibble(opcode, 1);
			auto registerIndexRight = GetNibble(opcode, 2);			
			if (currentCpuConfiguration.ShiftSetsVX) {
				registers[registerIndexLeft] = registers[registerIndexRight];
			}
			if (registers[registerIndexLeft] & 0x1) {
				registers[0xF] = 1;
			}
			else {
				registers[0xF] = 0;
			}
			registers[registerIndexLeft] = registers[registerIndexLeft] >> 1;
		}
		break;
		case 0xE: //VX = VY
		{
			//VX = VY
			//VX = VX >> 1
			auto registerIndexLeft = GetNibble(opcode, 1);
			auto registerIndexRight = GetNibble(opcode, 2);
			if (currentCpuConfiguration.ShiftSetsVX) {
				registers[registerIndexLeft] = registers[registerIndexRight];
			}
			if (registers[registerIndexLeft] & 0x80) {
				registers[0xF] = 1;
			}
			else {
				registers[0xF] = 0;
			}
			registers[registerIndexLeft] = registers[registerIndexLeft] << 1;
		}
		break;
		default:
			throw new std::exception("Unknown instruction");
			break;
		}		
	}
	break;
	case 0xA: //Set index register
	{		
		auto value = GetLast12Bits(opcode);
		I = value;
	}
	break;
	case 0xB: //jump with offset
	{
		auto value = GetLast12Bits(opcode);
		auto offset = 0;
		if (currentCpuConfiguration.JumpSuperChip) {
			auto registerIndex = GetNibble(opcode, 1);
			offset = registers[registerIndex];
		}
		else {
			offset = registers[0];
		}
		PC = value + offset;
	}
	break;
	case 0xC: //RND
	{
		auto value = (char)(rand() % 255);
		auto registerIndex = GetNibble(opcode, 1);
		auto andValue = GetLastByte(opcode);
		registers[registerIndex] = value & andValue;
	}
	break;
	case 0xD: //Draw
	{
		auto spriteHeight = GetNibble(opcode, 3);
		auto spriteAddress = I;
		auto xregister = GetNibble(opcode, 1);
		auto yregister = GetNibble(opcode, 2);
		auto xpos = registers[xregister] % ScreenWidth;
		auto ypos = registers[yregister] % ScreenHeight;
		registers[0xf] = 0;
		for (int i = 0; i < spriteHeight; i++) {
			for (int j = 0; j < SpriteWidth; j++) {
				SetPixel(xpos + j, ypos + i, GetBit(memory[spriteAddress + i], j));
			}
		}	
	}
	case 0xE:
	{
		switch (GetLastByte(opcode))
		{
		case 0x9e: //Keydown
		{
			auto registerIndex = GetNibble(opcode, 1);
			if (keysDown[registers[registerIndex]]) {
				PC += 2;
			}
		}
		break;
		case 0xa1:
		{
			auto registerIndex = GetNibble(opcode, 1);
			if (!keysDown[registers[registerIndex]]) {
				PC += 2;
			}
		}
		break;
		}
	}
	break;	
	case 0xF: 
	{
		switch (GetLastByte(opcode)) 
		{
		case 0x07: // store delay timer
		{
			auto registerIndex = GetNibble(opcode, 1);
			registers[registerIndex] = delayTimer;
		}
		break;
		case 0x0A:
		{
			auto registerIndex = GetNibble(opcode, 1);
			PC -= 2;
			for (int i = 0; i < 0x10; i++) {
				if (keysDown[i]) {
					PC += 2;
					registers[registerIndex] = i;
				}
			}
		}
		break;
		case 0x15: //set delay timer
		{
			auto registerIndex = GetNibble(opcode, 1);
			delayTimer = registers[registerIndex];
		}
		break;		
		case 0x18: // set sound timer
		{
			auto registerIndex = GetNibble(opcode, 1);
			soundTimer = registers[registerIndex];
		}
		break;
		case 0x1e: //Add to index
		{
			auto registerIndex = GetNibble(opcode, 1);
			if (((int)I + (int)registers[registerIndex]) > 0xFFF) {
				registers[0xF] = 1;
			}
			I = I + registers[registerIndex];
		}
		break;
		case 0x29: //Load font character address
		{
			auto registerIndex = GetNibble(opcode, 1);
			auto fontCharacter = registers[registerIndex];
			I = FontStartAddress + fontCharacter * FontHeight;
		}
		break;
		case 0x33: //BCD
		{
			auto registerIndex = GetNibble(opcode, 1);
			auto value = registers[registerIndex];
			auto firstDigit = value / 100;
			auto secondDigit = value / 10 % 10;
			auto lastDigit = value % 100 % 10;
			memory[I] = (unsigned char)firstDigit;
			memory[I + 1] = (unsigned char)secondDigit;
			memory[I + 2] = (unsigned char)lastDigit;
		}
		break;
		case 0x55: //store registers in memory
		{
			auto registerIndex = GetNibble(opcode, 1);
			memcpy(&memory[I], registers, registerIndex + 1);
			if (currentCpuConfiguration.StoreIncreasesI) {
				I = I + registerIndex + 1;
			}
		}
		break;
		case 0x65: //store registers in memory
		{
			auto registerIndex = GetNibble(opcode, 1);
			memcpy(registers, &memory[I], registerIndex + 1);
			if (currentCpuConfiguration.StoreIncreasesI) {
				I = I + registerIndex + 1;
			}
		}
		break;
		}
	}
	break;
	default:
		throw new std::exception("unknown instruction");
		break;
	}
	if (soundTimer > 0 && !soundPlaying) {
		if (soundStart) {			
			soundStart();
			soundPlaying = true;
		}
	}
	if (soundTimer == 0 && soundPlaying) {
		if (soundStop) {			
			soundStop();
			soundPlaying = false;
		}
	}
}

void Chip8Cpu::SetPixel(unsigned char x, unsigned char y, bool enable) {
	if (x > ScreenWidth || y > ScreenHeight) {
		return;
	}
	if (enable) {
		if (screen[x + y * ScreenWidth] == true) {
			registers[0xf] = 1;
		}
		auto test = 0;
		screen[x + y * ScreenWidth] = !screen[x + y * ScreenWidth];
	}
}
char* Chip8Cpu::GetScreenBGRABuffer() {
	for (int i = 0; i < ScreenWidth * ScreenHeight; i++) {
		char color = 0;
		if (screen[i]) {
			color = 255;
		}
		bgraScreen[i * 4 + 0] = color;
		bgraScreen[i * 4 + 1] = color;
		bgraScreen[i * 4 + 2] = color;
		bgraScreen[i * 4 + 3] = 255;
	}
	return bgraScreen;
}
int Chip8Cpu::GetScreenBGRABufferSize() {
	return ScreenWidth * ScreenHeight * 4;
}
unsigned char* Chip8Cpu::GetMemory() {
	return memory;
}
void Chip8Cpu::SetKey(unsigned char key, bool down) {
	keysDown[key] = down;
}