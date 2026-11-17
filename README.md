# Round Robin CPU Scheduler Using Circular Queue

A CPU scheduling project that demonstrates the **Round Robin Scheduling Algorithm** using a **Circular Queue**. This project includes two independent implementations:

1. **C++ Console Implementation** – Simulates CPU scheduling and displays execution results in the terminal.
2. **Browser-Based Visualizer** – Provides an interactive visual representation of the ready queue, CPU execution, Gantt chart, and scheduling statistics.

---

## Project Overview

Round Robin is a preemptive CPU scheduling algorithm in which each process receives a fixed amount of CPU time called the **Time Quantum**.

If a process is not completed within its allocated time quantum, it is moved to the rear of the ready queue. The CPU then executes the next available process.

A Circular Queue is used to manage the ready queue efficiently.

## Project Implementations

### 1. C++ Console Implementation

The C++ implementation simulates Round Robin scheduling using a manually implemented Circular Queue.

**Features:**
- Custom Circular Queue using a dynamically allocated array.
- Circular indexing using the modulo (`%`) operator.
- Automatic queue resizing when the queue becomes full.
- Support for process arrival times.
- Handling of CPU idle time.
- Configurable time quantum.
- Configurable context-switch cost.
- Gantt chart generation.
- Calculation of Completion Time, Turnaround Time, Waiting Time, and Response Time.
- CPU utilization and throughput calculation.
- Time-quantum comparison.
- Optional Circular Queue trace mode showing queue contents, head, tail, and size.

#### Requirements

- C++ compiler supporting C++17 or later.
- GCC / G++ recommended.

#### Compilation

Open a terminal in the directory containing the C++ source file and execute:

```bash
g++ -std=c++17 -O2 -o rr round_robin_scheduler.cpp
```

#### Execution Modes

| Command | Description |
|---|---|
| `./rr` | Runs the built-in demonstration |
| `./rr -i` | Runs the scheduler with interactive input |
| `./rr -t` | Runs the built-in demonstration with queue tracing |
| `./rr -i -t` | Interactive input with Circular Queue tracing |

On Windows PowerShell, use `.\rr.exe` instead of `./rr`.

#### Interactive Input

The program asks for:
- Number of processes
- Arrival time of each process
- Burst time of each process
- Time quantum
- Context-switch cost

Example input:

```text
Number of processes: 4
P1 arrival burst: 0 10
P2 arrival burst: 1 4
P3 arrival burst: 2 6
P4 arrival burst: 3 3
Time quantum: 4
Context-switch cost: 1
```

---

### 2. Browser-Based Interactive Visualizer

The browser implementation is a standalone HTML application developed using HTML, CSS, and JavaScript.

It provides a visual way to understand how Round Robin scheduling works.

**Features:**
- Add and remove processes.
- Configure arrival times and burst times.
- Set time quantum and context-switch cost.
- Visual representation of the Circular Queue.
- Head and tail position indicators.
- CPU execution visualization.
- Step-by-step simulation controls.
- Previous, Next, Play, and Last controls.
- Gantt chart generation.
- Scheduling metrics and performance statistics.
- Time-quantum comparison table.

#### How to Run

1. Download the HTML file from this repository.
2. Open the HTML file in a web browser.
3. Enter process details.
4. Set the time quantum and context-switch cost.
5. Click **Run** to start the simulation.
6. Use the navigation controls to explore the scheduling process.

No compiler or additional software installation is required for the browser version.

---

## Scheduling Metrics

The project calculates the following metrics:

| Metric | Formula |
|---|---|
| Completion Time (CT) | Time at which a process finishes |
| Turnaround Time (TAT) | CT − Arrival Time |
| Waiting Time (WT) | TAT − Burst Time |
| Response Time (RT) | First CPU Start Time − Arrival Time |
| CPU Utilization | Useful CPU Time / Total Time × 100 |
| Throughput | Number of Completed Processes / Total Time |

## Circular Queue Operations

The ready queue uses the following operations:

- **Enqueue:** Inserts a process at the rear of the queue.
- **Dequeue:** Removes a process from the front of the queue.
- **Wrap-around:** Uses modulo arithmetic to move indices back to the beginning of the array.
- **Dynamic Resizing:** Doubles the queue capacity when it becomes full.

## Algorithm Workflow

1. Initialize the processes and their remaining burst times.
2. Sort processes according to arrival time.
3. Insert arrived processes into the Circular Queue.
4. Remove the process at the front of the queue.
5. Execute it for the minimum of the time quantum and its remaining burst time.
6. Add newly arrived processes to the ready queue.
7. If the process is unfinished, insert it at the rear of the queue.
8. If the process is completed, record its completion time.
9. Repeat until all processes are completed.
10. Generate the Gantt chart and scheduling statistics.

## Technologies Used

- **C++17** – Console-based scheduling implementation.
- **HTML5** – Browser application structure.
- **CSS3** – User interface and visual styling.
- **JavaScript** – Scheduling simulation and interactive controls.
- **Circular Queue** – Ready queue data structure.

## Project Structure

```text
round-robin-cpu-scheduler/
│
├── round_robin_scheduler.cpp
│
├── Round_Robin_Visualizer.html
│
└── README.md
```

*The HTML filename shown above is a suggested repository filename.*

## Learning Objectives

- Understand preemptive CPU scheduling.
- Implement a Circular Queue using an array.
- Understand enqueue and dequeue operations.
- Learn circular indexing and wrap-around.
- Analyze process scheduling metrics.
- Compare the effects of different time quantum values.
- Understand the relationship between data structures and operating system scheduling.

---

## Conclusion

This project demonstrates how the Round Robin CPU Scheduling Algorithm can be implemented using a Circular Queue. The C++ implementation focuses on the scheduling logic and queue operations, while the browser-based visualizer provides an interactive way to understand process execution and queue movement.

Both implementations demonstrate the same scheduling approach through different interfaces.

---

**Project:** Round Robin CPU Scheduler Using Circular Queue  
**Language:** C++17, HTML, CSS, JavaScript  
**Category:** Data Structures and Algorithms / Operating Systems
