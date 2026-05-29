# Ultimate Source Engine (USE) – Echo 1.2

![Version](https://img.shields.io/badge/version-1.2.0_Echo-blue)
![License](https://img.shields.io/badge/license-GPLv3-green)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20(planned)-lightgrey)
![Status](https://img.shields.io/badge/status-active-brightgreen)

**The first 3D game engine built in Algeria.**  
Modern graphics. Old hardware. Total freedom.

---

<p align="center">
  <img src="docs/screenshot_showcase.png" alt="USE Engine Screenshot" width="80%">
</p>

---

## 📖 Table of Contents

- [Why USE?](#-why-use)
- [Features](#-features)
- [System Requirements](#-system-requirements)
- [Getting Started](#-getting-started)
- [Documentation](#-documentation)
- [Commercial Version (USE vsource)](#-commercial-version-use-vsource)
- [License](#-license)
- [Support & Community](#-support--community)
- [Credits](#-credits)

---

## 🤔 Why USE?

- **Lightning fast on old hardware** – Runs smoothly on a Core 2 Duo with integrated graphics, yet still delivers modern visual quality. No other engine supports such a range.
- **Familiar Source‑like workflow** – If you've ever used Hammer, you'll feel right at home. Built for modders and level designers who love the classic pipeline.
- **100% open‑source (GPLv3)** – Study, modify, and contribute. The engine belongs to the community.
- **AAA features without the bloat** – Clustered forward rendering, PBR, dynamic shadows, volumetric fog, and a full post‑processing stack—all tuned to be lightweight.
- **Networking built in** – Client‑server multiplayer with replication, RPCs, and Steam integration ready out of the box.
- **First Algerian engine** – Built by a solo developer in Algeria, for the world. Your support means everything.

---

## ✨ Features

### 🎨 Rendering
- **Clustered Forward+** rendering – efficient lighting with hundreds of dynamic lights
- **Deferred rendering** path available for complex scenes
- **Cascaded Shadow Maps (CSM)** – high‑quality directional shadows
- **Screen‑Space Reflections (SSR)**
- **Volumetric Fog & Light Propagation Volumes (LPV)**
- **Post‑Process Stack**: Depth of Field, Motion Blur, Color Grading, TAA, Dynamic Resolution
- **PBR material system** with roughness/metallic workflow
- Backends: **OpenGL**, **DirectX 9**, **Vulkan** (abstracted via `IRenderDevice`)

### ⚙️ Engine Core
- **Entity‑Component System (ECS)** with multi‑threaded job system
- **Custom binary `.map` format** – combines BSP geometry with entity key‑value data
- **PAK file system** for fast asset loading and archiving
- **Memory management** with leak detection
- **Lua & Python scripting** support
- **Thread pool** for parallel task execution

### 🏃 Animation & Physics
- **Skeletal animation** with motion matching, blend trees, and layered animators
- **IK solvers** for foot placement and look‑at
- **Animation compression** (PCA) and montages
- **Jolt Physics** (MIT) – character controller, vehicles, joints, ragdolls
- Optional **Bullet** physics backend

### 🔊 Audio
- **Abstract audio interface** – switch between OpenAL and FMOD at compile time
- 3D positional audio, environmental effects, music streaming

### 🌐 Networking
- **Authoritative server** with client‑side prediction
- **Delta compression** for low bandwidth usage
- **RPCs, replicated variables**, and object replication
- **Steamworks integration** for lobby and matchmaking

### 🛠️ Editor & Tools
- **World Editor** with transform gizmos, terrain sculpting, light placement
- **Material Editor** with node graph and live shader compilation
- **Animation Editor** – state machines, blend spaces, montage editing
- **Forge Asset Manager** – live linking, dependency graphs, PAK builder
- **Project Manager** – build settings, packaging, and wizard
- **In‑editor Play Mode (PIE)** for instant testing

---

## 💻 System Requirements

| Component | Minimum | Recommended |
|-----------|---------|-------------|
| **OS** | Windows 7 SP1 (64‑bit) | Windows 10/11 (64‑bit) |
| **CPU** | Dual‑core with SSE2 (Core 2 Duo) | Quad‑core 2.5 GHz+ |
| **GPU** | DirectX 9.0c / OpenGL 2.0, 256 MB VRAM | OpenGL 3.3 / Vulkan 1.0, 1 GB VRAM |
| **RAM** | 1 GB | 4 GB |
| **Storage** | 500 MB HDD | 2 GB SSD |

> **Linux**, **macOS**, **Android**, and **iOS** support is planned. Console export (PS5, Xbox, Switch) is available via the commercial version.

---

## 🚀 Getting Started

### Option A: Use Visual Studio (quick)
1. Clone the repository:
   ```bash
   git clone https://github.com/nookiaftth1199-ai/Ultimate-Source-Engine-USE-echo-1.2.git
