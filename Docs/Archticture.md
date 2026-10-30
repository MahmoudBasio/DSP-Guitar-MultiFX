# DSP Guitar MultiFX Architecture

The platform combines a Teensy 4.1, SGTL5000 audio codec, analog interface, OLED display, rotary encoder, and effect footswitches. Its implemented effects are delay, reverb, and chorus.

## Audio path

Analog input is converted to digital samples by the codec, processed by the Teensy, and converted back to analog output. The current routing keeps the dry input unfiltered. A separate optional 3.2 kHz filter supplies the chorus wet branch. The resulting chorus output feeds both the final dry mix and the delay/reverb branches. Delay repeats pass through a 1.8 kHz filter; delay output can also feed reverb. Wet outputs are mixed with the dry contribution and sent to both output channels.

With all effects disabled, audio still passes through the codec and digital graph. Mixer gain still applies; this is not relay bypass or guaranteed unity gain. Sending the same signal to both channels does not establish independent stereo processing.

See [Audio routing and controls](Audio-routing-and-controls.md) for controls, parameter limits, startup state, and validation status.

## Firmware organization

| Location | Responsibility |
|---|---|
| `firmware/src/audio/audio_graph.cpp` | Audio objects and static connections |
| `firmware/src/audio/audio_manager.cpp` | Audio initialization and settings coordination |
| `firmware/src/audio/mixer.cpp` | Dry/wet, feedback and effect-input gains |
| `firmware/src/effects/` | Chorus, delay, reverb and effect management |
| `firmware/src/dsp/` | DSP settings and presets |
| `firmware/src/core/` | Hardware, system state and scheduling |
| `firmware/src/ui/` | Display, menus and encoder handling |
| `firmware/include/config/` | Constants, pins and preset declarations |
| `firmware/tests/host/` | Host-side chorus regression checks |

The firmware uses the Arduino framework and Teensy Audio Library through PlatformIO. OLED, encoder and footswitch controls are implemented features. Distortion is a possible extension, not one of the three implemented effects described here.

## Timing and validation

The experimental operating rate is nominally 44.1 kHz with 128-sample blocks. Each block represents about 2.90 ms of audio. Block duration, CPU processing time, and analog input-to-output latency are different quantities.

A latency below 10 ms is a design target, not a validated specification for the current firmware. Historical CPU figures and oscilloscope captures must remain attributed to their tested configurations. The revised routing requires fresh hardware measurements.

The [measurement report](measurements.md) includes the earlier filtered effects-off response, delay timing and feedback observations, reverb settings, and preliminary harmonic/noise/latency results. It does not certify current-firmware performance. The [explanation guide](Measurements/understanding-the-measurements.md) defines the measurements and works through their calculations.

## Extension points

Additional effects can be added through the existing effect interface, parameter management, audio connections and UI. Each routing or gain change should be accompanied by an identified firmware revision and appropriate bench measurements. Host tests supplement, but do not replace, physical audio validation.
