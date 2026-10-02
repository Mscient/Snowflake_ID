Below is a **complete `README.md` file** ready to paste directly into your repository.

````markdown
# Snowflake ID Generator

A thread-safe, Snowflake-style distributed ID generator implemented in **C++20**.

The project generates unique 64-bit IDs using a combination of a timestamp, worker ID, and sequence number. The core functionality is packaged as a **reusable C++ library**, with a demo application and automated tests for uniqueness, sequence overflow, and concurrent generation.

---

## 📌 Overview

In distributed systems, multiple machines or services may need to generate unique IDs without relying on a centralized database or auto-increment mechanism.

This project implements a Snowflake-style ID generator that creates IDs by combining three components:

```text
┌─────────────────────────────── 64-bit ID ───────────────────────────────┐
│                                                                         │
│     Timestamp (41 bits)     Worker ID (10 bits)     Sequence (12 bits) │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
````

### ID Layout

| Component | Bits | Purpose                                              |
| --------- | ---: | ---------------------------------------------------- |
| Timestamp |   41 | Milliseconds elapsed since custom epoch              |
| Worker ID |   10 | Identifies the ID generator/worker                   |
| Sequence  |   12 | Differentiates IDs generated in the same millisecond |
| Unused    |    1 | Keeps the ID positive as a signed 64-bit integer     |

Total:

```text
41 + 10 + 12 = 63 bits
```

The implementation stores the ID in a `uint64_t`.

---

# ✨ Features

* 64-bit Snowflake-style ID generation
* 41-bit timestamp
* 10-bit worker ID
* 12-bit sequence number
* Custom epoch
* Thread-safe ID generation
* Sequence handling within the same millisecond
* Sequence overflow handling
* Clock rollback detection
* ID decoding
* C++20 implementation
* CMake build system
* Reusable static library
* Installable public headers
* Automated testing with CTest
* Multiple-ID uniqueness testing
* Sequence overflow testing
* Concurrent generation testing
* GitHub Actions CI

---

# 🧩 How the ID Works

A generated ID is constructed using bit shifting:

```cpp
(timestamp << 22)
    | (worker_id << 12)
    | sequence
```

The project defines the following bit positions:

```text
Sequence Shift  = 0
Worker ID Shift = 12
Timestamp Shift = 22
```

Therefore:

```text
63                     22 21          12 11           0
┌────────────────────────┬──────────────┬──────────────┐
│       Timestamp        │   Worker ID  │   Sequence   │
│         41 bits        │    10 bits   │    12 bits   │
└────────────────────────┴──────────────┴──────────────┘
```

---

# ⏱️ Timestamp

The timestamp field uses **41 bits**.

Instead of storing the complete Unix timestamp, the implementation stores:

```text
Current Time - Custom Epoch
```

The project's custom epoch is:

```text
2026-01-01 00:00:00 UTC
```

This reduces the number of bits required to represent the timestamp.

The timestamp is measured in milliseconds.

---

# 🖥️ Worker ID

The worker ID uses **10 bits**.

Therefore:

```text
2^10 = 1024
```

The supported worker ID range is:

```text
0 - 1023
```

A worker ID identifies a particular generator instance or worker.

For example:

```cpp
Snowflake generator(1);
```

creates a generator with:

```text
Worker ID = 1
```

---

# 🔢 Sequence Number

The sequence field uses **12 bits**.

Therefore:

```text
2^12 = 4096
```

Possible sequence values are:

```text
0 - 4095
```

The sequence number is used when multiple IDs are generated during the same millisecond.

For example:

```text
Timestamp     Sequence

1000             0
1000             1
1000             2
1000             3
...
1000          4095
```

When the timestamp moves to the next millisecond, the sequence resets to `0`.

---

# ⚠️ Sequence Overflow

The maximum sequence value is:

```text
4095
```

If another ID is requested while the generator is still in the same millisecond, the generator waits until the timestamp advances.

The sequence is then reset:

```text
4095
  │
  │ next ID requested
  ▼
wait for next millisecond
  │
  ▼
sequence = 0
```

This prevents two IDs from having the same timestamp, worker ID, and sequence combination.

The behavior is verified by the project's overflow test.

---

# 🔒 Thread Safety

The generator supports concurrent calls to `generate()`.

The internal state is protected using:

```cpp
std::mutex
```

and:

```cpp
std::lock_guard<std::mutex>
```

The protected state includes:

```text
sequence_
last_timestamp_
```

This ensures that multiple threads cannot modify the generator state simultaneously.

---

# 🕐 Clock Rollback Detection

The generator checks whether the system clock moves backwards.

If:

```text
current_timestamp < last_timestamp
```

the generator throws:

```cpp
std::runtime_error(
    "Clock moved backwards"
);
```

This prevents IDs from being generated using an invalid timestamp.

---

# 🔍 ID Decoding

A generated ID can be decoded back into its original logical components:

```text
Generated ID
     │
     ▼
┌──────────────┐
│    Decoder   │
└──────┬───────┘
       │
       ├──────► Timestamp
       │
       ├──────► Worker ID
       │
       └──────► Sequence
```

Example:

```cpp
uint64_t id = generator.generate();

auto data = generator.decode(id);

std::cout << data.timestamp;
std::cout << data.worker_id;
std::cout << data.sequence;
```

The timestamp can then be combined with the custom epoch to obtain the actual generation time.

---

# 🏗️ Project Architecture

The project is divided into three main components:

```text
                    ┌──────────────────────┐
                    │  Snowflake Library   │
                    └──────────┬───────────┘
                               │
              ┌────────────────┼────────────────┐
              │                │                │
              ▼                ▼                ▼
        Snowflake         Clock            Configuration
        Generator
              │
              ▼
        64-bit ID


                    ┌──────────────────────┐
                    │    Demo Application  │
                    └──────────────────────┘


                    ┌──────────────────────┐
                    │      Test Suite      │
                    └──────────┬───────────┘
                               │
             ┌─────────────────┼─────────────────┐
             ▼                 ▼                 ▼
       Multiple IDs        Overflow        Concurrency
```

---

# 📁 Project Structure

```text
Snowflake_ID/
│
├── include/
│   └── snowflake/
│       ├── snowflake.hpp
│       ├── clock.hpp
│       └── snow_flake_config.hpp
│
├── src/
│   ├── snow_flake.cpp
│   ├── clock.cpp
│   └── main.cpp
│
├── tests/
│   ├── multiple_ids.cpp
│   ├── overflow.cpp
│   └── concurrency.cpp
│
├── CMakeLists.txt
├── README.md
└── .gitignore
```

---

# 🛠️ Technologies

| Technology      | Purpose                |
| --------------- | ---------------------- |
| C++20           | Core implementation    |
| `std::chrono`   | Timestamp generation   |
| `std::mutex`    | Thread synchronization |
| CMake           | Build system           |
| CTest           | Automated testing      |
| GCC / MinGW-W64 | Compiler               |
| Git             | Version control        |
| GitHub Actions  | Continuous Integration |

---

# 📋 Requirements

## C++ Compiler

A C++20-compatible compiler is required.

The project was developed and tested using:

```text
GCC / MinGW-W64
C++20
```

Check your compiler:

```powershell
g++ --version
```

---

## CMake

CMake 3.20 or newer is recommended.

Check your installation:

```powershell
cmake --version
```

---

# 🚀 Getting Started

## 1. Clone the Repository

```powershell
git clone https://github.com/Mscient/Snowflake_ID.git
```

Move into the project:

```powershell
cd Snowflake_ID
```

---

# 🔨 Building the Project

Create the build directory and configure CMake:

```powershell
cmake -S . -B build -G "MinGW Makefiles"
```

Build the project:

```powershell
cmake --build build
```

The build generates:

```text
Snowflake Library
Demo executable
Test executables
```

---

# ▶️ Running the Demo

The demo application generates one Snowflake ID and decodes it.

Run:

```powershell
.\build\snowflake_demo.exe
```

Example output:

```text
Snowflake ID Generator
======================

Generated ID: 97782757396058112

Decoded ID
Worker ID: 1
Sequence: 0
Timestamp: 123456789
Generated at: ...
```

The generated ID and timestamp will naturally change every time the program runs.

---

# 🧪 Running Tests

The project uses CTest for automated testing.

Run the complete test suite:

```powershell
ctest --test-dir build --output-on-failure
```

A successful run should report:

```text
100% tests passed, 0 tests failed out of 3
```

---

# 🧪 Test 1 — Multiple ID Generation

The multiple-ID test generates thousands of IDs and checks whether they are unique.

Run:

```powershell
.\build\test_multiple_ids.exe
```

The test reports:

```text
Total IDs
Unique IDs
Duplicates
Generation time
```

Example:

```text
Total IDs: 10000
Unique IDs: 10000
Duplicates: 0
[PASS] Multiple ID generation
```

---

# 🧪 Test 2 — Sequence Overflow

The overflow test verifies the behavior when the sequence reaches its maximum value.

Run:

```powershell
.\build\test_overflow.exe
```

Example:

```text
Before overflow sequence: 4095
After generation sequence: 0
[PASS] Sequence overflow handling
```

This verifies that the generator waits for the timestamp to advance and resets the sequence.

---

# 🧪 Test 3 — Concurrent Generation

The concurrency test generates IDs from multiple threads.

Current test configuration:

```text
8 threads
10,000 IDs per thread
---------------------
80,000 total IDs
```

Run:

```powershell
.\build\test_concurrency.exe
```

Example:

```text
Total IDs: 80000
Unique IDs: 80000
[PASS] Concurrent generation
```

The test verifies that concurrent generation does not result in duplicate IDs.

---

# 📦 Library Installation

The Snowflake generator is implemented as a reusable CMake library.

After building the project:

```powershell
cmake --build build
```

install it using:

```powershell
cmake --install build --prefix install
```

The installation directory contains the public headers and compiled library.

Conceptually:

```text
install/
│
├── include/
│   └── snowflake/
│       ├── snowflake.hpp
│       ├── clock.hpp
│       └── snow_flake_config.hpp
│
└── lib/
    └── ...
```

This allows the Snowflake generator to be reused by another C++ application instead of copying the implementation files.

---

# 🔌 Using the Library

Include the public header:

```cpp
#include <snowflake/snowflake.hpp>
```

Create a generator:

```cpp
Snowflake generator(1);
```

Generate an ID:

```cpp
uint64_t id = generator.generate();
```

Decode the ID:

```cpp
auto data = generator.decode(id);
```

Access the decoded information:

```cpp
data.timestamp;
data.worker_id;
data.sequence;
```

---

# 💻 Library Usage Example

```cpp
#include <iostream>
#include <snowflake/snowflake.hpp>

int main()
{
    Snowflake generator(1);

    uint64_t id = generator.generate();

    std::cout << "Generated ID: "
              << id << '\n';

    auto data = generator.decode(id);

    std::cout << "Timestamp: "
              << data.timestamp << '\n';

    std::cout << "Worker ID: "
              << data.worker_id << '\n';

    std::cout << "Sequence: "
              << data.sequence << '\n';

    return 0;
}
```

---

# 🔧 CMake Integration

The core implementation is defined as a CMake library target:

```cmake
add_library(snowflake
    src/snow_flake.cpp
    src/clock.cpp
)
```

The public headers are exposed through:

```cmake
target_include_directories(snowflake
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)
```

Another CMake application can link against the library:

```cmake
target_link_libraries(my_application
    PRIVATE snowflake
)
```

---

# 📊 Performance and Concurrency

The project includes a concurrency benchmark using:

```text
8 threads
×
10,000 IDs
=
80,000 IDs
```

The test verifies:

```text
Total IDs generated
Unique IDs generated
```

Performance depends on the hardware, operating system, compiler, and runtime environment.

Therefore, benchmark results from a local machine should be treated as machine-specific rather than universal performance guarantees.

---

# 🧠 C++ Concepts Demonstrated

This project focuses on practical C++ concepts including:

### Bit Manipulation

Packing multiple values into a single 64-bit integer using:

* Bit shifting
* Bitwise OR
* Bitwise AND
* Bit masking

### Concurrency

Using:

```cpp
std::mutex
std::lock_guard
std::thread
```

### Time Handling

Using:

```cpp
std::chrono
```

for millisecond-precision timestamps.

### Exception Handling

Handling:

* Invalid worker IDs
* Clock rollback

### Object-Oriented Design

Separating:

```text
Snowflake
SnowflakeClock
SnowflakeConfig
```

into dedicated components.

### Build Systems

Using CMake to manage:

* Library compilation
* Demo compilation
* Test compilation
* Installation

### Automated Testing

Using CTest to execute the project's test suite.

### Library Development

Packaging the core Snowflake implementation as a reusable C++ library.

---

# ⚙️ Configuration

The main configuration is located in:

```text
include/snowflake/snow_flake_config.hpp
```

Current configuration:

```text
Timestamp bits : 41
Worker ID bits : 10
Sequence bits  : 12
```

Maximum values:

```text
Worker ID : 1023
Sequence  : 4095
```

Bit shifts:

```text
Sequence shift  : 0
Worker ID shift : 12
Timestamp shift : 22
```

Custom epoch:

```text
2026-01-01 00:00:00 UTC
```

---

# 🔄 Complete Generation Flow

The generation process can be summarized as:

```text
              generate()
                  │
                  ▼
          Get current time
                  │
                  ▼
        Compare with previous
             timestamp
                  │
        ┌─────────┼─────────┐
        │         │         │
        ▼         ▼         ▼
      New       Same      Earlier
      time      time        time
        │         │         │
        ▼         ▼         ▼
    Sequence   Increment   Throw
       = 0     sequence    exception
                  │
                  ▼
          Sequence overflow?
                  │
             ┌────┴────┐
             │         │
            No        Yes
             │         │
             │         ▼
             │    Wait for next
             │     millisecond
             │         │
             └────┬────┘
                  ▼
          Construct 64-bit ID
                  │
                  ▼
             Return ID
```

---

# 🔐 Uniqueness Model

For a single generator, an ID is uniquely determined by:

```text
Timestamp + Worker ID + Sequence
```

Therefore, two IDs generated by the same worker are different as long as:

* The timestamp does not move backwards.
* The sequence does not overflow without waiting for the next millisecond.

Across multiple workers, the worker ID provides an additional distinguishing component.

---

# 📈 Capacity

The current configuration provides:

```text
1024 possible worker IDs
```

and:

```text
4096 sequence values per millisecond per worker
```

Therefore, the theoretical sequence capacity per worker is:

```text
4096 IDs / millisecond
```

or:

```text
4,096,000 IDs / second
```

under the configured sequence space, assuming the generator can sustain that rate and the timestamp advances as expected.

This is a theoretical capacity derived from the bit allocation, not a measured performance claim.

---

# 🧪 Continuous Integration

The project includes GitHub Actions CI.

The CI workflow:

```text
Checkout repository
        │
        ▼
Configure CMake
        │
        ▼
Build project
        │
        ▼
Run CTest
```

This allows every pushed change or pull request to be automatically built and tested.

---

# 📌 Design Decisions

### Why `uint64_t`?

The Snowflake ID uses a 64-bit representation.

Using:

```cpp
uint64_t
```

provides an explicit unsigned 64-bit integer type.

---

### Why a custom epoch?

The timestamp does not need to store the complete Unix timestamp.

Instead:

```text
timestamp = current_time - epoch
```

This reduces the numerical value stored in the timestamp field and allows the available 41 bits to represent a long period of time.

---

### Why a worker ID?

Multiple independent generators need a way to distinguish their IDs.

The worker ID provides that distinction:

```text
Worker 1 → ...
Worker 2 → ...
Worker 3 → ...
```

---

### Why a sequence number?

Multiple IDs can be generated during the same millisecond.

The sequence number differentiates those IDs:

```text
Timestamp = T

T + sequence 0
T + sequence 1
T + sequence 2
...
```

---

# 🚧 Future Improvements

Possible future improvements include:

* Configurable epoch
* Configurable bit allocation
* Worker ID coordination
* Stronger clock rollback strategies
* Additional stress testing
* Dedicated benchmark executable
* Cross-platform CI testing
* Package manager support
* API documentation generation
* Distributed worker ID management

---

# 🎯 Learning Objectives

This project was developed to understand how distributed ID generation works while applying practical C++ concepts.

The main learning areas include:

* Distributed ID generation
* Bit manipulation
* Binary data representation
* Time-based systems
* Thread synchronization
* Concurrency
* Exception handling
* CMake
* Automated testing
* Library development
* Software architecture

---

# 📚 Repository

GitHub:

[https://github.com/Mscient/Snowflake_ID](https://github.com/Mscient/Snowflake_ID)

---

# 👨‍💻 Author

Developed as a C++ systems-oriented project to explore distributed ID generation, concurrency, library design, CMake, and automated testing.

````

### After replacing `README.md`

Run:

```powershell
git add README.md
git commit -m "docs: add complete project documentation"
git push origin main
````

Then the GitHub repository will have a README that explains the project **from installation → architecture → ID generation → library usage → testing → CI**.
