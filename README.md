# ESP32 HUB75 Matrix Panel DMA Graphics Demo

A comprehensive graphics demo for **ESP32** driving **HUB75 RGB LED panels** using the [ESP32-HUB75-MatrixPanel-I2S-DMA](https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA) library. Demonstrates basic shapes, text rendering, animations, gradients, and custom patterns on a 64x64 RGB LED matrix.

---

## Features

- Draw pixels, lines, rectangles, circles, triangles, and rounded rectangles
- Render text with adjustable size and centered alignment
- Draw custom shapes like stars
- Create horizontal gradients
- Animations: rotating lines with orbiting dots
- Patterns: checkerboard and gradient fills
- Adjustable brightness

---

## Hardware

- ESP32 board (tested with ESP32 DevKit)
- HUB75 64x64 RGB LED panel (1 panel chain)

**Connections:**

| Panel Pin | ESP32 Pin |
|-----------|-----------|
| R1        | 17        |
| G1        | 18        |
| B1        | 8         |
| R2        | 3         |
| G2        | 2         |
| B2        | 10        |
| A         | 15        |
| B         | 7         |
| C         | 5         |
| D         | 4         |
| E         | 13        |
| LAT       | 6         |
| OE        | 12        |
| CLK       | 5         |
![Untitled](https://github.com/user-attachments/assets/30fe7a4b-251d-4e1b-9f9d-e0778922268e)

> Adjust pins according to your ESP32 board layout.

---

## Software Requirements

- [Arduino IDE](https://www.arduino.cc/en/software) or PlatformIO
- ESP32 board package installed in Arduino IDE
- Libraries:
  - [ESP32-HUB75-MatrixPanel-I2S-DMA](https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA)

---

## Installation

1. Clone the repository:

```bash
git clone https://github.com/yourusername/ESP32-HUB75-DMA-Graphics.git
cd ESP32-HUB75-DMA-Graphics

Open the project in Arduino IDE or PlatformIO

Install required libraries if missing

Adjust pin configuration in main.cpp for your hardware setup

Compile and upload to your ESP32

Usage

The demo cycles through multiple graphics routines:

Basic shapes: pixels, lines, rectangles, circles, triangles

Text rendering: standard and centered text

Animations: rotating line with orbiting dots

Patterns: checkerboard, gradient, and stars

Each demo runs for a few seconds before moving to the next.

Customization

Brightness: graphics->setBrightness(brightness) (0-255)

Colors: Modify myRED, myGREEN, myBLUE, etc., in setup()

Text size: Adjust textSize in drawText() or drawCenteredText()

License

MIT License – free to use, modify, and share.
