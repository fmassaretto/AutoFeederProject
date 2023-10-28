#include <Arduino.h>
#include <WonMelody.h>
#include <LoseMelody.h>
#include <map>

// change this to make the song slower or faster
int tempo = 108;

// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
// there are two values per note (pitch and duration), so for each note there are four bytes
int notes;

// this calculates the duration of a whole note in ms
int wholenote = (60000 * 4) / tempo;

int divider = 0, noteDuration = 0;

void playMelody(uint8_t _pin, int notes, int melody[])
{
    // iterate over the notes of the melody.
    // Remember, the array is twice the number of notes (notes + durations)
    for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2)
    {

        // calculates the duration of each note
        divider = melody[thisNote + 1];
        if (divider > 0)
        {
            // regular note, just proceed
            noteDuration = (wholenote) / divider;
        }
        else if (divider < 0)
        {
            // dotted notes are represented with negative durations!!
            noteDuration = (wholenote) / abs(divider);
            noteDuration *= 1.5; // increases the duration in half for dotted notes
        }

        // we only play the note for 90% of the duration, leaving 10% as a pause
        tone(_pin, melody[thisNote], noteDuration * 0.9);

        // Wait for the specief duration before playing the next note.
        delay(noteDuration);

        // stop the waveform generation before the next note.
        noTone(_pin);
    }
}

void playWonMellody(uint8_t _pin)
{
    int notes = sizeof(wonMelody) / sizeof(wonMelody[0]) / 2;
    playMelody(_pin, notes, wonMelody);
}

void playLoseMellody(uint8_t _pin)
{
    int notes = sizeof(loseMelody) / sizeof(loseMelody[0]) / 2;
    playMelody(_pin, notes, loseMelody);
}