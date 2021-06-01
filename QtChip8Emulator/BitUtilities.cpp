unsigned char GetNibble(unsigned short opcode, int nibble) {
	switch (nibble) {
	case 0:
		return (opcode & 0xF000) >> 12;
	case 1:
		return (opcode & 0x0F00) >> 8;
	case 2:
		return (opcode & 0x00F0) >> 4;
	case 3:
		return (opcode & 0x000F);
	}
}
unsigned char GetLastByte(unsigned short opcode) {
	return (opcode & 0x00FF);
}
unsigned short GetLast12Bits(unsigned short opcode) {
	return (opcode & 0x0FFF);
}
bool GetBit(unsigned char value, char bitIndex) {
	return (value >> (7 - bitIndex)) & 1;
}