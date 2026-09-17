# Firmware verification and measurement procedure

## Reproducible builds

From `firmware/`, using PlatformIO Core 6.1.18:

```sh
pio run -e teensy41
pio run -e teensy41_diagnostics
pio pkg list -e teensy41
```

The platform and direct libraries are pinned in `platformio.ini`:
Teensy platform 6.0.0, U8g2 2.36.18, CriticalTaskScheduler 1.0.7.
The target clock is 600 MHz. Save the complete resolved package list, verbose
compiler command/options (`pio run -v -e teensy41`), build memory report,
git commit SHA, and whether diagnostics are enabled with each measurement.
Platform packages can themselves use version ranges; save their resolved
versions rather than assuming all indirect dependencies are locked.

Software checks completed for this change:

| Check | Result |
| --- | --- |
| `pio run -e teensy41` | Passed |
| `pio run -e teensy41_diagnostics` | Passed |
| Host suite, normal and diagnostic modes, AddressSanitizer + UndefinedBehaviorSanitizer | Passed; LeakSanitizer disabled for the ptrace runner |

The target builds resolved Arduino/Teensy 1.62 (`framework-arduinoteensy`
1.162.0), ARM GCC 15.2.1 (`toolchain-gccarmnoneeabi-teensy` 1.150201.0),
and Teensy tools 1.162.0. These results verify compilation/linking and the host
checks described below; no firmware was uploaded or tested on hardware here.

To upload, connect a Teensy 4.1 and use `pio run -e teensy41 --target upload`.
Start hardware tests with a low input level and amplifier/headphone volume.

## Host regression suite

Requires Bash and GCC with AddressSanitizer and UndefinedBehaviorSanitizer:

```sh
cd firmware
bash test/run_host_tests.sh
```

The script compiles all production source files in both normal and diagnostic
modes against small hardware/library API mocks, then runs:

- All 10,201 UI base/depth combinations and their delay bounds.
- Actual custom-chorus block processing over multiple LFO cycles, including
  unsafe legacy combinations, huge/non-finite inputs, and bypass behavior.
- Coherent full-scale mix normalization and every delay budget boundary used.
- Independent switch debounce, bounce/hold/release and timer rollover.
- Simultaneous/nearby footswitch presses, reset, standby and unmute behavior.
- Encoder startup (no phantom rotation) and diagnostic CSV/header output.

The mocks do not emulate I2S/DMA, interrupts, codec hardware, the cooperative
scheduler, or the library delay/Freeverb/filter algorithms. These are logic and
memory-safety regression tests, not substitutes for a real Teensy build and
bench testing. In a ptrace-based runner where LeakSanitizer cannot run, use
`ASAN_OPTIONS=detect_leaks=0 bash test/run_host_tests.sh`; this leaves address
and undefined-behavior checks enabled but does not test leaks.

## CPU and audio-pool logging

Build/upload `teensy41_diagnostics`, open a serial monitor at 115200, and send
lowercase `r` after selecting a test configuration and allowing it to settle.
The command prints metadata and CSV headers, then resets the library CPU/pool
high-water marks. Rows are reported roughly once per second; peaks accumulate
since the last reset. A row is skipped when USB transmit capacity is insufficient.

Each row includes effect states, actual applied parameters, CPU percent,
maximum audio CPU percent, current/peak audio-pool blocks, pool allocation,
and a block-time estimate derived from CPU percent.

Important interpretation:

- CPU values cover the AudioStream update, not the entire main-loop/UI workload.
- `estimated_max_block_us = cpu_max_percent / 100 * block_samples / fs * 1e6`.
  It is not an independent timing measurement and not analog round-trip latency.
- Audio-pool blocks do not equal total RAM usage.
- Muted library delay/reverb processing can remain active.
- Logging itself changes the foreground workload. State the diagnostic build
  in the paper and compare with a logging-disabled build when timing matters.

For each case, use a known repeatable input, note levels and equipment, allow
at least five seconds to settle, send `r`, record at least 60 seconds, and repeat
three times. This is a suggested protocol, not a record of completed tests.

| Configuration | Required conditions |
| --- | --- |
| All effects off | Same digital graph/prefilter; not true hardware bypass |
| Delay only | 10, 425, 750, 1000 ms base; feedback 0, preset and maximum |
| Chorus only | Preset, minimum base/max requested depth, maximum base/depth; rate extremes |
| Reverb only | Preset and parameter extremes |
| All effects | Maximum delay plus chorus/reverb; idle and actively adjusting controls |
| Standby/switching | Rapid independent presses, boot in each master-switch position, line-out and headphones |

Check that audio-pool peaks leave reserve, listen for dropouts and artifacts,
and repeat with sustained/high-level input. Do not infer no underruns merely
from low average CPU; use direct timing/capture where needed.

## Remaining hardware measurements

1. Capture analog input and output simultaneously for round-trip latency.
   Measure the dry/digital-bypass path separately from intentional effect delays.
2. Measure bypass frequency response, terminated-input noise, SNR, THD+N, and
   clipping thresholds with input level/load/bandwidth/calibration documented.
   The always-on 3.2 kHz prefilter intentionally attenuates high frequencies.
3. Listen and capture while changing delay time, chorus settings and switches;
   parameter smoothing and click-free crossfading are not implemented here.
4. Confirm the 400-block allocation and applied input level on the actual board.
5. Update the manuscript, cost table and measured result tables only with real
   data from a recorded commit. Keep the existing historical numbers separately.

Archive raw logs, instrument settings, toolchain/package versions and firmware
SHA together. After hardware validation, tag that exact tested commit; do not
mark an untested software-only change as the paper's measured release.
