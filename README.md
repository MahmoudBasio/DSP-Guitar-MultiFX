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

## Performance

Current experimental evaluation is performed at **44.1 kHz** using **128-sample audio blocks**.

| Metric                      | Measured Result |
| --------------------------- | --------------: |
| Sample Rate                 |        44.1 kHz |
| Audio Block Size            |     128 samples |
| Block Duration              |       ≈ 2.90 ms |
| Maximum DSP Processing Time |       145.29 µs |
| Maximum Reported CPU Usage  |           5.01% |

### CPU Utilization

| Configuration           | Maximum CPU Usage |
| ----------------------- | ----------------: |
| Bypass                  |             4.70% |
| Delay                   |             3.03% |
| Reverb                  |             2.86% |
| Chorus                  |             4.98% |
| Delay + Reverb + Chorus |             5.01% |

These values represent measurements from the current firmware implementation and test configuration.

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

* U8g2
* CriticalTaskScheduler

The repository is configured for:

```ini
platform = teensy
board = teensy41
framework = arduino
```

## Adding a New Effect

The DSP architecture is designed to make effects modular.

To implement a new effect:

1. Create the new effect implementation inside `firmware/src/effects/`.
2. Implement the project's effect interface.
3. Define the effect parameters and processing logic.
4. Register the effect with the effect-management system.
5. Connect its parameters to the user interface as required.

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
pio run
```

Upload to the Teensy 4.1:

```bash
pio run --target upload
```

## Research

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
* ORCID:(https://orcid.org/0009-0007-1747-1609)

## Project Status

Active development.

Planned work includes additional DSP effects, expanded experimental characterization, audio-quality measurements, and further hardware refinement.


<img width="666" height="375" alt="7448d674-3cfc-4cf9-ab88-5cb7c3a28c2d" src="https://github.com/user-attachments/assets/63558f9f-1172-43c1-9434-3829b08f5324" />

