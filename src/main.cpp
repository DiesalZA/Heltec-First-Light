#include <Arduino.h>
#include <U8g2lib.h>

#define OLED_CLOCK 15
#define OLED_DATA 4
#define OLED_RESET 16

U8G2_SSD1306_128X64_NONAME_F_HW_I2C g_OLED(U8G2_R2, OLED_RESET, OLED_CLOCK, OLED_DATA);
int g_lineHeight = 0;

void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);

  g_OLED.begin();
  g_OLED.clear();
  g_OLED.setFont(u8g2_font_profont15_tf);

  g_lineHeight = g_OLED.getFontAscent() - g_OLED.getFontDescent();      // Descent is a negative number so we add it to the total 

}

// FramesPerSecond
// 
// Tracks a weighted average in order to smooth out the values that is given. Computes the FPS as the simple recirocal
// of the amount of time taken specified by the caller. So 1/3 of a second is 3 fps. It will take around 10 frames to stablize. 
double FramesPerSecond(double seconds)
{
  static double framesPerSecond;
  framesPerSecond = (framesPerSecond * 0.9) + (1.0 / seconds * 0.1);
  return framesPerSecond;
}

void DrawLinesandGraphicsFrame(int framesPerSecond)
{
  g_OLED.clearBuffer();
  g_OLED.home();

  g_OLED.drawFrame(0, 0, g_OLED.getWidth(), g_OLED.getHeight());        // Draw a border around the screen

  // Draw some text into the frame
  g_OLED.setCursor(3, g_lineHeight * 2 + 2);                            // Draw "Hello World" and framerate on the left hand side
  g_OLED.print("Hello");
  g_OLED.setCursor(3, g_lineHeight * 3 + 2);
  g_OLED.print("World");
  g_OLED.setCursor(3, g_lineHeight * 4 + 2);
  g_OLED.printf("%03d", framesPerSecond);

  // Draw a moire pattern like it's 1984

  for (int x = 0; x < g_OLED.getWidth(); x+=4)
    g_OLED.drawLine(x, 0, g_OLED.getWidth() - x, g_OLED.getHeight());

  // Draw a reticle on the right hand side
  const int reticleY = g_OLED.getHeight()  / 2;                          // Vertical center of display
  const int reticleR = g_OLED.getHeight() / 4 - 2;                       // Slightly less than 1/4 screen height
  const int reticleX = g_OLED.getWidth() - reticleR - 8;                 // Right justified plus a margin

  for (int r = reticleR; r > 0; r -= 3)                                   // Draw series of nested circles
    g_OLED.drawCircle(reticleX, reticleY, r);
  
  g_OLED.drawHLine(reticleX - reticleR - 5, reticleY, 2 * reticleR + 10); // Horizontal Line through reticle center
  g_OLED.drawVLine(reticleX, reticleY - reticleR - 5, 2 * reticleR + 10); // Vertical line through reticle center

  g_OLED.sendBuffer();                                                    // Send text to screen
}

void loop() {
  static bool bLED = LOW;
  int fps = 0;

  for (;;)
  {
    bLED = !bLED;                                                         // Toggle the LED state
    digitalWrite(LED_BUILTIN, bLED);

    double dStart = millis() / 1000.0;
    DrawLinesandGraphicsFrame(fps);
    double dEnd = millis() / 1000.0;
    fps = FramesPerSecond(dEnd - dStart);
  }                                                
}