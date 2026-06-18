# System Architecture & Hardware

## System Overview

The system is an embedded Bluetooth Low Energy (BLE) speaker built around the STM32F407G microcontroller. It receives audio data from a BLE module via I2S, buffers it in RAM, and transmits it to a CS43L22 (Cirrus Logic) Digital-to-Analog Converter (DAC). The analog signal from the DAC is then driven through a 5V amplifier to output to the speakers.

Currently, audio data is fed to the DAC via a polling loop. The target architecture will implement Direct Memory Access (DMA) to automate these transfers, ensuring non-blocking operations for the CPU, enabling user input handling, and leaving overhead for potential Digital Signal Processing (DSP).

## Hardware Mapping

**Target MCU:** STM32F407G Discovery Board
**Audio DAC:** Cirrus Logic CS43L22
**Amplifier:** 5V Audio Amplifier

### Pinout & Peripheral Configuration

**I2C1 (DAC Control & Initialization)**

* **SCL:** PB6
* **SDA:** PB9
* **Settings:** Alternate Function 4 (AF4), Custom interrupt-driven implementation.

**I2S3 / SPI3 (Audio Data Transmission)**

* **WS (Word Select):** PA4
* **MCK (Master Clock):** PC7
* **SCK (Serial Clock):** PC10
* **SD (Serial Data):** PC12
* **Pin Settings:** Alternate Function 6 (AF6), Push-Pull, No Pull-Up/Pull-Down, High Speed.

## Power & Clocking Structure

* **System Clock:** 16MHz running on the default High-Speed Internal (HSI) oscillator. Clock configurations are abstracted within their respective peripheral initialization sequences rather than a central clock configuration function.
* **Audio Master Clock (MCLK):** 12.288MHz, generated via PLLI2S (Dividers and multipliers configured per RM0090 Section 28.4.4, Table 128 - Master clock enabled).
* **Audio Specs:** 16-bit Stereo, 48kHz Sample Rate, 256x Multiplier.
* **Power Delivery:** The system utilizes isolated power sources to minimize audio noise interference. The STM32 operates off a 5V battery supply, while the audio amplifier is powered by a separate 5V brick.

## DAC Configuration Settings (CS43L22)

Initialization is handled over I2C1. The following registers are configured during startup:

| Register | Address | Hex Value | Binary | Key Settings |
| --- | --- | --- | --- | --- |
| **Power Control 2** | `0x04` | `0xAF` | `10101111` | Headphone A/B ON, Speaker A/B OFF |
| **Clocking Control** | `0x05` | `0xA0` | `10100000` | Auto-detect ON, Speed: 4-50kHz, MCLK ratio: 00, MCLK_div_2: 0 |
| **Interface Control 1** | `0x06` | `0x07` | `00000111` | Slave mode, SCLK not inverted, DSP OFF, I2S Standard, 16-bit audio |

## Initialization Sequence

**Startup Flow:**

1. Set interrupt priority grouping.
2. Set specific interrupt priorities.
3. Enable interrupts globally.
4. Initialize and enable ITM (Print Service).
5. Initialize LEDs.
6. Initialize Buttons.
7. Initialize I2C1.
8. Initialize DAC (which triggers I2S3 initialization).
9. Enter main application loop (polling audio transmission).

---

## Debugging Tools & Workflow

This system utilizes a highly optimized, bare-metal debugging suite designed to provide precise error locations and causes without bloating the binary size or causing memory fragmentation.

### 1. Advanced Fault Handling

Hardware faults (Usage, Bus, and Memory Management faults) are explicitly enabled and routed to custom Interrupt Service Routines (ISRs) rather than a generic hard fault handler.

**How it works:**
When a fault occurs, the system automatically:

1. **Locates the Error:** Prints the exact fault type (e.g., `[USAGE FAULT DETECTED]`) and the processor stack (`print_stack()`) so the developer can trace where the fault originated.
2. **Decodes the Cause:** Reads the Core Fault Status Register (`SCB_CFSR`) and isolates the specific bit flags to print a human-readable reason for the crash.
3. **Isolates the Address:** For Bus and Memory faults, it checks for a valid fault address flag and prints the exact hex address (`BFAR` or `MMFAR`) where the violation occurred.
4. **Halts Safely:** Clears the register and enters a safe, infinite `nop` loop to prevent further unpredictable execution.

**Caught & Decoded Errors:**

**Usage Faults**

* **Division by Zero:** Attempted math operation dividing by zero.
* **Unaligned Access:** The processor made an unaligned memory access.
* **No Coprocessor:** Attempted to access a coprocessor that is not supported.
* **Invalid PC Load:** Caused by an invalid Program Counter load by `EXC_RETURN`.
* **Invalid State:** Attempted to execute an instruction that makes illegal use of the EPSR.
* **Undefined Instruction:** The processor attempted to execute an unmapped/undefined instruction.

**Bus Faults**

* **Instruction Bus Error:** The processor detected a bus error while prefetching an instruction.
* **Precise Error:** The stacked PC value points directly to the instruction that caused the fault.
* **Imprecise Error:** The return address in the stack frame is not related to the instruction that caused the error (usually due to buffered writes).
* **Unstacking Error:** An exception return caused one or more bus faults.
* **Stacking Error:** An exception entry caused one or more bus faults.
* **Floating-Point Error:** Bus fault during floating-point lazy state preservation.

**Memory Management Faults**

* **Instruction Access Violation:** Attempted an instruction fetch from a location that does not permit execution.
* **Data Access Violation:** Attempted a load or store at a location that does not permit the operation.
* **Unstacking Error:** An exception return caused one or more access violations.
* **Stacking Error:** An exception entry caused one or more access violations.
* **Floating-Point Error:** MemManage fault occurred during floating-point lazy state preservation.

**Hard Faults (Escalated Errors)**

Because Usage, Bus, and Memory Management faults are explicitly enabled and handled by their own ISRs, a Hard Fault in this system is highly specific. It indicates that an error bypassed the standard fault handlers or escalated due to priority conflicts.

**How it works:**
The Hard Fault handler prints the stack trace and reads the HardFault Status Register (`SCB_HFSR`) to determine why the fault escalated.

**Caught & Decoded Errors:**

* **Forced Hard Fault (`FORCED`):** A configurable fault (Usage, Bus, or MemManage) was escalated to a Hard Fault. Because the specific fault handlers are enabled, this almost exclusively means a fault occurred *inside* an active ISR, or worse, inside one of the fault handlers themselves.
* **Vector Table Read Fault (`VECTTBL`):** A Bus Fault occurred while the processor was trying to read the vector table during exception processing (often caused by a corrupted vector table pointer or bad memory map).
* **Debug Event (`DEBUGEVT`):** A debug event occurred (like a breakpoint instruction) but the debug subsystem was not fully enabled or attached.


### 2. ITM Print Service

Standard `stdio.h` implementations of `printf` are notorious for causing heap fragmentation and bloating binary size. This system completely bypasses standard I/O in favor of a custom, stack-based formatting pipeline sent over the Instrumentation Trace Macrocell (ITM).

**The Formatting Engine:**
String formatting is handled by a lightweight, custom `printf` implementation (adapted from Marco Paland's library). It formats strings entirely on the stack, guaranteeing zero dynamic memory allocation and preventing fragmentation.

**The Transport Layer (ITM):**
The formatted string is pushed directly to the debugger via the Cortex-M ITM. This requires zero overhead from the UART peripheral and executes significantly faster.

**Environment Setup (VS Code / Cortex-Debug):**
To view the debug output, your launch configuration must be aware of the system's 16MHz CPU clock and open the ITM port. Below is the required `launch.json` configuration using OpenOCD and an ST-Link:

```json
{
    "cwd": "${workspaceFolder}",
    "executable": "./build/Debug/BLE_speaker_bare_metal.elf",
    "name": "Debug with ST-Link",
    "request": "launch",
    "type": "cortex-debug",
    "runToEntryPoint": "main",
    "showDevDebugOutput": "none",
    "servertype": "openocd",
    "serverpath": "C:/openocd/bin/openocd.exe",
    "configFiles": [
        "C:/openocd/openocd/scripts/interface/stlink.cfg",
        "C:/openocd/openocd/scripts/target/stm32f4x.cfg"
    ],
    "swoConfig": {
        "enabled": true,
        "cpuFrequency": 16000000,
        "swoFrequency": 2000000,
        "source": "probe",
        "decoders": [
            { "type": "console", "label": "ITM", "port": 0 }
        ]
    }
}

```

### 3. Bare-Metal Assertions

To enforce runtime constraints without the massive overhead of the standard library, the system uses a custom macro: `BARE_ASSERT()`.

* **Location:** `assert.h` (Custom header, do not include standard `<assert.h>`)
* **Usage:** `BARE_ASSERT(self->rx_queue != NULL);`
* **Behavior:** If the condition evaluates to false, the macro halts execution and prints the exact file and line number of the failure over the ITM service.
* **Why it matters:** Stripping the `stdlib` assertion dependency reduced the compiled binary size by a factor of 3, keeping the firmware footprint lean for this constrained environment.

---
### 4. Custom I2C Driver Architecture

This system utilizes a custom, interrupt-driven I2C driver. It is built around a state machine and uses queues to manage data payloads, preventing the CPU from manually feeding bytes into the data register during active transfers.

#### Architecture & Current Limitations (Pseudo-Async)

The driver uses hardware interrupts to handle byte-by-byte transmission, which sets up an asynchronous architecture. However, in its current state, it operates synchronously from the user's perspective.

**The Limitation:** Functions like `dac_write()` currently rely on a busy-wait loop to block execution until the transfer completes:

```c
// Current blocking implementation
__STATIC_INLINE void dac_write(uint8_t reg_addr, uint8_t val){
    queue_enqueue(i2c1_tx_queue_ptr, reg_addr);
    queue_enqueue(i2c1_tx_queue_ptr, val);
    I2C_write(i2c1_handle);
    while(i2c1_handle->state != I2C_STATE_DONE){} // Blocks here
    while(I2C_get_SR2(i2c1_device->driver) & 0x02){} // Waits for bus to be free
}

```

**Planned Improvement:** The architecture supports a `transfer_complete_cb` callback. The planned update will move the bus busy flag check into the state machine. Once the bus is confirmed free, the interrupt will trigger the callback, fully removing the blocking `while` loops from the main code and achieving true asynchronous operation.

#### API & Usage Guide

**1. Initialization**
To set up an I2C peripheral, you must initialize its hardware GPIOs, define its RX/TX queues, configure the device, and finally link everything into an `I2C_handle_t`.

```c
// 1. Initialize Queues
i2c1_rx_queue_ptr = queue_init(&i2c1_rx_queue, i2c_rx_buffer, QUEUE_CAP_1);
i2c1_tx_queue_ptr = queue_init(&i2c1_tx_queue, i2c_tx_buffer, QUEUE_CAP_2);

// 2. Initialize Hardware Peripheral
i2c1_device = I2C_init(I2C_1, I2C_MODE_STANDARD, /* GPIO ports/pins & clock */);

// 3. Initialize the Driver Handle
i2c1_handle = I2C_handle_init(
    i2c1_device,
    I2C_1,
    i2c1_rx_queue_ptr, 
    i2c1_tx_queue_ptr, 
    DAC_I2C_addr, // Target slave address
    NULL          // Optional completion callback
);

```

**2. Performing Transactions**

* **Write (`I2C_write`):** Data must be pushed to the TX queue prior to calling the write function. The state machine will automatically dequeue and transmit until the queue is empty.
* **Read (`I2C_read`):** Requires a target transfer size. The driver will pull the specified number of bytes from the bus and load them into the RX queue.

```c
void I2C_write(I2C_handle_t* self);
void I2C_read(I2C_handle_t* self, uint16_t transfer_size);

```

#### The Event Interrupt (EV) State Machine

The core of the driver lives in the Event Interrupt handler, which reads the `SR1` and `SR2` status registers to advance the state machine.

**Start & Address Phase:**

* **START Bit (SB):** When triggered, the driver checks the requested direction (Read/Write) and sends the slave address accordingly, shifting the state to `I2C_STATE_SLAVE_ADDR_SENT`.
* **Address Matched (ADDR):** The hardware requires a specific read sequence to clear this flag depending on the transaction size (e.g., configuring NACK and POS bits for 1-byte or 2-byte reads). Once cleared, the state shifts to `I2C_STATE_BUSY`.

**Transmit Flow (Write):**

* Triggered by **TxE** (Transmit Buffer Empty).
* The driver dequeues a byte from the TX queue.
* If data exists, it is written to the Data Register (DR).
* If the queue is empty and **BTF** (Byte Transfer Finished) is set, the driver generates a STOP condition, sets the state to `I2C_STATE_DONE`, re-enables buffers, and fires the completion callback.

**Receive Flow (Read):**

* Triggered by **RxNE** (Receive Buffer Not Empty) and **BTF**.
* The driver reads the Data Register and pushes the value into the RX queue.
* The interrupt handles the strict STM32 timing requirements for ending a read transfer, manipulating the ACK bit and generating the STOP condition exactly when the remaining byte count reaches the threshold (e.g., handling the final 2 or 3 bytes).
* Once the requested `transfer_size` is met, it shifts to `I2C_STATE_DONE` and fires the callback.

---
### 5. I2S / SPI Driver Interaction

The I2S driver is currently implemented as a rigid, single-purpose wrapper around the SPI peripheral. Because I2S is an alternate function of the SPI silicon on the STM32F4, the I2S initialization leverages the existing `SPI_driver_t` functions to manipulate the registers.

#### Current State & Limitations

This driver is heavily hardcoded to support the exact requirements of the Cirrus Logic CS43L22 DAC. While the API accepts an instance parameter (supporting `I2S_INSTANCE_2` and `I2S_INSTANCE_3`), it essentially forces the peripheral into a 16-bit, 48kHz, Master Transmit configuration.

**Note:** Audio data transmission is currently handled via a polling/busy-wait loop outside of this initialization sequence. A future update will transition this to a DMA-driven architecture to free up CPU cycles.

#### API & Usage

There is currently only one user-facing initialization function.

```c
typedef enum I2S_instance{
    I2S_INSTANCE_2 = 0,
    I2S_INSTANCE_3 = 1
} I2S_instance_t;

void i2s_init(I2S_instance_t instance, RCC_t* rcc);

```

**What `i2s_init` does under the hood:**
When called (e.g., passing `I2S_INSTANCE_3`), the function executes the complete hardware setup sequence sequentially:

1. **Clock Generation:** Enables the dedicated Audio PLL (`PLLI2S`) via the RCC.
2. **Pin Mapping:** Initializes the required GPIO pins for the specific instance (e.g., PC7, PC10, PC12, PA4 for I2S3).
3. **Peripheral Clock:** Enables the bus clock for the target SPI/I2S instance.
4. **Hardware Configuration:** Maps the `I2S_instance_t` to its corresponding `SPI_instance_t` to fetch the SPI driver handle. It then temporarily disables the peripheral to apply the following hardcoded settings:
* **Sample Rate:** 48kHz (`I2SDIV_48K` and `ODD` bit set).
* **Master Clock (MCK):** Enabled (driving the 12.288MHz signal to the DAC).
* **Mode:** I2S Mode (disables standard SPI behavior).
* **Standard:** I2S Phillips Standard.
* **Operation:** Master Transmit.
* **Clock Polarity:** Low (`I2S_CKPOL_LOW`).
* **Data/Channel Length:** 16-bit data packed into a 16-bit channel frame.


5. **Enable:** Re-enables the peripheral, making it ready to transmit audio frames.

**Safety Interlock:** If an unsupported instance (anything other than 2 or 3) is passed, the system triggers a software breakpoint (`__BKPT(0)`) to halt execution immediately.

---

### 6. Interrupt Service Architecture

The system relies heavily on a custom, bare-metal interrupt architecture to manage peripheral events, data transfers, and hardware faults without relying on an RTOS.

#### Vector Table Management

The interrupt vector table is defined in the system startup assembly file (`startup_stm32f407xx.S`). The default handlers are defined as `weak` symbols. To link a custom Interrupt Service Routine (ISR), the system simply declares a C function with the exact matching name, which overrides the weak assembly definition.

```c
// Example ISR overrides defined in the C headers
void I2C1_EV_IRQHandler(void);
void I2C1_ER_IRQHandler(void);

```

#### Priority Grouping & Assignments

The system uses **Priority Grouping 4** (`PRIGROUP_4PRE_0SUB`), which allocates all 4 bits to preemption priority and 0 bits to sub-priority. This provides 16 distinct preemption levels, ensuring critical hardware faults and high-speed data transfers can always interrupt lower-priority tasks.

**Current Priority Mapping:**

* **Priority 0 (Highest):** `BusFault_IRQn`, `MemoryManagement_IRQn`, `UsageFault_IRQn` (Ensures faults preempt all other operations for safe halting).
* **Priority 2:** `I2C1_EV_IRQn` (I2C Event interrupts).
* **Priority 3:** `I2C1_ER_IRQn` (I2C Error interrupts).
* **Priority 4:** `SysTick_IRQn` (Standard system timer).

#### Shared Resources & Concurrency (Lock-Free Queues)

Passing data safely between the main execution loop and asynchronous ISRs is handled via a custom queue data structure (`queue_t`).

To prevent race conditions without the overhead of disabling interrupts or using atomic locks, the queue employs a **Single-Producer / Single-Consumer (SPSC)** model.

* The `head` and `tail` pointers are explicitly declared as `volatile` to prevent compiler optimization errors.
* Because only one context (either the ISR or the main loop) is ever writing to the head, and the other is exclusively reading from the tail, the queue remains inherently thread-safe.

```c
typedef struct queue {
    uint8_t* buffer;
    volatile uint32_t head;
    volatile uint32_t tail;
    uint32_t capacity;
} queue_t;

```

#### Flag Management

Interrupt flags (such as `ADDR`, `TxE`, `RxNE` in the I2C peripheral) are managed via hardware-level clears. The ISRs execute specific register read sequences (e.g., reading `SR1` followed by `SR2`) mandated by the STM32 reference manual. This inherently clears the interrupt flags without requiring explicit bit-clearing operations in software.

