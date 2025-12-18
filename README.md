# Escape Game: Multi-Level Algorithms Project

**Escape Game** is a first-person puzzle-adventure built in **Unreal Engine 5** using C++. The project demonstrates the practical application of advanced data structures and algorithms (A*, Hash Sets, Queues, Stacks) to solve real-time game development challenges.

## Game Overview

The player must navigate through three distinct levels, each introducing new mechanics and increasing algorithmic complexity:

1.  **Level 1: The Tutorial (Foundations)**
    * **Goal:** Escape a locked room using physics puzzles.
    * **Tech:** Finite State Machines (FSM) for doors and Linear Interpolation (Lerp) for object movement.
2.  **Level 2: The Facility (Search & Optimization)**
    * **Goal:** Collect "Data Shards" while evading an AI stalker.
    * **Tech:** **A* Pathfinding** with Priority Queues and **Hash Sets** for O(1) lookup. Resource collection is optimized via **Pointer Caching**.
3.  **Level 3: The Gauntlet (Advanced Structures)**
    * **Goal:** Survive a procedural bridge and solve a sorting puzzle.
    * **Tech:** **FIFO Queue** for infinite bridge generation and **LIFO Stack** for a "Time Rewind" checkpoint mechanic.

---

## Technical Features

### Algorithms & Data Structures
| Feature | Data Structure / Algorithm | Complexity | Use Case |
| :--- | :--- | :--- | :--- |
| **Enemy Navigation** | Priority Queue (Min-Heap behavior) | $O(E \log V)$ | A* Open List management. |
| **Path Optimization** | Hash Set (`TSet`) | O(1) | A* Closed List for instant visited checks. |
| **Time Rewind** | Stack (`TArray` as LIFO) | $O(1)$ | Reverting player position to safe spots. |
| **Procedural Bridge** | Queue (`TQueue` FIFO) | $O(1)$ | Managing infinite platform spawning/destruction. |
| **Puzzle Validator** | Array & Selection Sort | $O(N^2)$ | verifying physical object arrangement. |
| **Interaction** | Bounding Volume Hierarchy (BVH) | $O(\log N)$ | Sphere sweep for forgiving aim assist. |

### Optimization Highlights
* **Pointer Caching:** Reduced resource collection complexity from $O(N)$ to $O(1)$ by caching references at `BeginPlay`.
* **Throttled AI:** Pathfinding recalculates only 2 times/second to save CPU cycles.
* **Memory Management:** The Bridge Queue ensures a constant memory footprint $O(M)$ by destroying old actors as new ones spawn.

---

## Installation & Setup

### Prerequisites
* **Unreal Engine 5.1+**
* **Visual Studio 2022** (with "Game Development with C++" workload)

### Build Instructions
1.  **Clone the Repository:**
    ```bash
    git clone https://github.com/BlazedLith/EscapeGame.git
    ```
2.  **Generate Project Files:**
    * Right-click `EscapeGame.uproject` and select **"Generate Visual Studio project files"**.
3.  **Compile:**
    * Open `EscapeGame.sln` in Visual Studio.
    * Set configuration to **"Development Editor"** and build the solution (`Ctrl+Shift+B`).
4.  **Launch:**
    * Open `EscapeGame.uproject` to launch the Unreal Editor.
    * Press **Play** to start from the Main Menu.

### Alternative Setup Method
Refer to the setup folder. Download the setup file and install the game then simple open the .exe file. (Needs DirectX 12)

---

## Controls

| Action | Input |
| :--- | :--- |
| **Move** | `W`, `A`, `S`, `D` |
| **Look** | Mouse |
| **Jump** | `Spacebar` |
| **Interact / Pick Up** | `E` |
| **Use** | `F` |
| **Exit** | `P` |

---

## Team Contributions

* **Wasiq Amir:** Level 1 Design, Finite State Machines (Door/Safe Logic).
* **Zayna Qasim:** Level 2 Design, Enemy AI (A* Pathfinding), Resource Optimization.
* **Maha Mohsin:** Level 3 Design, Procedural Bridge (Queues), Time Rewind (Stacks).
* **Maryam Asif:** Technical Documentation, Big-O Analysis.
* **Luqman Shehzad:** Architecture Diagrams, Report Compilation, Quality Assurance.

---

## 📄 License
This project was developed for the **CS-250 Data Structures and Algorithms** course at **NUST SEECS**.

---

> **Note:** For a detailed breakdown of the code and algorithms, please refer to the `Technical_Report.pdf` included in the Setup directory.
