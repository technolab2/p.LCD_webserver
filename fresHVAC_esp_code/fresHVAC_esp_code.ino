#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

// need this to be able to use the heap space function
extern "C" {
#include "user_interface.h"
}

//#include "send_progmem.h"
//#include "logo_png.h"
#include "index_html.h"

//int buff=0;
int wifi=0;
byte u=0;
byte on_hour = 13;
byte on_min = 14;
byte off_hour = 15;
byte off_min = 16;
byte _onTimer = 17;
byte _offTimer = 18;
byte vent_pos = 12;
byte _hour = 1;
byte _minute = 2;
byte _second = 3;
byte mode = 0;
byte fpwmlcd = 0;
long int oldT=0;
long int t;

String st, XML;




ESP8266WebServer server(80);
//const char* ssid="princegateway2";
//const char* password="A18152229a";

String ssid,password;


/*
String millis2time(){
  Serial.print("1"); delay(600);
 String val ="";
 val+= "Time: ";
 val+= (String)_hour + ":" + (String)_minute + ":" + (String)_second;
 val+= " Mode: " + (String)mode;
 val+= " Timer: On: " + (String)_onTimer + " Off: " + (String)_offTimer ;
 val+= " Schedual: On Time: " + (String)on_hour + ":" + (String)on_min;
 val+= " Off Time: " + (String)off_hour + ":" + (String)off_min;
 return val;
 
  String Time="";
  unsigned long ss;
  byte mm,hh;
  ss=millis()/1000;
  hh=ss/3600;
  mm=(ss-hh*3600)/60;
  ss=(ss-hh*3600)-mm*60;
  if(hh<10)Time+="0";
  Time+=(String)hh+":";
  if(mm<10)Time+="0";
  Time+=(String)mm+":";
  if(ss<10)Time+="0";
  Time+=(String)ss;
  return Time;
  
}
*/
/*
void handleWebsite(){
  buildWebsite();
  server.send(200,"text/html",webSite);
}
*/


void scane(){
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("0 networks found");
  else
  {
    //Serial.print("._.");
    Serial.print(n);
    Serial.println(" networks found");
    
    for (int i = 0; i < n; ++i)
     {
      // Print SSID and RSSI for each network found
    //  Serial.print(i + 1);
      Serial.print("(\"");
      Serial.print(WiFi.SSID(i));
      Serial.print("\"");
     // Serial.print(WiFi.RSSI(i));
     // Serial.print(")");
     // Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
     }
     //Serial.print(".+.");
  }
  
}

void read_eeprom(){
   wifi = EEPROM.read(0);

  if (wifi == 1)
  {
    int ssid_length = EEPROM.read(1); //Serial.print("ssid_length= "); Serial.println(ssid_length);
    int pass_length = EEPROM.read(2); //Serial.print("pass_length= "); Serial.println(pass_length);
   String esid;
   String epass = "";
    for (int i = 0; i < ssid_length; ++i)
    {
      esid += char(EEPROM.read(i + 3));
    }

    for (int i = 0; i < pass_length; ++i)
    {
      epass += char(EEPROM.read(i + 35));
    }
    
    Serial.println(esid);
    Serial.println(epass);
    ssid = esid;
    testWifi(esid, epass);
  }
  else
  Serial.println("ssid not saved");
}


void setup() {
  Serial.begin(9600); 
  EEPROM.begin(512);
  delay(3000);
  Serial.println("its on");
  delay(10);
  read_eeprom();
 
}



void efun(){
  delay(100);
   int data = Serial.available();
   Serial.println(data);
    char n;
    
    st = "";
   while(data >0 ){
//    Serial.println("you are in");
   // delay(600);
    if(Serial.available()>0){data--;
    n= Serial.read();
    if(n != '\n')
    {
      st += n;
     // Serial.println(st);
    }
    else break;
   }//if(n =='\n')break;
   }
   byte aa=st.indexOf('\"');

String u = st.substring(0,aa);
String p = st.substring(aa+1,st.length());
//Serial.println("user and pass: ");
Serial.println(u);
Serial.println(p);
if ( u.length() > 1 ) {
      // test esid 
      
       
      if ( testWifi(u,p) == 20 ) { 
        WiFi.mode(WIFI_STA);
         Serial.print(WiFi.localIP());
         

     EEPROM.write(0, 1);
    // Serial.println("writing eeprom ssid:");
    int sl = u.length();
    EEPROM.write(1, sl);
    for (int i = 0; i < sl; ++i)
    {
      EEPROM.write(3 + i, u[i]);
      // Serial.print("Wrote: ");
      // Serial.println(u[i]);
    }

    int pl = p.length();
    EEPROM.write(2, pl);
    //   Serial.println("writing eeprom pass:");
    for (int i = 0; i < pl; ++i)
    {
      EEPROM.write(35 + i, p[i]);
      //  Serial.print("Wrote: ");
      //  Serial.println(p[i]);
    }
    EEPROM.commit();
       return;
      }
  }
}

int testWifi(String u, String p) {
 // WiFi.mode(WIFI_STA);
 //     WiFi.disconnect();
      delay(400);
      WiFi.begin(u.c_str(), p.c_str());
  int c = 0;
//  Serial.println("Waiting for Wifi to connect");  
  while ( c < 20 ) {
    if (WiFi.status() == WL_CONNECTED) {
      server.on("/", handle_index);
      server.on("/logo.png", handle_logo);
//         server.on("/",handleWebsite);
     //    server.on("/xml",handleXML);
         server.on("/ajax.xml",handleXML);
         server.on("/save.html", handleSave);
         server.begin();
      return(20); } 
    delay(500);
    //Serial.print(WiFi.status());    
    c++;
  }
  Serial.print('X');
  return(10);
}
void wifi_status(){
if (WiFi.status() == WL_CONNECTED) {
  Serial.println(WiFi.localIP());
  }
  else 
  Serial.print('X');
}
void get_val(){
 delay(100);
 // int data = Serial.available();
//  Serial.println(data);
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
Serial.print('P'); //just execute after that function in order to avoid collosion, call fpwmlcd
}
void loop() {
  server.handleClient();
  if(Serial.available()>0)
  {
    char a =Serial.read();
   if(a == 'U') efun();
   else if(a == 'W')get_val();
   else{
   switch(a){
    case 'F' : {delay(12);fpwmlcd = Serial.read();} break;
    case '1' : Serial.println(WiFi.scanNetworks());break;
    case '2' : scane();break;
    case '3' : wifi_status();break;
   // case '\n' : Serial.println("get new line");break;
 
  //  default: {Serial.print("get unknown "); Serial.write(a); Serial.println(a);}
   }}
    
  // Serial.println(a);
  }
  
  t=millis();
  if( t > (oldT+57000)) { 
    oldT = t;
    Serial.print("V"); //geting web values
  }
}
int var=0;
void handleXML(){
 String req;
  req= server.arg("start_test");
//  message+=server.argName(i) + "\"" + server.arg(i) +  ";";
  if(req.equals("0"))
  Serial.print(req);
  else if(req.equals("1"))
  {
    Serial.print(req);
    Serial.write(server.arg(1).toInt());
    Serial.write(server.arg(2).toInt());
  }
  req= server.arg("action");
  if(req.equals("save_control_panel")){
  Serial.print('S');
  Serial.write(server.arg(1).toInt());
  Serial.write(server.arg(2).toInt());
  }
  /*
  if(req.equals("fan_speed")
  Serial.print(server.arg(i));
  for (uint8_t i=0; i<server.args(); i++){
//    if(server.arg(i).length()!= 0)
 //  {
    
    if(s.equals("start_test"))
     
     else if(server.arg("action").length()>2)
     Serial.print('S');
     else
     Serial.write(server.arg(i).toInt());
//  }
  }
  String req =server.arg("start_test");
 
  if(req.equals("1")){
    Serial.print('T');
    Serial.write(req.toInt());
    req =server.arg("fan_speed");
    Serial.write(req.toInt());  //Serial.print("Fan speed is" + req);
    req =server.arg("door_position");
    Serial.write(req.toInt());  //Serial.print("Door position is" + req);
  }
  else if(req.equals("0")){
    Serial.print('T');
    Serial.write(req.toInt());
    }

  req =server.arg("action");
  if(req.equals("save_control_panel")){
    Serial.print("SF");
    req =server.arg("fan_speed");
    Serial.write(req.toInt());
    req =server.arg("door_position");
    Serial.write(req.toInt());
  }*/
// buildXML();
int fspeed;
if(fpwmlcd>0)
fspeed = map(fpwmlcd, 10, 100, 100, 5571);
else
fspeed=0;
//=============================
XML="<?xml version='1.0'?><root><fan_speed>";
XML+= (String)fpwmlcd +"% @ " +(String)fspeed + "rpm";
XML+="</fan_speed><temperature>"; 
XML+=(String)var +"C | 73F";     //(String)var
XML+="</temperature><vent>";
switch(vent_pos){
  case 12: XML+="Open";break;  //on
  case 19: XML+="75%";break; //75%
  case 22: XML+="50%";break; //50%
  case 30: XML+="25%";break; //25%
  case 37: XML+="Close";break; //off
  default: XML+=(String)vent_pos;
}

XML+="</vent><time>";
XML+=(String)_hour + ":" +(String)_minute;
XML+="</time><mode>";
if(mode==1){ XML+= "Mode = Timer Loop, On Time: " + (String)_onTimer + "Minute(s) Off Time: " + (String)_offTimer + "Minute(s)";}
else if (mode==2){ XML+= "Mode = Real Clock, On Time: " + (String)on_hour + ":" + (String)on_min + ", Off Time: " + (String)off_hour + ":" + (String)off_min ;}
else if (mode==0) {XML+= "Mode = N/A"; }
XML+="</mode></root>";
/*
XML="<?xml version='1.0'?><root><fan_speed>";
XML+="91% @ 5800rpm";
XML+="</fan_speed><temperature>"; 
XML+=(String)var +"C | 73F";     //(String)var
XML+="</temperature><vent>";
XML+="Open";
XML+="</vent><time>";
XML+="10:59 PM";
XML+="</time><mode>";
XML+="91% @ 5800rpm";
XML+="</mode></root>";
*/
//===========================
  server.send(200,"text/xml",XML);
    WiFiClient client = server.client();
//   Serial.print("geting xml\r\nFree heap: ");// Serial.println(system_get_free_heap_size());
//  sendProgmem(client,XML);
client.print(XML);
var++;
}

void handle_index()
{
  server.send(200, "text/html", "");
  WiFiClient client = server.client();
  // Serial.print("Free heap: "); Serial.println(system_get_free_heap_size());
  sendindex(client);
  delay(200);
 // client.write(XML);
}


void handle_logo()
{
  server.send(200, "image/png", "");
  WiFiClient client = server.client();
//  unsigned long startTime=millis();
//  sendlogo(client);
client.write("");
//client.write(XML);
//  unsigned long endTime=millis();
//  Serial.print("time=");
//  Serial.println(endTime-startTime);
}
void handleSave()
{
  server.send(200,"text/xml","");
 /* String message;
  for (uint8_t i=0; i<server.args(); i++){
    int l=server.arg(i).length();
    if(server.arg(i).length()!=0){
     if(server.argName(i).equals(""){
      
     }
     
      
    }
    message += " " + server.argName(i) + ": " + server.arg(i) + ", " + (String)l +"\n";
  }*/
 // Serial.println(message);
  byte b=0;
  byte len=0;
  String req =server.arg("do_action");
  if(req.equals("save")){
    req =server.arg("Tab");// Serial.println("r do action");
    if(req.equals("mode")){   //mode tab
      req =server.arg("mode");// Serial.println("r mode");
      if(req.equals("time")){//Serial.println("r mode time");
        b=0; 
        if((server.arg("open_hh").length()>0) || (server.arg("open_mm").length()>0) || (server.arg("open_ss").length()>0)){
        Serial.print('t');  //set timmer
    //    req =server.arg("open_hh");
        if(server.arg("open_hh").length()>0){
          Serial.print('h');Serial.write(server.arg("open_hh").toInt());
        }
        
        if(server.arg("open_mm").length()>0){
          Serial.print('m');
        Serial.write(server.arg("open_mm").toInt());
        }
        
        if(server.arg("open_ss").length()>0){
          Serial.print('s');
        Serial.write(server.arg("open_ss").toInt());
        }
        
        if(server.arg("close_hh").length()>0){
          Serial.print('H');
        Serial.write(server.arg("close_hh").toInt());
        }
        
        if(server.arg("close_mm").length()>0){
         Serial.print('M');
        Serial.write(server.arg("close_mm").toInt()); 
        }
        
        if(server.arg("close_ss").length()>0){
         Serial.print('S');
        Serial.write(server.arg("close_ss").toInt()); 
        }
       Serial.print('\n'); 
      }
      }
      else if(req.equals("clock")){//Serial.println("r mode clock");
        b=1; 
        if((server.arg("open_hh").length()>0) || (server.arg("open_mm").length()>0) || (server.arg("close_hh").length()>0) || (server.arg("close_mm").length()>0)) {
        Serial.print('c');  //set schedual
        if(server.arg("open_hh").length()>0){
        Serial.print('h');
        Serial.write(server.arg("open_hh").toInt());  
        }
        
        if(server.arg("open_mm").length()>0){
          Serial.print('m');
        Serial.write(server.arg("open_mm").toInt());
        }
        
        if((server.arg("open_hh").length()>0) || (server.arg("open_mm").length()>0)) {
          req =server.arg("open_ampm");
          if(req.equals("AM"))
          Serial.print('a');
          else if(req.equals("PM"))
          Serial.print('p');
        
        }
        if(server.arg("close_hh").length()>0){
         Serial.print('H');
        Serial.write(server.arg("close_hh").toInt());  
        }
        
        if(server.arg("close_mm").length()>0){
        Serial.print('M');
        Serial.write(server.arg("close_mm").toInt());
        }
        
        if((server.arg("close_hh").length()>0) || (server.arg("close_mm").length()>0)){
         req =server.arg("close_ampm");
          if(req.equals("AM"))
          Serial.print('A');
          else if(req.equals("PM"))
          Serial.print('P'); 
        }
        Serial.print('\n');
      }
      }
      
    }
    else if(req.equals("time")){   //Time Tab
   //   Serial.println("r tab time");
      if((server.arg("time_hh").length()>0) || (server.arg("time_mm").length()>0)){
      Serial.print('T');
      if(server.arg("time_hh").length()>0){
        Serial.print('h');
        Serial.write(server.arg("time_hh").toInt());
      }
      
      if(server.arg("time_mm").length()>0){
       Serial.print('m');
      Serial.write(server.arg("time_mm").toInt()); 
      }
        req =server.arg("time_ampm");
       if(req.equals("AM"))
       Serial.print('a');
       else if(req.equals("PM"))
       Serial.print('p');
       Serial.print('\n');
      }
      
      }
       
    
    else if(req.equals("admin")){  //admin tab
      req =server.arg("pin");// Serial.println("r tab admin");
      if(server.arg("pin").length()>0){
      Serial.print("L" + req.substring(0,4) ); //send pin code
    }
  }

}
}
/*
String urldecode(const char *src)
{
  String decoded = "";
  char a, b;
  while (*src) {
    if ((*src == '%') &&
        ((a = src[1]) && (b = src[2])) &&
        (isxdigit(a) && isxdigit(b))) {
      if (a >= 'a')
        a -= 'a' - 'A';
      if (a >= 'A')
        a -= ('A' - 10);
      else
        a -= '0';
      if (b >= 'a')
        b -= 'a' - 'A';
      if (b >= 'A')
        b -= ('A' - 10);
      else
        b -= '0';

      decoded += char(16 * a + b);
      src += 3;
    } else if (*src == '+') {
      decoded += ' ';
      *src++;
    } else {
      decoded += *src;
      *src++;
    }
  }
  decoded += '\0';

  return decoded;
}
*/
/** Handle the WLAN save form and redirect to WLAN config page again */
/*
void handleWifiSave() {
  DEBUG_PRINT(F("WiFi save"));

  //SAVE/connect here
  _ssid = urldecode(server.arg("s").c_str());
  _pass = urldecode(server.arg("p").c_str());

  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.

  String head = HTTP_HEAD;
  head.replace("{v}", "Credentials Saved");
  server.sendContent(head);
  server.sendContent_P(HTTP_SCRIPT);
  server.sendContent_P(HTTP_STYLE);
  server.sendContent_P(HTTP_HEAD_END);
  
  server.sendContent_P(HTTP_SAVED);

  server.sendContent_P(HTTP_END);
  server.client().stop();
  
  DEBUG_PRINT(F("Sent wifi save page"));  
  
  connect = true; //signal ready to connect/reset

}
  */
