#include <LedControl.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define OLED_SDA 14
#define OLED_SCL 12
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

LedControl lc = LedControl(13, 5, 15, 1);
byte state[8] = {0};

void drawState() {
  for(int i=0; i<8; i++) lc.setRow(0, i, state[i]);
}

void clearState() {
  for(int i=0; i<8; i++) state[i] = 0;
  lc.clearDisplay(0);
}

void showOLED(const char* title, const char* engTitle, const char* desc) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("[ LIVE BROADCAST ]");
  
  display.setTextSize(2);
  display.setCursor(0, 18);
  display.println(title);
  
  display.setTextSize(1);
  display.setCursor(0, 38);
  display.println(engTitle);

  display.setCursor(0, 52);
  display.println(desc);
  display.display();
}

void breathing_bright(int low, int high, int d_time) {
  for (int i = low; i <= high; i++) { lc.setIntensity(0, i); delay(d_time); }
}
void breathing_dim(int high, int low, int d_time) {
  for (int i = high; i >= low; i--) { lc.setIntensity(0, i); delay(d_time); }
}

void anim_InitialBootBreathing() {
  clearState();
  for(int i=0; i<8; i++) state[i] = 0xFF; drawState();
  showOLED("AWAKENING", "System Power On", "Warming up LED Matrix...");
  
  unsigned long t = millis();
  while(millis() - t < 6000) {
    breathing_bright(1, 15, 35);
    breathing_dim(15, 1, 35);
  }
}

// ============== ✨ 첫 시작 정보 시퀀스 & 카운트다운 ✨ ==============
void show_Info_Sequence() {
  clearState(); showOLED("DATE", "Today's Date", "2026. 02. 21");
  lc.setChar(0, 7, ' ', false); lc.setDigit(0, 6, 2, false); lc.setDigit(0, 5, 6, true);
  lc.setDigit(0, 4, 0, false); lc.setDigit(0, 3, 2, true);  lc.setDigit(0, 2, 2, false); lc.setDigit(0, 1, 1, false); lc.setChar(0, 0, ' ', false);
  breathing_bright(1, 15, 25); delay(1000); breathing_dim(15, 1, 15);

  clearState(); showOLED("TIME", "Current Time", "11 : 09 AM");
  lc.setChar(0, 7, ' ', false); lc.setDigit(0, 6, 1, false); lc.setDigit(0, 5, 1, true);
  lc.setDigit(0, 4, 0, false); lc.setDigit(0, 3, 9, true); lc.setDigit(0, 2, 4, false); lc.setDigit(0, 1, 7, false); lc.setChar(0, 0, ' ', false);
  breathing_bright(1, 15, 25); delay(1000); breathing_dim(15, 1, 15);

  clearState(); showOLED("TEMP", "Temperature", "24.5 Celsius");
  lc.setChar(0, 7, ' ', false); lc.setChar(0, 6, ' ', false); lc.setDigit(0, 5, 2, false);
  lc.setDigit(0, 4, 4, true); lc.setDigit(0, 3, 5, false); lc.setRow(0, 2, 0x63); lc.setChar(0, 1, 'C', false); lc.setChar(0, 0, ' ', false);
  breathing_bright(1, 15, 25); delay(1000); breathing_dim(15, 1, 15);
}

void anim_Countdown() {
  clearState();
  for(int i=0; i<8; i++) state[i] = 0xFF; drawState();
  for(int count=8; count>=1; count--) {
    showOLED("COUNTDOWN", "System Booting", String("Time remaining: " + String(count)).c_str());
    if(count < 8) state[7 - count] = 0; 
    drawState();
    breathing_bright(1, 15, 15); breathing_dim(15, 1, 15);   
  }
  clearState();
  showOLED("S T A R T !", "System is Ready", "Launch Sequence...");
  lc.setRow(0, 7, 0x5b); lc.setRow(0, 6, 0x0f); lc.setRow(0, 5, 0x5f); lc.setRow(0, 4, 0x05); lc.setRow(0, 3, 0x0f);
  for(int i=0; i<2; i++) { lc.setIntensity(0, 15); delay(150); lc.setIntensity(0, 1); delay(150); }
  lc.setIntensity(0, 8); delay(1000);
}

// ===========================================
// ✨✨ 30가지 감동의 애니메이션 세트 ✨✨
// ===========================================

void a01_LargeBox() {
  unsigned long t = millis();
  while(millis()-t < 6000) {
    clearState();
    for(int i=7; i>=0; i--) { state[i] |= 0x40; drawState(); delay(30); }
    state[0] |= 0x20; drawState(); delay(30); state[0] |= 0x10; drawState(); delay(30);
    for(int i=0; i<=7; i++) { state[i] |= 0x08; drawState(); delay(30); }
    state[7] |= 0x04; drawState(); delay(30); state[7] |= 0x02; drawState(); delay(30);
    for(int b=0; b<2; b++){ lc.clearDisplay(0); delay(100); drawState(); delay(100); }
  }
}

void a02_DotPingPong() {
  unsigned long t = millis();
  while(millis()-t < 6000) {
    clearState();
    for(int i=7; i>=0; i--) { state[i] = 0x80; drawState(); delay(30); state[i] = 0; }
    for(int i=1; i<=6; i++) { state[i] = 0x80; drawState(); delay(30); state[i] = 0; }
  }
}

void a03_Snake() {
  struct Pos { byte d; byte s; };
  Pos p[20] = {{7,0x40},{6,0x40},{5,0x40},{4,0x40},{3,0x40},{2,0x40},{1,0x40},{0,0x40}, {0,0x20},{0,0x10}, {0,0x08},{1,0x08},{2,0x08},{3,0x08},{4,0x08},{5,0x08},{6,0x08},{7,0x08}, {7,0x04},{7,0x02}};
  unsigned long t = millis();
  while(millis()-t < 6000) {
    for(int i=0; i<20; i++) {
      clearState();
      for(int sl=0; sl<4; sl++) { state[p[(i-sl+20)%20].d] |= p[(i-sl+20)%20].s; }
      drawState(); delay(25);
    }
  }
}

void a04_Scanlines() {
  unsigned long t = millis();
  while(millis()-t < 6000) {
    clearState();
    for(int i=0; i<8; i++) state[i] = 0x40; drawState(); delay(50); 
    for(int i=0; i<8; i++) state[i] = 0x01; drawState(); delay(50); 
    for(int i=0; i<8; i++) state[i] = 0x08; drawState(); delay(50); 
    for(int i=0; i<8; i++) state[i] = 0x01; drawState(); delay(50); 
  }
}

void a05_Elevator() {
  byte door = 0x36; 
  unsigned long t = millis();
  while(millis()-t < 6000) {
      for(int i=0; i<=3; i++) { clearState(); state[3-i]=door; state[4+i]=door; drawState(); delay(100); }
      for(int i=2; i>=0; i--) { clearState(); state[3-i]=door; state[4+i]=door; drawState(); delay(100); }
  }
}

void a06_Wave() {
   byte seq[4] = {0x40, 0x01, 0x08, 0x01}; 
   unsigned long t = millis(); int step=0;
   while(millis()-t < 6000) {
     clearState();
     for(int i=0; i<8; i++) state[7-i] = seq[(step+i)%4];
     drawState(); delay(50); step++;
   }
}

void a07_Stars() {
  byte parts[5] = {0x80, 0x80, 0x01, 0x40, 0x08}; 
  unsigned long t = millis();
  while(millis()-t < 6000) {
      clearState();
      for(int j=0; j<15; j++) { state[random(0,8)] ^= parts[random(0,5)]; drawState(); delay(40); }
  }
}

void a08_BreathingBox() {
   byte outer[8] = {0x78,0x48,0x48,0x48,0x48,0x48,0x48,0x4E};
   byte mid[8]   = {0,0x78,0x48,0x48,0x48,0x48,0x4E,0};      
   byte inner[8] = {0,0,0,0x78,0x4E,0,0,0};                  
   unsigned long t = millis();
   while(millis()-t < 6000) {
     clearState(); for(int i=0; i<8; i++) state[i]=inner[i]; drawState(); delay(150);
     clearState(); for(int i=0; i<8; i++) state[i]=mid[i]; drawState(); delay(150);
     clearState(); for(int i=0; i<8; i++) state[i]=outer[i]; drawState(); delay(150);
     clearState(); for(int i=0; i<8; i++) state[i]=mid[i]; drawState(); delay(150);
   }
}

void a09_RainDrop() {
  unsigned long t = millis();
  while(millis()-t < 6000) {
    for(int i=7; i>=0; i--) {
       clearState(); state[i] = 0x40; drawState(); delay(30);
       state[i] = 0x01; drawState(); delay(30);
       state[i] = 0x08; drawState(); delay(30);
       state[i] = 0x80; drawState(); delay(60); 
    }
  }
}

void a10_Zigzag() {
   unsigned long t = millis();
   while(millis()-t < 6000) {
       clearState();
       for(int i=7; i>=0; i--) { state[i]|=0x40; drawState(); delay(20); state[i]|=0x01; drawState(); delay(20); state[i]|=0x08; drawState(); delay(20); }
       for(int i=0; i<=7; i++) { state[i]&=~0x08; drawState(); delay(20); state[i]&=~0x01; drawState(); delay(20); state[i]&=~0x40; drawState(); delay(20); }
   }
}

void a11_Gears() {
  byte s[6] = {0x40, 0x20, 0x10, 0x08, 0x04, 0x02}; 
  unsigned long t = millis(); int step=0;
  while(millis()-t < 6000) {
      for(int i=0; i<8; i++) state[i] = s[(step+i)%6];
      drawState(); delay(35); step++;
  }
}

void a12_Loading() {
  unsigned long t = millis();
  while(millis()-t < 6000) {
    clearState();
    for(int i=7; i>=0; i--) { state[i] = 0x08; drawState(); delay(30); state[i] |= 0x80; drawState(); delay(60); }
    for(int i=7; i>=0; i--) { state[i] = 0; drawState(); delay(20); }
  }
}

void a13_EKG() {
  unsigned long t = millis(); int pos=7;
  while(millis()-t < 6000) {
    clearState();
    for(int i=0; i<8; i++) state[i] = 0x08; 
    state[pos] = 0x49; if(pos>0) state[pos-1] = 0x01;
    drawState(); delay(80); pos--;
    if(pos < 0) { pos=7; delay(300); }
  }
}

void a14_PacMan() {
  unsigned long t = millis(); int pos=7; bool open=true;
  while(millis()-t < 6000) {
    clearState();
    for(int i=0; i<pos; i++) state[i] = 0x80; 
    state[pos] = open ? 0x4E : 0x4F;
    drawState(); open = !open; delay(150); pos--;
    if(pos < 0) { pos=7; delay(500); }
  }
}

void a15_VolumeEQ() {
  byte lvls[] = {0x00, 0x08, 0x09, 0x49};
  unsigned long t = millis();
  while(millis()-t < 6000) {
    for(int i=0; i<8; i++) state[i] = lvls[random(0,4)];
    drawState(); delay(80);
  }
}

void a16_SlotMachine() {
  unsigned long t = millis(); int locked=8;
  while(millis()-t < 6000) {
    for(int i=0; i<locked; i++) state[i] = random(1,128);
    drawState(); delay(40);
    if(random(0,10)>7 && locked>0) locked--;
    if(locked==0) { delay(800); locked=8; }
  }
}

void a17_Inchworm() {
  unsigned long t = millis(); int pos=7;
  while(millis()-t < 6000) {
    clearState(); state[pos]=0x08; drawState(); delay(150);
    if(pos>0) { state[pos-1]=0x01; drawState(); delay(150); state[pos]=0; state[pos-1]=0x08; drawState(); delay(150); }
    pos--; if(pos < 0) { pos=7; delay(300); }
  }
}

void a18_BlinkEyes() {
  unsigned long t = millis();
  while(millis()-t < 6000) {
    clearState(); state[5]=0x4A; state[2]=0x4A; drawState(); delay(600);
    clearState(); state[5]=0x08; state[2]=0x08; drawState(); delay(120);
    clearState(); state[5]=0x62; state[2]=0x62; drawState(); delay(400);
    clearState(); state[5]=0x4A; state[2]=0x4A; drawState(); delay(600);
    clearState(); state[5]=0x30; state[2]=0x30; drawState(); delay(400);
  }
}

void a19_Laser() {
  unsigned long t = millis();
  while(millis()-t < 6000) {
    clearState(); state[7]=0x01; drawState(); delay(150);
    state[7]=0x41; drawState(); delay(150); state[7]=0x49; drawState(); delay(150);
    for(int i=6; i>=0; i--) state[i]=0x01; drawState(); delay(100);
    for(int i=6; i>=0; i--) state[i]=0; drawState(); delay(100);
    state[7]=0; drawState(); delay(400);
  }
}

void a20_Tetris() {
  unsigned long t = millis(); int h=0;
  while(millis()-t < 6000) {
    if(h>=8) { h=0; delay(500); continue; }
    for(int p=7; p>=h; p--) {
        clearState(); for(int i=0; i<h; i++) state[i]=0xDB;
        state[p]=0xDB; drawState(); delay(40);
    }
    h++;
  }
}

void a21_SpinnerParty() {
  byte s[6] = {0x40,0x20,0x10,0x08,0x04,0x02}; 
  int off[8]; for(int i=0;i<8;i++) off[i]=random(0,6);
  unsigned long t = millis(); int step=0;
  while(millis()-t < 6000) {
    for(int i=0; i<8; i++) state[i] = s[(step+off[i])%6];
    drawState(); delay(35); step++;
  }
}

void a22_Wiper() {
  unsigned long t = millis();
  while(millis()-t < 6000) {
    for(int i=0; i<8; i++) state[i]=random(0,128); drawState(); delay(200);
    for(int i=7; i>=0; i--) { state[i]=0xFF; drawState(); delay(40); state[i]=0; } drawState(); delay(200);
    for(int i=0; i<8; i++) state[i]=random(0,128); drawState(); delay(200);
    for(int i=0; i<8; i++) { state[i]=0xFF; drawState(); delay(40); state[i]=0; }
  }
}

void a23_PulseCenter() {
  unsigned long t = millis();
  while(millis()-t < 6000) {
    clearState(); state[3]=0x08; state[4]=0x08; drawState(); delay(120);
    state[3]=0x49; state[4]=0x49; drawState(); delay(120);
    state[2]=0x08; state[5]=0x08; drawState(); delay(120);
    state[2]=0x49; state[5]=0x49; drawState(); delay(120);
    state[1]=0x08; state[6]=0x08; drawState(); delay(120);
    state[1]=0x49; state[6]=0x49; drawState(); delay(120);
    state[0]=0x08; state[7]=0x08; drawState(); delay(120);
    state[0]=0x49; state[7]=0x49; drawState(); delay(120);
    clearState(); delay(200);
  }
}

void a24_BinaryMatrix() {
  unsigned long t = millis();
  while(millis()-t < 6000) {
    for(int i=0; i<8; i++) state[i] = random(0,2) ? 0x06 : 0x3F; 
    drawState(); delay(120);
  }
}

void a25_Dominoes() {
  unsigned long t = millis();
  while(millis()-t < 6000) {
    clearState(); for(int i=0; i<8; i++) state[i]=0x30; drawState(); delay(400);
    for(int i=7; i>=0; i--) { state[i]=0x08; drawState(); delay(80); }
    delay(500);
  }
}

void a26_Typewriter() {
  unsigned long t = millis();
  byte text[4] = {0x76, 0x79, 0x38, 0x38}; 
  int len=0;
  while(millis()-t < 6000) {
    clearState();
    for(int i=0; i<len; i++) state[7-i] = text[i];
    if(len<4) state[7-len] = (millis()/200)%2==0 ? 0x08 : 0x00;
    drawState();
    if(random(0,10)>7 && len<4) len++;
    if(len==4 && (millis()-t > 5000)) len=0; 
    delay(100);
  }
}

void a27_TrafficLight() {
  unsigned long t = millis();
  while(millis()-t < 6000) {
    clearState(); for(int i=0; i<8; i++) state[i]=0x40; drawState(); delay(800);
    for(int i=0; i<8; i++) state[i]=0x01; drawState(); delay(800);
    for(int i=0; i<8; i++) state[i]=0x08; drawState(); delay(800);
  }
}

void a28_UFO() {
  unsigned long t = millis(); int u=7;
  while(millis()-t < 6000) {
    clearState(); state[u] = 0x63; state[u] |= (millis()/100)%2 ? 0x14 : 0; drawState(); delay(120);
    if(random(0,10)>5) u += random(0,2) ? 1 : -1;
    if(u<1) u=1; if(u>7) u=7;
  }
}

void a29_Fireworks() {
  unsigned long t = millis();
  while(millis()-t < 6000) {
    int loc = random(1,7); clearState();
    state[loc]=0x08; drawState(); delay(150); state[loc]=0x01; drawState(); delay(150); state[loc]=0x40; drawState(); delay(150);
    state[loc]=0xFF; if(loc>0) state[loc-1]=0x55; if(loc<7) state[loc+1]=0x55; drawState(); delay(150);
    state[loc]=0x49; if(loc>0) state[loc-1]=0x00; if(loc<7) state[loc+1]=0x00; drawState(); delay(150);
  }
}

void a30_StrobeFinale() {
  unsigned long t = millis();
  while(millis()-t < 6000) {
    for(int i=0; i<8; i++) state[i]=0xFF; drawState(); delay(30);
    clearState(); delay(60);
  }
}

void a31_BouncingBall() { unsigned long t=millis(); int x=0, d=1; while(millis()-t<6000) { clearState(); state[x]=0x08; drawState(); delay(60); x+=d; if(x>=7||x<=0) d=-d; } }
void a32_ScannerLine() { unsigned long t=millis(); int x=0; while(millis()-t<6000) { clearState(); state[x]=0xFF; drawState(); delay(60); x=(x+1)%8; } }
void a33_Heartbeat() { unsigned long t=millis(); while(millis()-t<6000) { clearState(); state[3]=0x48; state[4]=0x48; drawState(); delay(100); clearState(); drawState(); delay(50); state[3]=0x48; state[4]=0x48; drawState(); delay(100); clearState(); drawState(); delay(600); } }
void a34_Windmill() { byte w[4]={0x42,0x24,0x18,0x81}; unsigned long t=millis(); int s=0; while(millis()-t<6000) { for(int i=0;i<8;i++) state[i]=w[(s+i)%4]; drawState(); delay(80); s++; } }
void a35_RainMatrix() { unsigned long t=millis(); while(millis()-t<6000) { int r=random(0,8); state[r]=(state[r]<<1)|(random(0,2)?1:0); drawState(); delay(40); } }
void a36_VUMeter() { unsigned long t=millis(); while(millis()-t<6000) { int v=random(0,5); clearState(); for(int i=0;i<v;i++){state[3-i]=0x49;state[4+i]=0x49;} drawState(); delay(100); } }
void a37_RandomPixels() { unsigned long t=millis(); while(millis()-t<6000) { state[random(0,8)]^=(1<<random(0,8)); drawState(); delay(20); } }
void a38_ExpandingRings() { unsigned long t=millis(); byte r1=0x00, r2=0x48, r3=0x3F; while(millis()-t<6000) { clearState(); state[3]=r2; state[4]=r2; drawState(); delay(150); state[3]=r3; state[4]=r3; state[2]=r2; state[5]=r2; drawState(); delay(150); state[2]=r3; state[5]=r3; state[1]=r2; state[6]=r2; drawState(); delay(150); } }
void a39_Crossfire() { unsigned long t=millis(); int x=0; while(millis()-t<6000) { clearState(); state[x]|=0x01; state[7-x]|=0x40; drawState(); delay(80); x=(x+1)%8; } }
void a40_Radar() { byte r[4]={0x01,0x02,0x40,0x20}; unsigned long t=millis(); int s=0; while(millis()-t<6000) { clearState(); for(int i=0;i<8;i++) state[i]=r[(s+i)%4]; drawState(); delay(80); s++; } }
void a41_TypingNumbers() { unsigned long t=millis(); int n=0; while(millis()-t<6000) { clearState(); for(int i=0;i<=n;i++) state[7-i]=0x08; drawState(); delay(150); n++; if(n>7){n=0;delay(300);} } }
void a42_MorseCode() { unsigned long t=millis(); while(millis()-t<6000) { clearState(); drawState(); delay(100); for(int i=0;i<8;i++) state[i]=random(0,2)?0x80:0x08; drawState(); delay(200); } }
void a43_Wormhole() { unsigned long t=millis(); while(millis()-t<6000) { clearState(); state[0]=0xFF; state[7]=0xFF; drawState(); delay(100); clearState(); state[1]=0x7E; state[6]=0x7E; drawState(); delay(100); clearState(); state[2]=0x3C; state[5]=0x3C; drawState(); delay(100); clearState(); state[3]=0x18; state[4]=0x18; drawState(); delay(100); } }
void a44_Hypnotic() { unsigned long t=millis(); while(millis()-t<6000) { for(int i=0;i<8;i++) state[i]=0xAA; drawState(); delay(100); for(int i=0;i<8;i++) state[i]=0x55; drawState(); delay(100); } }
void a45_MeteorShower() { unsigned long t=millis(); while(millis()-t<6000) { for(int i=0;i<7;i++) state[i]=state[i+1]; state[7]=random(0,3)==0?0x80:0; drawState(); delay(50); } }
void a46_Pinball() { unsigned long t=millis(); int p=random(0,8); while(millis()-t<6000) { clearState(); state[p]=0x80; drawState(); delay(80); p+=random(0,3)-1; if(p<0)p=0; if(p>7)p=7; } }
void a47_Pendulum() { unsigned long t=millis(); float a=0; while(millis()-t<6000) { clearState(); int p=3.5+3.5*sin(a); state[p]=0x08; drawState(); delay(50); a+=0.2; } }
void a48_BarberPole() { byte p[3]={0x01,0x02,0x04}; unsigned long t=millis(); int s=0; while(millis()-t<6000) { for(int i=0;i<8;i++) state[i]=p[(s+i)%3]; drawState(); delay(80); s++; } }
void a49_Sparkles() { unsigned long t=millis(); while(millis()-t<6000) { int r=random(0,8); clearState(); state[r]=0x08; drawState(); delay(50); state[r]=0xFF; drawState(); delay(50); state[r]=0; drawState(); delay(50); } }
void a50_Glitch() { unsigned long t=millis(); while(millis()-t<6000) { for(int i=0;i<8;i++) state[i]=random(0,256); drawState(); delay(30); if(random(0,10)>7){clearState();delay(100);} } }

// -----------------------------------
// 🔥 배열로 통합 관리!
// -----------------------------------
struct AnimRule {
  const char* title;     // OLED 큰제목
  const char* eng;       // OLED 부제목 (번호포함)
  const char* desc;      // OLED 설명
  void (*func)();        // 실행할 함수
};

AnimRule playlist[50] = {
  {"BIG BOX",   "1. Large Rectangle", "Connecting outer lines", a01_LargeBox},
  {"PING PONG", "2. Bouncing Dot", "Moving left <-> right", a02_DotPingPong},
  {"SNAKE",     "3. Crawling Snake", "Moving along borders", a03_Snake},
  {"SCAN!",     "4. Scanning Lines", "Top to Bottom sweep", a04_Scanlines},
  {"ELEVATOR",  "5. Opening Doors", "Center <-> Outer", a05_Elevator},
  {"WAVE",      "6. Sine Wave Form", "Flowing like ocean", a06_Wave},
  {"GALAXY",    "7. Twinkling Stars", "Random glowing dots...", a07_Stars},
  {"BREATHING", "8. Heart Beat Box", "Expanding & shrinking", a08_BreathingBox},
  {"RAIN DROP", "9. Water Splash", "Falling drops & boing!", a09_RainDrop},
  {"ZIGZAG",    "10. Drawing Stairs", "Up & Down sweeps", a10_Zigzag},
  {"GEARS",     "11. Spinning Chain", "Rotating segments", a11_Gears},
  {"LOADING",   "12. Dot Progress", "Filling up with (.)", a12_Loading},
  {"EKG LINE",  "13. Heart Monitor", "Beep... Beep... Spike!", a13_EKG},
  {"PAC-MAN",   "14. Hungry Ghost", "Eating the dots...", a14_PacMan},
  {"VOLUME EQ", "15. Audio Spectrum", "Bouncing to the beat", a15_VolumeEQ},
  {"CASINO",    "16. Slot Machine", "Spinning the jackpot!", a16_SlotMachine},
  {"INCHWORM",  "17. Crawling Bug", "Squeeze and stretch", a17_Inchworm},
  {"EYES",      "18. Blinking Eyes", "Looking left & right", a18_BlinkEyes},
  {"LASER",     "19. Plasma Cannon", "Charging & firing!", a19_Laser},
  {"TETRIS",    "20. Block Drops", "Stacking up lines", a20_Tetris},
  {"PARTY",     "21. Crazy Spinners", "Everything rotates!", a21_SpinnerParty},
  {"WIPER",     "22. Windshield", "Cleaning the rain", a22_Wiper},
  {"PULSE",     "23. Sonic Boom", "Center expands out", a23_PulseCenter},
  {"MATRIX",    "24. Binary Code", "0 and 1 Data stream", a24_BinaryMatrix},
  {"DOMINOES",  "25. Falling Tiles", "Knocking them down", a25_Dominoes},
  {"TYPING",    "26. Typewriter", "H.E.L.L.O...", a26_Typewriter},
  {"TRAFFIC",   "27. Stop & Go", "Red, Yellow, Green", a27_TrafficLight},
  {"ALIEN UFO", "28. Abduction", "Hovering flying saucer", a28_UFO},
  {"FIREWORK",  "29. Celebration", "Shooting up & BOOM!", a29_Fireworks},
  {"STROBE",    "30. Finale Flash", "Intense blinking!!", a30_StrobeFinale},
  {"BOUNCING",  "31. Bouncing Ball", "Simple ball bounce", a31_BouncingBall},
  {"SCANNER",   "32. Scanner Line", "Left to right scan", a32_ScannerLine},
  {"HEARTBEAT", "33. Heartbeat", "Double pulse", a33_Heartbeat},
  {"WINDMILL",  "34. Windmill", "Spinning blades", a34_Windmill},
  {"MATRIX",    "35. Rain Matrix", "Digital rain", a35_RainMatrix},
  {"VU METER",  "36. Audio EQ", "Jumping bars", a36_VUMeter},
  {"NOISE",     "37. Random Pixels", "Static noise", a37_RandomPixels},
  {"RINGS",     "38. Expanding Rings", "Outward expansion", a38_ExpandingRings},
  {"CROSSFIRE", "39. Crossfire", "Colliding lines", a39_Crossfire},
  {"RADAR",     "40. Radar Sweep", "Rotating beam", a40_Radar},
  {"TYPING",    "41. Type Numbers", "Revealing digits", a41_TypingNumbers},
  {"MORSE",     "42. Morse Code", "Dots and dashes", a42_MorseCode},
  {"WORMHOLE",  "43. Wormhole", "Pulling inwards", a43_Wormhole},
  {"HYPNOTIC",  "44. Hypnotic", "Alternating lines", a44_Hypnotic},
  {"METEOR",    "45. Meteor Shower", "Flying stars", a45_MeteorShower},
  {"PINBALL",   "46. Pinball", "Crazy bouncing", a46_Pinball},
  {"PENDULUM",  "47. Pendulum", "Swinging", a47_Pendulum},
  {"BARBER",    "48. Barber Pole", "Diagonal sweep", a48_BarberPole},
  {"SPARKLES",  "49. Sparkles", "Random pops", a49_Sparkles},
  {"GLITCH",    "50. System Glitch", "Corrupted data", a50_Glitch}
};

void setup() {
  Serial.begin(115200);
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) Serial.println("OLED Init Failed");
  
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  
  anim_InitialBootBreathing(); delay(500);
  show_Info_Sequence(); delay(500);
  anim_Countdown(); delay(1000);
}

void loop() {
  // 50가지 감동적인 애니메이션 연속 재생!
  for(int i=0; i<50; i++) {
    showOLED(playlist[i].title, playlist[i].eng, playlist[i].desc);
    playlist[i].func();
  }
}
