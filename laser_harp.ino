#include "songs.h"

// Number of strings the harp has.
const short STRINGS = 16;

const short BAUD_RATE = 19200;
const short DEMO_PIN = 16;
const byte MIDI_NOTE_OFF = 0b10000000;
const byte MIDI_NOTE_ON = 0b10010000;

// do, si, la, sol, fa, mi, re... (and so on)
const short NOTES[STRINGS] = {0, 11, 9, 7, 5, 4, 2, 12, 23, 21, 19, 17, 16, 14, 24, 35};
const short PHOTORESISTORS[STRINGS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
const short LEDS[STRINGS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

float photoReceptorValues[STRINGS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float photoReceptorVoltage[STRINGS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
bool playStrings[STRINGS] = {true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true};

// Variable for reading the push button status.
int playDemo = 0;

void setup() {
	Serial.begin(BAUD_RATE);

	for (short i = 0; i < STRINGS; i++) {
		pinMode(LEDS[i], OUTPUT);
		pinMode(PHOTORESISTORS[i], INPUT);

		// Turn off all the LEDs.
		digitalWrite(LEDS[i], LOW);
	}

	// Push button to activate demo mode.
	pinMode(DEMO_PIN, INPUT);
}

void loop() {
	playDemo = digitalRead(DEMO_PIN);

	if (playDemo == HIGH)
		demoMode(&demo);
	else
		normalMode();
}

void demoMode(struct Song *demo) {
	// Abraham pls, remember we discussed about this loop.
	// Stop all the current playing sounds.
	for (short i = 0; i < STRINGS; i++) {
		digitalWrite(LEDS[i], LOW);
		midiMessage(MIDI_NOTE_OFF, NOTES[i]);
	}

	//Serial.println("Playing %s.", demo->name);
	for (short i = 0; i < demo->size; i++) {
		// Turn on LED, play note for X period of time.
		digitalWrite(demo->leds[i], HIGH);
		midiMessage(MIDI_NOTE_ON, demo->notes[i]);
		delay(demo->play_time[i]);

		// Turn off LED, stop note and wait X period of time.
		digitalWrite(demo->leds[i], LOW);
		midiMessage(MIDI_NOTE_OFF, demo->notes[i]);
		delay(demo->silences[i]);
	}
}

void normalMode() {
	for (short i = 0; i < STRINGS; i++) {
		// Check if laser is not interrupted.
		if (playStrings[i]) {
			// Read value of photoresistor and convert to voltage value.
			photoReceptorValues[i] = analogRead(PHOTORESISTORS[i]);
			photoReceptorVoltage[i] = photoReceptorValues[i] * (5.0 / 1023.0);

			// If the string is interrupted stop the MIDI note.
			if (photoReceptorVoltage[i] > 1.0) {
				digitalWrite(LEDS[i], HIGH);
				midiMessage(MIDI_NOTE_OFF, NOTES[i]);
				playStrings[i] = false;
			}
		} else {
			// Read value of photoresistor and convert to voltage value.
			photoReceptorValues[i] = analogRead(PHOTORESISTORS[i]);
			photoReceptorVoltage[i] = photoReceptorValues[i] * (5.0 / 1023.0);

			// If the string is not interrupted play a MIDI note.
			if (photoReceptorVoltage[i] < 1.0) {
				digitalWrite(LEDS[i], LOW);
				midiMessage(MIDI_NOTE_ON, NOTES[i]);
				playStrings[i] = true;
			}
		}
	}
}

// Sends a serial message in MIDI format.
void midiMessage(uint8_t command, uint8_t MIDInote) {
	// Whether to play or stop the note.
	Serial.write(command);

	// Note to play or stop.
	Serial.write(MIDInote);

	// Note volume.
	Serial.write(127);
}
