#include <Adafruit_GFX.h> // For OLED Monitor
#include <Adafruit_SSD1306.h> // For OLED Monitor
// This is wrong, fix.
extern int SCREEN_WIDTH = 128; // OLED display width, in pixels
extern int SCREEN_HEIGHT = 64; // OLED display height, in pixels
extern int OLED_RESET = 4; // Reset pin # (or -1 if sharing Arduino reset pin)
extern Adafruit_SSD1306 display; // Declares the OLED display
extern int currentEffect = 1;
unsigned long lastEffectChangeTime = 0;
const unsigned long effectDuration = 5000; // Change effect every 5 seconds

void drawRandomPixels() {
  for (int i = 0; i < 50; i++) { // Draw 50 random pixels
    int x = random(SCREEN_WIDTH);
    int y = random(SCREEN_HEIGHT);
    display.drawPixel(x, y, WHITE);
  }
}

void drawMovingLine() {
  static int lineX = 0;
  display.drawLine(lineX, 0, SCREEN_WIDTH - lineX, SCREEN_HEIGHT, WHITE);
  lineX = (lineX + 5) % SCREEN_WIDTH; // Move line, wrap around
}

void drawExpandingCircles() {
  static int circleRadius = 0;
  display.drawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, circleRadius, WHITE);
  circleRadius = (circleRadius + 3) % (SCREEN_WIDTH / 2); // Expand, wrap around
}

void drawFallingStars() {
  struct Star {
    int x;
    int y;
  };
  static Star stars[20]; // Array to hold star data

  // Initialize star positions if they haven't been already
  if (stars[0].x == 0 && stars[0].y == 0) {
    for (int i = 0; i < 20; i++) {
      stars[i].x = random(SCREEN_WIDTH);
      stars[i].y = random(SCREEN_HEIGHT);
    }
  }

  for (int i = 0; i < 20; i++) {
    //move stars
    stars[i].y = (stars[i].y + 3) % SCREEN_HEIGHT; // Increased speed
    //draw stars
    display.drawPixel(stars[i].x, stars[i].y, WHITE);
  }
}

void drawBouncingBall() {
  struct Ball {
    int x;
    int y;
    int xSpeed;
    int ySpeed;
    int radius;
  };
  static Ball ball = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 3, 2, 5};

  // Move ball
  ball.x += ball.xSpeed;
  ball.y += ball.ySpeed;

  // Bounce off walls
  if (ball.x + ball.radius > SCREEN_WIDTH || ball.x - ball.radius < 0) {
    ball.xSpeed *= -1;
  }
  if (ball.y + ball.radius > SCREEN_HEIGHT || ball.y - ball.radius < 0) {
    ball.ySpeed *= -1;
  }
  //draw ball
  display.fillCircle(ball.x, ball.y, ball.radius, WHITE);
}

void drawSineWave() {
  static float phase = 0;
  for (int x = 0; x < SCREEN_WIDTH; x++) {
    // Calculate y position based on sine wave
    int y = SCREEN_HEIGHT / 2 + (sin(phase + x * 0.1) * SCREEN_HEIGHT / 3);
    display.drawPixel(x, y, WHITE);
  }
  phase += 0.2;
}

void drawRotatingBox() {
  static float rotation = 0;
  int centerX = SCREEN_WIDTH / 2;
  int centerY = SCREEN_HEIGHT / 2;
  int size = 20;

  // Calculate corner coordinates
  int x0 = centerX + size * cos(rotation - PI / 4);
  int y0 = centerY + size * sin(rotation - PI / 4);
  int x1 = centerX + size * cos(rotation + PI / 4);
  int y1 = centerY + size * sin(rotation + PI / 4);
  int x2 = centerX + size * cos(rotation + 3 * PI / 4);
  int y2 = centerY + size * sin(rotation + 3 * PI / 4);
  int x3 = centerX + size * cos(rotation - 3 * PI / 4);
  int y3 = centerY + size * sin(rotation - 3 * PI / 4);

  // Draw the box
  display.drawLine(x0, y0, x1, y1, WHITE);
  display.drawLine(x1, y1, x2, y2, WHITE);
  display.drawLine(x2, y2, x3, y3, WHITE);
  display.drawLine(x3, y3, x0, y0, WHITE);

  rotation += 0.05; // Adjust for rotation speed
}

// New Effects

void drawZigZag() {
  static int x = 0;
  static int y = 0;
  static int direction = 1; // 1 for down, -1 for up

  display.drawPixel(x, y, WHITE);
  x += 2; // Move horizontally
  y += direction * 2; // Move vertically

  if (y >= SCREEN_HEIGHT - 1 || y <= 0) {
    direction *= -1; // Reverse direction
  }
  if (x >= SCREEN_WIDTH)
  {
    x = 0;
    y = random(SCREEN_HEIGHT);
  }
}

void drawRain() {
  struct Drop {
    int x;
    int y;
    int length;
  };
  static Drop drops[30];

  if (drops[0].y == 0)
  {
    for (int i = 0; i < 30; i++)
    {
      drops[i].x = random(SCREEN_WIDTH);
      drops[i].y = random(SCREEN_HEIGHT);
      drops[i].length = random(10) + 3;
    }
  }

  for (int i = 0; i < 30; i++)
  {
    drops[i].y += 5;
    if (drops[i].y > SCREEN_HEIGHT)
    {
      drops[i].y = random(SCREEN_HEIGHT / 2) - drops[i].length;
      drops[i].x = random(SCREEN_WIDTH);
      drops[i].length = random(10) + 3;
    }
    for (int j = 0; j < drops[i].length; j++)
    {
      display.drawPixel(drops[i].x, drops[i].y - j, WHITE);
    }
  }
}

void drawCircleWave() {
  static int circleRadius = 1;
  static int wavePhase = 0;
  int centerX = SCREEN_WIDTH / 2;
  int centerY = SCREEN_HEIGHT / 2;

  for (int i = 0; i < 10; i++)
  {
    int currentRadius = circleRadius + i * 8 + wavePhase;
    if (currentRadius > 0 && currentRadius < SCREEN_WIDTH / 2)
    {
      display.drawCircle(centerX, centerY, currentRadius, WHITE);
    }
  }
  circleRadius = (circleRadius + 1) % 8;
  wavePhase = (wavePhase + 2) % 360;
}