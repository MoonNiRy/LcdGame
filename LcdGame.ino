#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);  

#define xPin 0
#define yPin 1
#define btnJoy 2

#define V2

byte player[] = {
  0x00,
  0x00,
  0x19,
  0x1F,
  0x1F,
  0x19,
  0x00,
  0x00
};
byte enemy[] = {
  0x00,
  0x00,
  0x13,
  0x1F,
  0x1F,
  0x13,
  0x00,
  0x00
};
int config_delay = 300;

void setup()
{                      
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, player);
  lcd.createChar(1, enemy);
}
void loop()
{
  static int select = 0;

  switch(Joy(yPin)) {
    case -1:
      if (select < 1) select++;
      break;
    case 1:
      if (select > 0) select--;
      break;
  }

  switch(select) {
    case 0:
      lcd.home();
      lcd.print("> Game    ");
      lcd.write(0);
      lcd.setCursor(0, 1);
      lcd.print("Settings      ");
      lcd.write(1);
      break;
    case 1:
      lcd.home();
      lcd.print("Game      ");
      lcd.write(0);
      lcd.setCursor(0, 1);
      lcd.print("> Settings    ");
      lcd.write(1);
  }

  if (btn()) {
    switch(select) {
      case 0: 
        game();
        break;
      case 1: 
        settings();
        break;
    }
  }
}

int Joy(char pin) {
  int val = analogRead(pin);
  val = map(val, 0, 1023, -1, 2);

  return val;
}
int btn() {
  static bool flag = false;
  static unsigned long long tmr = 0;
  int value = digitalRead(btnJoy);

  if (value && !flag && millis() - tmr > 100) {
    tmr = millis();
    flag = true;
    return 1;
  } else if (!value && flag && millis() - tmr > 100) {
    tmr = millis();
    flag = false;
  }
  return 0;
}

void clear_ch(char x, char y) {
  lcd.setCursor(x, y);
  lcd.print(' ');
}

void out(char ch, char x, char y) {
  lcd.setCursor(x, y);
  lcd.write(ch);
}

#ifdef V1
int game() {
  lcd.clear();
  unsigned int score = 0;

  char x = 0, y = 0;
  char cars[3][2] = {{5, 0}, {10, 1}, {15, 0}};

  unsigned long long int tmr_col = 0; 
  int delay_col = config_delay;

  bool is_yJoy = false;
  bool is_game = true;
  while (is_game) {
    if (btn()) break;

    int xJoy = Joy(0);
    int yJoy = Joy(1);
    // Работа с игроком
    if (!is_yJoy) { 
      if (yJoy != 0) {
        clear_ch(x, y);

        if (yJoy < 0 && y < 1) y++;
        else if (yJoy > 0 && y > 0) y--;
        
        is_yJoy = true;
      }
    } else if (is_yJoy && yJoy == 0) is_yJoy = false;
    out(0, x, y); // Выводим игрока

    if (xJoy > 0) {
      if (delay_col > 50) delay_col--;
    } else if (xJoy < 0 ) {
      if (delay_col < 1000) delay_col++;
    }
  
    // Работа с препятствиями
    for (int i = 0; i < 3 && millis() - tmr_col > delay_col; i++) { 
      if (cars[i][0] > 0) {
        clear_ch(cars[i][0], cars[i][1]);
        cars[i][0] -= 1;
      } else {
        clear_ch(0, cars[i][1]);
        cars[i][0] = 15;
        cars[i][1] = random(2);

        score++;
      }
      out(1, cars[i][0], cars[i][1]); // Выводим препятствия

      if (i == 2) tmr_col = millis();
    }
    for (int i = 0; i < 3; i++) {
      if (cars[i][0] == x && cars[i][1] == y) is_game = false; // Проверка столкновения
    }
  }
  lcd.clear();
  lcd.print("Score:"); lcd.print(score);
  while (!btn()) {} // Ждём нажатие кнопки

  return 0;
}
#else ifdef V2
int game() {
  lcd.clear();
  unsigned int score = 0;

  char x = 0, y = 0; // Координаты игрока
  char cars[3][2] = {{5, 0}, {10, 1}, {15, 0}}; // Координаты объектов

  unsigned long long int tmr_col = 0; 
  int delay_col = config_delay;

  bool is_Joy = false;
  bool is_game = true;
  while (is_game) {
    if (btn()) break;
    int xJoy = Joy(0);
    int yJoy = Joy(1);

    // Работа с игроком
    if (!is_Joy) { 
      if (xJoy != 0) {
        clear_ch(x, y);
      
        if (xJoy < 0 && x > 0) x--;
        else if (xJoy > 0 && x < 15) x++;
        is_Joy = true;
      }
      if (yJoy != 0) {
        clear_ch(x, y);

        if (yJoy < 0 && y < 1) y++;
        else if (yJoy > 0 && y > 0) y--;
        
        is_Joy = true;
      }
    } else if (is_Joy && xJoy == 0 && yJoy == 0) is_Joy = false;
    out(0, x, y); // Выводим игрока

    // Работа с препятствиями
    for (int i = 0; i < 3 && millis() - tmr_col > delay_col; i++) { 
      if (cars[i][0] > 0) {
        clear_ch(cars[i][0], cars[i][1]);
        cars[i][0] -= 1;
      } else {
        clear_ch(0, cars[i][1]);
        cars[i][0] = 15;
        cars[i][1] = random(2);

        score++;
      }
      out(1, cars[i][0], cars[i][1]); // Выводим препятствия

      if (i == 2) tmr_col = millis();
    }

    // Проверка столкновения
    for (int i = 0; i < 3; i++) {  
      if (cars[i][0] == x && cars[i][1] == y) is_game = false; 
    }
  }
  lcd.clear();
  lcd.print("Score:"); lcd.print(score);
  while (!btn()) {} // Ждём нажатие кнопки
  lcd.clear();
  return 0;
}
#endif

int settings() {
  lcd.clear();
  while (1) {
    if (btn()) break;
    lcd.home();
    lcd.write(127);
    lcd.write(126);

    lcd.setCursor(0, 1);
    lcd.print("Delay:"); 
    lcd.setCursor(6, 1);
    lcd.print(config_delay); lcd.print("       ");

    switch(Joy(0)) {
      case -1: 
        if (config_delay > 100) config_delay -= 25;
        break;
      case 1: 
        if (config_delay < 1000) config_delay += 25;
        break;
    }
  }
  lcd.clear();
  return 0;
}