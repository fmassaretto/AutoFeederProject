#include <Arduino.h>
#include <pitches.h>
#include <map>

uint8_t buzzerPin;
std::map<int, int> definedMellody;

std::map<int, int> won_mellody = {
    {NOTE_C4, 4},
    {NOTE_G3, 8},
    {NOTE_G3, 8},
    {NOTE_A3, 4},
    {NOTE_G3, 4},
    {REST, 4},
    {NOTE_B3, 4},
    {NOTE_C4, 4},
};

std::map<int, int> lose_mellody = {

    // Dart Vader theme (Imperial March) - Star wars
    // Score available at https://musescore.com/user/202909/scores/1141521
    // The tenor saxophone part was used

    {NOTE_AS4, 8},
    {NOTE_AS4, 8},
    {NOTE_AS4, 8}, // 1
    {NOTE_F5, 2},
    {NOTE_C6, 2},
    {NOTE_AS5, 8},
    {NOTE_A5, 8},
    {NOTE_G5, 8},
    {NOTE_F6, 2},
    {NOTE_C6, 4},
    {NOTE_AS5, 8},
    {NOTE_A5, 8},
    {NOTE_G5, 8},
    {NOTE_F6, 2},
    {NOTE_C6, 4},
    {NOTE_AS5, 8},
    {NOTE_A5, 8},
    {NOTE_AS5, 8},
    {NOTE_G5, 2},
    {NOTE_C5, 8},
    {NOTE_C5, 8},
    {NOTE_C5, 8},
    {NOTE_F5, 2},
    {NOTE_C6, 2},
    {NOTE_AS5, 8},
    {NOTE_A5, 8},
    {NOTE_G5, 8},
    {NOTE_F6, 2},
    {NOTE_C6, 4},

    {NOTE_AS5, 8},
    {NOTE_A5, 8},
    {NOTE_G5, 8},
    {NOTE_F6, 2},
    {NOTE_C6, 4}, // 8
    {NOTE_AS5, 8},
    {NOTE_A5, 8},
    {NOTE_AS5, 8},
    {NOTE_G5, 2},
    {NOTE_C5, -8},
    {NOTE_C5, 16},
    {NOTE_D5, -4},
    {NOTE_D5, 8},
    {NOTE_AS5, 8},
    {NOTE_A5, 8},
    {NOTE_G5, 8},
    {NOTE_F5, 8},
    {NOTE_F5, 8},
    {NOTE_G5, 8},
    {NOTE_A5, 8},
    {NOTE_G5, 4},
    {NOTE_D5, 8},
    {NOTE_E5, 4},
    {NOTE_C5, -8},
    {NOTE_C5, 16},
    {NOTE_D5, -4},
    {NOTE_D5, 8},
    {NOTE_AS5, 8},
    {NOTE_A5, 8},
    {NOTE_G5, 8},
    {NOTE_F5, 8},

    {NOTE_C6, -8},
    {NOTE_G5, 16},
    {NOTE_G5, 2},
    {REST, 8},
    {NOTE_C5, 8}, // 13
    {NOTE_D5, -4},
    {NOTE_D5, 8},
    {NOTE_AS5, 8},
    {NOTE_A5, 8},
    {NOTE_G5, 8},
    {NOTE_F5, 8},
    {NOTE_F5, 8},
    {NOTE_G5, 8},
    {NOTE_A5, 8},
    {NOTE_G5, 4},
    {NOTE_D5, 8},
    {NOTE_E5, 4},
    {NOTE_C6, -8},
    {NOTE_C6, 16},
    {NOTE_F6, 4},
    {NOTE_DS6, 8},
    {NOTE_CS6, 4},
    {NOTE_C6, 8},
    {NOTE_AS5, 4},
    {NOTE_GS5, 8},
    {NOTE_G5, 4},
    {NOTE_F5, 8},
    {NOTE_C6, 1}

};

void setMellody(std::map<int, int> _definedMellody, uint8_t _pin)
{
    definedMellody = _definedMellody;
    buzzerPin = _pin;
}

void playMellody()
{
    std::map<int, int>::iterator it;
    // iterate over the notes of the melody:
    for (it = definedMellody.begin(); it != definedMellody.end(); ++it)
    {
        Serial.println("starting playing here...");

        // to calculate the note duration, take one second divided by the note type.
        // e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
        int noteDuration = 1000 / it->second;
        tone(buzzerPin, it->first, noteDuration);

        // to distinguish the notes, set a minimum time between them.
        // the note's duration + 30% seems to work well:
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        // stop the tone playing:
        noTone(buzzerPin);
    }
}