#define MIDI_NOTE_OFF         0b10000000
#define MIDI_NOTE_ON          0b10010000
#define MIDI_AFTERTOUCH       0b10100000
#define MIDI_CONT_CONTROLLER  0b10110000
#define MIDI_PATCH_CHANGE     0b11000000
#define MIDI_CH_PRESSURE      0b11010000
#define MIDI_PITCH_BEND       0b11100000
#define MIDI_NON_MUSICAL      0b11110000

int tonos[] = {0, 11, 9, 7, 5, 4, 2, 12, 23, 21, 18, 17, 16, 14, 24, 35};
int fotoresistores[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
float valorFotoReceptor[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float voltajeFotoReceptor[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int leds[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

bool tocarCuerdas[] = {true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true};

void setup() {
  Serial.begin(19200);

  for (int i = 0; i < 4; i++) {
	pinMode(leds[i], OUTPUT);
	pinMode(fotoresistores[i], INPUT);
  }
}

void loop() {
	for (int i = 0; i < 1; i++) {
		if (tocarCuerdas[i]) {
			valorFotoReceptor[i] = analogRead(fotoresistores[i]);
			voltajeFotoReceptor[i] = valorFotoReceptor[i] * (5.0 / 1023.0);

			if (voltajeFotoReceptor[i] > 1.0) {
				digitalWrite(leds[i], HIGH);
				midiMessage(MIDI_NOTE_OFF, tonos[i]);
				tocarCuerdas[i] = false;
			}
		} else {
			valorFotoReceptor[i] = analogRead(fotoresistores[i]);
			voltajeFotoReceptor[i] = valorFotoReceptor[i] * (5.0 / 1023.0);

			if (voltajeFotoReceptor[i] < 1.0) {
				digitalWrite(leds[i], LOW);
				midiMessage(MIDI_NOTE_ON, tonos[i]);
				tocarCuerdas[i] = true;
			}
		}
	}
}

/*
 * Manda un mensaje serial en formato MIDI
 * @param command Un comando MIDO
 * @param MIDInote La nota para el comando anterior
 * @param MIDIvelocity (0~127) establece el volumen de la nota
*/
void midiMessage(uint8_t command, uint8_t MIDInote) {
	Serial.write(command);
	Serial.write(MIDInote);
	Serial.write(127);
}
