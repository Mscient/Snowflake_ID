# Snowflake ID Generator

A thread-safe, high-performance distributed unique ID generator implemented in modern C++20.

The project implements a Snowflake-style 64-bit ID generation system designed for distributed applications where IDs must be unique, sortable by creation time, and generated without relying on a centralized database.

---

## Features

- 64-bit unique ID generation
- Timestamp-based IDs
- Configurable worker/machine ID
- Per-millisecond sequence number
- Thread-safe ID generation
- Sequence overflow handling
- Clock rollback detection
- ID decoding
- CMake-based build system
- Automated tests using CTest
- Reusable static library

---

## ID Structure

Each generated ID is a 64-bit integer divided into three logical components:

```text
  41 bits              10 bits             12 bits
+----------------------+-------------------+----------------+
|      Timestamp       |     Worker ID      |    Sequence    |
+----------------------+-------------------+----------------+
          |                      |                  |
          |                      |                  +-- 0-4095
          |                      +--------------------- 0-1023
          +-------------------------------------------- Milliseconds