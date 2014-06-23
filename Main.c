/** @file Main.c
 * Chip-8 simple disassembler.
 * @author Adrien RICCIARDI
 * @version 1.0 : 09/04/2014
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

//-------------------------------------------------------------------------------------------------
// Private constants
//-------------------------------------------------------------------------------------------------
/** Default program entry point. */
#define PROGRAM_DEFAULT_ENTRY_POINT 0x0200

//-------------------------------------------------------------------------------------------------
// Private functions
//-------------------------------------------------------------------------------------------------
/** Disassemble a single instruction.
 * @param Instruction The instruction to disassemble.
 * @param String_Disassembled_Instruction On output, will hold the disassembled instruction corresponding text. Make sure the string is large enough.
 * @return 1 if the instruction was correctly disassembled,
 * @return 0 if the instruction was unknown.
 */
static int DisassembleInstruction(unsigned short Instruction, char *String_Disassembled_Instruction)
{
	unsigned short Address;
	unsigned char Opcode, High_Nibble, Middle_Nibble, Low_Nibble, Byte;

	// Precompute all existing fields to simplify the decoding machine
	Opcode = Instruction >> 12;
	Address = Instruction & 0x0FFF; // Low 12 bits
	Byte = (unsigned char) Instruction; // Low 8 bits
	High_Nibble = (Instruction >> 8) & 0x0F; // Bits 11 to 8
	Middle_Nibble = (Instruction >> 4) & 0x0F; // Bits 7 to 4
	Low_Nibble = Instruction & 0x0F; // Bits 3 to 0

	switch (Opcode)
	{
		case 0:
			// CLS
			if ((High_Nibble == 0) && (Byte == 0xE0)) strcpy(String_Disassembled_Instruction, "CLS");
			// RET
			else if ((High_Nibble == 0) && (Byte == 0xEE)) strcpy(String_Disassembled_Instruction, "RET");
			// SYS addr
			else  sprintf(String_Disassembled_Instruction, "SYS 0x%03X", Address);
			break;

		// JP addr
		case 1:
			sprintf(String_Disassembled_Instruction, "JP 0x%03X", Address);
			break;

		// CALL addr
		case 2:
			sprintf(String_Disassembled_Instruction, "CALL 0x%03X", Address);
			break;

		// SE Vx, byte
		case 3:
			sprintf(String_Disassembled_Instruction, "SE V%1X, 0x%02X (%d)", High_Nibble, Byte, Byte);
			break;

		// SNE Vx, byte
		case 4:
			sprintf(String_Disassembled_Instruction, "SNE V%1X, 0x%02X (%d)", High_Nibble, Byte, Byte);
			break;

		// SE Vx, Vy
		case 5:
			if (Low_Nibble != 0) return 0;
			sprintf(String_Disassembled_Instruction, "SE V%1X, V%1X", High_Nibble, Middle_Nibble);
			break;

		// LD Vx, byte
		case 6:
			sprintf(String_Disassembled_Instruction, "LD V%1X, 0x%02X (%d)", High_Nibble, Byte, Byte);
			break;

		// ADD Vx, byte
		case 7:
			sprintf(String_Disassembled_Instruction, "ADD V%1X, 0x%02X (%d)", High_Nibble, Byte, Byte);
			break;

		case 8:
			switch (Low_Nibble)
			{
				// LD Vx, Vy
				case 0:
					sprintf(String_Disassembled_Instruction, "LD V%1X, V%1X", High_Nibble, Middle_Nibble);
					break;

				// OR Vx, Vy
				case 1:
					sprintf(String_Disassembled_Instruction, "OR V%1X, V%1X", High_Nibble, Middle_Nibble);
					break;

				// AND Vx, Vy
				case 2:
					sprintf(String_Disassembled_Instruction, "AND V%1X, V%1X", High_Nibble, Middle_Nibble);
					break;

				// XOR Vx, Vy
				case 3:
					sprintf(String_Disassembled_Instruction, "XOR V%1X, V%1X", High_Nibble, Middle_Nibble);
					break;

				// ADD Vx, Vy
				case 4:
					sprintf(String_Disassembled_Instruction, "ADD V%1X, V%1X", High_Nibble, Middle_Nibble);
					break;

				// SUB Vx, Vy
				case 5:
					sprintf(String_Disassembled_Instruction, "SUB V%1X, V%1X", High_Nibble, Middle_Nibble);
					break;

				// SHR Vx, Vy
				case 6:
					sprintf(String_Disassembled_Instruction, "SHR V%1X, V%1X", High_Nibble, Middle_Nibble);
					break;

				// SUBN Vx, Vy
				case 7:
					sprintf(String_Disassembled_Instruction, "SUBN V%1X, V%1X", High_Nibble, Middle_Nibble);
					break;

				// SHL Vx, Vy
				case 0xE:
					sprintf(String_Disassembled_Instruction, "SHL V%1X, V%1X", High_Nibble, Middle_Nibble);
					break;

				// Unknown instructions
				default:
					return 0;
			}
			break;

		// SNE Vx, Vy
		case 9:
			if (Low_Nibble != 0) return 0;
			sprintf(String_Disassembled_Instruction, "SNE V%1X, V%1X", High_Nibble, Middle_Nibble);
			break;

		// LD I, addr
		case 0xA:
			sprintf(String_Disassembled_Instruction, "LD I, 0x%03X", Address);
			break;

		// JP V0, addr
		case 0xB:
			sprintf(String_Disassembled_Instruction, "JP V0, 0x%03X", Address);
			break;

		// RND Vx, byte
		case 0xC:
			sprintf(String_Disassembled_Instruction, "RND V%1X, 0x%02X (%d)", High_Nibble, Byte, Byte);
			break;

		// DRW Vx, Vy, nibble
		case 0xD:
			sprintf(String_Disassembled_Instruction, "DRW V%1X, V%1X, 0x%1X", High_Nibble, Middle_Nibble, Low_Nibble);
			break;

		case 0xE:
			switch (Byte)
			{
				// SKP Vx
				case 0x9E:
					sprintf(String_Disassembled_Instruction, "SKP V%1X", High_Nibble);
					break;

				// SKNP Vx
				case 0xA1:
					sprintf(String_Disassembled_Instruction, "SKNP V%1X", High_Nibble);
					break;

				// Unknown instructions
				default:
					return 0;
			}
			break;

		case 0xF:
			switch (Byte)
			{
				// LD Vx, DT
				case 0x07:
					sprintf(String_Disassembled_Instruction, "LD V%1X, DT", High_Nibble);
					break;

				// LD Vx, K
				case 0x0A:
					sprintf(String_Disassembled_Instruction, "LD V%1X, K", High_Nibble);
					break;

				// LD DT, Vx
				case 0x15:
					sprintf(String_Disassembled_Instruction, "LD DT, V%1X", High_Nibble);
					break;

				// LD ST, Vx
				case 0x18:
					sprintf(String_Disassembled_Instruction, "LD ST, V%1X", High_Nibble);
					break;

				// ADD I, Vx
				case 0x1E:
					sprintf(String_Disassembled_Instruction, "ADD I, V%1X", High_Nibble);
					break;

				// LD F, Vx
				case 0x29:
					sprintf(String_Disassembled_Instruction, "LD F, V%1X", High_Nibble);
					break;

				// LD B, Vx
				case 0x33:
					sprintf(String_Disassembled_Instruction, "LD B, V%1X", High_Nibble);
					break;

				// LD [I], Vx
				case 0x55:
					sprintf(String_Disassembled_Instruction, "LD [I], V%1X", High_Nibble);
					break;

				// LD Vx, [i]
				case 0x65:
					sprintf(String_Disassembled_Instruction, "LD V%1X, [I]", High_Nibble);
					break;

				// Unknown instructions
				default:
					return 0;
			}
			break;

		// Unknown instructions
		default:
			return 0;
	}

	return 1;
}

int main(int argc, char *argv[])
{
	FILE *File;
	unsigned short Instruction;
	char String_Disassembled_Instruction[64];
	int Instruction_Address = PROGRAM_DEFAULT_ENTRY_POINT;

	// Check parameters
	if (argc != 2)
	{
		printf("Error : bad parameters.\nUsage : %s programFileToDisassemble\n", argv[0]);
		return EXIT_FAILURE;
	}

	// Try to open the file
	File = fopen(argv[1], "rb");
	if (File == NULL)
	{
		printf("Error : could not load program '%s'.\n", argv[1]);
		return EXIT_FAILURE;
	}

	// Disassemble one instruction at a time
	while (1)
	{
		// Read next instruction
		if (fread(&Instruction, sizeof(Instruction), 1, File) != 1)
		{
			// End of file has been reached
			fclose(File);
			return EXIT_SUCCESS;
		}
		// Convert instruction to local system endianness
		Instruction = ntohs(Instruction);

		// Disassemble it
		if (!DisassembleInstruction(Instruction, String_Disassembled_Instruction)) strcpy(String_Disassembled_Instruction, "???");

		// Display instruction
		printf("0x%03X:\t0x%04X\t%s\n", Instruction_Address, Instruction, String_Disassembled_Instruction);
		Instruction_Address++;
	}
}
