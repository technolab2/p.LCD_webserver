#include <avr/pgmspace.h>
// Print a string from Program Memory directly to save RAM 
void p (const char * str)
{
  char c;
  if (!str) 
    return;
  while ((c = pgm_read_byte(str++)))
    lcd.print (c);
} // end of printProgStr

void lcd_p(int a){

const static char lcd0[] PROGMEM   ={"fresHVAC"};
const static char lcd1[] PROGMEM   ={"Main Menu"};
const static char lcd2[] PROGMEM   ={"WiFi Setup"};
const static char lcd3[] PROGMEM   ={"WiFi Status"};
const static char lcd4[] PROGMEM   ={"Clock Setup"};
const static char lcd5[] PROGMEM   ={"Reset"};
const static char lcd6[] PROGMEM   ={"Select Network:"};
//const static char lcd7[] PROGMEM   ={"<IP Address:"};
const static char lcd8[] PROGMEM   ={"Enter Password:"};
const static char lcd9 [ ] PROGMEM   ={"Connected"};
//const static char lcd10 [ ] PROGMEM   ={"<get_ip>"};
const static char lcd11 [ ] PROGMEM   ={"Set Clock"};
//const static char lcd12 [ ] PROGMEM   ={"Confirm Reset"};
//const static char lcd13 [ ] PROGMEM   ={"<Reset>"};
const static char lcd14 [ ] PROGMEM   ={"Mode"};
const static char lcd15 [ ] PROGMEM   ={"Fan Speed"};
const static char lcd16 [ ] PROGMEM   ={"Vent Positon"};
const static char lcd17 [ ] PROGMEM   ={"Web PIN"};
const static char lcd18 [ ] PROGMEM   ={"Confurm Reset"};
const static char lcd19 [ ] PROGMEM   ={"Set Time:"};

const static char lcd20 [ ] PROGMEM   ={"Are you Sure?"};
const static char lcd21 [ ] PROGMEM   ={"Timer Configured"};
const static char lcd22 [ ] PROGMEM   ={"Set On time"};
const static char lcd23 [ ] PROGMEM   ={"Set Off time"};
const static char lcd24 [ ] PROGMEM   ={"Web Configured"};
const static char lcd25 [ ] PROGMEM   ={"Searching"};
const static char lcd26 [ ] PROGMEM   ={"Network Found"};
const static char lcd27 [ ] PROGMEM   ={"No Network Found"};
const static char lcd28 [ ] PROGMEM   ={" On"};
const static char lcd29 [ ] PROGMEM   ={"Off"};
const static char lcd30 [ ] PROGMEM   ={"0"};
const static char lcd31 [ ] PROGMEM   ={"P"};
const static char lcd32 [ ] PROGMEM   ={":"};
const static char lcd33 [ ] PROGMEM   ={"A"};
const static char lcd34 [ ] PROGMEM   ={"Current:"};
const static char lcd35 [ ] PROGMEM   ={"RPM"};
const static char lcd36 [ ] PROGMEM   ={"%"};
const static char lcd37 [ ] PROGMEM   ={"Fan Speed set to"};
const static char lcd38 [ ] PROGMEM   ={"Time Configured"};
const static char lcd39 [ ] PROGMEM   ={"Select Mode:"};
const static char lcd40 [ ] PROGMEM   ={"Open Time "};
const static char lcd41 [ ] PROGMEM   ={"Close Time"};
const static char lcd42 [ ] PROGMEM   ={"Door Open Time"};
const static char lcd43 [ ] PROGMEM   ={" "};
const static char lcd44 [ ] PROGMEM   ={"Min"};
const static char lcd45 [ ] PROGMEM   ={"  "};
const static char lcd46 [ ] PROGMEM   ={"Door Close Time"};
const static char lcd47 [ ] PROGMEM   ={"Countdown Timer"};
const static char lcd48 [ ] PROGMEM   ={"Schedual Based "};
const static char lcd49 [ ] PROGMEM   ={"Vent Configured"};
const static char lcd50 [ ] PROGMEM   ={"Turn On Time "};
const static char lcd51 [ ] PROGMEM   ={"Turn Off Time"};
const static char lcd52 [ ] PROGMEM   ={"Enter Web PIN:"};
const static char lcd53 [ ] PROGMEM   ={"Not Connected!"};
const static char lcd54 [ ] PROGMEM   ={"Retry!"};
const static char lcd55 [ ] PROGMEM   ={"Connect?"};
const static char lcd56 [ ] PROGMEM   ={"Connecting to"};
const static char lcd57 [ ] PROGMEM   ={"50%"};
const static char lcd58 [ ] PROGMEM   ={"Press OK"};

const static char lcd59 [ ] PROGMEM   ={" 25% Open"};
const static char lcd60 [ ] PROGMEM   ={" 50% Open"};
const static char lcd61 [ ] PROGMEM   ={" 75% Open"};
const static char lcd62 [ ] PROGMEM   ={"100% Open"};
const static char lcd63 [ ] PROGMEM   ={" Close    "};
const static char lcd64 [ ] PROGMEM   ={"Timer OFF"};
const static char lcd65 [ ] PROGMEM   ={"Timer ON "};
const static char lcd66 [ ] PROGMEM   ={"Clock OFF"};
const static char lcd67 [ ] PROGMEM   ={"Clock ON "};
const static char lcd68 [ ] PROGMEM   ={"M"};/*
const static char lcd0 [ ] PROGMEM   ={" "};
const static char lcd0 [ ] PROGMEM   ={" "};
const static char lcd0 [ ] PROGMEM   ={" "};
*/

    switch(a){
      case 0: {p(lcd0);}break;
      case 1: {p(lcd1);}break;
      case 2: {p(lcd2);}break;
      case 3: {p(lcd3);}break;
      case 4: {p(lcd4);}break;
      case 5: {p(lcd5);}break;
      case 6: {p(lcd6);}break;
//      case 7: {p(lcd7);}break;
      case 8: {p(lcd8);}break;
      case 9: {p(lcd9);}break;
//      case 10: {p(lcd10);}break;
      case 11: {p(lcd11);}break;
//      case 12: {p(lcd12);}break;
//      case 13: {p(lcd13);}break;
      case 14: {p(lcd14);}break;
      case 15: {p(lcd15);}break;
      case 16: {p(lcd16);}break;
      case 17: {p(lcd17);}break;
      case 18: {p(lcd18);}break;
      case 19: {p(lcd19);}break;
      case 20: {p(lcd20);}break;
      case 21: {p(lcd21);}break;
      case 22: {p(lcd22);}break;
      case 23: {p(lcd23);}break;
      case 24: {p(lcd24);}break;
      case 25: {p(lcd25);}break;
      case 26: {p(lcd26);}break;
      case 27: {p(lcd27);}break;
      case 28: {p(lcd28);}break;
      case 29: {p(lcd29);}break;
      case 30: {p(lcd30);}break;
      case 31: {p(lcd31);}break;
      case 32: {p(lcd32);}break;
      case 33: {p(lcd33);}break;
      case 34: {p(lcd34);}break;
      case 35: {p(lcd35);}break;
      case 36: {p(lcd36);}break;
      case 37: {p(lcd37);}break;
      case 38: {p(lcd38);}break;
      case 39: {p(lcd39);}break;
      case 40: {p(lcd40);}break;
      case 41: {p(lcd41);}break;
      case 42: {p(lcd42);}break;
      case 43: {p(lcd43);}break;
      case 44: {p(lcd44);}break;
      case 45: {p(lcd45);}break;
      case 46: {p(lcd46);}break;
      case 47: {p(lcd47);}break;
      case 48: {p(lcd48);}break;
      case 49: {p(lcd49);}break;
      case 50: {p(lcd50);}break;
      case 51: {p(lcd51);}break;
      case 52: {p(lcd52);}break;
      case 53: {p(lcd53);}break;
      case 54: {p(lcd54);}break;
      case 55: {p(lcd55);}break;
      case 56: {p(lcd56);}break;
      case 57: {p(lcd57);}break;
      case 58: {p(lcd58);}break;
      case 59: {p(lcd59);}break;
      case 60: {p(lcd60);}break;
      case 61: {p(lcd61);}break;
      case 62: {p(lcd62);}break;
      case 63: {p(lcd63);}break;
      case 64: {p(lcd64);}break;
      case 65: {p(lcd65);}break;
      case 66: {p(lcd66);}break;
      case 67: {p(lcd67);}break;
      case 68: {p(lcd68);}break;

}
}
