# DSP Guitar MultiFX

**An open-source modular real-time embedded DSP platform for guitar multi-effects, built around the Teensy 4.1.**

The project combines real-time audio processing, embedded firmware, custom electronics, user-interface hardware, PCB design, and mechanical design into a complete standalone multi-effects platform.

![DSP Guitar MultiFX](Images/DSP_Final_Desing.png)

## Overview

DSP Guitar MultiFX was developed as an open and extensible platform for implementing real-time audio effects on embedded hardware.

The system uses a **Teensy 4.1** with the **SGTL5000-based Teensy Audio Board** and a modular C++ firmware architecture designed to make additional effects easy to integrate.

Current effects include:

* Delay
* Reverb
* Chorus

The project includes the complete firmware alongside hardware design files, PCB resources, mechanical design, documentation, and bill of materials.

## Key Features

* Teensy 4.1 ARM Cortex-M7 platform
* Real-time embedded audio processing
* Modular C++ DSP architecture
* Extensible `IEffect` interface for additional effects
* Delay, Reverb, and Chorus effects
* SGTL5000 audio interface
* OLED-based user interface
* Rotary encoder parameter control
* Dedicated effect footswitches
* PlatformIO + Arduino framework
* Custom PCB and hardware design
* Mechanical enclosure design
* Open-source hardware and firmware

## Audio configuration and validation status

The firmware uses **44.1 kHz**, **128-sample blocks**, and **signed 16-bit audio
samples**. The custom chorus uses float arithmetic internally. The graph is mono
with identical left/right outputs; codec hardware capabilities are not the
implemented processing resolution. The block period is not round-trip latency.

Safety fixes change gain staging, input-level configuration, audio-pool allocation,
chorus bypass processing, and control handling. The figures below are **historical
reported results**, retained unchanged, and must not be attributed to the revised
firmware without new measurements. Their exact test commit, raw logs and complete
test procedure have not been supplied here.

| Metric                      | Historical reported result |
| --------------------------- | --------------: |
| Sample Rate                 |        44.1 kHz |
| Audio Block Size            |     128 samples |
| Block Duration              |       ≈ 2.90 ms |
| Maximum DSP Processing Time |       145.29 µs |
| Maximum Reported CPU Usage  |           5.01% |

### Historical CPU utilization

| Configuration           | Maximum CPU Usage |
| ----------------------- | ----------------: |
| Bypass                  |             4.70% |
| Delay                   |             3.03% |
| Reverb                  |             2.86% |
| Chorus                  |             4.98% |
| Delay + Reverb + Chorus |             5.01% |

These figures are not newly verified results. Audio-library CPU utilization does
not include the whole foreground UI workload. Block-processing time calculated
from CPU percent is not an independent measurement or end-to-end audio latency.
Use the diagnostic build and [measurement procedure](Docs/Testing.md) to collect
new results at a recorded commit.

### Firmware safety changes

* Chorus depth is limited by base delay and the 2048-sample history buffer; the
  instantaneous read delay is bounded to 1-2046 samples.
* Each footswitch is independently debounced in foreground polling; effect
  management no longer runs in footswitch interrupts.
* The audio pool is increased to 400 blocks to budget for the 1000 ms delay plus
  3 ms modulation and graph overhead. Verify peak occupancy on hardware.
* Gain pairs are normalized, and delay input gain is reduced as feedback rises.
  This changes levels and reduces avoidable summing saturation; it does not
  certify the complete analog/digital path as distortion-free.
* Standby digitally mutes both output channels as well as headphone volume.
* An optional diagnostics build logs CPU, audio-pool usage, states and parameters.

The current firmware still needs hardware latency/audio-quality measurements,
worst-case stress tests, and evaluation of switching/modulation artifacts.

## System Architecture

The platform separates audio processing, DSP effects, system control, and user-interface functionality into independent modules.

```text
Guitar Input
     │
     ▼
Analog / Audio Interface
     │
     ▼
SGTL5000 Codec
     │
     ▼
Teensy 4.1
     │
     ├── Audio Routing
     ├── DSP / Effects
     ├── Effect Manager
     ├── System Control
     └── User Interface
     │
     ▼
Audio Output
```

The firmware source is divided into:

```text
firmware/src/
├── audio/
├── core/
├── dsp/
├── effects/
├── ui/
└── main.cpp
```

This organization keeps individual effects separated from audio routing and interface logic.

See [the actual signal graph and implementation details](Docs/Architecture.md).
Delay and Freeverb use Teensy Audio Library implementations; the modulated
chorus is implemented in this repository. Effects-off retains the codec,
3.2 kHz prefilter and output gains: it is not a transparent analog bypass.

## Hardware

The platform is built around:

* Teensy 4.1
* Teensy Audio Board / SGTL5000
* Custom interface PCB
* 128×64 OLED display
* Rotary encoder
* Dedicated effect footswitches
* Audio input/output connections
* Custom enclosure

### PCB

| Top                        | Bottom                        |
| -------------------------- | ----------------------------- |
| ![](Images/pcb_top_3d.png) | ![](Images/pcb_bottom_3d.png) |

### Schematic

![Schematic](Images/schematic.png)

Hardware design resources are available in the [`Hardware`](Hardware/) directory.

## Firmware

The firmware is implemented in **C++** using the **Arduino framework for Teensy** and built using **PlatformIO**.

Current PlatformIO dependencies include:

* Teensy platform 6.0.0
* U8g2 2.36.18
* CriticalTaskScheduler 1.0.7

The repository is configured for:

```ini
platform = teensy@6.0.0
board = teensy41
framework = arduino
```

## Adding a New Effect

The architecture provides a shared effect-control interface, not automatic
plugin discovery or runtime signal-chain reordering.

To implement a new effect:

1. Create the new effect implementation inside `firmware/src/effects/`.
2. Implement the project's effect interface.
3. Define the effect parameters and processing logic.
4. Add its audio objects and static connections in `audio_graph.cpp`.
5. Add explicit management, parameter mapping, menu/display entries, and tests.
6. Review signal levels, audio-pool capacity and processing time for the new graph.

This allows new algorithms such as distortion, tremolo, or additional modulation effects to be integrated without redesigning the complete audio system.

## Repository Structure

```text
DSP-Guitar-MultiFX/
├── BOM/          # Bill of materials
├── Docs/         # Technical documentation
├── Hardware/     # PCB and hardware design
├── Images/       # Project images and renders
├── Mechanical/   # Mechanical / enclosure design
├── firmware/     # Teensy firmware
└── README.md
```

## Getting Started

Clone the repository:

```bash
git clone https://github.com/MahmoudBasio/DSP-Guitar-MultiFX.git
cd DSP-Guitar-MultiFX/firmware
```

Open the `firmware` directory using **VS Code + PlatformIO**.

Build the project:

```bash
pio run -e teensy41
```

Upload to the Teensy 4.1:

```bash
pio run -e teensy41 --target upload
```

## Research

Run the host regression suite on a Bash/GCC system:

```bash
bash test/run_host_tests.sh
```

From the `firmware` directory, the optional hardware diagnostics build is:

```bash
pio run -e teensy41_diagnostics --target upload
pio device monitor --baud 115200
```

Send lowercase `r` after settling each test configuration to print a CSV header
and reset peak measurements. [Testing.md](Docs/Testing.md) describes coverage,
limits, data capture and the hardware checks required before publication.

This platform is also being developed and experimentally evaluated as part of research into modular, low-cost, open-source real-time embedded DSP systems.

Experimental results, architecture details, resource utilization, latency, and audio-performance measurements are being documented as part of the associated research work.

Publication information and DOI will be added here when available.

## Citation

If you use this project in academic or research work, please cite the repository using the citation information provided in `CITATION.cff`.

Publication citation information will be added following publication of the associated research article.

## License

This project is released under the **MIT License**. See [`LICENSE`](LICENSE) for details.

## Author

**Mahmoud Basiony**

Robotics • Embedded Systems • Real-Time DSP

* GitHub: [@MahmoudBasio](https://github.com/MahmoudBasio)
* LinkedIn: [linkedin.com/in/basio](https://www.linkedin.com/in/basio)
* ORCID: https://orcid.org/0009-0007-1747-1609

**Tamer Mohamed**
  
Robotics • Mechanical Designer • Real-Time DSP

  
* GitHub: [@Temo-MO](https://github.com/Temo-Mo)
* LinkedIn: https://www.linkedin.com/in/tamer-mohamed-16123a294/

## Project Status

Active development.

Planned work includes additional DSP effects, expanded experimental characterization, audio-quality measurements, and further hardware refinement.



