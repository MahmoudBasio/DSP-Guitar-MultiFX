# Audio routing and controls

The dry signal no longer passes through the former global 3.2 kHz low-pass
filter. With all three effects off, the chorus forwards the original audio
block to the final mixers. Existing mixer gains and output volume still apply;
this is digital effects bypass, not a relay bypass or guaranteed unity gain.

The chorus has separate unfiltered and filtered inputs. Its optional 3.2 kHz
wet filter affects only the samples written into the chorus delay buffer.
The dry component remains unfiltered. The filter defaults to OFF. The resulting
chorus mix still feeds the delay and reverb as in the original serial/parallel
routing. The 1.8 kHz delay repeat filter and reverb damping remain unchanged.

## Controls

The chorus submenu scrolls through Rate (Hz), Depth (ms), Base (ms), Wet level
(percent), and Wet filter (OFF/ON). The cutoff is fixed at 3.2 kHz. Delay time
and feedback display their mapped values rather than the raw encoder percentage.
FX OFF means all effects are disabled; STANDBY remains a mute state.

Chorus depth is capped at base delay minus 1 ms. Reducing base delay can therefore
reduce depth, and the display shows the resulting value. The DSP independently
bounds its parameters and uses a 4096-sample delay buffer. The default depth is
18.9 ms at a 20 ms base, ensuring the minimum delay stays above 1 ms.

Input gain now explicitly applies LINEIN_LEVEL (9). This was previously defined
but unused in the modular firmware, so measured gain and clipping headroom may
change independently of the filter routing change.

Footswitch interrupt handlers only record debounced events. Each switch has its
own debounce timer; settings updates run in the main loop.

## Validation for this change

Host chorus regression checks passed. All 19 project source files compiled
individually with the Teensy 4.1 toolchain (Teensy platform 5.2.0, Arduino Teensy
1.62) using PlatformIO's generated compilation database. The full PlatformIO
build stalled launching compiler commands through the local Windows shell;
the final firmware link and on-device behavior have not been verified.

## Hardware validation after flashing

1. Confirm startup/reset shows FX OFF; compare input/output waveforms with all
   effects off and repeat the frequency sweep at a level that avoids clipping.
2. Enable chorus, scroll through all five controls, and verify Wet filter changes
   the delayed tone without filtering the dry component. With wet level at zero,
   changing the filter should not change the steady-state dry tone.
3. Exercise minimum/maximum base and depth settings and verify their displayed
   values, then check independent footswitch operation.
4. Check all effects together for clipping, dropouts, peak CPU use, and latency.
   Previous performance measurements do not validate this firmware revision.
