# Effects-off frequency-response characterization

**Measurement status:** preliminary (single manually transcribed readings; no measurement uncertainty).

This folder reproduces the frequency-response analysis for an **earlier experimental effects-off firmware sketch**. All three guitar effects were disabled, **but the shared 3.2 kHz biquad low-pass pre-filter remained enabled**. This is *not* analog true bypass and must **not** be attributed to the repository's newer unfiltered dry path.

## Equipment and acquisition

- Teensy 4.1 + SGTL5000 codec; 44.1 kHz; 128-sample blocks.
- UNI-T UTG4082A function generator, sinusoidal excitation at the tabulated frequencies.
- Hantek MSO5102D oscilloscope; CH1=input, CH2=output; peak-to-peak voltage readings.
- Digital pre-filter: `preFilter.setLowpass(0, 3200.0f, 0.707f)`.
- Gain calculation: `20*log10(Output_mVpp/Input_mVpp)`.

The precise probe points, output load, oscilloscope bandwidth, and full acquisition conditions were not all recorded. Generator panel amplitude should not be substituted for measured CH1 voltage. See [the master measurement report](../../measurements.md) for limitations and original capture references.

## Contents

- [Freq_Response.m](Freq_Response.m) — MATLAB source; regenerates the data table, two figures, and PDF/PNG exports.
- [frequency_response.csv](frequency_response.csv) — numeric readings and per-point provenance notes.
- Measured response: Figure 1, absolute voltage gain (dB), logarithmic frequency axis.
- Theoretical comparison: Figure 2, both traces independently normalized to 1 kHz.

Run `Freq_Response.m` from this directory in MATLAB with Signal Processing Toolbox. Generated plots have IEEE single-column dimensions (3.5 × 2.6 in), a vector-PDF export, and a 600-DPI PNG export. Generated exports are produced locally by the script; absence of an export in the repository does not imply missing source data.

## Interpretation and cautions

Gain is approximately +0.44 to +1.43 dB from 20 Hz to 2 kHz, then decreases at higher frequencies: about −7.69 dB at 5 kHz, −18.54 dB at 10 kHz and −24.08 dB at 15 kHz. This behavior is consistent with the shared digital low-pass pre-filter but does not isolate that filter from analog circuitry and codec effects.

The MATLAB theoretical curve uses a *second-order Butterworth approximation* designed by `butter()`, **not necessarily the exact Teensy biquad coefficients**. Therefore it is a qualitative filter-shape comparison, not a measured transfer function or exact implementation simulation.

The **100 Hz** reading was explicitly corrected to **558 mVpp input, 656 mVpp output**. The **10 kHz** output (~75 mVpp) is approximate. The 1 kHz sweep table lists 580 mVpp input, whereas a separate screenshot shows 584 mVpp; these observations have not been merged. A 20 kHz point and repeatability/uncertainty measurements are still missing.

For published figures, label them *effects-off, 3.2 kHz pre-filter enabled* and disclose these limitations. The repository's [capture gallery](../capture-gallery.md) preserves supporting screenshots.
