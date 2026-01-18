// базовый пример работы с лентой, основные возможности
// библиотека microLED версии 3.0+
// для более подробной информации читай документацию
// константы для удобства
#define STRIP_PIN 9     // пин ленты
#define NUMLEDS 74     // кол-во светодиодов
// ===== ЦВЕТОВАЯ ГЛУБИНА =====
// 1, 2, 3 (байт на цвет)
// на меньшем цветовом разрешении скетч будет занимать в разы меньше места,
// но уменьшится и количество оттенков и уровней яркости!
// дефайн делается ДО ПОДКЛЮЧЕНИЯ БИБЛИОТЕКИ
// без него будет 3 байта по умолчанию
#define COLOR_DEBTH 1
#include <microLED.h>   // подключаем библу
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
// ======= ИНИЦИАЛИЗАЦИЯ =======
// <колво-ледов, пин, клок пин, чип, порядок>
// microLED<NUMLEDS, DATA_PIN, CLOCK_PIN, LED_WS2818, ORDER_GRB> strip;
// CLOCK пин нужен только для SPI лент (например APA102)
// для обычных WS лент указываем MLED_NO_CLOCK
// по APA102 смотри отдельный гайд в примерах
// различные китайские подделки могут иметь совместимость
// с одним чипом, но другой порядок цветов!
// поддерживаемые чипы лент и их официальный порядок цветов:
// microLED<NUMLEDS, STRIP_PIN, MLED_NO_CLOCK, LED_WS2811, ORDER_GBR> strip;
// microLED<NUMLEDS, STRIP_PIN, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB> strip;
// microLED<NUMLEDS, STRIP_PIN, MLED_NO_CLOCK, LED_WS2813, ORDER_GRB> strip;
// microLED<NUMLEDS, STRIP_PIN, MLED_NO_CLOCK, LED_WS2815, ORDER_GRB> strip;
// microLED<NUMLEDS, STRIP_PIN, MLED_NO_CLOCK, LED_WS2818, ORDER_RGB> strip;
// microLED<NUMLEDS, STRIP_PIN, MLED_NO_CLOCK, LED_WS6812, ORDER_RGB> strip;
// microLED<NUMLEDS, STRIP_PIN, CLOCK_PIN, LED_APA102, ORDER_BGR> strip;
// microLED<NUMLEDS, MLED_NO_CLOCK, MLED_NO_CLOCK, LED_APA102_SPI, ORDER_BGR> strip;  // для аппаратного SPI
// ======= ПРЕРЫВАНИЯ =======
// для повышения надёжности передачи данных на ленту можно отключать прерывания.
// В библиотеке есть 4 режима:
// CLI_OFF - прерывания не отключаются (возможны сбои в работе ленты)
// CLI_LOW - прерывания отключаются на время передачи одного цвета
// CLI_AVER - прерывания отключаются на время передачи одного светодиода (3 цвета)
// CLI_HIGH - прерывания отключаются на время передачи даных на всю ленту
// По умолчанию отключение прерываний стоит на CLI_OFF (не отключаются)
// Параметр передаётся 5ым при инициализации:
// microLED<NUMLEDS, STRIP_PIN, LED_WS2818, ORDER_GRB, CLI_AVER> strip;
// ======= СПАСИТЕ МИЛЛИС =======
// При отключении прерываний в режиме среднего и высокого проритета (CLI_AVER и CLI_HIGH)
// неизбежно будут чуть отставать функции времени millis() и micros()
// В библиотеке встроено обслуживание функций времени, для активации передаём SAVE_MILLIS
// 6-ым аргументом при инициализации:
// microLED<NUMLEDS, STRIP_PIN, MLED_NO_CLOCK, LED_WS2818, ORDER_GRB, CLI_AVER, SAVE_MILLIS> strip;
// это НЕЗНАЧИТЕЛЬНО замедлит вывод на ленту, но позволит миллису считать без отставания!
// инициализирую ленту (выше был гайд!)
microLED<NUMLEDS, STRIP_PIN, MLED_NO_CLOCK, LED_WS2812, ORDER_GRB, CLI_AVER> strip;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
// Параметры: ширина, высота, ссылка на интерфейс (I2C), номер пина сброса (-1 если нет)
const uint8_t right_butt_pin = 4;
const uint8_t Left_butt_pin = 3;
const uint8_t Select_butt_pin = 2;

// ТЕКСТОВЫЕ МАССИВЫ
const char* MAIN_mode[] = {"RGB settings", "RGB modes", "About"};
const char* RGB_mode[] = {"Breath mode", "Snake mode","Return to menu"};//"Rainbow","Arrow mode",
const char* RGB_setting_mode[] = {"Brightness", "Colour", "Return to menu"};
const char* Colors[] = {"Red", "Green", "Blue", "Purple", "Aqua", "Yellow","Pink", "Orange","Silver","Return"};// GOLD - #FFD700 // light Aqua #80ffe6
const char* main_menu_text = "RGB main menu";

// СОСТОЯНИЯ
int8_t MAIN_mode_selected = 0;
const uint8_t MAIN_selected_variant[] = {25, 40, 55};
int8_t RGB_mode_selected = 0;
int8_t Setting_mode_selected = 0;
int Brightness_RGB = 0;
int8_t Color_selected_variant = 0;
bool Jump_mode_act = 0;
bool Snake_mode_act = 0;
bool Br_up_dw = 1;

bool Main_act = true, RGB_setting_mode_act = false, About_mode_act = false , Color_act = false;
bool RGB_modes_act = false, Brightness_act = false;

int8_t direction = 1; // 1 - вверх, 0 - вниз
uint8_t snake_for = 0;

// КНОПКИ
bool left_last_raw_butt = 0, right_last_raw_butt = 0, select_last_raw_butt = 0;
bool right_stable_butt = 0, left_stable_butt = 0, Select_stable_butt = 0;
bool right_flag = 0, left_flag = 0, select_flag = 0;
bool settings_select_flag = 0, About_select_flag = 0, Brightness_select_flag = 0 , Color_select_flag = 0;
bool right_RGB_select_flag = 0, left_RGB_select_flag = 0, RGB_modes_select_flag = 0,Jump_mode_select_flag = 0, Snake_mode_select_flag = 0;
bool R_Brightness_flag = 0, L_Brightness_flag = 0;
bool R_Color_flag = 0, L_Color_flag = 0;
bool S_Snake_flag = 0;
unsigned long right_delay_flag = 0, left_delay_flag = 0, select_delay_flag = 0;
const uint8_t debounce_delay = 50;

uint8_t scroll_Y_pos_BRIGHTNESS = 5;

// uint8_t scroll_Y_pos_RED = 5;
// uint8_t scroll_Y_pos_GREEN = 5;
// uint8_t scroll_Y_pos_BLUE = 5;
// int8_t RGB_scroll_sellected = 0;
uint8_t Brightness = 0;
const uint8_t X_middle = 64;
//---------ФУНКЦИИ
void Draw_scroll_pos(uint8_t y) {
  display.drawLine(120, 4, 120, 60, WHITE);
  display.fillRoundRect(118, y, 5, 10, 2, WHITE);
  display.display();
}
// void Draw_scroll_pos_Snake_R(uint8_t y) {
//   display.drawLine(80, 4, 80, 60, WHITE);
//   display.fillRoundRect(78, y, 5, 10, 2, WHITE);
//   display.display();
// }
// void Draw_scroll_pos_Snake_G(uint8_t y) {
//   display.drawLine(100, 4, 100, 60, WHITE);
//   display.fillRoundRect(98, y, 5, 10, 2, WHITE);
//   display.display();
// }
// void Draw_scroll_pos_Snake_B(uint8_t y) {
//   display.drawLine(120, 4, 120, 60, WHITE);
//   display.fillRoundRect(118, y, 5, 10, 2, WHITE);
//   display.display();
// }
void Snake_mode_selected(){
  strip.clear();
  strip.setBrightness(255);
  strip.show();
  delay(20);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("Snake mode active"));
  display.setCursor(0, 49);
  display.print(F("Select to exit"));
  display.display();
}

void drawMainMenu() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(X_middle - (strlen(main_menu_text) * 3), 0);
  display.print(main_menu_text);
  for (uint8_t i = 0; i < 3; i++) {
    display.setCursor(X_middle - (strlen(MAIN_mode[i]) * 3), 25 + (i * 15));
    display.print(MAIN_mode[i]);
  }
  display.setCursor(110, MAIN_selected_variant[MAIN_mode_selected]);
  display.print(F("<-"));
  display.display();
}
// ---RGB settings functions
void RGB_setting_mode_update() {
  display.fillRect(20, 32, 88, 10, BLACK);
  uint8_t w = strlen(RGB_setting_mode[Setting_mode_selected]) * 6;
  display.setCursor(X_middle - (w / 2), 32);
  display.setTextColor(BLACK, WHITE);
  display.print(RGB_setting_mode[Setting_mode_selected]);
  display.display();
}

void RGB_settings_selected() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(10, 32);  display.print(F("<"));
  display.setCursor(118, 32); display.print(F(">"));
  RGB_setting_mode_update();
}
// ---RGB modes functions
void RGB_modes_main_update() {
  display.fillRect(20, 32, 88, 10, BLACK);
  uint8_t w = strlen(RGB_mode[RGB_mode_selected]) * 6;
  display.setCursor(X_middle - (w / 2), 32);
  display.setTextColor(BLACK, WHITE);
  display.print(RGB_mode[RGB_mode_selected]);
  display.display();
}

void RGB_modes_main_selected() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(10, 32);  display.print(F("<"));
  display.setCursor(118, 32); display.print(F(">"));
  RGB_modes_main_update();
}
// ---About functions
void About_selected() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(F("This is a test"));
  display.println(F("Made by Elijah"));
  display.println(F("Started: 21.12.25"));
  display.println(F("R butt: Select/Exit"));
  display.println(F("L butt: Left/Change"));
  display.println(F("S butt: Right/Change"));
  display.setCursor(0, 55);
  display.print(F("Select to exit"));
  display.display();
}
// ---Brightness functions
void Brightness_selected() {
  display.clearDisplay();
  Draw_scroll_pos(scroll_Y_pos_BRIGHTNESS);

  Brightness = map(scroll_Y_pos_BRIGHTNESS, 5, 50, 0, 100);
  Brightness_RGB = map(Brightness, 0, 100, 0, 255);
  strip.setBrightness(Brightness_RGB);
  strip.show();

  display.setTextColor(WHITE);
  display.setCursor(0, 0);    display.print(F("Select to exit"));
  display.setCursor(0, 32);   display.print(F("Brightness: "));
  display.setCursor(75, 32);  display.print(Brightness);
  display.print(F("%"));
  display.display();
}
//----Сolor functions
void Color_mode_update() {
  display.fillRect(20, 32, 88, 10, BLACK);
  uint8_t w = strlen(Colors[Color_selected_variant]) * 6;
  display.setCursor(X_middle - (w / 2), 32);
  display.setTextColor(BLACK, WHITE);
  display.print(Colors[Color_selected_variant]);
  display.display();
}
void Color_selected() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(10, 0);  display.print(F("COLOR"));
  display.setCursor(10, 32);  display.print(F("<"));
  display.setCursor(118, 32); display.print(F(">"));
  display.display();
  Color_mode_update();
}
void Breath_mode_selected(){
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(F("Breath mode active"));
  display.setCursor(0, 55);
  display.print(F("Select to Stop & Exit"));
  display.display();
}

void setup() {
  pinMode(right_butt_pin,INPUT_PULLUP);
  pinMode(Left_butt_pin,INPUT_PULLUP);
  pinMode(Select_butt_pin,INPUT_PULLUP);
  // ===================== БАЗОВЫЕ ШТУКИ =====================
  // яркость (0-255)
  strip.setBrightness(Brightness_RGB);
  strip.clear();
  // применяется при выводе .show() !
  strip.show(); // вывод изменений на ленту
  delay(40);     // между вызовами show должна быть пауза минимум 40 мкс !!!!
  
  strip.fill(mRed);  // заливаем весь массив красным
  strip.show();

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Ошибка! Не найден дисплей SSD1306!"));
  for(;;); // Бесконечный цикл (стоп)
  }
  Serial.println(F("Дисплей инициализирован!"));
  // Очищаем буфер дисплея (заливаем всё чёрным)
  drawMainMenu();
}
void loop() {
  bool r_raw = !digitalRead(right_butt_pin);
  bool l_raw = !digitalRead(Left_butt_pin);
  bool s_raw = !digitalRead(Select_butt_pin);
  unsigned long now = millis();

  if (r_raw != right_last_raw_butt) {right_delay_flag = now;}
  if (now - right_delay_flag > debounce_delay){
    if (r_raw != right_stable_butt){right_stable_butt = r_raw;}} 
  right_last_raw_butt = r_raw;

  if (l_raw != left_last_raw_butt) {left_delay_flag = now;}
  if (now - left_delay_flag > debounce_delay){ 
    if (l_raw != left_stable_butt){left_stable_butt = l_raw;}}
  left_last_raw_butt = l_raw;

  if (s_raw != select_last_raw_butt) {select_delay_flag = now;}
  if (now - select_delay_flag > debounce_delay) {
    if (s_raw != Select_stable_butt){Select_stable_butt = s_raw;}}
  select_last_raw_butt = s_raw;
  
   if (Main_act) {
    if (right_stable_butt == 1 && right_flag == 0) {
      right_flag = 1;
      display.fillRect(110, MAIN_selected_variant[MAIN_mode_selected], 15, 10, BLACK);
      MAIN_mode_selected++; if (MAIN_mode_selected > 2) MAIN_mode_selected = 0;
      display.setCursor(110, MAIN_selected_variant[MAIN_mode_selected]);
      display.setTextSize(1);display.setTextColor(WHITE); display.print(F("<-")); display.display();
    }
    if (right_stable_butt == 0 && right_flag == 1) right_flag = 0;
  
     if (left_stable_butt == 1 && left_flag == 0) {
      left_flag = 1;
      display.fillRect(110, MAIN_selected_variant[MAIN_mode_selected], 15, 10, BLACK);
      MAIN_mode_selected--; if (MAIN_mode_selected < 0) MAIN_mode_selected = 2;
      display.setCursor(110, MAIN_selected_variant[MAIN_mode_selected]);
      display.setTextColor(WHITE); display.print(F("<-")); display.display();
    }
    if (left_stable_butt == 0 && left_flag == 1) left_flag = 0;

    if (Select_stable_butt == 1 && select_flag == 0) {
      select_flag = 1; 
    }
      if (Select_stable_butt == 0 && select_flag == 1) {
      select_flag = 0;
      if (MAIN_mode_selected == 0) { Main_act = 0; RGB_setting_mode_act = 1; RGB_settings_selected(); }
        else if (MAIN_mode_selected == 1) { Main_act = 0; RGB_modes_act = 1; RGB_modes_main_selected();}
        else if (MAIN_mode_selected == 2) { Main_act = 0; About_mode_act = 1; About_selected(); }
    }
  }
  // -------------------------------------------
  // 2. RGB SETTINGS
  // -------------------------------------------

  if (RGB_setting_mode_act) {
    //L&R
    if (right_stable_butt == 1 && right_flag == 0) { right_flag = 1; Setting_mode_selected++; if (Setting_mode_selected > 2) Setting_mode_selected = 0; RGB_setting_mode_update(); }
    if (left_stable_butt && !left_flag) { left_flag = 1; Setting_mode_selected--; if (Setting_mode_selected < 0) Setting_mode_selected = 2; RGB_setting_mode_update(); }
    
    if (!right_stable_butt) right_flag = 0; if (!left_stable_butt) left_flag = 0;
    //S
    if (Select_stable_butt == 1 && settings_select_flag == 0) {
      settings_select_flag = 1;
    }
    if (Select_stable_butt == 0 && settings_select_flag == 1){
      settings_select_flag = 0;
      if (Setting_mode_selected == 0) { RGB_setting_mode_act = 0; Brightness_act = 1; Brightness_selected();}
      if (Setting_mode_selected == 1) { RGB_setting_mode_act = 0; Color_act = 1; Color_selected();}
      if (Setting_mode_selected == 2) { RGB_setting_mode_act = 0; Main_act = 1; drawMainMenu();}
    }
  }
    // 3. BRIGHTNESS
  if (Brightness_act) {
    if (left_stable_butt && !L_Brightness_flag) { L_Brightness_flag = 1; if (scroll_Y_pos_BRIGHTNESS > 5) scroll_Y_pos_BRIGHTNESS -= 5; Brightness_selected(); }
    if (right_stable_butt && !R_Brightness_flag) { R_Brightness_flag = 1; if (scroll_Y_pos_BRIGHTNESS < 50) scroll_Y_pos_BRIGHTNESS += 5; Brightness_selected(); }
    if (!left_stable_butt) L_Brightness_flag = 0; if (!right_stable_butt) R_Brightness_flag = 0;
    
    if (Select_stable_butt == 1 && Brightness_select_flag == 0) { 
      Brightness_select_flag = 1; 
    }
    if (Select_stable_butt == 0 && Brightness_select_flag == 1){ 
      Brightness_select_flag = 0;
      Brightness_act = 0; RGB_setting_mode_act = 1; RGB_settings_selected(); 
    }
  }
    // 3. COLOR
  if (Color_act) {
    if (left_stable_butt && !L_Color_flag) {
    L_Color_flag = 1;
    Color_selected_variant--;
    if (Color_selected_variant < 0) Color_selected_variant = 9; Color_mode_update(); }
    if (right_stable_butt && !R_Color_flag) {
    R_Color_flag = 1;
    Color_selected_variant++;
    if (Color_selected_variant > 9) Color_selected_variant = 0; Color_mode_update(); }
    if (!left_stable_butt) L_Color_flag = 0; if (!right_stable_butt) R_Color_flag = 0;
    
    // Применение цвета
    if (Select_stable_butt == 1 && Color_select_flag == 0) {
      Color_select_flag = 1;
    }
    if (Select_stable_butt == 0 && Color_select_flag == 1){ 
      Color_select_flag = 0;
      if (Color_selected_variant == 0) {strip.fill(mRed);strip.show();Color_act = 0; RGB_setting_mode_act = 1; RGB_settings_selected();}
    else if (Color_selected_variant == 1) {strip.fill(mGreen);strip.show();Color_act = 0; RGB_setting_mode_act = 1; RGB_settings_selected();}
    else if (Color_selected_variant == 2) {strip.fill(mBlue);strip.show();Color_act = 0; RGB_setting_mode_act = 1; RGB_settings_selected();}
    else if (Color_selected_variant == 3) {strip.fill(mPurple);strip.show();Color_act = 0; RGB_setting_mode_act = 1; RGB_settings_selected();}
    else if (Color_selected_variant == 4) {strip.fill(mAqua);strip.show();Color_act = 0; RGB_setting_mode_act = 1; RGB_settings_selected();}
    // else if (Color_selected_variant == 5) {strip.fill(mWhite);strip.show();Color_act = 0; RGB_setting_mode_act = 1; RGB_settings_selected();}
    else if (Color_selected_variant == 5) {strip.fill(mYellow);strip.show();Color_act = 0; RGB_setting_mode_act = 1; RGB_settings_selected();}
    else if (Color_selected_variant == 6) {for (uint8_t i = 0;i < NUMLEDS;i++){strip.set(i,mRGB(255,192,203));};strip.show();Color_act = 0; RGB_setting_mode_act = 1; RGB_settings_selected();}
    else if (Color_selected_variant == 7) {strip.fill(mOrange);strip.show();Color_act = 0; RGB_setting_mode_act = 1; RGB_settings_selected();}
    else if (Color_selected_variant == 8) {strip.fill(mSilver);Color_act = 0; RGB_setting_mode_act = 1; RGB_settings_selected();} // Gold
    else if (Color_selected_variant == 9) Color_act = 0; RGB_setting_mode_act = 1; RGB_settings_selected();
    }
    
  }
  
    // 4. RGB MODES
  if (RGB_modes_act) {
    if (right_stable_butt && !right_RGB_select_flag) { right_RGB_select_flag = 1; RGB_mode_selected++; if (RGB_mode_selected > 2) RGB_mode_selected = 0; RGB_modes_main_update(); }
    if (left_stable_butt && !left_RGB_select_flag) { left_RGB_select_flag = 1; RGB_mode_selected--; if (RGB_mode_selected < 0) RGB_mode_selected = 2; RGB_modes_main_update(); }
    if (!right_stable_butt) right_RGB_select_flag = 0; if (!left_stable_butt) left_RGB_select_flag = 0;
    
    if (Select_stable_butt == 1 && RGB_modes_select_flag == 0) {
      RGB_modes_select_flag = 1;
    }
    if (Select_stable_butt == 0 && RGB_modes_select_flag == 1){ 
      RGB_modes_select_flag = 0;
      if (RGB_mode_selected == 2) { RGB_modes_act = 0; Main_act = 1; drawMainMenu();}
      else if (RGB_mode_selected == 0){
        RGB_modes_act = 0;
        Jump_mode_act = 1;
        Breath_mode_selected();
      }
      else if (RGB_mode_selected == 1){
        RGB_modes_act = 0;
        Snake_mode_act = 1;
        Snake_mode_selected();
      }
    }
  }
  //--------------SNAKE MODE-----------
  if (Snake_mode_act){
    if (Select_stable_butt == 1 && S_Snake_flag == 0) { S_Snake_flag = 1;}
    if (Select_stable_butt == 0 && S_Snake_flag == 1) { S_Snake_flag = 0;
      Snake_mode_act = 0; Main_act = 1;snake_for = 0; drawMainMenu();strip.clear();delay(40);strip.fill(mRed); strip.setBrightness(255); strip.show();return;   
    }
    snake_for++;
    if (snake_for == NUMLEDS){snake_for = 0;}
    strip.set(snake_for, mRGB(0,0,0)); 
    strip.set(snake_for + 1, mRGB(255,255,255));
    strip.show();
    delay(40);
  }
  //  BREATH MODE------------
  if (Jump_mode_act){
    if (Select_stable_butt == 1 && Jump_mode_select_flag == 0) {
      Jump_mode_select_flag = 1;
    }
    if (Select_stable_butt == 0 && Jump_mode_select_flag == 1){ 
      Jump_mode_select_flag = 0;
      Jump_mode_act = 0; Main_act = 1;delay(100); drawMainMenu();strip.setBrightness(255); strip.show();
    }
   // 1. МЕНЯЕМ ЯРКОСТЬ
 Brightness_RGB = Brightness_RGB + (direction * 2);
  // 2. ПРОВЕРЯЕМ ГРАНИЦЫ
  if (Brightness_RGB >= 255) {
    Brightness_RGB = 255;
    direction = -1;      // меняем направление на уменьшение
  }
  if (Brightness_RGB <= 0) {
    Brightness_RGB = 0;
    direction = 1;       // меняем направление на увеличение
  }
  strip.setBrightness(Brightness_RGB); 
  strip.show();
  delay(10); // Задержка для видимости эффекта
  }
      // 5. ABOUT EXIT
  if (About_mode_act) {
    if (Select_stable_butt  == 1 && About_select_flag  == 0) {
      About_select_flag = 1; 
    }
    if (Select_stable_butt == 0 && About_select_flag == 1){ 
      About_select_flag = 0;
      About_mode_act = 0; Main_act = 1; drawMainMenu();  
    }
  }
}

