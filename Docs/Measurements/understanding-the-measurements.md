# Understanding the measurements

[Main results and methodology](../measurements.md) · [All oscilloscope captures](capture-gallery.md) · [Repository review](repository-review.md)

This guide explains what each test asks, how its numbers are calculated, what a reader should look for in the screenshots, and what the result can support. It explains the existing evidence; it does not represent additional bench testing.

## 1. What is being tested?

The pedal receives an analog voltage representing sound. The SGTL5000 ADC converts it into digital samples. The Teensy processes those samples, and the DAC converts the result back into an analog voltage.

A signal generator provides a repeatable input. A sine wave tests one frequency at a time; a brief pulse helps reveal when an echo starts and how long a reverb tail persists. A guitar performance is useful for listening, but its changing notes and levels make precise input/output comparisons harder.

CH1, shown in yellow, is the input reference. CH2, shown in cyan, is the output. The experimenter identified these roles. The exact physical measurement points still need confirmation: measuring at pedal jacks includes different circuitry from probing directly at the codec.

The experiments answer different questions:

| Test | Question |
|---|---|
| Frequency response | Which frequencies get amplified or attenuated? |
| Harmonics | Does a sine-wave output contain energy at multiples of its frequency? |
| Delay timing | Is the echo spaced by the requested time? |
| Feedback | How much of one echo remains in the next? |
| Latency | How long does the first output response take to appear? |
| Noise | How much unwanted output remains without the intended signal? |
| Reverb | How does the tail persist and change with settings? |
| Chorus | Does the time-varying delay behave as programmed? |

A successful echo-timing test does not establish low noise or low distortion. Each test supports a separate claim.

## 2. Reading the oscilloscope

### Horizontal axis: time

A setting of 20 ms/div means one major horizontal division represents 20 milliseconds. Five divisions represent 100 ms. A setting of 200 µs/div shows a much shorter interval.

- 1 second = 1000 milliseconds (ms).
- 1 millisecond = 1000 microseconds (µs).
- 345 µs = 0.345 ms; it is not 345 ns.

Time cursors mark two selected positions. Their difference, `dt` or delta time, is the interval being measured. The scope may also show `1/dt`. For a 100 ms interval, this is 10 Hz. That does not mean the generator is running at 10 Hz: it means the chosen interval has a reciprocal of 10 per second.

### Vertical axis: voltage

Each channel has its own scale. If CH1 is 200 mV/div and CH2 is 100 mV/div, an equal-height trace on CH2 represents half the voltage. The channels may also be positioned at different vertical offsets for readability; a trace appearing higher on screen is not necessarily a larger signal.

**Peak-to-peak voltage (Vpp)** is maximum voltage minus minimum voltage. A zero-centered sine with a +0.3 V peak and −0.3 V trough has 0.6 Vpp.

**Baseline-to-peak amplitude** is the height of one pulse above its local baseline. It is the quantity used in the feedback table. A pulse with undershoot may have a Vpp greater than its positive peak height, so the two measures cannot be interchanged.

**RMS voltage** describes the effective magnitude over a specified interval. For an ideal zero-mean sine only, Vrms = Vpp / (2 × sqrt(2)). This shortcut does not apply to arbitrary pulses or noise. DC offset also changes an RMS reading unless excluded.

**DC coupling** preserves the signal's DC component. **AC coupling** removes DC and can affect low-frequency behavior. Changing coupling between acquisitions can change the meaning of a comparison.

### What a screenshot preserves—and what it does not

A screenshot preserves the visible trace, scales, selected measurements and cursor positions. Raw waveform data preserve sampled voltage values over time, allowing new calculations afterward. A screenshot cannot recover the exact original sample record, FFT settings, or hidden acquisition configuration.

The gallery contains 31 PNG display copies verified pixel-identical to the source BMPs. The original BMP files are retained. The archive's GIF entries were empty. Image conversion improves compatibility; it does not improve measurement accuracy.

## 3. Effects OFF does not mean the entire circuit is bypassed

In the measured sketch, disabling delay, reverb and chorus does not remove the ADC, DAC, pre-filter or output mixer. The signal still goes through the 3.2 kHz low-pass filter and a dry gain of 0.65.

- **Dry** means the component that is not given the named time-based effect; it is not necessarily unfiltered.
- **Wet** means the processed effect contribution.
- **Digital effects bypass** means effects are disabled while audio still passes through the digital system.
- **Analog hardware bypass** routes around the digital processing chain. This test does not demonstrate that mode.
- **Dual mono** means the same signal is sent to left and right. It does not establish independent stereo processing.

The code enables the filter regardless of the effect flags. When chorus is disabled, it forwards the filtered signal, with small numeric rescaling. Delay/reverb wet contributions are muted. This is why the measured high-frequency reduction can remain with all effects OFF.

The supplied sketch starts with the flags set to true. The experimenter switched them OFF for the sweep. Record the runtime state, not just the startup code.

## 4. Frequency response, step by step

### What gain means

Voltage gain is output amplitude divided by input amplitude:

```text
A = Vout / Vin
G_dB = 20 × log10(A)
```

Using the same units makes the ratio dimensionless. Positive dB means output voltage is larger; negative dB means it is smaller; 0 dB means equal voltage amplitudes at that frequency. This voltage comparison alone does not specify power gain into different impedances.

For the corrected 100 Hz reading:

```text
A = 656 / 558 = 1.1756
G = 20 × log10(1.1756) ≈ +1.41 dB
```

The output amplitude is about 17.6% greater than the input. This is a voltage percentage, not a percentage increase in perceived loudness.

At 5 kHz:

```text
A = 260 / 630 = 0.4127
G ≈ −7.69 dB
```

Only about 41.3% of the input voltage amplitude remains. At 15 kHz, 40/640 = 0.0625: about 6.25% remains, giving −24.08 dB.

### Absolute versus normalized response

Absolute gain compares output with input at each frequency. A normalized response subtracts a chosen reference gain to show changes in tone independently of overall level.

```text
Relative gain at 5 kHz = G(5 kHz) − G(100 Hz)
                       ≈ −7.69 − 1.41 ≈ −9.10 dB
```

Using unrounded readings gives approximately −9.09 dB. Small last-digit differences come from rounding.

The input amplitude changes across the sweep. Calculating a separate output/input ratio at every point accounts for that amplitude change for a linear operating system. It does not prove that every measurement avoided clipping or other level-dependent behavior.

### What the filter does

A low-pass filter progressively reduces higher frequencies. It is not a brick wall at 3200 Hz: signals above that frequency remain, but at lower amplitude. The programmed Q of 0.707 gives the intended second-order response a smooth transition without a strong resonant peak.

For that intended response, the cutoff corresponds approximately to a 3 dB reduction relative to the low-frequency filter gain. A −3 dB voltage ratio is about 0.708, not half the voltage. Half voltage is approximately −6.02 dB.

The code's 3200 Hz setting is a design parameter. A measured system cutoff needs sufficiently close sweep points around the transition, referenced to the system's passband gain. The existing 2 kHz and 5 kHz points do not locate it precisely.

Reducing upper harmonics can make guitar audio less bright even when its fundamental notes pass. These electrical measurements show spectral shaping; they do not measure listener preference or perceived sound quality.

### Why the high-frequency mismatch matters

The ideal filter comparison matches at 100 Hz and predicts more attenuation at 10 and 15 kHz than measured. This means that the ideal filter alone does not reproduce every reading. Noise can inflate Vpp because Vpp uses extreme sample values; leakage or other signal paths may also matter. These are possible causes, not findings.

The 0.65 mixer coefficient corresponds to 20 log10(0.65) ≈ −3.74 dB within DSP. Positive measured gain at 100 Hz is still possible because other gain stages contribute. The data do not separately identify each stage's gain.

## 5. Harmonics and the 0.921% estimate

For a 1 kHz input, the fundamental H1 is at 1 kHz. H2, H3 and H4 are at 2, 3 and 4 kHz. These frequencies must be checked in the original FFT before treating a peak as a harmonic.

An FFT displays frequency content. Its window and record length affect peak shape, leakage and resolution. A reported peak near a harmonic frequency can be contaminated by noise or leakage; the numeric level alone is not sufficient evidence.

The reported levels use an unverified dB reference. If all four readings use the same reference, subtracting H1 yields levels relative to the fundamental, called dBc:

```text
H2 relative level = −9.56 − 34 = −43.56 dBc
H2 / H1 = 10^(−43.56 / 20) ≈ 0.00664 = 0.664%
```

A negative dB value does not mean negative voltage. It means a smaller level than the chosen reference.

Combine the harmonic amplitudes using root-sum-square:

```text
THD(H2–H4) = sqrt(0.664² + 0.531² + 0.355²) percent
           ≈ 0.921%
```

The values are squared and combined because distinct harmonic components contribute mean-square energy. Simply adding their amplitude percentages would give a different, inappropriate quantity.

This estimate excludes H5 and higher. THD+N also includes noise, whereas this calculation does not. A generator-only measurement is necessary because the input may already contain harmonics. A linear filter shapes existing harmonics; it does not inherently create new ones. The calculation cannot identify whether a particular op-amp, codec stage or numerical operation caused the observed levels.

## 6. Delay time versus latency

Imagine a pulse entering the pedal. First, its dry output appears after the system's latency. Later, the echo appears after the programmed delay relative to that dry output.

For the 250 ms experiment:

```text
Input to echo:       256.5 ms
Dry output to echo:  250.0 ms
Inferred offset:      6.5 ms
```

This subtraction separates the programmed echo interval from an approximate common-path offset. It assumes the selected features correspond and acquisitions are comparable; it is not a separate calibrated latency measurement.

The results support agreement at the displayed cursor resolution. A displayed 250.0 ms does not establish exact timing to every sample. Cursor placement, edge smoothing and screenshot resolution limit accuracy.

Use corresponding features on each waveform. An input pulse's peak and an output pulse's first rise are different features and can introduce a systematic offset. The existing record does not document a standardized threshold for every capture.

## 7. Feedback and echo amplitude

Feedback sends part of a delayed signal back into the delay input, creating further echoes. In a simplified model without frequency-dependent shaping:

```text
Next echo amplitude ≈ feedback coefficient × previous echo amplitude
```

For the 0.50 setting, the measured first and second echoes are 264 and 136 mV:

```text
Measured ratio = 136 / 264 = 0.515
Expected simplified ratio = 0.50
```

That is close agreement, but not identical. The repeat filter changes pulse shape, and peak readings have uncertainty.

The dry pulse must not be treated as the preceding echo when estimating feedback. Dry and wet branches have different mixer gains:

```text
Dry coefficient = 0.65
Wet coefficient = 0.75 × 0.55 = 0.4125
Predicted first echo / dry = 0.4125 / 0.65 ≈ 0.635
```

This predicts roughly 63.5% before additional filter effects, even when feedback is zero. Feedback zero still permits one delayed copy; it prevents further recirculation.

The 20 mV last echo is especially sensitive to cursor placement and noise. As an illustrative example, a 2 mV reading error is 10% of 20 mV but only 1% of 200 mV. This example explains relative uncertainty; it is not an assigned instrument error specification.

## 8. Why 345 µs is not established total latency

A 1 kHz periodic waveform repeats every 1 ms. Comparing one output edge with a nearby input edge can select the wrong cycle. A real delay of 9.345 ms and an apparent alignment of 0.345 ms differ by nine complete cycles.

The 9.350 and 9.500 ms captures are therefore more informative candidates than the short periodic separation, but firmware attribution, event selection and repeatability still need resolution. They must not be averaged into a precise final specification without that work.

Three timing quantities must stay separate:

| Quantity | Meaning |
|---|---|
| Block duration | Time represented by 128 samples: 128/44100 ≈ 2.90 ms |
| Processing time | CPU time spent executing audio work |
| End-to-end latency | Time from the chosen analog input event to its corresponding analog output event |

Fast computation does not remove buffering and conversion delays. Conversely, the 2.90 ms block duration does not by itself establish how many blocks of delay the complete graph introduces.

## 9. Noise and SNR

Noise is unwanted variation in the output. SNR compares a defined signal level to noise measured under compatible conditions:

```text
SNR_dB = 20 × log10(signal RMS / noise RMS)
```

For the reported values:

```text
224 / 5.20 ≈ 43.08
20 × log10(43.08) ≈ 32.68 dB
```

The arithmetic is correct for those numbers, but the experimental comparison is incomplete. Random noise has no stable cycle, so the scope's Cycle RMS measurement may use an unsuitable interval. Different coupling, vertical ranges and bandwidth can also affect the result. Instrument noise contributes to what is displayed.

A quiet-looking trace does not automatically mean low audio-band noise, and a nonzero voltage does not identify a noise source. Future measurements should define the input termination, output load, frequency bandwidth, gain, record duration and DC removal. The existing result remains an indicative ratio.

## 10. Reverb: room size, damping and RT60

Reverb is a dense set of delayed contributions that forms a decaying tail. Unlike a simple delay test, individual reflections are generally not isolated as one clearly separated echo each.

R1–R3 hold damping at 0.3 and change room size from 0.2 to 0.5 to 0.9. This comparison asks how the algorithm's persistence changes. The parameter does not mean a room of 0.2, 0.5 or 0.9 meters, and it does not establish physical room dimensions.

R4–R6 hold room size at 0.7 and change damping from 0.1 to 0.5 to 0.9. This comparison is intended to examine frequency-dependent decay. The screenshots alone do not separate low-frequency and high-frequency energy, so they cannot establish a numerical damping response.

The supplied reverb output is wet-only. The cyan trace shows the reverb contribution rather than a dry signal added to it. Input/output coefficients of 0.5 and 0.45 scale the signal around the algorithm, but its internal response prevents interpreting their product as the full system gain.

**RT60** is the time associated with a 60 dB decay in reverberant energy level. A 60 dB reduction corresponds to a factor of one million in energy, or one thousand in amplitude. The point where a trace disappears into screen noise is not necessarily this point.

With sufficient raw data, a decay curve can be evaluated above the noise floor and a documented fitting range used. For example, extrapolating a reliable 20 dB decay interval to 60 dB assumes an approximately linear decay in dB over that region. That is a method for future analysis, not a calculation performed on these screenshots. No RT60 has been established here.

## 11. Chorus: base, depth and rate

The supplied experiment varies delay according to:

```text
d(t) = 20 + 5 × sin(2π × 0.5 × t) milliseconds
```

The 20 ms base is the center value. The 5 ms depth is the maximum change either side, so delay ranges from 15 to 25 ms. Its peak-to-peak variation is 10 ms. The 0.5 Hz rate means half a cycle per second, or one full cycle every two seconds.

A changing delay shifts the instantaneous pitch of the wet signal. With no dry signal, this is vibrato-like behavior. Adding dry audio produces the interference and layering associated with the mixed chorus effect.

The ideal frequency-deviation prediction follows from how quickly delay changes:

```text
For y(t) = sin(2π f0 [t − d(t)]), with d in seconds:
instantaneous frequency = f0 × [1 − derivative of d(t)]
maximum deviation = f0 × depth_seconds × 2π × rate_hz
                  = 1000 × 0.005 × 2π × 0.5
                  ≈ 15.7 Hz
```

This is an ideal model calculation. Interpolation and the actual implementation affect the waveform. It is not an experimentally measured frequency excursion.

The C1 capture shows only a few milliseconds, a tiny fraction of the two-second cycle. It can show input/output waveforms at one moment but cannot verify the full modulation. A longer recording covering several cycles is required.

## 12. Evidence strength and uncertainty

**Measured** means a value was read from an instrument under a particular setup. **Calculated** means arithmetic was applied to readings. **Predicted** means the value comes from code or a mathematical model. A correctly calculated number can still depend on uncertain measurements.

**Qualitative** evidence describes a visible pattern, such as a longer tail. **Quantitative** evidence gives a value with a defined method and preferably uncertainty. More decimal places do not create more measurement accuracy.

The strongest existing results are the displayed delay intervals and feedback ratios. The frequency-response table also documents a useful filtered baseline, with limits on source-image coverage. Harmonic distortion, SNR, total latency, reverb decay time and chorus modulation need further validation before use as final specifications.

A limitation is not a claim that the hardware failed. It identifies the boundary of what the present record demonstrates.

## 13. How this fits the repository

The repository review is pinned to commit `0fa40894193a667b490a2ec143032e2c0e5c9ec3`. The current-at-review firmware has a separate chorus wet filter and an unfiltered dry path. This differs from the older tested sketch. Keeping both configurations explicit lets readers understand design evolution without attributing old data to new code.

| File or folder | Why it exists |
|---|---|
| `Docs/measurements.md` | Main report: methods, results and representative images |
| `Docs/Measurements/understanding-the-measurements.md` | This explanation guide |
| `Docs/Measurements/capture-gallery.md` | All screenshots with individual reading notes |
| `images/` | PNG copies that render in Markdown |
| `originals/` | Unmodified BMP source evidence |
| `data/frequency-response.csv` | Sweep readings and calculated gain for reuse in plots |
| `data/delay-feedback.csv` | Pulse amplitudes and echo ratios |
| `data/screenshot-manifest.csv` | Maps display files to original paths and checksums |
| `firmware-reference/` | Original supplied effects-off paste, not a compiled build |
| `repository-review.md` | Recommended integration and documentation corrections |
| Root `README.md` | Entry point linking to the measurement report |

A SHA-256 checksum identifies file contents and helps detect later changes. It does not certify a measurement's validity. The original pasted firmware retains Markdown escapes and is a provenance record, not a ready-to-flash file.

The README should introduce the evidence and link to the detailed report. It should not promote provisional harmonic/SNR figures as verified product specifications. Use the report in the paper as a traceable experimental record, retaining the limitations and exact tested configuration.
