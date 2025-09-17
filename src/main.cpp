#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// Panel configuration
#define PANEL_RES_X 64      // Number of pixels wide of each INDIVIDUAL panel module. 
#define PANEL_RES_Y 64      // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1       // Total number of panels chained one to another

// Pin configuration - adjust these for your ESP32 setup
#define R1_PIN 17 //25/ok
#define B1_PIN 8  //27 /ok
#define R2_PIN  3 //14/ok  ------
#define B2_PIN 10  //13/ok
#define A_PIN  15  //23 ok ?
#define C_PIN 7  //5/ok
#define CLK_PIN 5 //rx2 ok
#define OE_PIN 12 //15/ ok

 #define G1_PIN 18  //26 ok ?
 //#define ground
 #define G2_PIN   2//46 //12 ok
 #define E_PIN  13   //18/ ok // Required for 1/32 scan panels (64x64)

 #define B_PIN  11 //19/  ok
 #define D_PIN   4 //tx2 /ok
 #define LAT_PIN 6 //4/
//#define grnd
  
 
 


// Create display object
MatrixPanel_I2S_DMA *dma_display = nullptr;

// Color definitions (RGB565 format)
uint16_t myRED = dma_display->color565(255, 0, 0);
uint16_t myGREEN = dma_display->color565(0, 255, 0);
uint16_t myBLUE = dma_display->color565(0, 0, 255);
uint16_t myWHITE = dma_display->color565(255, 255, 255);
uint16_t myYELLOW = dma_display->color565(255, 255, 0);
uint16_t myCYAN = dma_display->color565(0, 255, 255);
uint16_t myMAGENTA = dma_display->color565(255, 0, 255);
uint16_t myBLACK = dma_display->color565(0, 0, 0);

// Graphics Library Class with all drawing functions
class GraphicsLib {
private:
  MatrixPanel_I2S_DMA* display;

public:
  GraphicsLib(MatrixPanel_I2S_DMA* disp) : display(disp) {}

  // Set a single pixel
  void setPixel(int x, int y, uint16_t color) {
    if (x >= 0 && x < PANEL_RES_X && y >= 0 && y < PANEL_RES_Y) {
      display->drawPixel(x, y, color);
    }
  }

  // Get pixel color
  uint16_t getPixel(int x, int y) {
    // Note: The DMA library doesn't have a direct getPixel function
    // You would need to maintain your own buffer for this
    return 0; // Placeholder
  }

  // Clear screen
  void clearScreen(uint16_t color = 0) {
    display->fillScreen(color);
  }

  // Draw line
  void drawLine(int x0, int y0, int x1, int y1, uint16_t color) {
    display->drawLine(x0, y0, x1, y1, color);
  }

  // Draw rectangle
  void drawRectangle(int x, int y, int width, int height, uint16_t color, bool filled = false) {
    if (filled) {
      display->fillRect(x, y, width, height, color);
    } else {
      display->drawRect(x, y, width, height, color);
    }
  }

  // Draw circle
  void drawCircle(int centerX, int centerY, int radius, uint16_t color, bool filled = false) {
    if (filled) {
      display->fillCircle(centerX, centerY, radius, color);
    } else {
      display->drawCircle(centerX, centerY, radius, color);
    }
  }

  // Draw triangle
  void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color, bool filled = false) {
    if (filled) {
      display->fillTriangle(x0, y0, x1, y1, x2, y2, color);
    } else {
      display->drawTriangle(x0, y0, x1, y1, x2, y2, color);
    }
  }

  // Draw rounded rectangle
  void drawRoundRect(int x, int y, int width, int height, int radius, uint16_t color, bool filled = false) {
    if (filled) {
      display->fillRoundRect(x, y, width, height, radius, color);
    } else {
      display->drawRoundRect(x, y, width, height, radius, color);
    }
  }

  // Draw text
  void drawText(int x, int y, const char* text, uint16_t color, uint8_t textSize = 1) {
    display->setCursor(x, y);
    display->setTextColor(color);
    display->setTextSize(textSize);
    display->print(text);
  }

  // Draw centered text
  void drawCenteredText(const char* text, uint16_t color, uint8_t textSize = 1) {
    display->setTextSize(textSize);
    int16_t x1, y1;
    uint16_t w, h;
    display->getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    
    int centerX = (PANEL_RES_X - w) / 2;
    int centerY = (PANEL_RES_Y - h) / 2;
    
    drawText(centerX, centerY, text, color, textSize);
  }

  // Custom drawing: Draw a star
  void drawStar(int centerX, int centerY, int outerRadius, int innerRadius, uint16_t color, bool filled = false) {
    const int points = 5;
    int x[10], y[10];
    
    for (int i = 0; i < 10; i++) {
      float angle = (i * PI) / points - PI/2;
      int radius = (i % 2 == 0) ? outerRadius : innerRadius;
      x[i] = centerX + radius * cos(angle);
      y[i] = centerY + radius * sin(angle);
    }
    
    if (filled) {
      // Simple filled star (not perfect but functional)
      for (int i = 0; i < 10; i++) {
        int next = (i + 1) % 10;
        drawTriangle(centerX, centerY, x[i], y[i], x[next], y[next], color, true);
      }
    } else {
      // Draw star outline
      for (int i = 0; i < 10; i++) {
        int next = (i + 1) % 10;
        drawLine(x[i], y[i], x[next], y[next], color);
      }
    }
  }

  // Draw gradient (horizontal)
  void drawHorizontalGradient(int x, int y, int width, int height, uint16_t color1, uint16_t color2) {
    for (int i = 0; i < width; i++) {
      // Simple linear interpolation between colors
      uint8_t r1 = (color1 >> 11) & 0x1F;
      uint8_t g1 = (color1 >> 5) & 0x3F;
      uint8_t b1 = color1 & 0x1F;
      
      uint8_t r2 = (color2 >> 11) & 0x1F;
      uint8_t g2 = (color2 >> 5) & 0x3F;
      uint8_t b2 = color2 & 0x1F;
      
      float ratio = (float)i / (width - 1);
      uint8_t r = r1 + (r2 - r1) * ratio;
      uint8_t g = g1 + (g2 - g1) * ratio;
      uint8_t b = b1 + (b2 - b1) * ratio;
      
      uint16_t color = ((r & 0x1F) << 11) | ((g & 0x3F) << 5) | (b & 0x1F);
      
      display->drawFastVLine(x + i, y, height, color);
    }
  }

  // Animation: Rotating line
  void animateRotatingLine(int centerX, int centerY, int length, uint16_t color, float angle) {
    int x1 = centerX + length * cos(angle);
    int y1 = centerY + length * sin(angle);
    drawLine(centerX, centerY, x1, y1, color);
  }

  // Pattern: Checkerboard
  void drawCheckerboard(int squareSize, uint16_t color1, uint16_t color2) {
    for (int y = 0; y < PANEL_RES_Y; y += squareSize) {
      for (int x = 0; x < PANEL_RES_X; x += squareSize) {
        uint16_t color = ((x/squareSize + y/squareSize) % 2) ? color1 : color2;
        drawRectangle(x, y, squareSize, squareSize, color, true);
      }
    }
  }

  // Set brightness (0-255)
  void setBrightness(uint8_t brightness) {
    display->setBrightness8(brightness);
  }
};

// Create graphics object
GraphicsLib* graphics = nullptr;

void setup() {
  Serial.begin(115200);
  
  // Configure the matrix
  HUB75_I2S_CFG::i2s_pins _pins = {
    R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, 
    A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, 
    LAT_PIN, OE_PIN, CLK_PIN
  };

  HUB75_I2S_CFG mxconfig(
    PANEL_RES_X,   // module width
    PANEL_RES_Y,   // module height
    PANEL_CHAIN,   // chain length
    _pins          // pin mapping
  );

  // Display Setup
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(90); // 0-255
  dma_display->clearScreen();

  // Initialize color definitions after display is created
  myRED = dma_display->color565(255, 0, 0);
  myGREEN = dma_display->color565(0, 255, 0);
  myBLUE = dma_display->color565(0, 0, 255);
  myWHITE = dma_display->color565(255, 255, 255);
  myYELLOW = dma_display->color565(255, 255, 0);
  myCYAN = dma_display->color565(0, 255, 255);
  myMAGENTA = dma_display->color565(255, 0, 255);
  myBLACK = dma_display->color565(0, 0, 0);

  // Create graphics library instance
  graphics = new GraphicsLib(dma_display);
  
  Serial.println("*****************************************************");
  Serial.println(" ESP32-HUB75-MatrixPanel-DMA Graphics Demo");
  Serial.println("*****************************************************");
}

void demoBasicShapes() {
  graphics->clearScreen();
  
  // Draw pixels
  graphics->setPixel(5, 5, myRED);
  graphics->setPixel(6, 5, myGREEN);
  graphics->setPixel(7, 5, myBLUE);
  
  // Draw rectangles
  graphics->drawRectangle(10, 10, 15, 10, myWHITE, false);
  graphics->drawRectangle(30, 10, 15, 10, myYELLOW, true);
  
  // Draw circles
  graphics->drawCircle(20, 35, 8, myCYAN, false);
  graphics->drawCircle(45, 35, 8, myMAGENTA, true);
  
  // Draw triangles
  graphics->drawTriangle(5, 50, 15, 50, 10, 60, myGREEN, false);
  graphics->drawTriangle(25, 50, 35, 50, 30, 60, myRED, true);
  
  // Draw lines
  graphics->drawLine(0, 25, 63, 25, myWHITE);
  graphics->drawLine(32, 0, 32, 63, myWHITE);
  
  delay(3000);
}

void demoText() {
  graphics->clearScreen();
  
  graphics->drawText(2, 5, "HUB75", myRED, 1);
  graphics->drawText(2, 15, "LED", myGREEN, 1);
  graphics->drawText(2, 25, "Matrix", myBLUE, 1);
  graphics->drawCenteredText("ESP32", myWHITE, 2);
  
  delay(3000);
}

void demoAnimations() {
  // Rotating line animation
  for (int angle = 0; angle < 360; angle += 5) {
    graphics->clearScreen();
    graphics->animateRotatingLine(32, 32, 25, myRED, angle * PI / 180);
    
    // Add some orbiting dots
    int x1 = 32 + 15 * cos(angle * PI / 180);
    int y1 = 32 + 15 * sin(angle * PI / 180);
    graphics->setPixel(x1, y1, myGREEN);
    
    int x2 = 32 + 20 * cos(-angle * PI / 180);
    int y2 = 32 + 20 * sin(-angle * PI / 180);
    graphics->setPixel(x2, y2, myBLUE);
    
    delay(50);
  }
}

void demoPatterns() {
  // Checkerboard pattern
  graphics->clearScreen();
  graphics->drawCheckerboard(8, myRED, myBLUE);
  delay(2000);
  
  // Gradient
  graphics->clearScreen();
  graphics->drawHorizontalGradient(0, 0, 64, 64, myRED, myBLUE);
  delay(2000);
  
  // Star
  graphics->clearScreen();
  graphics->drawStar(32, 32, 20, 10, myYELLOW, false);
  delay(2000);
  
  graphics->clearScreen();
  graphics->drawStar(32, 32, 15, 7, myCYAN, true);
  delay(2000);
}

void loop() {
  Serial.println("Running Basic Shapes Demo...");
  demoBasicShapes();
  
  Serial.println("Running Text Demo...");
  demoText();
  
  Serial.println("Running Animation Demo...");
  demoAnimations();
  
  Serial.println("Running Pattern Demo...");
  demoPatterns();
  
  Serial.println("Demo cycle complete. Restarting...");
  delay(1000);
}