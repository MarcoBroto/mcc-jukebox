
void turnOffBuzzer()
{
	digitalWrite(BUZZER, 0xFF);
}

void setBuzzerFrequency(int frequency)
{
	digitalWrite(BUZZER, frequency);
}

void playSong(int *frequencies, int *durations, size_t length)
{
	turnOffBuzzer();
	for (int i = 0; i < length; i++)
	{
		setBuzzerFrequency(frequencies[i]);
		delay(durations[i]);
	}
	turnOffBuzzer();
}

void playFrequenciesWithTempo(int *frequencies, size_t length, int tempo)
{
	if (tempo < 1)
		return;
	turnOffBuzzer();
	for (int i = 0; i < length; i++)
	{
		setBuzzerFrequency(frequencies[i]);
		delay(1000);  // TODO: match tempo
		delay(tempo); // BPM = 1 beat * 60 seconds/1 minute * 1000 milliseconds/1 second
	}
	turnOffBuzzer();
}

char *toneFrequencyToNote(int frequency)
{
	return "?";
}
