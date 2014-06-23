all:
	gcc Main.c -W -Wall -o Chip8_Disassembler

clean:
	rm -rf Chip8_Disassembler
