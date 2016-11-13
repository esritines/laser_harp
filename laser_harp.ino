#include "songs.h"

const byte MIDI_NOTE_OFF = 0b10000000;
const byte MIDI_NOTE_ON = 0b10010000;
const short STRINGS = 16;

// do, si, la, sol, fa, mi, re... (and so on)
short tones[STRINGS] = {0, 11, 9, 7, 5, 4, 2, 12, 23, 21, 19, 17, 16, 14, 24, 35};
short photoresistors[STRINGS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
float photoReceptorValues[STRINGS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float photoReceptorVoltage[STRINGS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
short leds[STRINGS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

bool playStrings[STRINGS] = {true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true};

bool playDemo = false;

void setup() {
	Serial.begin(19200);

	for (short i = 0; i < STRINGS; i++) {
		pinMode(leds[i], OUTPUT);
		pinMode(photoresistors[i], INPUT);

		// Turn off all the LEDs.
		digitalWrite(leds[i], LOW);
	}

	// Switch to activate demo mode.
	pinMode(16, INPUT);
}

void loop() {
	playDemo = DigitalRead(16);

	if (playDemo)
		demoMode(&demo);
	else
		normalMode();
}

void demoMode(struct Song *demo) {
	// Abraham pls, remember we discussed about this loop.
	// Stop all the current playing sounds.
	for (short i = 0; i < STRINGS; i++) {
		digitalWrite(leds[i], LOW);
		midiMessage(MIDI_NOTE_OFF, tones[i]);
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
			photoReceptorValues[i] = analogRead(photoresistors[i]);
			photoReceptorVoltage[i] = photoReceptorValues[i] * (5.0 / 1023.0);

			// If the string is interrupted stop the MIDI note.
			if (photoReceptorVoltage[i] > 1.0) {
				digitalWrite(leds[i], HIGH);
				midiMessage(MIDI_NOTE_OFF, tones[i]);
				playStrings[i] = false;
			}
		} else {
			// Read value of photoresistor and convert to voltage value.
			photoReceptorValues[i] = analogRead(photoresistors[i]);
			photoReceptorVoltage[i] = photoReceptorValues[i] * (5.0 / 1023.0);

			// If the string is not interrupted play a MIDI note.
			if (photoReceptorVoltage[i] < 1.0) {
				digitalWrite(leds[i], LOW);
				midiMessage(MIDI_NOTE_ON, tones[i]);
				playStrings[i] = true;
			}
		}
	}
}

/*
 * Manda un mensaje serial en formato MIDI
 * @param command Un comando MIDI
 * @param MIDInote La nota para el comando anterior
 * @param MIDIvelocity (0~127) establece el volumen de la nota
*/
void midiMessage(uint8_t command, uint8_t MIDInote) {
	Serial.write(command);
	Serial.write(MIDInote);
	Serial.write(127);
}
