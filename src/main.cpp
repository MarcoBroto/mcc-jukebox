
#define IR 2 // Interrupt
#define BUZZER 3 // 490 Hz PWM, Interrupt
#define SEG_LATCH 4
#define SEG_CLK 7
#define SEG_SDA 8
#define LED1 10 // 490 Hz PWM
#define LED2 11 // 490 Hz PWM
#define LED3 12
#define LED4 13

#define S1 A1
#define S2 A2
#define S3 A3
#define TEMP A4
#define D1 LED4
#define D2 LED3
#define D3 LED2
#define D4 LED1

#define GPIO1 5 // 980 Hz PWM
#define GPIO2 6 // 980 Hz PWM
#define GPIO3 9 // 490 Hz PWM
#define GPIO4 A5

const byte SEG_DIGIT_LENGTH = 4;
const byte SEG_DIGIT_POS[] = { 0xF1, 0xF2, 0xF4, 0xF8 }; // this table sets a "selector" for what digit is selected
const byte SEG_DIGIT_TABLE[] = { 0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90 }; // Digit to segment mask map table
const byte SEG_LETTER_TABLE[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Char to segment mask map table


byte seg_buffer[] = { 0xFF, 0xFF, 0xFF, 0xFF }; // Stores values of segment display at each digit
unsigned long seg_buff = 0xFFFFFFFFuL; // Store segment display mask values at each digit in sequential bytes (big endian)

void setup() {
  initShield();
  // setSegBufferStr("3456", 4);
  streamStrToSegBuffer("0 1 2 3 4 5 6 7 8 9", 19);
}

void loop() {
  handleButtonPress(readButtonState());
  displaySegBuffer();
}

void initShield() {
  // Setup pin io modes
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(S1, INPUT_PULLUP);
  pinMode(S2, INPUT_PULLUP);
  pinMode(S3, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  pinMode(SEG_LATCH, OUTPUT);
  pinMode(SEG_CLK, OUTPUT);
  pinMode(SEG_SDA, OUTPUT);
  // pinMode(IR, INPUT_PULLUP);
  // pinMode(TEMP, INPUT);
  // pinMode(GPIO1, OUTPUT);
  // pinMode(GPIO2, OUTPUT);
  // pinMode(GPIO3, OUTPUT);
  // pinMode(GPIO4, OUTPUT);

  // Enable interrupts
  // attachInterrupt(2, <handler>, CHANGE);
  // attachInterrupt(3, <handler>, CHANGE);

  // Set initial states
  turnOffBuzzer();
  setActiveLeds(0xF); // Turn off shield leds
  clearSegBuffer();
}

void displayMask(byte mask, byte pos) {
  if (pos < 0 || SEG_DIGIT_LENGTH <= pos) return;
  digitalWrite(SEG_LATCH, LOW);
  shiftOut(SEG_SDA, SEG_CLK, MSBFIRST, mask); // Display character mask
  shiftOut(SEG_SDA, SEG_CLK, MSBFIRST, SEG_DIGIT_POS[pos]); // Select position to display character
  digitalWrite(SEG_LATCH, HIGH);
  delay(2);
}

void displaySegBuffer() {
  // for (int i = 0; i < SEG_DIGIT_LENGTH; i++) {
  //   displayMask(seg_buffer[i], i);
  //   // displayMask(seg_buff >> i * 8, i);
  // }

  for (int i = 0; i < SEG_DIGIT_LENGTH; i++) {
    displayMask(seg_buff >> ((SEG_DIGIT_LENGTH - (i+1)) * 8), i);
  }
}

void clearSegBuffer() {
  for (int i = 0; i < SEG_DIGIT_LENGTH; i++) {
    seg_buffer[i] = 0xFF;
  }
  seg_buff = ~0;
}

void setSegBufferInt(int num) {
  if (num < -999 || 9999 < num) {
    char ofloStr[] = "ovfl";
    setSegBufferStr(ofloStr, sizeof(ofloStr));
    return;
  }
  for (int i = 0; i <= 3; i++) {
    seg_buffer[i] = 0xFF;
  }
}

void setSegBufferFloat(float num) {
  for (int i = 0; i <= 3; i++) {
    seg_buffer[i] = 0xFF;
  }
}

void setSegBufferStr(char *str, size_t length) {
  for (int i = 0; i < length && i < SEG_DIGIT_LENGTH; i++) {
    // seg_buffer[i] = charToSegMask(str[i]);
    int shift_pos = i * 8;
    seg_buff &= ~(0xFFL << shift_pos); // Clear bit mask at digit position
    seg_buff |= (long)charToSegMask(str[i]) << shift_pos; // Set bit mask at digit position
  }
}

void streamStrToSegBuffer(char *stream_buff, size_t length) {
  for (int i = 0; i < length; i++) {
    // seg_buff <<= 8L;
    seg_buff = seg_buff << 8;
    seg_buff |= charToSegMask(stream_buff[i]);
    for (long j = 0; j < 25; j++) { // Makeshift delay
      displaySegBuffer();
    }
  }

  for (int i = 0; i < SEG_DIGIT_LENGTH; i++) { // Stream the remaining masks from buffer
    seg_buff <<= 8;
    seg_buff |= 0xFF; // Insert empty digit mask (space character)
    for (long j = 0; j < 25; j++) { // Makeshift delay
      displaySegBuffer();
    }
  }
}

void setSegBufferErr() {
  char errStr[] = "Err";
  setSegBufferStr(errStr, sizeof(errStr));
}

byte charToSegMask(char chr) {
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
  return 0xFF;  // Empty mask (space character)
}

void turnOffBuzzer() {
  digitalWrite(BUZZER, 0xFF);
}

void setBuzzerFrequency(int frequency) {
  digitalWrite(BUZZER, frequency);
}

void playSong(int *frequencies, int *durations, size_t length) {
  turnOffBuzzer();
  for (int i = 0; i < length; i++) {
    setBuzzerFrequency(frequencies[i]);
    delay(durations[i]);
  }
  turnOffBuzzer();
}

void playFrequenciesWithTempo(int *frequencies, size_t length, int tempo) {
  if (tempo < 1) return;
  turnOffBuzzer();
  for (int i = 0; i < length; i++) {
    setBuzzerFrequency(frequencies[i]);
    delay(1000); //TODO: match tempo
    delay(tempo); // BPM = 1 beat * 60 seconds/1 minute * 1000 milliseconds/1 second
  }
  turnOffBuzzer();
}

char* toneFrequencyToNote(int frequency) {
  return "?";
}

byte readButtonState() {
  return digitalRead(S3) << 2 | digitalRead(S2) << 1 | digitalRead(S1);
}

void handleButtonPress(byte mask) {
  if (mask & 1) { // S1 pressed
  }
  if (mask & 2) { // S2 pressed
  }
  if (mask & 3) { // S3 pressed
  }
  setActiveLeds(mask | 8);
}

void setActiveLeds(byte mask) {
  digitalWrite(D1, mask & 1);
  digitalWrite(D2, mask & 2);
  digitalWrite(D3, mask & 4);
  digitalWrite(D4, mask & 8);
}
