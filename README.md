# build-everything

> Building 25 fundamental systems from scratch — memory to browser.

This repository is a long-term systems engineering project. Each folder contains a complete, self-contained implementation of a foundational piece of software — written from scratch, no shortcuts. The goal is not to ship products. The goal is to understand how everything actually works at the lowest level possible.

---

## The Roadmap

Projects are ordered by dependency. Each one builds on the knowledge and code from those before it.

| # | Project | Phase | Language | Status |
|---|---------|-------|----------|--------|
| 01 | [Memory Allocator](./01-memory-allocator) | Hardware Fundamentals | C | ✅ Done |
| 02 | [CPU Emulator](./02-cpu-emulator) | Hardware Fundamentals | C / C++ | 🔲 Not started |
| 03 | [Encryption Algorithm](./03-encryption-algorithm) | Hardware Fundamentals | C / Python | 🔲 Not started |
| 04 | [OS Kernel in Assembly](./04-os-kernel-asm) | OS Core Primitives | x86 Assembly | 🔲 Not started |
| 05 | [File System](./05-file-system) | OS Core Primitives | C | 🔲 Not started |
| 06 | [Scheduler](./06-scheduler) | OS Core Primitives | C | 🔲 Not started |
| 07 | [Operating System](./07-operating-system) | Full OS | C / Assembly | 🔲 Not started |
| 08 | [Shell](./08-shell) | Full OS | C | 🔲 Not started |
| 09 | [Compiler](./09-compiler) | Language Toolchain | C / Rust | 🔲 Not started |
| 10 | [Programming Language](./10-programming-language) | Language Toolchain | C / Rust | 🔲 Not started |
| 11 | [Package Manager](./11-package-manager) | Language Toolchain | Rust / Go | 🔲 Not started |
| 12 | [Virtual Machine](./12-virtual-machine) | Virtualization | C / Rust | 🔲 Not started |
| 13 | [Container Runtime](./13-container-runtime) | Virtualization | Go / Rust | 🔲 Not started |
| 14 | [Hypervisor](./14-hypervisor) | Virtualization | C / Rust | 🔲 Not started |
| 15 | [Database](./15-database) | Data & Networking | C / Rust | 🔲 Not started |
| 16 | [Network Protocol](./16-network-protocol) | Data & Networking | C / Rust | 🔲 Not started |
| 17 | [Web Server](./17-web-server) | Data & Networking | C / Rust | 🔲 Not started |
| 18 | [Blockchain](./18-blockchain) | Data & Networking | Rust / Go | 🔲 Not started |
| 19 | [Window Manager](./19-window-manager) | UI Layer | C / Rust | 🔲 Not started |
| 20 | [GUI Toolkit](./20-gui-toolkit) | UI Layer | C++ / Rust | 🔲 Not started |
| 21 | [Text Editor](./21-text-editor) | UI Layer | Rust / C++ | 🔲 Not started |
| 22 | [Version Control System](./22-version-control) | End Products | Rust / C | 🔲 Not started |
| 23 | [Game Engine](./23-game-engine) | End Products | C++ / Rust | 🔲 Not started |
| 24 | [IDE](./24-ide) | End Products | Rust / C++ | 🔲 Not started |
| 25 | [Browser](./25-browser) | End Products | C++ / Rust | 🔲 Not started |

---

## Phases

**Phase 1 — Hardware Fundamentals** `#01 → #03`
Memory, CPU emulation, and cryptographic primitives. Zero external dependencies. These are the atoms everything else is built from.

**Phase 2 — OS Core Primitives** `#04 → #06`
Bare-metal kernel in assembly, a working file system, and a process scheduler. The hardest phase. Everything clicks after this.

**Phase 3 — Full Operating System** `#07 → #08`
Integrate the kernel, file system, and scheduler into a complete OS. Add a shell — the first interactive program you run on your own system.

**Phase 4 — Language Toolchain** `#09 → #11`
A compiler, a programming language built on top of it, and a package manager. The compiler is the most educational project in this entire list.

**Phase 5 — Virtualization** `#12 → #14`
Bytecode VM, Linux container runtime, and a hypervisor using hardware virtualization (VT-x / AMD-V). The hypervisor is the apex of systems work.

**Phase 6 — Data & Networking** `#15 → #18`
Storage engine, TCP/IP from scratch, HTTP server, and a blockchain that ties together encryption and networking.

**Phase 7 — User Interface Layer** `#19 → #21`
Window manager, GUI toolkit, and text editor. Each one depends on the previous. The GUI toolkit powers everything in Phase 8.

**Phase 8 — End Products** `#22 → #25`
VCS (minimal Git clone), game engine, IDE, and finally — a browser. The browser is the final boss: it is an OS, a VM, a network stack, a layout engine, and a JS runtime fused into one.

---

## Repository Structure

```
build-everything/
│
├── 01-memory-allocator/
│   ├── README.md          ← what it is, how to build, how to run
│   ├── src/
│   ├── tests/
│   └── docs/
│
├── 02-cpu-emulator/
│   ├── README.md
│   ├── src/
│   ├── tests/
│   └── docs/
│
├── ...
│
└── 25-browser/
    ├── README.md
    ├── src/
    ├── tests/
    └── docs/
```

Each project folder contains:
- `README.md` — architecture decisions, what was learned, how to build and run
- `src/` — all source code
- `tests/` — test cases and test harness
- `docs/` — notes, diagrams, references used

---

## Rules

These rules keep the project honest:

1. **No libraries for the core logic.** Standard C library is allowed where necessary. No frameworks. No engines. No "almost from scratch."
2. **Every project must build and run.** Incomplete exploratory code goes in a branch, not main.
3. **Every project gets its own README** documenting what was built, key decisions made, and what was learned.
4. **Order matters.** Projects are built in the numbered order. No skipping ahead.
5. **Understand before moving on.** The goal is depth, not completion speed.

---

## Progress

```
Phase 1  ████████░░░░░░░░░░░░  1 / 3
Phase 2  ░░░░░░░░░░░░░░░░░░░░  0 / 3
Phase 3  ░░░░░░░░░░░░░░░░░░░░  0 / 2
Phase 4  ░░░░░░░░░░░░░░░░░░░░  0 / 3
Phase 5  ░░░░░░░░░░░░░░░░░░░░  0 / 3
Phase 6  ░░░░░░░░░░░░░░░░░░░░  0 / 4
Phase 7  ░░░░░░░░░░░░░░░░░░░░  0 / 3
Phase 8  ░░░░░░░░░░░░░░░░░░░░  0 / 4

Total    █░░░░░░░░░░░░░░░░░░░  1 / 25
```

---

## Why

Most developers use these systems every day without knowing how they work. This project is an attempt to close that gap completely — not by reading about them, but by building them.

The browser you are reading this on runs on an OS, which runs on a CPU, which uses a memory allocator, which relies on hardware abstractions that were themselves designed by someone who had to figure all of this out from first principles. This repo is that process, documented.

---

## References

Key books and resources used across the project:

- *Computer Systems: A Programmer's Perspective* — Bryant & O'Hallaron
- *Operating Systems: Three Easy Pieces* — Arpaci-Dusseau
- *Crafting Interpreters* — Robert Nystrom (free online)
- *Database Internals* — Alex Petrov
- *TCP/IP Illustrated* — W. Richard Stevens
- *Game Engine Architecture* — Jason Gregory
- [OSDev Wiki](https://wiki.osdev.org) — bare-metal OS development
- [Writing a Simple OS from Scratch](https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf) — Nick Blundell

---

## Author

**Ayush Anand** — [github.com/Developer-Ayush](https://github.com/Developer-Ayush)

---

*Started: 2026 · Est. completion: never (that's the point)*
