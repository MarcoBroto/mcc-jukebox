
const char SEG_DIGIT_LENGTH = 4;
const char SEG_DIGIT_POS[] = {0xF1, 0xF2, 0xF4, 0xF8};																  // this table sets a "selector" for what digit is selected
const char SEG_DIGIT_TABLE[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};						  // Digit to segment mask map table
const char SEG_LETTER_TABLE[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Char to segment mask map table

char seg_buffer[] = {0xFF, 0xFF, 0xFF, 0xFF}; // Stores values of segment display at each digit
unsigned long seg_buff = 0xFFFFFFFFuL;		  // Store segment display mask values at each digit in sequential chars (big endian)


void displayMask(char mask, char pos) {
	if (pos < 0 || SEG_DIGIT_LENGTH <= pos) return;
	digitalWrite(SEG_LATCH, LOW);
	shiftOut(SEG_SDA, SEG_CLK, MSBFIRST, mask); // Display character mask
	shiftOut(SEG_SDA, SEG_CLK, MSBFIRST, SEG_DIGIT_POS[pos]); // Select position to display character
	digitalWrite(SEG_LATCH, HIGH);
	delay(2);
}


void displaySegBuffer()
{
	// for (int i = 0; i < SEG_DIGIT_LENGTH; i++) {
	//   displayMask(seg_buffer[i], i);
	//   // displayMask(seg_buff >> i * 8, i);
	// }

	for (int i = 0; i < SEG_DIGIT_LENGTH; i++)
	{
		displayMask(seg_buff >> ((SEG_DIGIT_LENGTH - (i + 1)) * 8), i);
	}
}

void clearSegBuffer()
{
	for (int i = 0; i < SEG_DIGIT_LENGTH; i++)
	{
		seg_buffer[i] = 0xFF;
	}
	seg_buff = ~0;
}

void setSegBufferInt(int num)
{
	if (num < -999 || 9999 < num)
	{
		char ofloStr[] = "ovfl";
		setSegBufferStr(ofloStr, sizeof(ofloStr));
		return;
	}
	for (int i = 0; i <= 3; i++)
	{
		seg_buffer[i] = 0xFF;
	}
}

void setSegBufferFloat(float num)
{
	for (int i = 0; i <= 3; i++)
	{
		seg_buffer[i] = 0xFF;
	}
}

void setSegBufferStr(char *str, size_t length)
{
	for (int i = 0; i < length && i < SEG_DIGIT_LENGTH; i++)
	{
		// seg_buffer[i] = charToSegMask(str[i]);
		int shift_pos = i * 8;
		seg_buff &= ~(0xFFL << shift_pos);					  // Clear bit mask at digit position
		seg_buff |= (long)charToSegMask(str[i]) << shift_pos; // Set bit mask at digit position
	}
}

void streamStrToSegBuffer(char *stream_buff, size_t length)
{
	for (int i = 0; i < length; i++)
	{
		// seg_buff <<= 8L;
		seg_buff = seg_buff << 8;
		seg_buff |= charToSegMask(stream_buff[i]);
		for (long j = 0; j < 25; j++)
		{ // Makeshift delay
			displaySegBuffer();
		}
	}

	for (int i = 0; i < SEG_DIGIT_LENGTH; i++)
	{ // Stream the remaining masks from buffer
		seg_buff <<= 8;
		seg_buff |= 0xFF; // Insert empty digit mask (space character)
		for (long j = 0; j < 25; j++)
		{ // Makeshift delay
			displaySegBuffer();
		}
	}
}

void setSegBufferErr()
{
	char errStr[] = "Err";
	setSegBufferStr(errStr, sizeof(errStr));
}

char charToSegMask(char chr)
{
	if (chr == ' ') return 0xFF;
	if (48 <= chr || chr <= 57) {
		return SEG_DIGIT_TABLE[chr - 48]; // Subtract ascii offset
	}
	if (66 <= chr || chr <= 90) {
		// return SEG_LETTER_TABLE[chr - 66]; // Subtract ascii offset
		return 0xFF;
	}
	if (97 <= chr || chr <= 122) {
		// return SEG_LETTER_TABLE[chr - 71] // Subtract ascii offset 96 add 26 uppercase letters
		return 0xFF;
	}
	return 0xFF; // Empty mask (space character)
}