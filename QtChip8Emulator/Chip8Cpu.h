#pragma once
#include <deque>
#include <vector>
#include <functional>
static const char font[] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
struct Chip8Configuration {
	bool ShiftSetsVX;
	bool JumpSuperChip;
	bool StoreIncreasesI;
};
class Chip8Cpu
{
public:
	Chip8Cpu();
	Chip8Cpu(Chip8Configuration configuration);
	~Chip8Cpu();
	void LoadRom(std::vector<unsigned char> buffer);
	void Reset();
	void ResetAndReloadCurrentRom();
	void Run(int nr_instructions);
	void SetKey(unsigned char key, bool down);	
	char* GetScreenBGRABuffer();
	int GetScreenBGRABufferSize();
	unsigned char* GetMemory();
	void SetSoundCallbacks(std::function<void(void)> soundStartCb, std::function<void(void)> soundStopCb);
	static const int MemorySize = 4 * 1024;
	static const unsigned char ScreenWidth = 64;
	static const unsigned char ScreenHeight = 32;
	static const unsigned char SpriteWidth = 8;
	static const unsigned char FontHeight = 5;
	static const int FontStartAddress = 0x50;

	Chip8Configuration currentCpuConfiguration;

	unsigned int PC;
	unsigned short I;
	std::deque<unsigned short> stack;
	unsigned char registers[0x10]; //16 gp registers
	

private:
	void ExecuteInstruction(unsigned short opcode);
	unsigned short FetchInstruction();
	void SetPixel(unsigned char x, unsigned char y, bool enable);

	bool keysDown[0x10]; //16 keys
	unsigned char memory[MemorySize]; //4k memory
	bool screen[ScreenWidth * ScreenHeight];
	char bgraScreen[ScreenWidth * ScreenHeight * 4];
	std::vector<unsigned char> currentRom;
	unsigned char delayTimer;
	unsigned char soundTimer;
	bool soundPlaying = false;
	std::function<void(void)> soundStart;
	std::function<void(void)> soundStop;
};

