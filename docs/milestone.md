# Task Scheduler Simulator's Milestones

The Task Scheduler Simulator will be developed sequentially, with parallel task development where possible.

## Target: Task Scheduler

- **CPU Scheduler** must fully run correctly (including context switching time).
- **Job Scheduler** only needs to handle queue delay.
- **Device Queue** must include IO time.

---

## Milestone 1: Minimal CPU Scheduler

### Requirements

- **Timestamp In**: Equal to Request Time.
- **IO Burst**: Not required.
- **Number of CPU Bursts**: 1.
- **Context Switch Time**: Not included.
- **Process Real Burst Time**: Equal to Process CPU Burst Time.

### Milestone 1.1: Single Algorithm ✅

- **Algorithm**: FCFS (First-Come, First-Served).
- **Queue**: Ready.
- **Arrival Time**: Equal to Timestamp In.

### Milestone 1.2: Full Algorithm Support ⏳

- **Algorithms**: FCFS, RR (Round-Robin), SJF (Shortest Job First), SRTF (Shortest Remaining Time First), Priority Scheduling.
- **Queue**: Ready.
- **Arrival Time**: Equal to Timestamp In.

---

## Milestone 2: Minimal Device Queue & Minimal Job Scheduler

**Task 1** and **Task 2** can be implemented in parallel.

### 1. Task 1: Minimal Device Queue

- **Queue**: Device.
- **Input**: List of elements containing:
  - Timestamp In
  - Process In
  - IO Burst Time
- **Output**: List of elements containing:
  - Timestamp Out
  - Process Out

### 2. Task 2: Minimal Job Scheduler

- **Queue**: Job.
- **Input**: List of elements containing:
  - Timestamp In
  - Process In
- **Output**: List of elements containing:
  - Timestamp Out
  - Process Out

---

## Milestone 3: Integration of CPU Scheduler + Job Scheduler + Device Queue

### Milestone 3.1: Cross Integration

#### 1. Task 1: CPU Scheduler + Job Scheduler

- **IO Burst**: Not important.
- **Number of CPU Bursts**: Not important.
- **Context Switch Time**: Not included.
- **Process Real Burst Time**: Not important.
- **Algorithm**: Not important.
- **Queue**: Ready, Job.
- **Output of Job Scheduler = Input of CPU Scheduler**:
  - Request Time = Job Scheduler's Timestamp In.
  - Arrival Time = Job Scheduler's Timestamp Out = CPU Scheduler's Timestamp In.

#### 2. Task 2: CPU Scheduler + Device Queue

- **IO Burst**: Equal to (Number of CPU Bursts - 1).
- **Number of CPU Bursts**: Any.
- **Context Switch Time**: Not included.
- **Process Real Burst Time**: Equal to Process CPU Burst Time.
- **Algorithm**: FCFS, RR, SJF, SRTF, Priority Scheduling.
- **Queue**: Ready, Device.
- **Output of Device Queue = Input of CPU Scheduler**:
  - CPU Scheduler's Process In = Device Queue's Process Out.
  - CPU Scheduler's Timestamp In = Device Queue's Timestamp Out.

### Milestone 3.2: Full Integration: CPU Scheduler + Job Scheduler + Device Queue

- **IO Burst**: Equal to (Number of CPU Bursts - 1).
- **Number of CPU Bursts**: Any.
- **Context Switch Time**: Not included.
- **Process Real Burst Time**: Equal to Process CPU Burst Time.
- **Algorithm**: FCFS, RR, SJF, SRTF, Priority Scheduling.
- **Queue**: Ready, Job, Device.

---

## Milestone 4: Enhance the Simulator

### 1. Task 1: Add Delay

- **Context Switching Time**: Time taken to switch from one process to another.
- **Implementation**:
  - Assume process Pi is running and process Pj is ready to replace Pi whenever Pi ends.
  - **Pj's Timestamp In** = **Pi's Timestamp Out** + **Context Switching Time**.
  - This means that after Pi ends, the CPU must consume the Context Switching Time before it is ready to receive Pj.

### 2. Task 2: Predict CPU Burst Time for Algorithm

- **Current Process Real Burst Time**: Equal to `predict_function(Last Process CPU Burst Time)`.
- If a process is running for the first time, **Current Process Real Burst Time** = default value.
