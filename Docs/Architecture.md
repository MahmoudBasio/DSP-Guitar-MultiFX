# DSP Guitar MultiFX architecture

This Teensy 4.1 firmware integrates custom chorus DSP, the Teensy Audio Library's
delay and mono Freeverb objects, physical controls, and a static audio graph.
The contribution is a reproducible system integration, not a new delay/reverb
algorithm or a runtime-loadable plugin engine.

## Actual signal routing

- Codec left input -> 3.2 kHz biquad low-pass -> custom chorus (or its passthrough).
- Chorus output -> both final mixers' dry inputs.
- Chorus output -> delay input mixer -> library delay -> 1.8 kHz repeat filter.
- Repeat-filter output -> delay feedback, wet mixer, and reverb input mixer.
- Chorus output also feeds the reverb input mixer; Freeverb feeds the wet mixer.
- The wet mixer feeds both final mixers; their identical signals feed I2S L/R.

This is a fixed series/parallel mono graph with dual-mono output, not a freely
reorderable stereo effects chain. The feedback connection crosses the library's
audio-update order; nominal delay-tap time is not a complete feedback-loop delay.

"Effects off" is a digital bypass of the effect contributions, not a relay/true
bypass: the ADC, 3.2 kHz prefilter, audio buffers, final gain, and DAC remain in
the path. The prefilter intentionally changes frequency response. The master
switch provides standby/mute, not an analog bypass.

## Source ownership

| Files | Responsibility |
| --- | --- |
| `src/audio/audio_graph.cpp` | Statically instantiated audio objects and patch cords |
| `src/audio/audio_manager.cpp`, `mixer.cpp` | Codec, filtering, input/output gains and routing |
| `src/effects/chorus.cpp` | Custom floating-point, sinusoidally modulated delay with linear interpolation, converted back to signed 16-bit blocks |
| `src/effects/delay.cpp`, `reverb.cpp` | Wrappers configuring library algorithms, not reimplementations |
| `include/effects/ieffect.h`, `src/effects/effect_manager.cpp` | Enable/query/update control interface and parameter mapping |
| `src/core/system.cpp`, `scheduler.cpp` | Foreground controls, debounce, modulation and display scheduling |
| `src/ui/` | Encoder, menus, display; actual clamped parameter values are shown |
| `src/core/diagnostics.cpp` | Optional USB serial resource logging; absent from normal builds |

`IEffect` abstracts effect controls. It does not expose a generic audio-processing
callback or automatic registration. Adding an effect requires edits to the audio
graph, manager, parameter data, UI, and tests as well as the new wrapper.

## Audio format and resources

The supported build uses 44,100 Hz, 128 samples/block, and signed 16-bit sample
transport. The chorus uses float arithmetic internally. Codec hardware
capabilities are not the resolution/sample rate evaluated by this firmware.
Compile-time assertions reject changed rate/block settings until memory and
algorithm limits are reviewed.

- Audio block period: about 2.9025 ms; not end-to-end latency.
- Audio pool: 400 statically reserved blocks shared by the graph.
- Library delay: requested base 10-1000 ms, plus +/-3 ms modulation at 0.18 Hz.
  Control updates are nominally every 20 ms; tap changes are not an interpolating
  tape-delay implementation and can still create modulation/switching artifacts.
- At the 1003 ms maximum tap, the library queue budget is 347 blocks
  (`ceil(round(ms * fs / 1000) / 128) + 1`). Another 32 blocks are reserved in
  the budget for graph activity; 400 is a design allocation, not a measured peak.
- Chorus history: 2048 float samples (8192 bytes). Base is 10-30 ms. Requested
  depth is 0-30 ms, but applied depth is limited by both the base and the buffer.
  Instantaneous delay is constrained to 1-2046 samples (about 0.0227-46.3946 ms).
  Rate is 0.1-5 Hz; wet gain remains the preset's fixed 0.8.
- Chorus history keeps advancing when disabled, preventing frozen audio replay.
- Default mapped values are 425 ms / 0.5795 / 0.55 for delay,
  0.90 / 0.30 / 0.60 for reverb, and 0.296 Hz / 19.8 ms / 20 ms for chorus.
  The preset percentages, rather than the initial structure literals, determine
  values after startup/reset.

Audio pool allocation is static, but block occupancy is dynamic and delay-time
dependent. `AudioMemoryUsageMax()` reports pool blocks, not total RAM1/RAM2,
stack, or static effect storage. Muting an effect through mixer gains does not
necessarily stop its library object's processing. CPU results therefore
describe graph configurations, not isolated algorithm costs.

## Gain and control safety

Chorus, reverb-input, wet, and final dry/wet gain pairs are normalized when their
sum exceeds unity. Delay source gain is `1 - feedback`, with feedback capped at
0.95. These changes reduce avoidable mixer saturation but also change audible
levels; historical audio and CPU measurements must be repeated.

Normalization cannot prove that the analog front-end, ADC, filters, or Freeverb
internals never clip. Hard output saturation remains a safety fallback in the
custom chorus. Listen and measure across input levels, feedback values, and
effect combinations before treating the device as validated.

Footswitches are polled in the foreground, nominally every 5 ms, each with
independent 30 ms stable-state debounce. The master input keeps its existing
active-low, level-controlled behavior. It is sampled at boot before unmuting.
No footswitch ISR calls effect setters. Short audio-update critical sections
protect multi-field parameter changes and paired mixer changes; no display,
I2C, serial, or long calculation is performed inside those sections.

The task scheduler is cooperative. OLED transfers and serial reporting may
delay foreground tasks; listed periods are targets, not hard real-time
guarantees. DSP blocks run through the library's audio interrupt.

SGTL5000 `volume()` controls headphones. Standby additionally zeros both final
mixers so line-out is muted too. Codec line-input level is explicitly set from
`LINEIN_LEVEL` (9). This changes any behavior that previously depended on the
codec default; recalibrate actual input/output levels.

## Validation

See [Testing and measurements](Testing.md) for repeatable builds, host tests,
diagnostic logging, and the hardware checks still needed. The current source
does not establish end-to-end latency, SNR, THD+N, frequency-response quality,
or the historical CPU figures.
