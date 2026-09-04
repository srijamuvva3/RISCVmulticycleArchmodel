# RISCVMulticyclearchmodel

Refer https://github.com/srijamuvva3/RV32Iarchmodel/blob/main/README.md for more details

# Singlecycle vs Multicycle vs Pipelined

![alt text](image-4.png)


| Feature | Single-Cycle | Multi-Cycle | Pipelined |
| :--- | :--- | :--- | :--- |
| **Cycles Per Instruction (CPI)** | Exactly 1 | Greater than 1 (varies by instruction) | Ideal CPI = 1 (in steady state) |
| **Clock Cycle Time** | Longest (limited by the slowest full instruction) | Shortest (limited by the slowest individual stage) | Shortest (limited by the slowest individual stage + register overhead) |
| **Hardware Reuse** | None (Requires duplicate ALUs and memories) | High (Reuses ALU and memory across cycles) | Low (Each stage needs its own dedicated hardware) |
| **Control Logic** | Simple (Pure combinational logic) | Complex (Requires a Finite State Machine) | Complex (Requires hazard detection and forwarding logic) |
| **Throughput** | Low (One instruction per long clock cycle) | Low to Medium (Takes multiple short cycles per instruction) | High (Finishes an instruction every short clock cycle) |
