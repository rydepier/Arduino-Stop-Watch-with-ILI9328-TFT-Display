/***************************************************************
 * Stop Clock using Touch Screen TTF Display
 * by Chris Rouse
 * Dec 2015
 * 
 * Uses Adafruit ttf libraries
***************************************************************/
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <SD.h>
#include <SPI.h>
#include <TouchScreen.h>
//
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
// Assign  names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
// touchscreen stuff
#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin
//
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940
//
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
//
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4
//
#define PENRADIUS 3
// end touchscreen stuff
// Set the chip select line to whatever you use (10 doesnt conflict with the library)
// In the SD card, place 24 bit color BMP files)
#define SD_CS 10     
//
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, A4);
// Store all the filenames for the graphics
char * counter[60] = {"0.bmp","1.bmp","2.bmp","3.bmp","4.bmp","5.bmp","6.bmp","7.bmp","8.bmp","9.bmp",
"10.bmp","11.bmp","12.bmp","13.bmp","14.bmp","15.bmp","16.bmp","17.bmp","18.bmp","19.bmp",
"20.bmp","21.bmp","22.bmp","23.bmp","24.bmp","25.bmp","26.bmp","27.bmp","28.bmp","29.bmp",
"30.bmp","31.bmp","32.bmp","33.bmp","34.bmp","35.bmp","36.bmp","37.bmp","38.bmp","39.bmp",
"40.bmp","41.bmp","42.bmp","43.bmp","44.bmp","45.bmp","46.bmp","47.bmp","48.bmp","49.bmp",
"50.bmp","51.bmp","52.bmp","53.bmp","54.bmp","55.bmp","56.bmp","57.bmp","58.bmp","59.bmp",
};
// General Variables
unsigned long previousMillis = 0; // will store last time LED was updated
int secs = 0;
int mins = 0;
int hrs = 0;
const long interval = 1000; // 1 second
boolean runClock = false;
boolean reset = false;
//

void setup()
{
  Serial.begin(9600);
  tft.reset();
  uint16_t identifier = tft.readID();
  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    return;
  }
  tft.begin(identifier);
//
// Now initialise the SD Card
  Serial.print(F("Initializing SD card..."));
  if (!SD.begin(SD_CS)) {
    Serial.println(F("failed!"));
    return;
  }
  Serial.println(F("OK!"));
// 
  tft.setRotation(0); // puts the display in portrait rotation
// load the background graphics  
  bmpDraw("stopcl.bmp", 0, 0); // draw the base graphic
  bmpDraw("colon.bmp", 90, 143);
  bmpDraw("colon.bmp", 143, 143);  
  bmpDraw(counter[secs], 150, 136);
  bmpDraw(counter[mins], 97,136); 
  bmpDraw(counter[hrs], 45,136);  
//
  pinMode(13, OUTPUT); // onboar LED lights when screen touched
  //
  tft.setTextSize(2);
}

#define MINPRESSURE 10  // minimum pressure on screen to register
#define MAXPRESSURE 1000


void loop()
{
  if(runClock){ // only update if START pressed
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis; 
      bmpDraw(counter[secs], 150, 136);
      bmpDraw(counter[mins], 97,136); 
      bmpDraw(counter[hrs], 45,136);       
      secs++; // increment counter
      if(secs>59){
        secs = 0;
        mins++;  // update minutes
        if(mins>59){
          mins = 0;
          hrs++;  // update hours
          if(hrs>23){
            hrs = 0;
          }
        }
      }
    }
  } 
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // un REM next 2 lines when trying to fin coordinates for a switch etc
    //Serial.print("X = "); Serial.print(p.x);
    //Serial.print("\tY = "); Serial.println(p.y);
    // Checkto see if START pressed
    if ((p.x > 690 && p.x <830) && (p.y > 360 && p.y <450)) {
     runClock = true; // start the clock
    }
    // Chek to see if STOP pressed
    if ((p.x > 500 && p.x <620) && (p.y > 360 && p.y <450)) {
      runClock = false; // stop the clock
      secs =0; // rest all the counters
      mins = 0;
      hrs = 0;
    }
    // Check to see if RESET pressed
    if ((p.x > 327 && p.x <447) && (p.y > 360 && p.y <450)) {
      runClock = false; // stop the clock
      secs =0;
      mins = 0;
      hrs = 0;
      bmpDraw(counter[secs], 150, 136); // write all zeros to screen
      bmpDraw(counter[mins], 97,136); 
      bmpDraw(counter[hrs], 45,136);   
    }
  }
}

/**************************************************************/
// This function opens a Windows Bitmap (BMP) file 
// this is the Adafruit code
#define BUFFPIXEL 20

void bmpDraw(char *filename, int x, int y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
  uint16_t lcdbuffer[BUFFPIXEL];  // pixel out buffer (16-bit per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
  uint8_t  lcdidx = 0;
  boolean  first = true;

  if((x >= tft.width()) || (y >= tft.height())) return;
  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.println(F("File not found"));
    return;
  }
  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.println(F("File size: ")); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print(F("Header size: ")); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed
        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print(F("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);
        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;
        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }
        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;
        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+w-1, y+h-1);
        for (row=0; row<h; row++) { // For each scanline...
          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }
          for (col=0; col<w; col++) { // For each column...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              // Push LCD buffer to the display first
              if(lcdidx > 0) {
                tft.pushColors(lcdbuffer, lcdidx, first);
                lcdidx = 0;
                first  = false;
              }
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }
            // Convert pixel from BMP to TFT format
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            lcdbuffer[lcdidx++] = tft.color565(r,g,b);
          } // end pixel
        } // end scanline
        // Write any remaining data to LCD
        if(lcdidx > 0) {
          tft.pushColors(lcdbuffer, lcdidx, first);
        } 
        Serial.print(F("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }
  bmpFile.close();
  if(!goodBmp) Serial.println(F("BMP format not recognized."));
}

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
/*****************************************************************/

