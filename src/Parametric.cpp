#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <math.h>

// Panel configuration for 64x64 HUB75
#define PANEL_RES_X 64
#define PANEL_RES_Y 64
#define PANEL_CHAIN 1

// ESP32-S3 Pin configuration for HUB75
// Adjust these pins based on your specific wiring
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
  


void drawParticleShape(int x, int y, float size, int shapeType, uint16_t color);
void updateAndDrawParticles();
void calculateCurvePoint(float theta, float r, int type, float* x, float* y);
 void drawParametricCurveType(int curveType, float alpha);
void drawParametricCurve();
void initializeParticles() ;
void loop();
void initializeColorPalettes();
float noise(float x);


// Create display object
MatrixPanel_I2S_DMA *dma_display = nullptr;

// Animation variables
float t = 0;
int currentCurve = 0;
int nextCurve = 1;
float transitionProgress = 0;
unsigned long lastChangeTime = 0;
float animationSpeed = 0.03;
float maxRadius;
float minRadius;
const int CENTER_X = PANEL_RES_X / 2;
const int CENTER_Y = PANEL_RES_Y / 2;

// Particle system (reduced for 64x64)
struct Particle {
  float angle;
  float speed;
  float offset;
  uint16_t color;
  float size;
  float life;
  float birthTime;
  int shapeType;
  float pulseOffset;
};

const int MAX_PARTICLES = 20;
Particle particles[MAX_PARTICLES];

// Color palettes optimized for RGB565
const int NUM_PALETTES = 8;
uint16_t colorPalettes[NUM_PALETTES][3];

// HSV to RGB565 conversion for smooth colors
uint16_t hsvToRgb565(float h, float s, float v) {
  float c = v * s;
  float x = c * (1 - abs(fmod(h / 60.0, 2) - 1));
  float m = v - c;
  
  float r, g, b;
  if (h >= 0 && h < 60) { r = c; g = x; b = 0; }
  else if (h >= 60 && h < 120) { r = x; g = c; b = 0; }
  else if (h >= 120 && h < 180) { r = 0; g = c; b = x; }
  else if (h >= 180 && h < 240) { r = 0; g = x; b = c; }
  else if (h >= 240 && h < 300) { r = x; g = 0; b = c; }
  else { r = c; g = 0; b = x; }
  
  r = (r + m) * 255;
  g = (g + m) * 255;
  b = (b + m) * 255;
  
  return dma_display->color565((uint8_t)r, (uint8_t)g, (uint8_t)b);
}

// Noise function (simplified Perlin-like)
float noise(float x) {
  int xi = (int)x;
  float xf = x - xi;
  float u = xf * xf * (3.0 - 2.0 * xf);
  
  float a = sin(xi * 12.9898 + 78.233) * 43758.5453;
  float b = sin((xi + 1) * 12.9898 + 78.233) * 43758.5453;
  a = a - floor(a);
  b = b - floor(b);
  
  return a * (1 - u) + b * u;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ESP32-S3 HUB75 Parametric Curves...");
  
  // Configure HUB75 pins for ESP32-S3
  HUB75_I2S_CFG::i2s_pins _pins = {
    R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN,
    A_PIN, B_PIN, C_PIN, D_PIN, E_PIN,
    LAT_PIN, OE_PIN, CLK_PIN
  };
  
  HUB75_I2S_CFG mxconfig(
    PANEL_RES_X,
    PANEL_RES_Y,
    PANEL_CHAIN,
    _pins
  );
  
  // Initialize display
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(80); // Adjust brightness (0-255)
  dma_display->clearScreen();
  
  // Initialize color palettes
  initializeColorPalettes();
  
  // Set radius constraints for 64x64 panel
  maxRadius = min(PANEL_RES_X, PANEL_RES_Y) * 0.4;
  minRadius = maxRadius * 0.3;
  
  // Initialize particles
  initializeParticles();
  
  Serial.println("Initialization complete!");
}

void initializeColorPalettes() {
  // Plasma fractal colors
  colorPalettes[0][0] = hsvToRgb565(270, 0.8, 1.0);
  colorPalettes[0][1] = hsvToRgb565(15, 0.9, 1.0);
  colorPalettes[0][2] = hsvToRgb565(240, 0.7, 1.0);
  
  // Neon cosmic glow
  colorPalettes[1][0] = hsvToRgb565(120, 0.8, 1.0);
  colorPalettes[1][1] = hsvToRgb565(210, 0.9, 1.0);
  colorPalettes[1][2] = hsvToRgb565(300, 0.8, 1.0);
  
  // Aurora veil
  colorPalettes[2][0] = hsvToRgb565(160, 0.7, 1.0);
  colorPalettes[2][1] = hsvToRgb565(280, 0.8, 1.0);
  colorPalettes[2][2] = hsvToRgb565(200, 0.9, 1.0);
  
  // Solar flare
  colorPalettes[3][0] = hsvToRgb565(30, 1.0, 1.0);
  colorPalettes[3][1] = hsvToRgb565(60, 0.9, 1.0);
  colorPalettes[3][2] = hsvToRgb565(0, 0.8, 1.0);
  
  // Quantum prism
  colorPalettes[4][0] = hsvToRgb565(180, 0.8, 1.0);
  colorPalettes[4][1] = hsvToRgb565(300, 0.9, 1.0);
  colorPalettes[4][2] = hsvToRgb565(60, 0.7, 1.0);
  
  // Galactic dust
  colorPalettes[5][0] = hsvToRgb565(220, 0.6, 1.0);
  colorPalettes[5][1] = hsvToRgb565(270, 0.8, 1.0);
  colorPalettes[5][2] = hsvToRgb565(330, 0.7, 1.0);
  
  // Electric blue
  colorPalettes[6][0] = hsvToRgb565(200, 1.0, 1.0);
  colorPalettes[6][1] = hsvToRgb565(240, 0.9, 1.0);
  colorPalettes[6][2] = hsvToRgb565(180, 0.8, 1.0);
  
  // Fire colors
  colorPalettes[7][0] = hsvToRgb565(0, 1.0, 1.0);
  colorPalettes[7][1] = hsvToRgb565(30, 0.9, 1.0);
  colorPalettes[7][2] = hsvToRgb565(60, 0.8, 1.0);
}

void initializeParticles() {
  for (int i = 0; i < MAX_PARTICLES; i++) {
    particles[i].angle = random(0, 628) / 100.0;
    particles[i].speed = random(10, 30) / 1000.0;
    particles[i].offset = random(0, 628) / 100.0;
    particles[i].color = colorPalettes[currentCurve % NUM_PALETTES][random(0, 3)];
    particles[i].size = random(1, 3);
    particles[i].life = random(100, 300) / 10.0;
    particles[i].birthTime = t;
    particles[i].shapeType = random(0, 4);
    particles[i].pulseOffset = random(0, 628) / 100.0;
  }
}

void loop() {
  // Clear screen with slight fade
  dma_display->fillScreen(0x0000);
  
  // Check for curve transition
  if (millis() - lastChangeTime > 6000) {
    transitionProgress += 0.02;
    if (transitionProgress >= 1.0) {
      transitionProgress = 0;
      currentCurve = nextCurve;
      nextCurve = (nextCurve + 1) % 25; // 25 different curve types
      lastChangeTime = millis();
    }
  }
  
  // Draw main parametric curve
  drawParametricCurve();
  
  // Draw secondary curve during transition
  if (transitionProgress > 0) {
    drawParametricCurveType(nextCurve, transitionProgress * 0.5);
  }
  
  // Draw particles
  updateAndDrawParticles();
  
  // Update time
  t += animationSpeed;
  
  delay(40); // ~25 FPS for smooth animation
}

void drawParametricCurve() {
  drawParametricCurveType(currentCurve, 1.0);
}

void drawParametricCurveType(int curveType, float alpha) {
  int resolution = 120; // Optimized for 64x64
  float size = maxRadius * (1.0 + 0.1 * sin(t * 0.5));
  
  uint16_t palette[3];
  for (int i = 0; i < 3; i++) {
    palette[i] = colorPalettes[curveType % NUM_PALETTES][i];
  }
  
  float prevX = 0, prevY = 0;
  bool firstPoint = true;
  
  for (int i = 0; i <= resolution; i++) {
    float theta = (float)i / resolution * 8 * PI; // More rotations for complexity
    
    float x, y;
    calculateCurvePoint(theta, size, curveType, &x, &y);
    
    // Apply radius constraints
    float r = sqrt(x * x + y * y);
    if (r < minRadius && r > 0.1) {
      float scaleFactor = minRadius / r;
      x *= scaleFactor;
      y *= scaleFactor;
    }
    
    // Transform to screen coordinates
    int screenX = CENTER_X + (int)x;
    int screenY = CENTER_Y + (int)y;
    
    // Ensure coordinates are within screen bounds
    if (screenX >= 0 && screenX < PANEL_RES_X && 
        screenY >= 0 && screenY < PANEL_RES_Y) {
      
      if (!firstPoint) {
        // Dynamic color based on position and time
        float hue = fmod(theta * 30 + t * 50, 360);
        uint16_t color = hsvToRgb565(hue, 0.8, alpha);
        
        dma_display->drawLine(prevX, prevY, screenX, screenY, color);
      }
      
      prevX = screenX;
      prevY = screenY;
      firstPoint = false;
    }
  }
}

void calculateCurvePoint(float theta, float r, int type, float* x, float* y) {
  float tFactor = t * 0.8;
  
  switch(type % 25) {
    case 0: // Rose curve
      *x = r * cos(3 * theta + tFactor) * cos(theta);
      *y = r * sin(2 * theta + tFactor) * sin(theta);
      break;
      
    case 1: // Spiral with modulation
      {
        float spiral = r * (0.6 + 0.3 * sin(theta * 0.5 + tFactor));
        *x = spiral * cos(theta) + r * 0.15 * cos(9 * theta + tFactor);
        *y = spiral * sin(theta) - r * 0.15 * sin(7 * theta - tFactor);
      }
      break;
      
    case 2: // Lissajous curves
      *x = r * sin(theta * 2.3 + tFactor) * cos(theta * 1.7 - tFactor);
      *y = r * cos(theta * 1.9 - tFactor) * sin(theta * 2.1 + tFactor);
      break;
      
    case 3: // Modulated circle
      *x = r * cos(theta) * (1 + 0.25 * sin(7 * theta + tFactor * 2));
      *y = r * sin(theta) * (1 + 0.25 * cos(5 * theta - tFactor * 1.5));
      break;
      
    case 4: // Vortex
      {
        float vortex = r * (0.4 + 0.4 * pow(sin(theta * 0.5 + tFactor), 2));
        *x = vortex * cos(theta + 3 * sin(theta * 0.3 + tFactor * 0.4));
        *y = vortex * sin(theta + 3 * cos(theta * 0.25 - tFactor * 0.3));
      }
      break;
      
    case 5: // Complex flower
      *x = r * sin(3 * theta + tFactor * 1.2) * cos(2 * theta - tFactor * 0.8);
      *y = r * cos(4 * theta - tFactor * 0.9) * sin(5 * theta + tFactor * 1.1);
      break;
      
    case 6: // Branching pattern
      {
        float branch = r * (0.5 + 0.1 * noise(theta * 3 + tFactor));
        *x = branch * cos(theta) * (1 + 0.2 * sin(11 * theta + tFactor * 2));
        *y = branch * sin(theta) * (1 + 0.2 * cos(13 * theta - tFactor * 1.8));
      }
      break;
      
    case 7: // Orbital motion
      {
        float orbital = r * (0.7 + 0.15 * sin(theta * 1.8 + tFactor * 2.5));
        *x = orbital * cos(theta + sin(theta * 6 + tFactor * 1.8));
        *y = orbital * sin(theta + cos(theta * 7 - tFactor * 2.1));
      }
      break;
      
    case 8: // Hyperbolic
      *x = r * 0.5 * tan(theta * 0.15 + tFactor * 0.5) * cos(theta * 0.8);
      *y = r * 0.5 * tan(theta * 0.18 - tFactor * 0.4) * sin(theta * 0.9);
      break;
      
    case 9: // Power curves
      *x = r * pow(abs(sin(theta * 0.8 + tFactor)), 1.8) * cos(theta * 1.5);
      *y = r * pow(abs(cos(theta * 0.9 - tFactor)), 1.6) * sin(theta * 1.8);
      break;
      
    case 10: // Cardioid
      {
        float card = r * (1 + cos(theta + tFactor * 0.7)) * 0.6;
        *x = card * cos(theta);
        *y = card * sin(theta);
      }
      break;
      
    case 11: // Epicycloid
      {
        float a = r * 0.5, b = r * 0.15;
        *x = (a + b) * cos(theta) - b * cos((a/b + 1) * theta + tFactor);
        *y = (a + b) * sin(theta) - b * sin((a/b + 1) * theta + tFactor);
      }
      break;
      
    case 12: // Hypocycloid
      {
        float a = r * 0.6, b = r * 0.12;
        *x = (a - b) * cos(theta) + b * cos((a/b - 1) * theta - tFactor);
        *y = (a - b) * sin(theta) - b * sin((a/b - 1) * theta - tFactor);
      }
      break;
      
    case 13: // Star pattern
      {
        float star = r * (0.6 + 0.2 * sin(theta * 5 + tFactor * 1.3));
        *x = star * cos(theta) * (1 + 0.15 * sin(8 * theta + tFactor * 2));
        *y = star * sin(theta) * (1 + 0.15 * cos(8 * theta - tFactor * 1.7));
      }
      break;
      
    case 14: // Torus knot
      {
        float p = 3, q = 2;
        *x = r * 0.4 * cos(p * theta + tFactor) * (2 + cos(q * theta));
        *y = r * 0.4 * sin(p * theta + tFactor) * (2 + cos(q * theta));
      }
      break;
      
    case 15: // Butterfly curve
      *x = r * 0.3 * sin(theta + tFactor) * (exp(cos(theta)) - 2 * cos(4 * theta));
      *y = r * 0.3 * cos(theta + tFactor) * (exp(cos(theta)) - 2 * cos(4 * theta));
      break;
      
    case 16: // Logarithmic spiral
      {
        float spiral = r * 0.1 * exp(theta * 0.08);
        *x = spiral * cos(theta + tFactor * 0.5);
        *y = spiral * sin(theta + tFactor * 0.5);
      }
      break;
      
    case 17: // Rhodonea (rose)
      {
        float k = 4; // Number of petals
        float rho = r * 0.8 * cos(k * theta + tFactor);
        *x = rho * cos(theta);
        *y = rho * sin(theta);
      }
      break;
      
    case 18: // Limacon
      {
        float a = r * 0.4, b = r * 0.6;
        float rho = a + b * cos(theta + tFactor * 0.8);
        *x = rho * cos(theta);
        *y = rho * sin(theta);
      }
      break;
      
    case 19: // Astroid
      {
        float a = r * 0.6;
        *x = a * pow(cos(theta + tFactor * 0.5), 3);
        *y = a * pow(sin(theta + tFactor * 0.5), 3);
      }
      break;
      
    case 20: // Cycloid
      {
        float a = r * 0.1;
        *x = a * (theta - sin(theta + tFactor));
        *y = a * (1 - cos(theta + tFactor));
      }
      break;
      
    case 21: // Trochoid
      {
        float a = r * 0.3, b = r * 0.1;
        *x = a * theta * cos(theta + tFactor) - b * sin(theta + tFactor);
        *y = a * theta * sin(theta + tFactor) + b * cos(theta + tFactor);
      }
      break;
      
    case 22: // Deltoid
      {
        float a = r * 0.3;
        *x = a * (2 * cos(theta + tFactor) + cos(2 * theta + tFactor));
        *y = a * (2 * sin(theta + tFactor) - sin(2 * theta + tFactor));
      }
      break;
      
    case 23: // Nephroid
      {
        float a = r * 0.25;
        *x = a * (3 * cos(theta + tFactor) - cos(3 * theta + tFactor));
        *y = a * (3 * sin(theta + tFactor) - sin(3 * theta + tFactor));
      }
      break;
      
    default: // Simple modulated circle
      *x = r * cos(theta) * (1 + 0.2 * sin(6 * theta + tFactor));
      *y = r * sin(theta) * (1 + 0.2 * cos(6 * theta + tFactor));
      break;
  }
}

void updateAndDrawParticles() {
  for (int i = 0; i < MAX_PARTICLES; i++) {
    Particle* p = &particles[i];
    
    // Reset particle if life expired
    if (t - p->birthTime > p->life) {
      p->angle = random(0, 628) / 100.0;
      p->speed = random(10, 30) / 1000.0;
      p->offset = random(0, 628) / 100.0;
      p->color = colorPalettes[currentCurve % NUM_PALETTES][random(0, 3)];
      p->size = random(1, 3);
      p->life = random(100, 300) / 10.0;
      p->birthTime = t;
      p->shapeType = random(0, 4);
      continue;
    }
    
    // Calculate particle position
    float theta = t * p->speed + p->offset;
    float r = maxRadius * (0.8 + 0.2 * sin(p->offset + t * 0.2));
    
    float x, y;
    calculateCurvePoint(theta, r, currentCurve, &x, &y);
    
    // Transform to screen coordinates
    int screenX = CENTER_X + (int)x;
    int screenY = CENTER_Y + (int)y;
    
    // Draw particle if within bounds
    if (screenX >= 1 && screenX < PANEL_RES_X - 1 && 
        screenY >= 1 && screenY < PANEL_RES_Y - 1) {
      
      // Calculate alpha based on life
      float alpha = 1.0 - (t - p->birthTime) / p->life;
      alpha = max(0.2f, alpha);
      
      // Dynamic color with pulsing
      float hue = fmod(theta * 20 + t * 80, 360);
      uint16_t color = hsvToRgb565(hue, 0.9, alpha);
      
      drawParticleShape(screenX, screenY, p->size, p->shapeType, color);
    }
  }
}

void drawParticleShape(int x, int y, float size, int shapeType, uint16_t color) {
  int s = max(1, (int)size);
  
  switch(shapeType) {
    case 0: // Dot
      dma_display->drawPixel(x, y, color);
      break;
      
    case 1: // Small cross
      dma_display->drawPixel(x, y, color);
      if (s > 1) {
        dma_display->drawPixel(x-1, y, color);
        dma_display->drawPixel(x+1, y, color);
        dma_display->drawPixel(x, y-1, color);
        dma_display->drawPixel(x, y+1, color);
      }
      break;
      
    case 2: // Small square
      dma_display->drawPixel(x, y, color);
      if (s > 1) {
        dma_display->drawRect(x-1, y-1, 3, 3, color);
      }
      break;
      
    default: // Diamond
      dma_display->drawPixel(x, y, color);
      if (s > 1) {
        dma_display->drawPixel(x-1, y, color);
        dma_display->drawPixel(x+1, y, color);
        dma_display->drawPixel(x, y-1, color);
        dma_display->drawPixel(x, y+1, color);
      }
      break;
  }
}