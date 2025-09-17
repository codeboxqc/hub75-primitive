# ESP32 HUB75 LED Matrix Graphics Library

A comprehensive graphics library for controlling HUB75 LED matrix panels using ESP32 without WiFi. Built on top of the ESP32-HUB75-MatrixPanel-I2S-DMA library for high-performance, flicker-free display.

## Features

- ✅ **High Performance**: DMA-based rendering for smooth animations
- ✅ **Complete Graphics API**: setPixel, lines, rectangles, circles, triangles
- ✅ **Text Rendering**: Multiple font sizes and positioning
- ✅ **Advanced Graphics**: Gradients, patterns, animations, stars
- ✅ **No WiFi Required**: Standalone operation
- ✅ **Low CPU Overhead**: Hardware-accelerated display refresh
- ✅ **Color Support**: Full RGB565 color space
- ✅ **Brightness Control**: 0-255 brightness levels

## Hardware Requirements

- ESP32 (original), ESP32-S2, or ESP32-S3
- HUB75 LED Matrix Panel (64x64 recommended)
- Proper power supply (5V, adequate amperage)
- Jumper wires for connections

## Pin Configuration

```cpp
#define R1_PIN 25
#define G1_PIN 26
#define B1_PIN 27
#define R2_PIN 14
#define G2_PIN 12
#define B2_PIN 13
#define A_PIN 23
#define B_PIN 19
#define C_PIN 5
#define D_PIN 17
#define E_PIN 18  // Required for 64x64 panels
#define LAT_PIN 4
#define OE_PIN 15
#define CLK_PIN 16
```

## Installation

### Arduino IDE
1. Install **ESP32-HUB75-MatrixPanel-DMA** from Library Manager
2. Install **Adafruit GFX Library** dependency
3. Copy the code below to your sketch

### PlatformIO
Add to your `platformio.ini`:
```ini
lib_deps = 
    https://github.com/mrcodetastic/ESP32-HUB75-MatrixPanel-DMA
    adafruit/Adafruit GFX Library
```

## Memory Requirements

- **64x32 panel**: ~25KB RAM
- **64x64 panel**: ~49KB RAM  
- **128x64 panel**: ~98KB RAM

Check the [memory calculator](https://github.com/mrcodetastic/ESP32-HUB75-MatrixPanel-DMA/blob/master/doc/memcalc.md) for larger configurations.

## Quick Start

```cpp
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

#define PANEL_RES_X 64
#define PANEL_RES_Y 64
#define PANEL_CHAIN 1

MatrixPanel_I2S_DMA *dma_display = nullptr;

void setup() {
    // Configure pins and create display
    HUB75_I2S_CFG::i2s_pins _pins = {R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN};
    HUB75_I2S_CFG mxconfig(PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN, _pins);
    
    dma_display = new MatrixPanel_I2S_DMA(mxconfig);
    dma_display->begin();
    dma_display->setBrightness8(90);
}

void loop() {
    // Your graphics code here
    dma_display->drawPixel(10, 10, dma_display->color565(255, 0, 0));
    delay(100);
}
```

## Graphics API Reference

### Basic Drawing
```cpp
// Set individual pixels
graphics->setPixel(x, y, color);

// Clear screen
graphics->clearScreen(color);

// Draw lines
graphics->drawLine(x0, y0, x1, y1, color);
```

### Shapes
```cpp
// Rectangles
graphics->drawRectangle(x, y, width, height, color, filled);

// Circles  
graphics->drawCircle(centerX, centerY, radius, color, filled);

// Triangles
graphics->drawTriangle(x0, y0, x1, y1, x2, y2, color, filled);

// Stars
graphics->drawStar(centerX, centerY, outerRadius, innerRadius, color, filled);
```

### Text
```cpp
// Basic text
graphics->drawText(x, y, "Hello", color, textSize);

// Centered text
graphics->drawCenteredText("ESP32", color, textSize);
```

### Advanced Graphics
```cpp
// Gradients
graphics->drawHorizontalGradient(x, y, width, height, color1, color2);

// Patterns
graphics->drawCheckerboard(squareSize, color1, color2);

// Animations
graphics->animateRotatingLine(centerX, centerY, length, color, angle);
```

### Color Management
```cpp
// Create colors
uint16_t red = dma_display->color565(255, 0, 0);
uint16_t green = dma_display->color565(0, 255, 0);
uint16_t blue = dma_display->color565(0, 0, 255);

// Set brightness
graphics->setBrightness(128); // 0-255
```

## Example Projects

### Simple Pixel Drawing
```cpp
void drawPattern() {
    graphics->clearScreen();
    
    // Draw a cross pattern
    for(int i = 0; i < 64; i++) {
        graphics->setPixel(i, 32, myRED);
        graphics->setPixel(32, i, myGREEN);
    }
}
```

### Animation Example
```cpp
void rotatingSquare() {
    for(int angle = 0; angle < 360; angle += 5) {
        graphics->clearScreen();
        
        // Calculate rotated square corners
        int centerX = 32, centerY = 32, size = 15;
        // ... rotation math ...
        
        graphics->drawRectangle(x, y, size, size, myBLUE, true);
        delay(50);
    }
} 

```

### Text Display
```cpp
void displayInfo() {
    graphics->clearScreen();
    graphics->drawText(2, 5, "ESP32", myRED, 1);
    graphics->drawText(2, 15, "HUB75", myGREEN, 1);
    graphics->drawCenteredText("LED", myWHITE, 2);
}
```

## Troubleshooting

### Display Issues
- **Ghosting**: Try `dma_display->setLatBlanking(2)`
- **Color offset**: Set `mxconfig.clkphase = false`
- **Flickering**: Check power supply and connections
- **Dim display**: Increase brightness with `setBrightness8(255)`

### Power Requirements
- Use adequate 5V power supply
- Add 1000-2000µF capacitors across VCC/GND on each panel
- Keep power cables short and thick

### Memory Issues
- Monitor free heap: `Serial.println(ESP.getFreeHeap())`
- Use smaller panels or reduce color depth for large displays
- Consider ESP32-S3 with PSRAM for bigger setups

## Wiring Diagram

```
ESP32 Pin  ->  HUB75 Pin
R1 (25)    ->  R1
G1 (26)    ->  G1  
B1 (27)    ->  B1
R2 (14)    ->  R2
G2 (12)    ->  G2
B2 (13)    ->  B2
A (23)     ->  A
B (19)     ->  B  
C (5)      ->  C
D (17)     ->  D
E (18)     ->  E (64x64 panels only)
LAT (4)    ->  LAT
OE (15)    ->  OE
CLK (16)   ->  CLK
GND        ->  GND
5V         ->  VCC (External PSU)
```

![Untitled](https://github.com/user-attachments/assets/84edbf2a-8e12-424d-b3a0-1b758331da51)



## Performance Tips

1. **Use DMA efficiently**: Minimize `clearScreen()` calls
2. **Batch operations**: Group drawing commands together  
3. **Optimize colors**: Pre-calculate color565 values
4. **Memory management**: Monitor heap usage
5. **Frame timing**: Use consistent delay intervals

## License

This project builds upon the excellent ESP32-HUB75-MatrixPanel-I2S-DMA library. Please check the original library's license terms.

## Contributing

Feel free to submit issues and enhancement requests!

## Acknowledgments

- [ESP32-HUB75-MatrixPanel-I2S-DMA](https://github.com/mrcodetastic/ESP32-HUB75-MatrixPanel-DMA) - Base library
- [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library) - Graphics primitives
- ESP32 community for hardware support

## Support

For hardware-specific issues, refer to the [original library documentation](https://github.com/mrcodetastic/ESP32-HUB75-MatrixPanel-DMA).

---
**Ready to create amazing LED matrix displays with your ESP32!** 🚀
