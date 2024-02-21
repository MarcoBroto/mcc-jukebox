
char readButtonState()
{
	return digitalRead(S3) << 2 | digitalRead(S2) << 1 | digitalRead(S1);
}

void handleButtonPress(char mask)
{
	if (mask & 1)
	{ // S1 pressed
	}
	if (mask & 2)
	{ // S2 pressed
	}
	if (mask & 3)
	{ // S3 pressed
	}
	setActiveLeds(mask | 8);
}
