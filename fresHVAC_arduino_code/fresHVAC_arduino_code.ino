#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Servo.h>




/*
EEPROM mem map
0
1
2
200

*/
//LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
// initialize lcd pins (rs,en,d4,d5,d6,d7)
LiquidCrystal lcd(A4, A5, A0, A1, A2, A3);
Servo ser;
Servo ser2;
const int DS1307 = 0x68; // Address of DS1307 see data sheets

byte _second = 0;
byte _minute = 0;
byte _hour = 0;
byte r_hour = 0;
byte weekday = 0;
byte monthday = 0;
//byte month = 0;
//byte year = 0;
byte web_pin[4];
/* byte onTime_hour=0;
byte offTime_hour=0;
byte onTime_mint=0;
byte offTime_mint=0;
unsigned int onTime_min = 0;
unsigned int offTime_min = 0;
*/

byte on_hour;
byte on_min;
byte off_hour;
byte off_min;

unsigned int Time_check = 0;

byte s_hour = 0;
byte s_mint = 0;
byte old_hour = 99;
byte old_min = 99;

byte mode = 0;
byte mode_s;
byte sch = 0;
unsigned int s_timer1 = 1;
unsigned int s_timer2 = 1;
unsigned int set_onTimer = 1;
unsigned int set_offTimer = 1;
//unsigned long _onTimer;
//unsigned long _offTimer;
//unsigned long prev_t=0;
unsigned int _onTimer;
unsigned int _offTimer;
unsigned int prev_t = 0;
byte _secTimer = 59;

//byte mode_on=0;
byte vent_pos = 12;
byte vent_cnt = 0;
//int serpos2;
//int serpos;
//int pos;
//int pos2;
unsigned int fspeed=0;
byte fpwmlcd = 35;
byte fpwm;
//volatile int fan_in = LOW;

//RTC_Millis RTC;


const int fan_pwm_out = 14;
const int fan_speed_in = 15;
/*
+CWLAP : 0 = OPEN
         1 = WEP
+CWLAP : 2 = WPA_PSK
+CWLAP : 3 = WPA2_PSK
+CWLAP : 4 = WPA/WPA2
*/
//keypad code
byte h = 0, v = 0; //variables used in for loops
const unsigned long period = 50; //little period used to prevent error
unsigned long kdelay = 0;      // variable used in non-blocking delay
const byte rows = 3;           //number of rows of keypad
const byte columns = 3;          //number of columnss of keypad
//const byte Output[rows]={43,45,47}; //array of pins used as output for rows of keypad
//const byte Input[columns]={49,51,53}; //array of pins used as input for columnss of keypad
const byte Output[rows] = {18, 19, 20}; //array of pins used as output for rows of keypad
const byte Input[columns] = {21, 22, 23}; //array of pins used as input for columnss of keypad

// these variables are used to measure the RPM of Fan by counting the input pin for specific time, because its not connected to INT pin
unsigned long previousMillis = 0; //
const long interval = 500;           // interval at which to get vals (milliseconds)
unsigned long currentMillis =0;

//const String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>";

byte keypad() // function used to detect which button is used
{
  static bool no_press_flag = 0; //static flag used to ensure no button is pressed
  for (byte x = 0; x < columns; x++) // for loop used to read all inputs of keypad to ensure no button is pressed
  {
    if (digitalRead(Input[x]) == HIGH); //read evry input if high continue else break;
    else
      break;
    if (x == (columns - 1)) //if no button is pressed
    {
      no_press_flag = 1;
      h = 0;
      v = 0;
    }
  }
  if (no_press_flag == 1) //if no button is pressed
  {
    for (byte r = 0; r < rows; r++) //for loop used to make all output as low
      digitalWrite(Output[r], LOW);
    for (h = 0; h < columns; h++) // for loop to check if one of inputs is low
    {
      if (digitalRead(Input[h]) == HIGH) //if specific input is remain high (no press on it) continue
        continue;
      else    //if one of inputs is low
      {
        for (v = 0; v < rows; v++) //for loop used to specify the number of row
        {
          digitalWrite(Output[v], HIGH);  //make specified output as HIGH
          if (digitalRead(Input[h]) == HIGH) //if the input that selected from first sor loop is change to high
          {
            no_press_flag = 0;              //reset the no press flag;
            for (byte w = 0; w < rows; w++) // make all outputs as low
              digitalWrite(Output[w], LOW);
            return v * 4 + h; //return number of button
          }
        }
      }
    }
  }
  return 50;
}

void kpSetup() {
  for (byte i = 0; i < rows; i++) //for loop used to make pin mode of outputs as output
  {
    pinMode(Output[i], OUTPUT);
  }
  for (byte s = 0; s < columns; s++) //for loop used to makk pin mode of inputs as inputpullup
  {
    pinMode(Input[s], INPUT_PULLUP);
  }

}

//end keypad code

#define DEBUG true
#define MENU_ITEM 8
const int SUBMENU_ITEM[] = {0, 2, 1, 1, 1};

const char Alpha[66] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_-0123456789 "};
//const char Alpha[66]=  {"1852916789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_- "};
//const char Alpha2[13]={"0123456789AP"};

int PosX = 0;
int PosY = -1;

char passX[64];

//String ssid = "lkafjlkdfjlk (  \"sssid1\" jte\" df\" kst ( sldkfj\"ssid2\"ffjlksjf\"fds\"fdf )";
String ssid;
String pass;
String m_ssid;
String m_pass;

//byte Up = 0;
//byte Down = 0;
//byte Left = 0;
//byte Right = 0;
int Pos = 0;
int Pos2 = 0;
byte m = 0;

byte s_AMPM = 0;
//byte m_ssid_pass=0;
byte ssid_found = 0;
byte r[50];
byte a[50];
byte b[50];
byte n = 0;
String ss;
//String names;
byte star = 0;
byte wifi_connect = 0;
//byte wifi_last=0;
String local_ip;

//LCD Backlight
int led = 13;
// get speed from pin 15
int fanspeed =15;

void mode_screen1() {
  switch (vent_pos) {
    case 37 : {
        lcd.setCursor(0, 1);
        lcd_p(64);
        lcd.setCursor(9, 1);
        if (_onTimer < 1001) {lcd_p(30);} if (_onTimer < 101){lcd_p(30);}  if (_onTimer < 11){lcd_p(30);}
        lcd.print(_onTimer - 1);
        lcd_p(32);
        if (_secTimer < 10)lcd_p(30);
        lcd.print(_secTimer);
      } break;
    case 12 : {
        lcd.setCursor(0, 1);
        lcd_p(65);
        lcd.setCursor(9, 1);if (_onTimer < 1001) {lcd_p(30);} if (_onTimer < 101){lcd_p(30);}  if (_onTimer < 11){lcd_p(30);}
        lcd.print(_offTimer - 1);
        lcd_p(32);
        if (_secTimer < 10)lcd_p(30);
        lcd.print(_secTimer);
      } break;
    default : {
        lcd.setCursor(0, 1);
        lcd.print("unknown menu");
      }
  }
}
void home_screen()
{
  lcd.clear(); lcd.setCursor(0, 0); lcd_p(0); m = 0; Pos = 0; Pos2 = 0; PosX = 0; lcd.noBlink();
  if (mode_s == 0)Time(8);
  else if (mode_s == 1)
    mode_screen1();

  else if(mode_s==2)
  {
    lcd.setCursor(0, 1);
  
    switch (vent_pos) {
    case 37 : {lcd_p(66);}break;
    case 12 : {lcd_p(67);}break;
    }
    Time(8);
  }

}
void menu(int s)
{
  switch (s) {
    case 0: {
        home_screen();
      } break;
    case 1: {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd_p(1);
        lcd.setCursor(0, 1);
        lcd_p(2);
      } break;
    case 2: {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd_p(1);
        lcd.setCursor(0, 1);
        lcd_p(3);
      } break;
    case 3: {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd_p(1);
        lcd.setCursor(0, 1);
        lcd_p(4);
      } break;
    case 4: {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd_p(1);
        lcd.setCursor(0, 1);
        lcd_p(5);
      } break;
    case 5: {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd_p(6);
        lcd.setCursor(0, 0);
        lcd_p(7);
      } break;
    case 6: {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd_p(8);
      } break;
    //case 7: { lcd.clear();lcd.setCursor(0,0);lcd_p(9);lcd.setCursor(0,1);lcd_p(10);}break;
    //case 8: { lcd.clear();lcd.setCursor(0,0);lcd.print("<Connection Failed>");lcd.setCursor(0,1);lcd.print("Retry");}break;
    // case 9: { lcd.clear();lcd.setCursor(0,0);lcd.print("<connectd_ssid>");lcd.setCursor(0,1);lcd_p(10);}break;
    //  case 10: { lcd.clear();lcd.setCursor(0,0);lcd_p(11);lcd.setCursor(0,1);lcd.print("<set_clock>");}break;
    case 11: {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd_p(12);
        lcd.setCursor(0, 1);
        lcd_p(13);
      } break;
    case 12: {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd_p(1);
        lcd.setCursor(0, 1);
        lcd_p(14);
      } break;
    case 13: {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd_p(1);
        lcd.setCursor(0, 1);
        lcd_p(15);
      } break;
    case 14: {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd_p(1);
        lcd.setCursor(0, 1);
        lcd_p(16);
      } break;
    case 15: {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd_p(1);
        lcd.setCursor(0, 1);
        lcd_p(17);
      } break;
    case 7:  {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd_p(0);
        lcd.setCursor(0, 1);
        lcd.print("inintlizing");
      }
  }
}

void get_key(byte s, byte t) {
  if (s == 0) {
    switch (t) {
      case 0: menu(0); break;
        //  case 1: menu(1);break;
        //  case 2: menu(2);break;
        //  case 3: menu(3);break;
        //  case 4: menu(4);break;
    }
  }
  if (s == 1) {
    switch (t) {
      case 0: menu(1); break;
      case 1: {
          menu(5);
        } break;
      case 2: menu(6); break;
    }
  }
  if (s == 2) {
    switch (t) {
      case 0: menu(2); break;
      case 1: menu(9); break;
    }
  }
  if (s == 3) {
    switch (t) {
      case 0: menu(3); break;
      case 1: menu(10); break;
    }
  }
  if (s == 4) {
    switch (t) {
      case 0: menu(4); break;
      case 1: menu(11); break;
    }
  }
  if (s == 5) {
    switch (t) {
      case 0: menu(12); break;
        //case 1: menu(11);break;
    }
  }
  if (s == 6) {
    switch (t) {
      case 0: menu(13); break;
        // case 1: menu(11);break;
    }
  }
  if (s == 7) {
    switch (t) {
      case 0: menu(14); break;
        // case 1: menu(11);break;
    }
  }
  if (s == 8) {
    switch (t) {
      case 0: menu(15); break;
        // case 1: menu(11);break;
    }
  }

}



void button(byte s) {
  switch (s) {
    case 'm': {
        if (m == 0) {
          Pos = 1;
          get_key(Pos, Pos2);
          m = 1;
        }
        else
        {
          menu(0);
        }
      } break;
    //{m=0;Pos=0;Pos2=0;menu(0);Time();lcd.noBlink();}}break;
    case 'e': {

        if ((m == 1) && (Pos == 1)) {
          //if(Pos2<SUBMENU_ITEM[Pos]){Pos2++;}
          list_ssid(); m = 2; Pos2 = -1;
        } //if(m=2; save to ssid and goto m3
        // else
        // {get_key(Pos,Pos2);}

        else if ((m == 2) && (Pos == 1) && (ssid_found > 0) && (Pos2 != -1)) {
          ssid = m_ssid;
          m = 3;
          menu(6);
        }
        else if ((m == 3) && (Pos == 1)) {
          for (int j = 0; j <= star; j++) {
            pass += passX[j];
          } connectToAP(ssid, pass);
        }
        //2nd menu
        else if ((m == 1) && (Pos == 2)) {
          wifi_status();
          m = 2;
          Pos2 = 0;
        }
        else if ((m == 2) && (Pos == 2)) {
          if (wifi_connect == 1) {
            menu(0);
            Pos = 0;
            Pos2 = 0;
          } else {
            m = 1;
            Pos = 1;
            list_ssid();
            m = 2;
            Pos2 = 0;
          }
        }
        else if ((m == 4) && (Pos == 2)) {
          list_ssid();
          m = 2;
          Pos = 1;
          Pos2 = 0;
        }
        else if ((m == 3) && (Pos == 2)) {
          Pos = 0;
          Pos2 = 0;
          m = 0;
          get_key(Pos, Pos2);
        }
        else if ((m == 1) && (Pos == 4)) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd_p(12);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd_p(18);
          lcd.setCursor(0, 1);
          lcd_p(58);
          m = 2;
        }
        else if ((m == 2) && (Pos == 4)) {
          resetting();
        }
        else if (m == 10) {
          list_ssid();
          m = 2;
          Pos2 = 0;
        }

        else if ((m == 1) && (Pos == 3)) {
          m = 2;
          PosX = 0;
        //  lcd.clear();
        //  lcd.setCursor(0, 0);
        //  lcd_p(19);
          
          //Time(7);
          
         
          s_hour = _hour;
          s_mint = _minute;
/*          lcd.setCursor(5, 1);
      //    if (s_hour > 12)s_hour = 1;
            if (s_hour < 10)lcd_p(30);
            lcd.print(s_hour);
            lcd_p(32);
            lcd.setCursor(8, 1);
            
    //  if (s_mint > 59)s_mint = 0;
            if (s_mint < 10)lcd_p(30);
            lcd.print(s_mint);
            if(s_AMPM==1){lcd_p(31);lcd_p(68);}
            else
            {lcd_p(33);lcd_p(68);}
*/
            set_time('e');
       //    lcd.blink();
       //   lcd.setCursor(5, 1);
        }
        else if ((m == 2) && (Pos == 3)) {
          lcd.noBlink();
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd_p(20);
          lcd.setCursor(0, 1);
          lcd_p(58);
          m = 3;
        }
        else if ((m == 3) && (Pos == 3)) {
          setTime();
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd_p(38);
          lcd.setCursor(0, 1);
          lcd_p(58);
          menu(0);
        }

        else if ((m == 1) && (Pos == 5)) {
          m = 2;  //mode selected
          select_mode('e');
        }
        else if ((m == 2) && (Pos == 5)) {
          if (mode == 0) {
            m = 4;  //first val of each mode
            set_timer('e');
          } else if (mode == 1) {
            m = 6;
            s_hour = _hour;
            s_mint = _minute;
            set_time('e');
          }
        }
        //else if((m==3) && (Pos==5)){if(Pos2==0)m=4;else m=5; set_timer('e');}
        else if ((m == 4) && (Pos == 5)) {
          set_onTimer = s_timer1;  //m=3;set_timer('e');
          _onTimer = set_onTimer;
          m = 5;
          set_timer('e');
//          Serial.println("\nset timer open");
        }
        else if ((m == 5) && (Pos == 5)) {
          set_offTimer = s_timer2;
          _offTimer = set_offTimer;
          mode_s = 1;
          m = 8;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd_p(21);
          lcd.setCursor(0, 1);
          lcd_p(58);
//          Serial.println("\nset timer close");
        }
        else if ((m == 8) && (Pos == 5)) {
          menu(0); if (mode == 0) {
            mode_s = 1;
            EEPROM.write(203, mode_s);
            EEPROM.write(220, (set_onTimer & 0xff));
            EEPROM.write(221, ((set_onTimer >> 8) & 0xff));
            EEPROM.write(222, (set_offTimer & 0xff));
            EEPROM.write(223, ((set_offTimer >> 8) & 0xff));
          }
          else if (mode == 1) {
            _secTimer = 59;
            mode_s = 2;
            EEPROM.write(203, mode_s);
          }
        }


        else if ((m == 6) && (Pos == 5)) {
          set_schedule(1);
          m = 7;
          s_hour = _hour;
          s_mint = _minute; //Serial.println("\nset on time");

          set_time('e');
        }
        else if ((m == 7) && (Pos == 5)) {
          set_schedule(2);
          m = 8;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd_p(38);
          lcd.setCursor(0, 1);
          lcd_p(58);
//          Serial.println("\nset off time");
        }

        //else if((m==3) && (Pos==5)){m=4;select_mode('e');} //second val of each mode
        //else if((m==4) && (Pos==5)){select_mode('t');}
        //else if((m==4) && (Pos==5)){select_mode('e');}

        else if ((m == 1) && (Pos == 6)) {
          m = 2;  //lcd.setCursor(7,1);lcd.print(fpwmlcd);fpwmlcd = (byte)EEPROM.read(200);
          calc_speed();
          lcd.setCursor(7, 1);
          lcd.print(fpwmlcd);
          lcd.setCursor(10, 1);
          lcd_p(36);
          PosX = 0;
          PosY = -1;
        }
        else if ((m == 2) && (Pos == 6)) {
          lcd.clear();  //
          lcd.setCursor(0, 0);
          lcd_p(38);
          lcd.setCursor(0, 1);
          lcd_p(58);
          m = 4;
        }
        // else if((m==3) && (Pos==6)){lcd.clear();lcd.setCursor(0,0);lcd_p(38);lcd.setCursor(0,1);lcd_p(58);m=4; }
        else if ((m == 4) && (Pos == 6)) {
          EEPROM.write(200, fpwmlcd);
          menu(0);
        }
        else if ((m == 1) && (Pos == 7)) {
          m = 2;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd_p(16);
          set_vent('e');
        }
        else if ((m == 2) && (Pos == 7)) {
          m = 3;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd_p(49);
          lcd.setCursor(0, 1);
          lcd_p(58);
        }
        else if ((m == 3) && (Pos == 7)) {
          menu(0);
          EEPROM.write(202, vent_cnt);
        }
        else if ((m == 1) && (Pos == 8)) {
          m = 8;
          PosY = 54;
          enter_pin('e');
        }
        else if ((m == 8) && (Pos == 8)) {
          lcd.clear(); lcd.setCursor(0, 0); lcd_p(24); EEPROM.write(204, 1);
          for (int i = 0; i <= star; i++) {
            lcd.setCursor(i, 1); lcd.print(passX[i]); web_pin[i] = passX[i];
            EEPROM.write(i + 205, web_pin[i]);
            Serial.print(web_pin[i]);
          }
        }

      } break;
    case 'w': {//Up
        if (m == 1) {
          if (Pos2 == 0) {
            if (Pos > 1) {
              Pos--;
            }
            get_key(Pos, Pos2);
          }
        }
        else if ((m == 2) && (Pos == 1)) {
          if (Pos2 > 1) {
            Pos2--;
          } m_ssid = ssid_lcd(Pos2);
        }
        else if ((m == 3) && (Pos == 1)) {
          enter_pass('+');
        }
        else if ((m == 8) && (Pos == 8)) {
          enter_pin('+'); // entering web pin
        }

        else if ((m == 2) && (Pos == 3)) {
          set_time('+');
        }

        else if ((m == 6) && (Pos == 5)) {
          set_time('+'); // select sch time menu
        }
        else if ((m == 7) && (Pos == 5)) {
          set_time('+'); // time selection 4 sch menu
        }

        else if ((m == 2) && (Pos == 5)) {
          select_mode('+');
        }
        else if (((m == 3) || (m == 4) || (m == 5)) && (Pos == 5)) {
          if (mode == 0)set_timer('+');
        }
        else if ((m == 2) && (Pos == 6)) {
          set_speed('+');
        }
        else if ((m == 2) && (Pos == 7)) {
          set_vent('+');
        }


      } break;
    case 's': {//Down
        if (m == 1) {
          if (Pos2 == 0) {
            if (Pos < MENU_ITEM) {
              Pos++;
            }
            get_key(Pos, Pos2);
          }
        }
        else if ((m == 2) && (Pos == 1)) {
          if (Pos2 < ssid_found) {
            Pos2++;
          } m_ssid = ssid_lcd(Pos2);
        }
        else if ((m == 3) && (Pos == 1)) {
          enter_pass('-');
        }
        else if ((m == 8) && (Pos == 8)) {
          enter_pin('-'); // entering web pin
        }

        else if ((m == 2) && (Pos == 3)) {
          set_time('-');
        }

        else if ((m == 6) && (Pos == 5)) {
          set_time('-'); // select sch time menu
        }
        else if ((m == 7) && (Pos == 5)) {
          set_time('-'); // time selection 4 sch menu
        }

        else if ((m == 2) && (Pos == 5)) {
          select_mode('+');
        }
        //else if(((m==3)||(m==4)) && (Pos==5)){select_mode('-');}
        else if ((m == 3) && (Pos == 5)) {
          if (mode == 0)set_timer('+');
        }
        else if (((m == 4) || (m == 5)) && (Pos == 5)) {
          if (mode == 0)set_timer('-');
        }

        else if ((m == 2) && (Pos == 6)) {
          set_speed('-');
        }
        else if ((m == 2) && (Pos == 7)) {
          set_vent('-');
        }
      } break;
    case 'a': {//Left
        if ((m == 3) && (Pos == 1)) {
          enter_pass('<');
        }
        else if ((m == 2) && (Pos == 3)) {
          set_time('<');
        }
        else if ((m == 6) && (Pos == 5)) {
          set_time('<');
        }
        else if ((m == 7) && (Pos == 5)) {
          set_time('<'); // time selection 4 sch menu
        }
        else if ((m == 8) && (Pos == 8)) {
          enter_pin('<'); // entering web pin
        }
      } break;
    case 'd': {//Left
        if ((m == 3) && (Pos == 1)) {
          enter_pass('>');
        }
        else if ((m == 2) && (Pos == 3)) {
          set_time('>');
        }
        else if ((m == 6) && (Pos == 5)) {
          set_time('>');
        }
        else if ((m == 7) && (Pos == 5)) {
          set_time('>'); // time selection 4 sch menu
        }
        else if ((m == 8) && (Pos == 8)) {
          enter_pin('>'); // entering web pin
        }
      } break;


    case 'c': {//Back
        if ((m == 1) && (Pos >= 0) && (Pos2 == 0)) {
          Pos = 0;
          Pos2 = 0;
          m = 0;
          get_key(Pos, Pos2);
        }
        //if(Pos2>0){Pos2--;}
        //get_key(Pos,Pos2);
        else if ((m == 2) && (Pos == 1)) {
          m = 1;
          ss = "";
          Pos2 = 0;
          get_key(Pos, Pos2);
        }
        else if ((m == 3) && (Pos == 1)) {
          enter_pass('*');  //list_ssid();m=2;Pos2=0;
        }
        else if (m == 2) {
          m--;
          get_key(Pos, 0);
        }
        // else if((m==3) &&(Pos==2)){get_key(Pos,Pos2);}
        else if ((m == 3) && (Pos == 1)) { } //del one char
        else if ((m == 2) && (Pos == 3)) {
          m = 1;
          PosX = 0;
          lcd.noBlink();
          menu(3);
        }
        else if ((m == 2) && (Pos == 5)) {
          m--;
          menu(12);
        }
        else if (((m == 4) || (m == 5)) && (Pos == 5)) {
          m = 3;
          set_timer('e');
        }
        else if ((m == 3) && (Pos == 5)) {
          m = 1;
          Pos2 = 0;
          menu(12);
        }
        else if ((m == 2) && (Pos == 6)) {
          m--;
          menu(13);
        }
        else if ((m == 2) && (Pos == 7)) {
          m--;
          menu(14);
        }
        else if ((m == 8) && (Pos == 8)) {
          m = 1;
          menu(15);
        }
        else if ((m == 7) && (Pos == 5)) {
          m = 6;
          set_time('e');
        }
        else if ((m == 6) && (Pos == 5)) {
          m = 1;
          menu(12);
        }
        else if ((m == 4) && (Pos == 6)) {
          m = 2;
          calc_speed();
          lcd.setCursor(7, 1);
          lcd.print(fpwmlcd);
          lcd_p(36);
          PosX = 0;
          PosY = -1;
        }
        //else if((m==8) && (Pos==8)){m=1;;}  // entering web pin
        else if ((m == 3) && (Pos == 7)) {
          m = 2;
          lcd.clear();
          lcd.setCursor(0, 0);
          set_vent('e');
          lcd_p(16);
          lcd.setCursor(7, 1);
          if (vent_pos == 0) lcd_p(29);
          else lcd_p(28);
        }
      /*  else if ((m == 2) && (Pos == 3)) {
          m = 1;
          PosX = 0;
          //lcd.clear();
          //lcd.setCursor(0, 0);
          //lcd_p(19);
         // Time(8);
          set_time('e');
          s_hour = _hour;
          s_mint = _minute;
        }*/
      } break;
  }
}

void resetting()
{
 // sendCommand("AT+CWQAP\r\n", 1000, DEBUG);
// Serial.print('q');
 // sendCommand("AT+RST\r\n", 3000, DEBUG);
// Serial.print('r');
  EEPROM.write(0, 0);
}

String sendCommand(String command, const int timeout, boolean debug)
{
  String response = "";

  Serial.print(command); // send the read character to the esp8266
  // Serial.print(F(command));
  //  if(debug)
  //  Serial.print(command);

  long int time = millis();

  while ( (time + timeout) > millis()) { //make > greater then
    while (Serial.available())
    {

      // The esp has data so display its output to the serial window
      char c = Serial.read(); // read the next character.
      response += c;
      //response = Serial.readStringUntil('K');
    }
  }
/*
  if (debug)
  {
    Serial.print(response);
  }*/
  return response;
}

void setup()
{
  analogWrite(14, 50);
  
  pinMode(led, OUTPUT);
  //Turn On LED backight
  digitalWrite(led, HIGH);

  pinMode(fan_speed_in, INPUT); //to read Fan speed 
  ssid.reserve(35);
  pass.reserve(25);
  m_ssid.reserve(35);
  m_pass.reserve(25);
  ss.reserve(1024);
  //names.reserve(50);
  local_ip.reserve(14);

  Wire.begin();
//  Serial1.begin(9600); // your esp's baud rate might be different
  Serial.begin(9600);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // RTC.begin(DateTime(_DATE_, _TIME_));

  //analogWrite(fan_pwm_out,1000);
  kpSetup();
  delay(10);
  menu(7);

 /* sendCommand("AT+RST\r\n", 200, DEBUG); // reset module
  delay(100);
  sendCommand("ATE0\r\n", 300, DEBUG); // Turn off replaying back command AT+CIFSR
  delay(100);
  sendCommand("AT+CWMODE=1\r\n", 400, DEBUG); // configure as access point
  delay(100);
  //sendCommand("AT+CWLAP\r\n",3000,DEBUG);
  // sendCommand("AT+CWJAP=\"princegateway2\",\"18152229\"\r\n",3000,DEBUG);
*/
  check_setup();
  menu(0);
  Serial.flush();
}


void list_ssid() {
  lcd.clear(); lcd.setCursor(0, 0); lcd_p(6);

  lcd.setCursor(0, 1); lcd_p(25);
 // ss = sendCommand("AT+CWLAP\r\n", 3100, false);
ss = sendCommand("2", 3100, true);

  int cnt = 0;
  n = 0;
  int start = 0;

  while (cnt != -1) {
    cnt = ss.indexOf('(', cnt + 1);
    if (cnt != -1)
    {
      r[n++] = cnt;
    }
  }

  ssid_found = n;
  if (n > 0) {
    a[0] = ss.indexOf('\"', r[0]) + 1;
    b[0] = ss.indexOf('\"', a[0]);

    lcd.clear(); lcd.setCursor(0, 0); lcd_p(6);
    lcd.setCursor(0, 1); lcd.print(n);
    lcd.setCursor(3, 1); lcd_p(26);
    delay(300);
  }
  else
  {
    lcd.setCursor(0, 1); lcd_p(27);
    m = 10;
  }
}
String ssid_lcd(int e) {
  if (ssid_found > 0) {

    a[e] = ss.indexOf('\"', r[e]) + 1;
    b[e] = ss.indexOf('\"', a[e]);
    String st = ss.substring(a[e], b[e]);
    lcd.clear(); lcd.setCursor(0, 0); lcd_p(6); lcd.setCursor(0, 1); lcd.print(st);
    return st;
  }
}

void re_make(int x) {
  char a[64];

  for (int j = 0; j < star; j++)
  {
    lcd.setCursor(j, 1); lcd_p(45);
    if (j == x) {
      j++;
    }

    a[j] = passX[j];
  }
  star--;
  for (int j = 0; j <= star; j++) {
    passX[j] = a[j];
    lcd.setCursor(j, 1); lcd.print(passX[j]);
  }
  PosX--;
}

void goto_pos(byte b) {
  ser2.attach(4);
  if (b == 1)
  {
    vent_pos = 12;
    ser2.write(vent_pos);
    delay(50);
  }
  else if (b == 3)
  {
    vent_pos = 37;
    ser2.write(vent_pos);
    delay(50);
  }
  switch (b) {
    case 0: {
        vent_pos = 37;
        ser2.write(vent_pos);
      } break; //off
    case 1: {
        vent_pos = 30;
        ser2.write(vent_pos);
      } break; //25%
    case 2: {
        vent_pos = 22;
        ser2.write(vent_pos);
      } break; //50%
    case 3: {
        vent_pos = 19;
        ser2.write(vent_pos);
      } break; //75%
    case 4: {
        vent_pos = 12;
        ser2.write(vent_pos);
      } break; //100%
      /* case 5: {vent_pos=70; ser2.write(vent_pos);}break;
       case 6: {vent_pos=80; ser2.write(vent_pos);}break;
       case 7: {vent_pos=100; ser2.write(vent_pos);}break;
       case 8: {vent_pos=120; ser2.write(vent_pos);}break;
       case 9: {vent_pos=150; ser2.write(vent_pos);}break;
       case 11: {vent_pos=170; ser2.write(vent_pos);}break;
       case 10: {vent_pos=180; ser2.write(vent_pos);}break;*/

  }
  delay(250); ser2.detach();
}

void set_vent(char vn) {
  lcd.setCursor(7, 1);
  switch (vn) {
    case '+': {
        if (vent_cnt < 4)vent_cnt++;
        lcd.setCursor(0, 1);
        lcd.print(vent_cnt);
      } break;
    case '-': {
        if (vent_cnt > 0)vent_cnt--;
        lcd.setCursor(0, 1);
        lcd.print(vent_cnt);
      } break;
    case 'e': {
        lcd.setCursor(0, 1);
        vent_cnt = (byte)EEPROM.read(202);
        lcd.print(vent_cnt);
      } break;
  }
  // vent_pos=vent_cnt;ser2.attach(4); lcd.clear(); lcd.setCursor(0,0);lcd.print(vent_pos);ser2.write(vent_pos++);delay(250);ser2.detach();
  switch (vent_cnt) {
    case 0: {
        lcd.setCursor(6, 1);
        lcd_p(29);
        goto_pos(vent_cnt);
      } break;
    case 1: {
        lcd.setCursor(6, 1);
        lcd_p(59);
        goto_pos(vent_cnt);
      } break;
    case 2: {
        lcd.setCursor(6, 1);
        lcd_p(60);
        goto_pos(vent_cnt);
      } break;
    case 3: {
        lcd.setCursor(6, 1);
        lcd_p(61);
        goto_pos(vent_cnt);
      } break;
    case 4: {
        lcd.setCursor(6, 1);
        lcd_p(62);
        goto_pos(vent_cnt);
      } break;

  }

}

void calc_speed() {
 // fspeed = map(fpwmlcd, 10, 100, 100, 5571);
 fspeed = get_freq();
  lcd.clear(); lcd.setCursor(0, 0); lcd_p(34); lcd.print(fspeed); lcd_p(35);
  
  //lcd.setCursor(7,1);
}
unsigned int get_freq(){
  currentMillis = millis();
  unsigned int pwm=0;
  while(currentMillis - previousMillis < interval){currentMillis = millis();
  if(digitalRead(fan_speed_in)){
   while(digitalRead(fan_speed_in)){};
   pwm++;
  }
  } 
previousMillis = currentMillis;
return pwm;
}
void set_speed(char fs) {
  lcd.setCursor(7, 1);
  switch (fs) {
    case '+': {
        if (fpwmlcd < 100)fpwmlcd++;
        lcd.print(fpwmlcd);
        lcd.setCursor(10, 1);
        lcd_p(36);
        fpwm = map(fpwmlcd, 10, 100, 35, 255);
        //fspeed = map(fpwmlcd, 10, 100, 100, 5571);
        analogWrite(14, fpwm);
        
        if (vent_pos != 37)analogWrite(14, fpwm);
      } break;
    case '-': {
        if (fpwmlcd > 10)fpwmlcd--;
        lcd.print(fpwmlcd);
        if (fpwmlcd < 100)lcd_p(43);
        lcd.setCursor(10, 1);
        lcd_p(36);
        fpwm = map(fpwmlcd, 10, 100, 35, 255);
        //fspeed = map(fpwmlcd, 10, 100, 100, 5571);
        if (vent_pos != 37)analogWrite(14, fpwm);
      } break;
    case 'i': {
        fpwm = map(fpwmlcd, 10, 100, 35, 255);
        //fspeed = map(fpwmlcd, 10, 100, 100, 5571);
        if (vent_pos != 37)analogWrite(14, fpwm);
      } break;
  }


  // lcd.clear();lcd.setCursor(0,7);lcd.print(fpwm); lcd.print("50%");

}

void set_schedule(byte s) {
  //lcd.clear();lcd.setCursor(0,0);lcd_p(38);
  if (s == 1)
  {

    if (s_AMPM == 1)
      on_hour = s_hour + 12;
    else
      on_hour = s_hour;
    EEPROM.write(211, on_hour);
    EEPROM.write(212, s_mint);
    
    on_min = s_mint;


    //   Serial1.print("\ntime= "); Serial1.print(s_hour);Serial1.print(":"); Serial1.println(s_mint);
    //   Serial1.print("on time= "); Serial1.println(onTime_min);
  }
  else if (s == 2)

  {

    if (s_AMPM == 1)
      off_hour = s_hour + 12;
    else
      off_hour = s_hour;

    EEPROM.write(213, off_hour);
    EEPROM.write(214, s_mint);
    
    off_min= s_mint;


    // Serial1.print("\ntime= "); Serial1.print(s_hour);Serial1.print(":"); Serial1.println(s_mint);
    // Serial1.print("on time= "); Serial1.println(offTime_min);
  }
}

void set_timer(char c) {

  if (m == 4) {
    //Time(8);
    lcd.setCursor(4, 1);
    switch (c) {
      case 'e': {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd_p(42);
          lcd.setCursor(4, 1);
         // lcd_p(43);
          s_timer1 = set_onTimer;
            if (s_timer1 < 1000) {
            lcd_p(30);
          //  lcd_p(45);
          } if (s_timer1 < 100)lcd_p(30);
           if (s_timer1 < 10)lcd_p(30);
          lcd.print(s_timer1);
          
          lcd.setCursor(9, 1);
          lcd_p(44);
        } break;
      case '+': {
          if (s_timer1 < 9998)s_timer1++;
          if (s_timer1 < 1000) {
            lcd_p(30);
          //  lcd_p(45);
          } if (s_timer1 < 100)lcd_p(30);
           if (s_timer1 < 10)lcd_p(30);
          lcd.print(s_timer1);
         
         /*
          if (s_timer1 < 1000) {
            lcd_p(43);
          //  lcd_p(45);
          }else if (s_timer1 < 100)lcd_p(43);
          else if (s_timer1 < 10)lcd_p(45);
          lcd.print(s_timer1);
          */
          lcd.setCursor(9, 1);
          lcd_p(44);
        } break;
      case '-': {
          if (s_timer1 > 1)s_timer1--;
          if (s_timer1 < 1000) {
            lcd_p(30);
          //  lcd_p(45);
          } if (s_timer1 < 100)lcd_p(30);
           if (s_timer1 < 10)lcd_p(30);
          lcd.print(s_timer1);
         // if (s_timer1 < 10)lcd_p(43);
         // if (s_timer1 < 100)lcd_p(45);
          lcd.setCursor(9, 1);
          lcd_p(44);
        } break;
        // case 's': { set_onTimer=s_timer1;}break;
    }
  }
  if (m == 5) {
    lcd.setCursor(4, 1);
    switch (c) {
      case 'e': {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd_p(46);
          lcd.setCursor(4, 1);
          //lcd_p(43);
          s_timer2 = set_offTimer;
          if (s_timer2 < 1000) {
            lcd_p(30);
          //  lcd_p(45);
          } if (s_timer2 < 100)lcd_p(30);
           if (s_timer2 < 10)lcd_p(30);
          lcd.print(s_timer2);
          lcd.setCursor(9, 1);
          lcd_p(44);
        } break;
      case '+': {
          if (s_timer2 < 9998)s_timer2++;
          if (s_timer2 < 1000) {
            lcd_p(30);
          //  lcd_p(45);
          } if (s_timer2 < 100)lcd_p(30);
           if (s_timer2 < 10)lcd_p(30);
          lcd.print(s_timer2);
          lcd.setCursor(9, 1);
          lcd_p(44);
        } break;
      case '-': {
          if (s_timer2 > 1)s_timer2--;
         if (s_timer2 < 1000) {
            lcd_p(30);
          //  lcd_p(45);
          } if (s_timer2 < 100)lcd_p(30);
           if (s_timer2 < 10)lcd_p(30);
           lcd.print(s_timer2);
          lcd.setCursor(9, 1);
          lcd_p(44);
        } break;
    }
  }
}

void vent_mov(byte mov)
{
  switch(mov){
    case 1 : { //turn on
              vent_pos = 12; set_speed('i');
              ser2.attach(4);
              ser2.write(vent_pos);
              delay(250);
              ser2.detach();
              EEPROM.write(202, vent_pos); //Serial.println("servo goes on");
             }break;
    case 0 :{
              vent_pos = 37;
              ser2.attach(4);
              ser2.write(vent_pos);
              delay(250);
              ser2.detach(); analogWrite(14, LOW);
              EEPROM.write(202, vent_pos); //Serial.println("servo goes off");
            }break;
    }
}

void timeOutTimer() {
  //lcd.setCursor(0,1);
  if (m == 0)
    mode_screen1();


  if ((vent_pos == 37) && (_onTimer == 0))
  {
    _offTimer = set_offTimer;
    vent_mov(1);// turn on
 //    digitalWrite(led, HIGH);
  }
  else if ((vent_pos == 12) && (_offTimer == 0))
  {
    _onTimer = set_onTimer;
    _offTimer = set_offTimer;
    vent_mov(0);//turn off
 //   digitalWrite(led, LOW);
  }
  /*

     unsigned long t = millis();

  if((vent_pos == 37) && (t - prev_t >= _onTimer))
    {
      vent_pos = 12;
      prev_t =t;
      ser2.attach(4);
  ser2.write(vent_pos);
  delay(200);
  ser2.detach();
  EEPROM.write(202,vent_pos); Serial.println("servo goes off");
    }
    else if ((vent_pos == 12) && (t - prev_t >= _offTimer))
    {
      vent_pos = 37;
      prev_t =t;
      ser2.attach(4);
  ser2.write(vent_pos);
  delay(200);
  ser2.detach();
  EEPROM.write(202,vent_pos);Serial.println("servo goes on");
    }  */
}

void select_mode(char c) {

  if (m == 2) {
    switch (c) {
      case 'e': {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd_p(39);
        } break;
      case '+': {
          if (mode == 0)mode = 1;
          else mode = 0;
        } break;
    }
    lcd.setCursor(0, 1);
    if (mode == 0)
      lcd_p(47);
    else
      lcd_p(48);
  }
}



void set_time(char t) {

  // Serial.print("\n\rValue m=");Serial.print(m); Serial.print(" value of input= "); Serial.println(t);
  if (t == 'e') {
    lcd.clear();
    lcd.setCursor(0, 0);
    if (m == 6)
      lcd_p(22);
    else if (m == 7)
      lcd_p(23);
    else if (m == 2)
      lcd_p(19);
      Time(0);
          s_hour = _hour;
          s_mint = _minute;
      lcd.setCursor(5, 1);
      if (s_hour > 12)s_hour = 1;
            if (s_hour < 10)lcd_p(30);
            lcd.print(s_hour); lcd_p(32); lcd.setCursor(8, 1);
            
            
      if (s_mint > 59)s_mint = 0;
            if (s_mint < 10)lcd_p(30);
            lcd.print(s_mint);lcd.setCursor(11, 1);
            if(s_AMPM==1){lcd_p(31);lcd_p(68);}
            else
            {lcd_p(33);lcd_p(68);}
            lcd.setCursor(5, 1);
  }

  else if ((m == 2) || (m == 6) || (m == 7)) {

    if (PosX == 0)
    {

      lcd.setCursor(5, 1); lcd.blink();
      switch (t) {
        case '+': {
            s_hour++;
            if (s_hour > 12)s_hour = 1;
            if (s_hour < 10)lcd_p(30);
            lcd.print(s_hour);
            lcd.setCursor(5, 1);
          } break;
        case '-': {
            s_hour--;
            if (s_hour < 1)s_hour = 12;
            if (s_hour > 12)s_hour = 1;
            if (s_hour < 10)lcd_p(30);
            lcd.print(s_hour);
            lcd.setCursor(5, 1);
          } break;
        case '>': {
            PosX = 1;
            lcd.setCursor(8, 1);
          } break;
        case '<': {
            PosX = 2;
            lcd.setCursor(11, 1);
          } break;
      }
    }
    else if (PosX == 1)
    {
      lcd.setCursor(8, 1);
      switch (t) {
        case '+': {
            s_mint++;
            if (s_mint > 59)s_mint = 0;
            if (s_mint < 10)lcd_p(30);
            lcd.print(s_mint);
            lcd.setCursor(8, 1);
          } break;
        case '-': {
            s_mint--;
            if (s_mint <= 0)s_mint = 59;
            if (s_mint > 59)s_mint = 0;
            if (s_mint < 10)lcd_p(30);
            lcd.print(s_mint);
            lcd.setCursor(8, 1);
          } break;
        case '<': {
            PosX = 0;
            lcd.setCursor(5, 1);
          } break;
        case '>': {
            PosX = 2;
            lcd.setCursor(11, 1);
          } break;
      }
    }
    else if (PosX == 2)
    {
      lcd.setCursor(11, 1);
      switch (t) {
        case '+': {
            if (s_AMPM == 0) {
              s_AMPM = 1;
              lcd_p(31);
            } else {
              s_AMPM = 0;
              lcd_p(33);
              lcd.setCursor(11, 1);
            }
          } break;
        case '-': {
            if (s_AMPM == 0) {
              s_AMPM = 1;
              lcd_p(31);
            } else {
              s_AMPM = 0;
              lcd_p(33);
              lcd.setCursor(11, 1);
            }
          } break;
        case '<': {
            PosX = 1;
            lcd.setCursor(8, 1);
          } break;
        case '>': {
            PosX = 0;
            lcd.setCursor(5, 1);
          } break;
      }
    }
  }
}

void enter_pin(char pn) {
  lcd.clear(); lcd.setCursor(0, 0); lcd_p(52);

  if (star < PosX) {
    star = PosX;
  }
  for (int i = 0; i <= star; i++) {
    lcd.setCursor(i, 1);
    lcd.print(passX[i]);
  }
  switch (pn) {
    case '+': {
        if (PosY > 54) {
          PosY--;
          lcd.setCursor(PosX, 1);
          lcd.print(Alpha[PosY]);
          passX[PosX] = Alpha[PosY];
        }
      } break;
    case '-': {
        if (PosY < 63) {
          PosY++;
          lcd.setCursor(PosX, 1);
          lcd.print(Alpha[PosY]);
          passX[PosX] = Alpha[PosY];
        }
      } break;
    case '<': {
        if (PosX > 0) {
          PosX--;
          lcd.setCursor(PosX, 1);
          lcd.print(Alpha[PosY]);
          passX[PosX] = Alpha[PosY];
        }
      } break;
    case '>': {
        if (PosX < 3) {
          PosX++;
          lcd.setCursor(PosX, 1);
          lcd.print(Alpha[PosY]);
          passX[PosX] = Alpha[PosY];
        }
      } break;
    case '*': {
        if (PosX == star) {
          re_make(PosX);
        }
      } break;
  }
  // if(PosY==64){if(PosX>0){PosX--;lcd.setCursor(PosX,1);lcd.print(Alpha[PosY]);passX[PosX]=Alpha[PosY];PosY=0;}}

  // delay(500);
  if (star < PosX) {
    star = PosX;
  }
  for (int i = 0; i <= star; i++) {
    lcd.setCursor(i, 1);
    lcd.print(passX[i]);
  }
  Serial.println(PosY);
}


void enter_pass(char ch) {
  lcd.clear(); lcd.setCursor(0, 0); lcd_p(8); //lcd8

  if (star < PosX) {
    star = PosX;
  }
  for (int i = 0; i <= star; i++) {
    lcd.setCursor(i, 1);
    lcd.print(passX[i]);
  }
  switch (ch) {
    case '+': {
        if ((PosY == 0) || (PosY == -1)) {
          PosY = 64;
        } if (PosY > 0) {
          PosY--;
          lcd.setCursor(PosX, 1);
          lcd.print(Alpha[PosY]);
          passX[PosX] = Alpha[PosY];
        }
      } break;
    case '-': {
        if (PosY == 63) {
          PosY = -1;
        } if (PosY <= 63) {
          PosY++;
          lcd.setCursor(PosX, 1);
          lcd.print(Alpha[PosY]);
          passX[PosX] = Alpha[PosY];
        }
      } break;
    case '<': {
        if (PosX > 0) {
          PosX--;
          lcd.setCursor(PosX, 1);
          lcd.print(Alpha[PosY]);
          passX[PosX] = Alpha[PosY];
        }
      } break;
    case '>': {
        if (PosX < 63) {
          PosX++;
          lcd.setCursor(PosX, 1);
          lcd.print(Alpha[PosY]);
          passX[PosX] = Alpha[PosY];
        }
      } break;
    case '*': {
        if (PosX == star) {
          re_make(PosX);
        }
      } break;
  }
  // if(PosY==64){if(PosX>0){PosX--;lcd.setCursor(PosX,1);lcd.print(Alpha[PosY]);passX[PosX]=Alpha[PosY];PosY=0;}}

  // delay(500);
  if (star < PosX) {
    star = PosX;
  }
  for (int i = 0; i <= star; i++) {
    lcd.setCursor(i, 1);
    lcd.print(passX[i]);
  }
}

void check_setup() {

//  String esid;
//  String epass;
//  byte _t;  byte _min; //byte _min2; byte _t2;
  // EEPROM.write(0,1);
  fpwmlcd = (byte)EEPROM.read(200);
  if (fpwmlcd > 100)fpwmlcd = 100;
  
  vent_cnt  = (byte)EEPROM.read(202);
  if (vent_cnt < 5) {
    goto_pos(vent_cnt);
    set_speed('i');
  }
  //  EEPROM.write(203,1);EEPROM.write(209,1);EEPROM.write(210,1);
  // pos2  = EEPROM.read(202);
  mode_s = (byte)EEPROM.read(203);
  if (mode_s == 1) {
    
    _onTimer = (byte)EEPROM.read(220) + ((byte)EEPROM.read(221) << 8);
    set_onTimer = _onTimer;
    _offTimer = (byte)EEPROM.read(222) + ((byte)EEPROM.read(223) << 8);
    set_offTimer = _offTimer;
    if(set_onTimer>9999)
    set_onTimer = 1;
    if(set_offTimer>9999)
    set_offTimer=1;
  }
  else if (mode_s == 2) {
    on_hour = EEPROM.read(211);
    on_min = EEPROM.read(212);

    off_hour = EEPROM.read(213);
    off_min = EEPROM.read(214);
  }

  byte d = (byte)EEPROM.read(204);
  if ( d != 0) {
    web_pin[0] = (byte)EEPROM.read(205);
    web_pin[1] = (byte)EEPROM.read(206);
    web_pin[2] = (byte)EEPROM.read(207);
    web_pin[3] = (byte)EEPROM.read(208);
  }
delay(400);
Serial.flush(); // to prevent some random garbig from esp
    String toAP = sendCommand("3", 300, DEBUG);
  int ii = toAP.length();
  if (ii >= 3) {
    wifi_connect = 1;
    local_ip = toAP;
     int ssid_length = EEPROM.read(1);
     for (int i = 0; i < ssid_length; ++i)
    {
      ssid += char(EEPROM.read(i + 3));
    }
  }

  
/*
  int c = EEPROM.read(0);

  if (c != 0)
  {
    int ssid_length = EEPROM.read(1); //Serial.print("ssid_length= "); Serial.println(ssid_length);
    int pass_length = EEPROM.read(2); //Serial.print("pass_length= "); Serial.println(pass_length);

    for (int i = 0; i < ssid_length; ++i)
    {
      esid += char(EEPROM.read(i + 3));
    }

    for (int i = 0; i < pass_length; ++i)
    {
      epass += char(EEPROM.read(i + 35));
    }

    ssid = epass;
    connectToAP(esid, epass);
  }*/



}


void connectToAP(String u, String p) {

  // Serial.println("resetting");
 // sendCommand("AT+CWQAP\r\n", 1000, DEBUG); // Turn off replaying back command , TO GET LOCAL IP AT+CIFSR
// Serial.print('q');
  wifi_connect = 0;
  EEPROM.write(0, 0);
  lcd.clear(); lcd.setCursor(0, 0); lcd_p(56); lcd.setCursor(0, 1); lcd.print(u);

  //String c = "AT+CWJAP=\"" + u + "\",\"" + p + "\"\r\n" ;
  String c = "U"+ u + "\"" + p +"\n\n";
  Serial.print(c);
  String toAP = sendCommand(c, 20000, DEBUG);
  int ii = toAP.length();
  if (ii >= 3) {
    wifi_connect = 1;
    local_ip = toAP;
    lcd.clear(); lcd.setCursor(0, 0); lcd_p(7); lcd.setCursor(0, 1); lcd.print(local_ip);
  

//    EEPROM.write(0, 1);
    // Serial.println("writing eeprom ssid:");
    int sl = u.length();
    EEPROM.write(1, sl);
    for (int i = 0; i < sl; ++i)
    {
      EEPROM.write(3 + i, u[i]);
      // Serial.print("Wrote: ");
      // Serial.println(u[i]);
    }
/*
    int pl = p.length();
    EEPROM.write(2, pl);
    //   Serial.println("writing eeprom pass:");
    for (int i = 0; i < pl; ++i)
    {
      EEPROM.write(35 + i, p[i]);
      //  Serial.print("Wrote: ");
      //  Serial.println(p[i]);
    }
*/
   // sendCommand("AT+CIPMUX=1\r\n", 1000, DEBUG); // configure for multiple connections
   // sendCommand("AT+CIPSERVER=1,80\r\n", 1000, DEBUG); // turn on server on port 80
    //Serial.println("Server Ready");
 //   delay(1000);
    Pos = 0; Pos2 = 0; m = 0; menu(0);


  }
  else {
  //  ii = toAP.indexOf('L'); int i2 = toAP.indexOf('R');

  //  if ((ii >= 0) || (i2 >= 0)) {
    //  sendCommand("AT+CWQAP\r\n", 1000, DEBUG);
  //  }
    lcd.clear(); lcd.setCursor(0, 0); lcd_p(53); lcd.setCursor(0, 1); lcd_p(54);
    menu(6); pass = ""; //passX ; Posx, posy;
    EEPROM.write(0, 0);
  }
  //Serial.println(toAP);

}

void wifi_status()
{
  if (wifi_connect == 1)
  {
   // local_ip = sendCommand("i", 1000, DEBUG);
    lcd.clear(); lcd.setCursor(0, 0); lcd.print(ssid); lcd.setCursor(0, 1); lcd.print(local_ip);
    m = 3;
  }
  else
  {
    lcd.clear(); lcd.setCursor(0, 0); lcd_p(53); lcd.setCursor(0, 1); lcd_p(55);
    // m=1;Pos=1;Pos2=0;
  }

}
void update_web(){
//String s = String("W,"  + _hour + "," + _minute +"," +_second + ","+ fpwmlcd +","+ on_hour + "," + on_min + "," + off_hour + "," + off_min + "," + _onTimer + "," + _offTimer + "," + vent_pos);

//Serial.print(s);
Serial.print('W');//Serial.print("test");
Serial.write(_hour);
Serial.write(_minute);
Serial.write(_second);
Serial.write(on_hour);
Serial.write(on_min);
Serial.write(off_hour);
Serial.write(off_min);
Serial.write(_onTimer);
Serial.write(_offTimer); //_secTimer
Serial.write(vent_pos);
Serial.write(mode_s);

/*
Serial.print('W');
Serial.write(_hour);
Serial.write(_minute);
Serial.write(_second);
Serial.write(on_hour);
Serial.write(on_min);
Serial.write(off_hour);
Serial.write(off_min);
Serial.write(_onTimer);
Serial.write(_offTimer);
Serial.write(vent_pos);
Serial.write(mode);*/
}
//---geting data from esp
/*
void quick_test(){
  if
}

void get_val(){
 delay(100);
if(a == 'W')get_val();

_hour= Serial.read();
_minute= Serial.read();
_second= Serial.read();
on_hour = Serial.read();
on_min = Serial.read();
off_hour = Serial.read();
off_min = Serial.read();
_onTimer = Serial.read();
_offTimer = Serial.read();
vent_pos = Serial.read();
mode = Serial.read();
}*/
//----

//=========Web control functions

void start_save_test(char s)
{
delay(30);
byte temppwm = Serial.read();
fpwmlcd = temppwm;
fpwm = map(fpwmlcd, 10, 100, 35, 255);
//fspeed = map(fpwmlcd, 10, 100, 100, 5571);
byte door = Serial.read();
switch(door){
  case 0 : vent_cnt= 0; break;
  case 25 : vent_cnt= 1; break;
  case 50 : vent_cnt= 2; break;
  case 75 : vent_cnt= 3; break;
  case 100 : vent_cnt= 4; break;
}
goto_pos(vent_cnt);
//Serial.print("Fan speed is: " + (String)fpwmlcd + " ,door positon is: " + (String)vent_cnt + '\n');

if (vent_pos != 37)analogWrite(14, fpwm);

if(s== 'S')
{
 EEPROM.write(200,temppwm);
 EEPROM.write(202,vent_cnt);
  
}
}

void stop_test()
{
//  Serial.print("test is stoped\n");
  fpwmlcd =(byte)EEPROM.read(200);
  fpwm = map(fpwmlcd, 10, 100, 35, 255);
 // fspeed = map(fpwmlcd, 10, 100, 100, 5571);
  
  vent_cnt=(byte)EEPROM.read(202);
  goto_pos(vent_cnt);
  
  if (vent_pos != 37)analogWrite(14, fpwm);
}
void set_Wmode(char c){
delay(100); 
   char data = Serial.available();
    char n;   
byte h,m,s,H,M,S;
byte ampm=2;
byte AMPM=2;
  if(c=='t'){
    while(data >0 ){
    if(Serial.available()>0){data--;
    n= Serial.read();
    if(n != '\n')
    {
    switch(n){
      case 'h' :  h=Serial.read();break;
      case 'm' :  m=Serial.read();break;
      case 's' :  s=Serial.read();break;
      case 'H' :  H=Serial.read();break;
      case 'M' :  M=Serial.read();break;
      case 'S' :  S=Serial.read();break;
    }
    }
    else break;
    }
  }
    set_offTimer = (H*60) + M ;
    _offTimer = set_offTimer;
    set_onTimer = (h*60) + m ;
    _onTimer = set_onTimer;
    mode_s=1;
    EEPROM.write(203, 1);   //set mode =1 for timer mode
            EEPROM.write(220, (set_onTimer & 0xff));
            EEPROM.write(221, ((set_onTimer >> 8) & 0xff));
            EEPROM.write(222, (set_offTimer & 0xff));
            EEPROM.write(223, ((set_offTimer >> 8) & 0xff));
  }
  else if(c=='c'){
      while(data >0 ){
    if(Serial.available()>0){data--;
    n= Serial.read();
    if(n != '\n')
    {
    switch(n){
      case 'h' :  on_hour=Serial.read();break;
      case 'm' :  on_min=Serial.read();break;
      case 'a' :  ampm=0;break;
      case 'p' :  ampm=1;break;
      case 'H' :  off_hour=Serial.read();break;
      case 'M' :  off_min=Serial.read();break;
      case 'A' :  AMPM=0;break;
      case 'P' :  AMPM=1;break;
    }
    }
    else break;
    }
  }  
    if(ampm==1)
    on_hour=h+12;
    if(AMPM==1)
    off_hour=H+12;
    mode_s=2;
    EEPROM.write(203, 2);   //set mode =2 for schedual mode
    EEPROM.write(211, on_hour); //data in 24h format
    EEPROM.write(212, on_min);

    EEPROM.write(213, off_hour);
    EEPROM.write(214, off_min);
  }
}

void set_Wtime(){
  delay(100);
     char data = Serial.available();
    char n;
      while(data >0 ){
    if(Serial.available()>0){data--;
    n= Serial.read();
    if(n != '\n')
    {
    switch(n){
      case 'h' :  s_hour=Serial.read();break;
      case 'm' :  s_mint=Serial.read();break;
      case 'a' :  s_AMPM=0;break;
      case 'p' :  s_AMPM=1;break;
    }
    }
    else break;
    }
  }
  setTime();
}

void set_Wpin(){
delay(100);
 if(Serial.available()>3){
 web_pin[0]=Serial.read();
 web_pin[1]=Serial.read();
 web_pin[2]=Serial.read();
 web_pin[3]=Serial.read();
 EEPROM.write(204, 1);
 EEPROM.write(205, web_pin[0]);
 EEPROM.write(206, web_pin[0]);
 EEPROM.write(207, web_pin[0]);
 EEPROM.write(208, web_pin[0]);

}
}

//====== End Web control functions
void loop() {
  //stream.readBytesUntil(character, buffer, length)
  //if(m==0){
  //  DateTime now = RTC.now();
  //  now_hour = now._hour(); now_min = now._minute();
  Time(0);
 

    if (mode_s == 2) {
    //  Serial.print("\ntime= "); Serial.print(r_hour); Serial.print(":"); Serial.println(_minute);
    //  Serial.print("\r\non time= "); Serial.print(on_hour);
    //  Serial.print("off time= "); Serial.print(offTi);
   //   Serial.print("Time check time= "); Serial.println(Time_check);
      
   // if(on_hour>off_hour){
    if((_hour>=on_hour) && (_minute>=on_min)&& (_hour<=off_hour) && (_minute<off_min))
        {
          if(vent_pos == 37){
        vent_mov(1);// digitalWrite(led, HIGH);
        if(m==0)home_screen();
        }}
        else
        {if(vent_pos == 12){
        vent_mov(0);// digitalWrite(led, LOW);
        if(m==0)home_screen(); 
        }}
    }
 
  

 if (mode_s == 1)
    timeOutTimer();

  if (Serial1.available()) {

    int b = Serial.read();

    /*   switch(b){
        case '1':  {ss=sendCommand("AT+CWLAP\r\n",3000,false);Serial.print(ss);}break;
        case '2':  {ss=sendCommand("AT+CWLAP\r\n",3000,false);listAP(ss);}break;
        case '3':  {seprate_each_ssid(names);}bfreak;
        case '4':  {nS=sendCommand("AT+CWJAP=\"prince.gateway03457568850\",\"adfg\"\r\n",6000,false);Serial.print(nS);test(nS);}break;
        case '5':  {nS=sendCommand("AT+CWJAP=\"prince.gateway03457568850\",\"18152229\"\r\n",6000,false);Serial.print(nS);test(nS);}break;
        case '6':  {Serial1.print('q');}break;
        case '7':  {sendCommand("AT+CIFSR\r\n",4000,DEBUG);}break;
        case '8':  {sendCommand("AT+CIPSTATUS\r\n",4000,DEBUG);}break;
        case '9':  {check_ver();}break;

        default:
     }*/
    button(b);


  }

  if (Serial.available()) {
        int a=Serial.read();
        switch(a){
        case 'V':  {update_web();}break;
        case 'P':  {Serial.print('F');Serial.write(fpwmlcd); Serial.write(highByte(fspeed));Serial.write(lowByte(fspeed));}break;   //added two more bytes
        
        case '1':  {start_save_test('a');}break;
        case '0':  {stop_test();}break;
        case 'S':  {start_save_test('S');}break;
        case 't':  {set_Wmode('t');}break;
        case 'c':  {set_Wmode('c');}break;
        case 'T':  {set_Wtime();}break;
        case 'L':  {set_Wpin();}break;
        }
//        if(a == 'S')get_val();
//        else if(a == 'T')quick_test();
  }

  if (millis() - kdelay > period) //used to make non-blocking delay
  {
    kdelay = millis(); //capture time from millis function
    switch (keypad())  //switch used to specify which button
    {
      case 1:
        // Serial.println("On n off");
        button('w');
        break;
      case 4:
        // Serial.println("Menu");
        button('m');
        break;
      case 7:
        // Serial.println("back");
        button('c');
        break;
      case 2:
        //  Serial.println("Ok");
        button('e');
        break;
      case 5:
        // Serial.println("Left");
        button('a');
        break;
      case 8:
        // Serial.println("Back");
        button('c');
        break;
      case 6:
        // Serial.println("Right");
        button('d');
        break;

      case 9:
        // Serial.println("Down");
        button('s');
        break;
      case 10:
        // Serial.println(9);
        break;
    }
  }
}

byte decToBcd(byte val) {
  return ((val / 10 * 16) + (val % 10));
}
byte bcdToDec(byte val) {
  return ((val / 16 * 10) + (val % 16));
}

void setTime() {

  if (s_AMPM == 1)
  {
    s_hour += 12;
  }

  // The following codes transmits the data to the RTC
  Wire.beginTransmission(DS1307);
  Wire.write(byte(0));
  Wire.write(decToBcd(55));
  Wire.write(decToBcd(s_mint));
  Wire.write(decToBcd(s_hour));
  Wire.write(decToBcd(1));
  Wire.write(decToBcd(15));
  Wire.write(decToBcd(8));
  Wire.write(decToBcd(15));
  Wire.write(byte(0));
  Wire.endTransmission();
  // Ends transmission of data
}

byte osec = 0;
byte sec_blink=0;
void Time(byte b) {
  char buffer[3];

  //const char* AMPM = 0;
  char* AMPM;
  readTime();

  r_hour = _hour;
  if (_hour > 12) {
    _hour -= 12;
    AMPM = " PM";
    s_AMPM = 1;
  }
  else {
    AMPM = " AM";
    s_AMPM = 0;
  }
  if (_hour == 0) _hour = 12;

  
  if (osec != _second) {
    osec = _second;

   if(sec_blink==0)sec_blink=1; else sec_blink=0;
/*
    Serial.print("mode: "); Serial.println(mode_s); Serial.print("sec: "); Serial.println(_secTimer);
    Serial.print("vent pos: "); Serial.println(vent_pos);
    if(mode_s==1){
    Serial.print("onTimer:"); Serial.println(_onTimer); Serial.print("offTimer:"); Serial.println(_offTimer);
    }
    else if(mode_s==2){
   Serial.print("on hour:"); Serial.print(on_hour); Serial.print(" on min:"); Serial.println(on_min);
   Serial.print("off hour:"); Serial.print(off_hour); Serial.print(" off min:"); Serial.println(off_min);
    }
*/
    if (mode_s == 1) {
      if (_secTimer != 0) {
        _secTimer--;
      }
      if ((vent_pos == 37) && (_secTimer == 0) && (_onTimer != 0)) {
        _onTimer--;
        _secTimer = 59;
      }
      if ((vent_pos == 12) && (_secTimer == 0) && (_offTimer != 0)) {
        _offTimer--;
        _secTimer = 59;
      }
    }

/*    if(b==7){
        lcd.setCursor(5, 1);
        if (_hour < 10)lcd_p(30);
        lcd.print(_hour); lcd_p(32);
        
        if (_minute < 10)lcd_p(30);
        lcd.print(_minute); lcd_p(32); if (m == 0)lcd.print(_second); lcd.print(AMPM); lcd_p(43); 
    }
*/
    if ((mode_s == 0)||(mode_s==2))
      if ((m == 0) || (b == 8)) {
        if (mode_s==2)lcd.setCursor(10, 1);
        else if(m == 0)
          lcd.setCursor(2, 1);
        if (_hour < 10)lcd_p(30);
        lcd.print(_hour); if(sec_blink==1)lcd_p(32); else lcd_p(43);
        //sprintf(buffer, "%02d", _minute);
        if (_minute < 10)lcd_p(30);
        lcd.print(_minute);// lcd_p(32); if (m == 0)lcd.print(_second);
        lcd.print(AMPM); lcd_p(43);
      }
  }/*
  Serial.print(_hour);
  Serial.print(":");
  //sprintf(buffer, "%02d", _minute);
  Serial.print(buffer); //lcd_p(32);
  Serial.print(_second);
  //Serial.println(_minute);
  Serial.println(AMPM); */
}
void readTime() {
  Wire.beginTransmission(DS1307);
  Wire.write(byte(0));
  Wire.endTransmission();
  Wire.requestFrom(DS1307, 7);
  _second = bcdToDec(Wire.read() & 0x7f); //sec
  _minute = bcdToDec(Wire.read()); //minute
  _hour = bcdToDec(Wire.read() & 0x3f);  //does 0x3f  hour
  /* _second = bcdToDec(Wire.read()); //weekday
   _second = bcdToDec(Wire.read()); //monthday
   _second = bcdToDec(Wire.read()); //month
   _second = bcdToDec(Wire.read()); //year
  */
  Wire.read(); Wire.read(); Wire.read(); Wire.read();
}



