const char index_html[] PROGMEM = R"=====(
<html>
  <head>
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/2.1.3/jquery.min.js"></script>
    <!--[if lt IE 9]>
    <script src="https://oss.maxcdn.com/html5shiv/3.7.2/html5shiv.min.js"></script>
    <script src="https://oss.maxcdn.com/respond/1.4.2/respond.min.js"></script>
    <![endif]-->
  </head>
  <body>
    <script>
      (function($) {
        $(function() {
          var remote_server_url ="http://ny08.s7ny.com/~sky7/freshvac_com/xml/";// "http://ny08.s7ny.com/~sky7/freshvac_com/xml/";
          $("<link/>", {
             rel: "stylesheet",
             type: "text/css",
             href: remote_server_url + "styles.css"
          }).appendTo("head");
          $.get(remote_server_url + "content.php", function(html) {
            $('body').append(html);
            $("<script/>", {
               src: remote_server_url + "scripts.js"
            }).appendTo("head");
          });
        });
      })(jQuery);
    </script>
  </body>
</html>
)=====";
/*
String time_()
{
byte _hour = 1;
byte _minute = 2;
char* AMPM;
  if (_hour > 12) {
    _hour -= 12;
    AMPM = " PM";
  }
  else {
    AMPM = " AM";
  }
String s= _hour + ":" + _minute + " "+ AMPM; //11:59 PM
return s;
}

*/

/*
void handle_index()
{
  server.send(200, "text/html", "");
  WiFiClient client = server.client();
  // Serial.print("Free heap: "); Serial.println(system_get_free_heap_size());
  sendProgmem(client,index_html);
}
*/


//    const char huge_progmem[] PROGMEM = R"=====(...blablabla...)=====";
   // sendProgmem(WiFiClient client, byte b)
void sendindex(WiFiClient client)
    {

 // Serial.print("Sendingindex\r\n  ");// Serial.println(system_get_free_heap_size());
    char chunkUnit[2921];
    chunkUnit[2920] = '\0'; 
    size_t remaining_size = sizeof(index_html);
    PGM_P buf = index_html;
    while (buf != NULL && remaining_size > 0) {
      size_t chunkUnitLen = 1000;
      if (remaining_size < 1000) chunkUnitLen = remaining_size;
      memcpy_P((void*)chunkUnit, (PGM_VOID_P)buf, chunkUnitLen);
      buf += chunkUnitLen;
      remaining_size -= chunkUnitLen;
      client.write((const char*)chunkUnit, chunkUnitLen);
    }
    }
