# PWM IP Core — Avalon-MM Peripheral on Cyclone V FPGA

A synthesizable VHDL implementation of a Pulse Width Modulation (PWM) IP core, designed as an Avalon-MM slave peripheral and integrated into a full Qsys SoC with a Nios II processor.

---

## Overview

This IP core generates a PWM signal with programmable period, duty cycle, and interrupt support. It is designed to be compatible with the Intel/Altera Avalon Memory-Mapped interface and can be instantiated as a peripheral in any Qsys-based SoC system.

The IP was developed following the complete RTL design flow: VHDL modeling, functional simulation, logic synthesis, timing analysis, place-and-route, SoC integration, and software driver development.

---

## Features

- Programmable PWM period and duty cycle via memory-mapped registers
- Shadow registers for glitch-free synchronous parameter updates
- Interrupt generation after a configurable number of PWM cycles
- Avalon-MM slave interface (32-bit data bus, 3-bit address)
- Clock Domain Crossing support via Avalon-MM Clock Crossing Bridge
- C API for software-level register access
- Validated on Altera DE0-CV (Cyclone V — 5CSEMA4U23C6) at 25 MHz

---

## Register Map

| Register  | Address | Access | Description                        |
|-----------|---------|--------|------------------------------------|
| Status    | 0x0     | R      | PWMState, PulseState, IrqRq        |
| SetCtrl   | 0x0     | W      | Set Start and IrqEn bits           |
| Period    | 0x1     | R/W    | PWM period value                   |
| DCycle    | 0x2     | R/W    | Duty cycle value                   |
| NbCycles  | 0x3     | R/W    | Number of cycles before IRQ        |
| ClrCtrl   | 0x4     | W      | Clear Start and IrqEn bits         |

---

## Repository Structure

```
PWM_IP_Core/
├── README.md
├── rtl/
│   └── pwm.vhd                  # Synthesizable VHDL source
├── sim/
│   └── macroInterfaceAvalon.do  # ModelSim simulation macro
├── constraints/
│   └── pwm.sdc                  # Timing constraints (25 MHz clock)
└── software/
    ├── pwm_regs.h               # Register access macros
    ├── pwmAPI.h                 # C API header
    └── pwmAPI.c                 # C API implementation

```

---

## Design Flow

1. **RTL modeling** — VHDL implementation of the PWM datapath and control logic based on the functional specification and register map
2. **Functional simulation** — ModelSim simulation using TCL-driven macro scripts covering register access, PWM generation (25% and 75% duty cycle), IRQ behavior, and boundary conditions (0%, 1%, 99%, 100% duty cycle)
3. **Logic synthesis** — Quartus Prime synthesis targeting Cyclone V, with synthesis warnings reviewed for synchronous design compliance
4. **Timing analysis** — TimeQuest timing analysis at 25 MHz (period = 40 ns); setup slack verified on Slow corner, hold slack on Fast corner
5. **Place and route** — Full compilation and bitstream generation
6. **SoC integration** — IP instantiated as an Avalon-MM slave in a Qsys system alongside a Nios II processor; Clock Domain Crossing managed via Avalon-MM Clock Crossing Bridge
7. **Hardware validation** — Register access verified using System Console TCL scripts over JTAG
8. **Software driver** — C API developed for register-level control; tested via Nios II application on hardware

---

## Timing Results

| Parameter         | Value         |
|-------------------|---------------|
| Target frequency  | 25 MHz        |
| Clock period      | 40 ns         |
| Setup slack (Slow)| +36.466 ns    |
| Hold slack (Fast) | +0.135 ns     |
| Timing closure    | Achieved      |

---

## How to Simulate

1. Open ModelSim and create a new project in `sim/`
2. Add `rtl/pwm.vhd` as source file and compile
3. Start simulation on entity `pwm`
4. Execute the macro: `Tools > Tcl > Execute Macro > macroInterfaceAvalon.do`
5. There are several marco available. You just have to choose the one describing best what you want to simulate.


---

## Target Hardware

| Parameter   | Value                  |
|-------------|------------------------|
| FPGA family | Intel Cyclone V        |
| Device      | 5CSEMA4U23C6           |
| Board       | Altera DE0-CV          |
| Clock       | 25 MHz (pll.outclk2)   |

---

## Author

**Duval MAMBOU**
Engineering student — FPGA & SoC Design
ENIB — National School of Engineering of Brest, France
[LinkedIn](https://www.linkedin.com/in/duval-mambou) | [GitHub](https://github.com/duval-mambou)