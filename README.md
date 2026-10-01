# Snowflake ID Generator

A thread-safe distributed unique ID generator implemented in modern C++20.

This project implements a Snowflake-style 64-bit ID generation algorithm that combines a timestamp, worker ID, and sequence number to generate unique IDs without requiring a centralized database or coordination service.

---

## Features

- 64-bit Snowflake-style ID generation
- 41-bit timestamp
- 10-bit worker ID
- 12-bit sequence number
- Thread-safe ID generation
- Sequence overflow handling
- Clock rollback detection
- ID decoding
- Custom epoch
- C++20 implementation
- CMake build system
- Automated testing with CTest
- Reusable static library

---

## ID Structure

Each generated ID uses 63 bits of a 64-bit integer.

```text
  41 bits              10 bits              12 bits
+----------------------+--------------------+----------------+
|      Timestamp       |     Worker ID       |    Sequence    |
+----------------------+--------------------+----------------+
          |                     |                    |
          |                     |                    +-- 0 to 4095
          |                     +----------------------- 0 to 1023
          +--------------------------------------------- Milliseconds
