# STM32F446RE Bare-Metal Driver Development

A register-level embedded C project developed for the **STM32F446RE Nucleo** board using **STM32CubeIDE**.

The main objective of this project is to understand the STM32F4 microcontroller at the hardware/register level and develop reusable peripheral drivers without relying on the STM32 HAL driver library.

## Hardware

- **Microcontroller:** STM32F446RE
- **Board:** NUCLEO-F446RE
- **Core:** ARM Cortex-M4
- **Architecture:** 32-bit ARM
- **Debugger/Programmer:** ST-LINK
- **IDE:** STM32CubeIDE

## Software

- Embedded C
- STM32CubeIDE
- ARM GNU Toolchain
- Register-level programming
- STM32 Reference Manual
- STM32F446RE Datasheet

## Peripherals / Drivers

The project focuses on developing low-level drivers and understanding peripheral registers.

### GPIO

- GPIO initialization
- Input/output configuration
- Push-pull output
- Pull-up / pull-down configuration
- GPIO read/write operations

### USART / UART

- USART peripheral initialization
- Baud-rate configuration
- Transmit and receive operations
- Status register handling
- UART communication using registers

### ADC

- ADC peripheral configuration
- Analog channel selection
- ADC conversion
- Reading ADC data

### PWM

- Timer-based PWM generation
- Prescaler and auto-reload configuration
- Duty-cycle control

### Timer

- TIM2 configuration
- Timer-based delay generation
- Prescaler and counter configuration

### External Interrupt

- GPIO external interrupt configuration
- EXTI configuration
- NVIC interrupt handling
- Interrupt service routine implementation

## Project Structure

```text
STM32NucleoF46RE_project/
│
├── Inc/
│   ├── Header files
│   └── Driver/API declarations
│
├── Src/
│   ├── Driver implementations
│   └── Application source files
│
├── Startup/
│   └── STM32 startup and interrupt vector files
│
├── STM32F446RETX_FLASH.ld
├── STM32F446RETX_RAM.ld
│
├── Stm32_drivers Debug.cfg
├── Stm32_drivers Debug.launch
│
├── STM32F46RE Datasheet.pdf
├── STM32F46RE reference manual.pdf
└── STM32F46RE schematic.pdf
```

## Driver Architecture

The project follows a layered approach:

```text
        Application
             │
             ▼
      Driver API / Handle
             │
             ▼
      Peripheral Driver
             │
             ▼
     STM32 Peripheral Registers
             │
             ▼
        STM32F446RE
```

The goal is to keep the peripheral drivers reusable so that application code does not need to directly manipulate peripheral registers everywhere.

## Example

A typical driver usage follows this concept:

```c
USART_Handle_t usart1;

usart1.pUSARTx = USART1;

/* Configure USART1 */

USART_Init(&usart1);

/* Transmit data */
USART_SendData(&usart1, data, length);
```

This approach separates **application logic** from **low-level peripheral configuration**.

## Key Concepts Practiced

Through this project, I have worked with:

- Memory-mapped peripheral registers
- Bit manipulation
- Register configuration
- Pointers and structures
- `volatile` variables
- Peripheral clock configuration
- GPIO alternate functions
- Interrupts
- NVIC
- Timers
- ADC
- PWM
- UART/USART
- ARM Cortex-M4 architecture
- Linker scripts
- Startup code
- Debugging using ST-LINK

## Clock Configuration

The project uses the STM32F446RE peripheral clock architecture and requires understanding of:

```text
System Clock
     │
     ├── AHB
     │
     ├── APB1
     │     └── TIM2 / USART2 / ...
     │
     └── APB2
           └── USART1 / ADC / ...
```

Understanding the clock tree is important because peripheral registers cannot operate correctly unless the corresponding peripheral clock is enabled and configured.

## Why Bare-Metal?

This project was developed to understand what happens underneath high-level MCU libraries.

Instead of directly depending on HAL APIs such as:

```c
HAL_GPIO_WritePin();
HAL_UART_Transmit();
HAL_ADC_Start();
```

the project focuses on configuring the STM32 registers directly.

This provides practical understanding of:

- How peripherals are initialized
- How registers control hardware
- How GPIO alternate functions work
- How timers generate PWM
- How UART communication works
- How interrupts reach the CPU
- How MCU memory-mapped peripherals are accessed

## Development Environment

```text
Board       : NUCLEO-F446RE
MCU         : STM32F446RE
Core        : ARM Cortex-M4
IDE         : STM32CubeIDE
Language    : Embedded C
Debugger    : ST-LINK
Toolchain   : ARM GNU Toolchain
```

## Learning Outcome

This project helped me move from using high-level MCU libraries toward understanding **register-level embedded firmware development**.

It also serves as a foundation for implementing additional drivers such as:

- I2C
- SPI
- CAN
- DMA
- Advanced interrupt handling

## Future Improvements

Planned improvements include:

- [ ] I2C driver
- [ ] SPI driver
- [ ] CAN driver
- [ ] DMA driver
- [ ] More robust interrupt handling
- [ ] Driver error/status handling
- [ ] Improved API documentation
- [ ] Unit testing where applicable
- [ ] Embedded RTOS integration

## References

The project uses the official STM32F446RE technical documentation:

- STM32F446RE Datasheet
- STM32F446RE Reference Manual
- STM32F446RE Nucleo Board Schematic

These documents are included in the repository for development and reference purposes.

## Author

**Narayanamoorthy R**

Embedded Systems & IoT Developer

GitHub:\
[https://github.com/NarayanamoorthyRagu](https://github.com/NarayanamoorthyRagu)

---

### Project Focus

**Embedded C | STM32 | ARM Cortex-M4 | Bare-Metal Programming | Peripheral Drivers | UART | GPIO | ADC | PWM | Timers | Interrupts**
