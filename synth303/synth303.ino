
#include <Arduino.h>
#include <Audio.h>

#include <USBHost_t36.h>
#include "./AudioFilter303.h"

#define MIDI_COUNT 4

USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
USBHub hub3(myusb);
MIDIDevice midi[MIDI_COUNT] = MIDIDevice(myusb);

AudioOutputMQS audioOut;
AudioSynthWaveform waveform;
// AudioFilterStateVariable filter;
AudioFilter303 filter;

AudioConnection patchCordMixer1(waveform, filter);
AudioConnection patchCordMixer2(filter, audioOut);

void setup()
{
  Serial.begin(115200);
  AudioMemory(25);
  midiInit();

  waveform.frequency(440);
  waveform.amplitude(1.0);
  waveform.begin(WAVEFORM_SAWTOOTH);

  // filter.octaveControl(2.5);
  // filter.frequency(1300);
  // filter.resonance(0.9);

  filter.setCutoff(64);
  filter.setResonance(64);
}

void loop()
{
  // put your main code here, to run repeatedly:
  midiLoop();
}

void controlChangeHandler(byte channel, byte control, byte value)
{
  Serial.print("Control Change, ch=");
  Serial.print(channel, DEC);
  Serial.print(", control=");
  Serial.print(control, DEC);
  Serial.print(", value=");
  Serial.println(value, DEC);

  if (control == 20)
  {
    waveform.amplitude(value / 127.0f);
  }
  // else if (control == 17)
  // {
  //   float freqVal = value / 127.0f;
  //   filter.frequency(freqVal * freqVal * 4000.0f);
  //   Serial.printf("frequency %.6f\n", freqVal * freqVal * 4000.0f);
  // }
  // else if (control == 18)
  // {
  //   filter.resonance((value / 127.0f * 4.3) + 0.7);
  //   Serial.printf("resonance %.6f\n", (value / 127.0f * 4.3) + 0.7);
  // }
  // else if (control == 19)
  // {
  //   filter.octaveControl(value / 127.0f * 7.0f);
  //   Serial.printf("octaveControl %.6f\n", value / 127.0f * 7.0f);
  // }
  else if (control == 17)
  {
    filter.setCutoff(value);
  }
  else if (control == 18)
  {
    filter.setResonance(value);
  }
}

void noteOnHandler(byte channel, byte note, byte velocity)
{
  // When a USB device with multiple virtual cables is used,
  // midi[n].getCable() can be used to read which of the virtual
  // MIDI cables received this message.
  Serial.print("Note On, ch=");
  Serial.print(channel, DEC);
  Serial.print(", note=");
  Serial.print(note, DEC);
  Serial.print(", velocity=");
  Serial.println(velocity, DEC);
}

void noteOffHandler(byte channel, byte note, byte velocity)
{
  Serial.print("Note Off, ch=");
  Serial.print(channel, DEC);
  Serial.print(", note=");
  Serial.print(note, DEC);
  Serial.print(", velocity=");
  Serial.println(velocity, DEC);
}

void midiInit()
{
  myusb.begin();
  for (byte n = 0; n < MIDI_COUNT; n++)
  {
    midi[n].setHandleNoteOn(noteOnHandler);
    midi[n].setHandleNoteOff(noteOffHandler);
    midi[n].setHandleControlChange(controlChangeHandler);
  }
}

void midiLoop()
{
  myusb.Task();
  for (byte n = 0; n < MIDI_COUNT; n++)
  {
    while (midi[n].read())
      ;
  }
}
