#pragma once

unsigned char GetNibble(unsigned short opcode, int nibble);
unsigned char GetLastByte(unsigned short opcode);
unsigned short GetLast12Bits(unsigned short opcode);
bool GetBit(unsigned char value, char bitIndex);