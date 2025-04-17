//----------------------------------------
// c_training_p1.c
//
//		Outputs the hexadeimal of the EBCDIC
//		equivalent of a given ASCII character.
//
//		Copyright (c) 1998 Universal Computer Consulting, Inc.
//
//		VMOD#		PROJECT#		DATE		DESCRIPTION OF CHANGE
//		--------------------------------------------------------------------------
//		1.5		##MDV#####	03/25/25	Added command line argument functionality
//												& switched to dynamic memory allocation.
//		1.4		##MDV#####	03/18/25	Added Ascii Hex input option & a feature
//												to continously use same input stream until
//												user decides otherwise.
//		1.3		##MDV#####	03/17/25	Added a menu for reproducibility &
//												EBCDIC to Ascii conversion.
//		1.2		##MDV#####	03/14/25	Change to allow multiple characters
//												And to output in 16-wide hexdump format.
//		1.0		##MDV#####	03/12/25	Original creation.
//

#include <uccpr.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH				59
#define INPUT_MAX_LINE_LENGTH		16
#define HEX_LINE_LENGTH				40
#define MAX_INPUT_SIZE				2048
#define FIRST_CHAR					0
#define ODD								2
#define COMMAND						1
#define WORD							8
#define MAX_VALUE						256
#define FILENAME_START				3
#define EXTENSION_LENGTH			4

unsigned char* g_lpszInputBuffer = NULL; // buffer for all input
size_t			g_uiInputBufferSize = 0;
size_t			g_uiCurrentBufferSize = MAX_INPUT_SIZE;

char* g_alpszAsciiToEbcdic[MAX_VALUE] =  // ASCII to EBCDIC conversion table (key : ASCII, value : EBCDIC)
{
	"00", "40", "40", "40", "40", "40", "40", "40", "40", "40", "25", "40", "40", "0D", "40", "40",
	"40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40",
	"40", "4F", "7F", "7B", "5B", "6C", "50", "7D", "4D", "5D", "5C", "4E", "6B", "60", "4B", "61",
	"F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "7A", "5E", "4C", "7E", "6E", "6F",
	"7C", "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "D1", "D2", "D3", "D4", "D5", "D6",
	"D7", "D8", "D9", "E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9", "4A", "E0", "5A", "5F", "6D",
	"79", "81", "82", "83", "84", "85", "86", "87", "88", "89", "91", "92", "93", "94", "95", "96",
	"97", "98", "99", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "8B", "6A", "9B", "A1", "40",
	"40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40",
	"40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40",
	"40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40",
	"40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40", "40",
	"41", "42", "43", "44", "45", "40", "40", "46", "47", "48", "51", "52", "53", "54", "55", "56",
	"40", "57", "58", "59", "62", "63", "64", "40", "40", "65", "66", "67", "68", "69", "40", "40",
	"71", "72", "76", "74", "75", "40", "40", "76", "77", "78", "AA", "AB", "AC", "AE", "AF", "B1",
	"40", "B2", "B3", "B4", "B5", "B6", "B7", "40", "40", "B8", "B9", "BA", "BB", "BC", "40", "40"
};

char* g_alpszEbcdicToAscii[MAX_VALUE] = // EBCDIC to ASCII conversion table (key : EBCDIC, value : ASCII)
{
	"00", "01", "02", "03", "20", "09", "20", "7F", "20", "20", "20", "0B", "0C", "0D", "0E", "0F",
	"10", "11", "12", "13", "20", "20", "08", "17", "18", "19", "20", "20", "1C", "1D", "1E", "1F",
	"20", "20", "20", "20", "20", "0A", "17", "1B", "20", "20", "20", "20", "20", "05", "06", "07",
	"20", "20", "16", "20", "20", "20", "20", "04", "20", "20", "20", "20", "14", "15", "20", "1A",
	"20", "C0", "C1", "C2", "C3", "C4", "C7", "C8", "C9", "20", "5B", "2E", "3C", "28", "2B", "21",
	"26", "CA", "CB", "CC", "CD", "CE", "CF", "D1", "D2", "D3", "5D", "24", "2A", "29", "3B", "5E",
	"2D", "2F", "D4", "D5", "D6", "D9", "DA", "DB", "DC", "DD", "7C", "2C", "25", "5F", "3E", "3F",
	"20", "E0", "E1", "E2", "E3", "E4", "E7", "E8", "E9", "60", "3A", "23", "40", "27", "3D", "22",
	"20", "61", "62", "63", "64", "65", "66", "67", "68", "69", "20", "7B", "20", "20", "20", "20",
	"20", "6A", "6B", "6C", "6D", "6E", "6F", "70", "71", "72", "20", "7D", "20", "20", "20", "20",
	"20", "7E", "73", "74", "75", "76", "77", "78", "79", "7A", "EA", "EB", "EC", "5B", "ED", "EE",
	"20", "EF", "F1", "F2", "F3", "F4", "F5", "F6", "F9", "FA", "FB", "FC", "FD", "5D", "20", "60",
	"7B", "41", "42", "43", "44", "45", "46", "47", "48", "49", "20", "20", "20", "20", "20", "20",
	"7D", "4A", "4B", "4C", "4D", "4E", "4F", "50", "51", "52", "20", "20", "20", "20", "20", "20",
	"5C", "20", "53", "54", "55", "56", "57", "58", "59", "5A", "20", "20", "20", "20", "20", "20",
	"30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "20", "20", "20", "20", "20", "20"
};

//----------------------------------------
// EbcdicToAsciiChars
//
//		Converts EBCDIC hex or Ascii hex within
//		g_achInputBuffer to ASCII characters.
//
//		Args:
//			EbcdicHex	= 1 if EBCDIC Hex, 0 if ASCII Hex
//
void HexToAsciiChars(int nEbcdicHex)
{
	size_t	uiInputIndex;
	size_t	uiNewInputBufferIndex = 0;

	//----------------------------------------
	// error message - odd hex input
	//
	if (g_uiInputBufferSize % ODD == TRUE)
	{
		printf("\n ERROR: Invalid HEX input (non-even) \n");
		scanf("%c");
	}

	for (uiInputIndex = 0; uiInputIndex < g_uiInputBufferSize; uiInputIndex += 2)
	{
		char					achSmallBuffer[HEX_LINE_LENGTH];
		size_t				uiHexVal;
		unsigned char		ucTempBuffer;

		//----------------------------------------
		// error message - space detected in input
		//
		if (g_lpszInputBuffer[uiInputIndex] == ' ' || g_lpszInputBuffer[uiInputIndex + 1] == ' ')
		{
			printf("\n ERROR: Invalid HEX input (space detected) \n");
			scanf("%c");
		}

		achSmallBuffer[0] = g_lpszInputBuffer[uiInputIndex];
		achSmallBuffer[1] = g_lpszInputBuffer[uiInputIndex + 1];
		achSmallBuffer[2] = '\0';

		uiHexVal = strtol(achSmallBuffer, NULL, 16);

		if (nEbcdicHex)
		{
			char* lpszTempString = g_alpszEbcdicToAscii[uiHexVal];
			ucTempBuffer = (unsigned char)strtol(lpszTempString, NULL, 16);
		}
		else
		{
			ucTempBuffer = (unsigned char)uiHexVal;
		}

		g_lpszInputBuffer[uiNewInputBufferIndex++] = ucTempBuffer;
	}
	g_uiInputBufferSize = uiNewInputBufferIndex;
	g_lpszInputBuffer[uiNewInputBufferIndex++] = '\0';
}

//----------------------------------------
// PrintFormat
//
//		formats last line of output & 
//		prints result.
//
//		Args:
//			nlpInputCharOutputIndex	= index of input chars
//												that need to be printed.
//			nlpOutputBufferIndex		= index of output buffer.
//			lpachOutputBuffer			= output buffer.
//			lpszOutputFile				= file to write Output Buffer to.
//
void PrintFormat(size_t* puiInputCharOutputIndex, size_t* puiOutputBufferIndex, char* lpszOutputBuffer, char* lpszOutputFile)
{
	if (*puiInputCharOutputIndex < g_uiInputBufferSize)
	{
		int	nCharsInLastLine = *puiOutputBufferIndex % MAX_LINE_LENGTH;
		int	nLastLineSpaces = HEX_LINE_LENGTH - nCharsInLastLine + 2;
		int	nInputIndex;
		
		for (nInputIndex = 0; nInputIndex < nLastLineSpaces; nInputIndex++)
		{
			lpszOutputBuffer[(*puiOutputBufferIndex)++] = ' ';
		}

		while (*puiInputCharOutputIndex < g_uiInputBufferSize)
		{
			lpszOutputBuffer[(*puiOutputBufferIndex)++] = g_lpszInputBuffer[(*puiInputCharOutputIndex)++];
		}
	}
	
	lpszOutputBuffer[*puiOutputBufferIndex] = '\0';

	if (!lpszOutputFile)
	{
		printf("\n%s\n", lpszOutputBuffer);
	}
	else
	{
		FILE* pOutputFile = fopen(lpszOutputFile, "a");
		fprintf(pOutputFile, "%s\n", lpszOutputBuffer);
		fclose(pOutputFile);
	}
}

//----------------------------------------
// ConvertFormatPrint
//
//		Convert Ascii hex to EBCDIC hex or EBCDIC hex 
//		to Ascii hex. Prints output in 16-wide hexdump
//		format.
//
//		Args:
//			lpszTable				= determines type of conversion 
//											ASCII->EBCDIC or EBCDIC->ASCII.
//			lpszOutputFileName	= output file name.
//
void ConvertFormatPrint(char* lpszInputType, char* lpszOutputFileName)
{
	char*		lpszOutputBuffer;
	size_t	uiOutputBufferIndex = 0;
	size_t	uiInputIndex;
	size_t	uiInputCharOutputIndex = 0;

	//----------------------------------------
	// BufferSize - 1 to account for line feed,
	// Handles console input.
	//
	if (!lpszOutputFileName)
	{
		printf("\n%s Input: ", lpszInputType);
		fgets((char*)g_lpszInputBuffer, MAX_INPUT_SIZE, stdin);
		g_uiInputBufferSize = strlen((char*)g_lpszInputBuffer) - 1;
	}

	//----------------------------------------
	// Ensures enough space in OutputBuffer
	//
	lpszOutputBuffer = (char*)malloc(g_uiInputBufferSize * WORD);
	if (!lpszOutputBuffer)
	{
		printf("\nERROR: achOutputBuffer in ConvertFormatPrint() failed memory allocation.\n");
		exit(1);
	}

	//----------------------------------------
	// Handle EBCDIC hex or Ascii hex 
	// 
	if (strcmp(lpszInputType, "EBCDIC") == 0 || strcmp(lpszInputType, "AsciiHex") == 0)
	{
		HexToAsciiChars(strcmp(lpszInputType, "EBCDIC") == 0 ? 1 : 0);
	}

	for (uiInputIndex = 0; uiInputIndex < g_uiInputBufferSize; uiInputIndex++)
	{
		unsigned char	ucTempBuffer = g_lpszInputBuffer[uiInputIndex];
		char* lpszTempString = g_alpszAsciiToEbcdic[(int)ucTempBuffer];

		lpszOutputBuffer[uiOutputBufferIndex++] = lpszTempString[0];
		lpszOutputBuffer[uiOutputBufferIndex++] = lpszTempString[1];
		//----------------------------------------
		// Every other hex value gets a space,
		// Every 4 hex pairs gets an extra space.
		// 
		if (uiInputIndex % ODD == TRUE)
		{
			lpszOutputBuffer[uiOutputBufferIndex++] = ' ';


			if ((uiInputIndex + 1) % WORD == 0)
			{
				lpszOutputBuffer[uiOutputBufferIndex++] = ' ';
			}
		}

		//----------------------------------------
		// prints 16 input characters after 8 hex 
		// pairs have already been printed.
		//
		if ((uiInputIndex + 1) % INPUT_MAX_LINE_LENGTH == 0)
		{
			int nInputCharOutputBound = uiInputCharOutputIndex + INPUT_MAX_LINE_LENGTH;
			while (uiInputCharOutputIndex < nInputCharOutputBound)
			{
				lpszOutputBuffer[uiOutputBufferIndex++] = g_lpszInputBuffer[uiInputCharOutputIndex++];
			}

			lpszOutputBuffer[uiOutputBufferIndex++] = '\n';
		}
	}

	PrintFormat(&uiInputCharOutputIndex, &uiOutputBufferIndex, lpszOutputBuffer, lpszOutputFileName);
	free(lpszOutputBuffer);
}

//----------------------------------------
// ClearInputBuffer
//
//		Clears standard input buffer.
//
void ClearInputBuffer()
{
	char chTemp;
	while ((chTemp = getchar()) != '\n' && chTemp != EOF)
	{
	}
}

//----------------------------------------
// PromptUser
//
//		Prompts user if they want to continue to
//		use same input format or exit to main menu.
//
//		Return:
//			int	=	1 if continue, 0 if exit
//
int PromptUser()
{
	int nAnswer;

	while (TRUE)
	{
		printf("\n- (1 to continue with format, 0 for main menu) - \n Input: ");
		scanf("%d", &nAnswer);

		//----------------------------------------
		// getchar() pops newline char,
		// error message - invalid input.
		// Inner while loop clears buffer.
		//
		getchar();
		if (nAnswer != 1 && nAnswer != 0)
		{
			printf("\nERROR: Invalid Input. Please try again.\n");
			ClearInputBuffer();
		}
		else
		{
			break;
		}
	}
	return nAnswer;
}

//----------------------------------------
// ReadInputFile
//
//		reads input file and stores in global buffer.
//
//		Args:
//			lpszInputFile	= file to read from.
//			lpszOutputFile = file to write to.
//			lpszInputType	= type of input (Ascii, EBCDIC, Ascii Hex)
//
void ReadInputFile(char* lpszInputFile, char* lpszOutputFile, char* lpszInputType)
{
	FILE* pInputFile;
	size_t nTempBuffSize;
	unsigned char* lpszTempBuffer = (unsigned char*)malloc(MAX_INPUT_SIZE);
	if (!lpszTempBuffer)
	{
		printf("\nERROR: lpszTempBuffer malloc failed.\nEXITING PROGRAM.\n");
		exit(1);
	}

	pInputFile = fopen(lpszInputFile, "r");
	if (!pInputFile)
	{
		printf("\nERROR: INPUT FILE DOESNT EXIST. \nEXITING PROGRAM.\n");
		exit(1);
	}

	while (fgets((char*)lpszTempBuffer, MAX_INPUT_SIZE, pInputFile))
	{
		//----------------------------------------
		// delete new line ('\n') at end of TempBuffer if exists.
		// reallocate enough space in Input Buffer to fit
		// size of Temp Buffer + size of current input buffer if needed.
		//
		if (strlen((char*)lpszTempBuffer) < 1)
		{
			continue;
		}

		if (lpszTempBuffer[strlen((char*)lpszTempBuffer) - 1] == '\n')
		{
			lpszTempBuffer[strlen((char*)lpszTempBuffer) - 1] = '\0';
		}

		nTempBuffSize = strlen((char*)lpszTempBuffer);

		if (nTempBuffSize + g_uiInputBufferSize > g_uiCurrentBufferSize)
		{
			while (g_uiInputBufferSize + nTempBuffSize > g_uiCurrentBufferSize)
			{
				g_uiCurrentBufferSize *= 2;
			}
			g_lpszInputBuffer = (unsigned char*)realloc(g_lpszInputBuffer, g_uiCurrentBufferSize);
			if (!g_lpszInputBuffer)
			{
				printf("\nERROR: reallocating memory for global Input Buffer failed.\n EXITING PROGRAM\n");
				exit(1);
			}
		}

		//----------------------------------------
		// copy line to end of InputBuffer, delete 
		// new line at end of line.
		//
		memcpy(&g_lpszInputBuffer[g_uiInputBufferSize], lpszTempBuffer, nTempBuffSize);

		g_uiInputBufferSize += nTempBuffSize;
	}

	//----------------------------------------
	// Ensure proper formatting of buffer.
	//
	g_lpszInputBuffer[g_uiInputBufferSize] = '\0';


	ConvertFormatPrint(lpszInputType, lpszOutputFile);
	free(lpszTempBuffer);
	fclose(pInputFile);
}

//----------------------------------------
// AddMissingArguments
//
//		Handles missing Command Line Arguments. 
//
//		Args:
//			lpszInputFile	= file to read from.
//			lpszOutputFile = file to write to.
//			lpszInputType	= type of input (Ascii, EBCDIC, Ascii Hex)
//		Return:
//			lpszHeapAllocStrings = a 2 letter string of 0 or 1 ('00')
//			acting as a flag for releasing memory allocated on the heap.
//
char* AddMissingArguments(char** lpszInputFile, char** lpszOutputFile, char** lpszInputType)
{
	static char lpszHeapAllocStrings[] = { '0', '0' };
	size_t		uiStrSize;

	if (!*lpszInputType)
	{
		printf("\nMissing InputType in command line arguments: \n Please choose the input type? \n [1. Ascii Chars] \n [2. EBCDIC Hex] \n [3. Ascii Hex]\n Input: ");
		fgets((char*)g_lpszInputBuffer, MAX_INPUT_SIZE, stdin);

		switch (g_lpszInputBuffer[FIRST_CHAR])
		{
		case '1':
		{
			printf("\nAscii input.\n");
			*lpszInputType = "Ascii";
			break;
		}
		case '2':
		{
			printf("\nEBCDIC hex input.\n");
			*lpszInputType = "EBCDIC";
			break;
		}
		case '3':
		{
			printf("\nAscii hex input.\n");
			*lpszInputType = "AsciiHex";
			break;
		}
		}
	}

	//----------------------------------------
	// Change last char in FileStrings to 
	// eliminate line feed.
	//
	if (!*lpszInputFile)
	{
		lpszHeapAllocStrings[0] = '1';
		printf("\nPlease provide missing Input File: ");
		fgets(g_lpszInputBuffer, MAX_INPUT_SIZE, stdin);

		*lpszInputFile = (char*)malloc(sizeof(unsigned char) * strlen((char*)g_lpszInputBuffer) + 1);
		strcpy(*lpszInputFile, (char*)g_lpszInputBuffer);

		uiStrSize = strlen(*lpszInputFile) - 1;
		(*lpszInputFile)[uiStrSize] = '\0';
	}

	if (!*lpszOutputFile)
	{
		lpszHeapAllocStrings[1] = '1';
		printf("\nPlease provide missing Output File: ");
		fgets(g_lpszInputBuffer, MAX_INPUT_SIZE, stdin);

		*lpszOutputFile = (char*)malloc(sizeof(unsigned char) * strlen(g_lpszInputBuffer) + 1);
		strcpy(*lpszOutputFile, (char*)g_lpszInputBuffer);

		uiStrSize = strlen(*lpszOutputFile) - 1;
		(*lpszOutputFile)[uiStrSize] = '\0';
	}
	return lpszHeapAllocStrings;
}

//----------------------------------------
// CommandLineManager
//
//		Handles all Command Line Arguments. 
//
//		Args:
//			pnArgumentCount	= number of arguments passed.
//			alpszArgumentVector = array of arguments passed.
//
void CommandLineManager(int* pnArgumentCount, char* alpszArgumentVector[])
{
	char* lpszInputFile = NULL;
	char* lpszOutputFile = NULL;
	char* lpszInputType = NULL;
	char* lpszTempString;
	int	nCount;

	for (nCount = 1; nCount < *pnArgumentCount; nCount++)
	{
		lpszTempString = alpszArgumentVector[nCount];
		switch (lpszTempString[COMMAND])
		{
		case 't':
		{
			lpszInputType = "Ascii";
			break;
		}
		case 'a':
		{
			lpszInputType = "AsciiHex";
			break;
		}
		case 'e':
		{
			lpszInputType = "EBCDIC";
			break;
		}
		case 'i':
		{
			lpszInputFile = &lpszTempString[FILENAME_START];
			break;
		}
		case 'o':
		{
			lpszOutputFile = &lpszTempString[FILENAME_START];
			break;
		}
		case '?':
		{
			printf("\n--Valid Command Line Arguments--\n /t          - Input is ASCII text \n /a          - Input is hex-expanded ASCII\n /e          - Input is hex-expanded EBCDIC\n /i:filename - Input filename\n /o:filename - Output filename\n /?          - Print all valid command line arguments\n");
			if (*pnArgumentCount == 2)
			{
				exit(1);
			}
			break;
		}
		}
	}
	lpszTempString = AddMissingArguments(&lpszInputFile, &lpszOutputFile, &lpszInputType);

	printf("Arguments:\n Input Type: %s.\n Input File: %s.\n Output File: %s.\nPress enter to confirm...", lpszInputType, lpszInputFile, lpszOutputFile);

	ReadInputFile(lpszInputFile, lpszOutputFile, lpszInputType);

	if (lpszTempString[0] == '1')
	{
		free(lpszInputFile);
	}
	if (lpszTempString[1] == '1')
	{
		free(lpszOutputFile);
	}
}

//----------------------------------------
// main
//
//		Main menu to determine desired operation.
//
//		Return:
//			none
//
int main(int nArgc, char* alpszArgv[])
{
	int	nExit = 0;
	char* lpszInputType;
	char* lpszInputFileName = NULL;
	char*	lpszOutputFileName = NULL;
	
	g_lpszInputBuffer = (unsigned char*)malloc(MAX_INPUT_SIZE);
	
	if (!g_lpszInputBuffer) {
		perror("malloc failed");
		exit(1);
	}
	memset(g_lpszInputBuffer, 0, MAX_INPUT_SIZE);

	if (nArgc > 1)
	{
		CommandLineManager(&nArgc, alpszArgv);
		free(g_lpszInputBuffer);
		scanf("%c");
		return (0);
	}

	while (!nExit)
	{
		printf("\n-----------Welcome!----------- \n Please choose the input type? \n [1. Ascii Chars] \n [2. EBCDIC Hex] \n [3. Ascii Hex]\n [4. exit] \n\n- (1, 2, 3 or 4) -\n Input: ");
		fgets(g_lpszInputBuffer, MAX_INPUT_SIZE, stdin);

		switch (g_lpszInputBuffer[FIRST_CHAR])
		{
			case '1':
			{
				do
				{
					lpszInputType = "Ascii";
					ConvertFormatPrint(lpszInputType, NULL);
				} while (PromptUser());
				break;
			}
			case '2':
			{
				do
				{
					lpszInputType = "EBCDIC";
					ConvertFormatPrint(lpszInputType, NULL);
				} while (PromptUser());
				break;
			}
			case '3':
			{
				do
				{
					lpszInputType = "AsciiHex";
					ConvertFormatPrint(lpszInputType, NULL);
				} while (PromptUser());
				break;
			}
			case '4':
			{
				printf("\nPress any character to fully exit....");
				nExit = 1;
				break;
			}
			default:
			{
				printf("\nInvalid Input. Please choose (1, 2, 3 or 4)");
			}
		}
	}

	free(g_lpszInputBuffer);
	return (0);
}
