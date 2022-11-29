/*
  ESP32 Remote WiFi PWM Control
*/
 
#include <WiFi.h>
 
static const int en_1 = 14;
static const int en_2 = 32;
static const int in_1 = 27;
static const int in_2 = 26;
static const int in_3 = 33;
static const int in_4 = 25;
static const int eye = 2;

const int freq = 2000;
const int ledChannel = 0;
const int resolution = 8;

 
// Network credentials
const char* ssid     = "TC8717TDA_2GEXT";
const char* password = "TC8717T9344DA";
 
// Web server on port 80 (http)
WiFiServer server(80);
 
// Variable to store the HTTP request
String header;
 
// Decode HTTP GET value
String valueString = String(5);
int dutyCycle1 = 0;
int dutyCycle2 = 0;
 
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
 
void setup() {

  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(en_1, ledChannel);
  ledcAttachPin(en_2, ledChannel);
  pinMode(in_1, OUTPUT);
  pinMode(in_2, OUTPUT);
  pinMode(in_3, OUTPUT);
  pinMode(in_4, OUTPUT);
  pinMode(eye, OUTPUT);  
  
 // Start serial
  Serial.begin(115200);
 
 
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}
 
void loop(){
  
  // Listen for incoming clients
  WiFiClient client = server.available();   
  
  // Client Connected
  if (client) {                             
    // Set timer references
    currentTime = millis();
    previousTime = currentTime;
    
    // Print to serial port
    Serial.println("New Client."); 
    
    // String to hold data from client
    String currentLine = ""; 
    
    // Do while client is connected
    while (client.connected() && currentTime - previousTime <= timeoutTime) { 
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
        
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK) and a content-type
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
 
            // Display the HTML web page
            
            // HTML Header
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            
            // CSS - Modify as desired
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial; margin-left:auto; margin-right:auto; }");
            client.println("button {font-size: 17px; border-radius: 12px; padding: 16px 32px; }");
            client.println(".slider { -webkit-appearance: none; width: 300px; height: 25px; border-radius: 10px; background: #ffffff; outline: none;  opacity: 0.7;-webkit-transition: .2s;  transition: opacity .2s;}");
            client.println(".slider::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 35px; height: 35px; border-radius: 50%; background: #ff3410; cursor: pointer; }</style>");
            
            // Get JQuery
            client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.6.0/jquery.min.js\"></script>");
                     
            // Page title
            client.println("</head><body style=\"background-color:#70cfff;\"><h1 style=\"color:#ff3410;\">PWM Robot Control</h1>");
            
            // Position display
            client.println("<h2 style=\"color:#ffffff;\">Position: <span id=\"pwmPos\"></span></h2>"); 
                     
            // Slider control
            client.println("<input type=\"range\" min=\"0\" max=\"255\" class=\"slider\" id=\"pwmSlider\" onchange=\"pwm(this.value)\" value=\""+valueString+"\"/>");


            // Buttons
            client.println("<table align=\"center\"><tr><td></td><td><button style=\"background: magenta;\" id=\"forward\">^</button></td></tr>");
            client.println("<tr><td><button style=\"background: magenta;\" id=\"left\"><</button></td>");
            client.println("<td><button style=\"background: magenta;\" id=\"blink\">O</button></td>");
            client.println("<td><button style=\"background: magenta;\" id=\"right\">></button></td></tr>");
            client.println("<tr><td></td><td><button style=\"background: magenta;\" id=\"backward\">v</button></td><td></td></tr></table>");

            
            // Javascript
            
            
            client.println("<script>var slider = document.getElementById(\"pwmSlider\");");
            client.println("var pwmP = document.getElementById(\"pwmPos\"); pwmP.innerHTML = slider.value;");
            client.println("slider.oninput = function() { slider.value = this.value; pwmP.innerHTML = this.value; }");
            client.println("$.ajaxSetup({timeout:1000}); function pwm(pos) { ");
            client.println("$.get(\"/?value=\" + pos + \"&\"); {Connection: close};}");
            client.println("$.ajaxSetup({timeout:1000}); $(\'#forward\').on(\'mousedown\', function(){ $.get(\'/forward\'); {Connection: close}; });");
            client.println("$.ajaxSetup({timeout:1000}); $(\'#forward\').on(\'mouseup\', function(){ $.get(\'/stop\'); {Connection: close}; });");
            client.println("$.ajaxSetup({timeout:1000});$(\'#backward\').on(\'mousedown\', function(){ $.get(\'/backward\'); {Connection: close}; });");
            client.println("$.ajaxSetup({timeout:1000}); $(\'#backward\').on(\'mouseup\', function(){ $.get(\'/stop\'); {Connection: close}; });");
            client.println("$.ajaxSetup({timeout:1000});$(\'#left\').on(\'mousedown\', function(){ $.get(\'/left\'); {Connection: close}; });");
            client.println("$.ajaxSetup({timeout:1000}); $(\'#left\').on(\'mouseup\', function(){ $.get(\'/stop\'); {Connection: close}; });");   
            client.println("$.ajaxSetup({timeout:1000});$(\'#right\').on(\'mousedown\', function(){ $.get(\'/right\'); {Connection: close}; });");
            client.println("$.ajaxSetup({timeout:1000}); $(\'#right\').on(\'mouseup\', function(){ $.get(\'/stop\'); {Connection: close}; });");
            client.println("$.ajaxSetup({timeout:1000}); $(\'#blink\').on(\'mousedown\', function(){ $.get(\'/on\'); {Connection: close}; });");
            client.println("$.ajaxSetup({timeout:1000}); $(\'#blink\').on(\'mouseup\', function(){ $.get(\'/off\'); {Connection: close}; }); </script>");             


            
            
            // End page
            client.println("</body></html>");     
            
            // GET data
            if(header.indexOf("GET /?value=")>=0) {
              dutyCycle1 = header.indexOf('=');
              dutyCycle2 = header.indexOf('&');
              
              // String with motor position
              valueString = header.substring(dutyCycle1+1, dutyCycle2);
              
              // Move servo into position
              ledcWrite(ledChannel, valueString.toInt());
              
              // Print value to serial monitor
              Serial.print("Val =");
              Serial.println(valueString); 
            }

          
           // The HTTP response ends with another blank line
            client.println();
            
            // Break out of the while loop
            break;
          
          } else { 
            // New lline is received, clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

          if (currentLine.endsWith("GET /forward")) {
                 digitalWrite(in_1, HIGH);
                 digitalWrite(in_2, LOW);
                 digitalWrite(in_3, HIGH);
                 digitalWrite(in_4, LOW);               
             }
          if (currentLine.endsWith("GET /backward")) {
                  digitalWrite(in_1, LOW);
                  digitalWrite(in_2, HIGH);
                  digitalWrite(in_3, LOW);
                  digitalWrite(in_4, HIGH);               
               }
          if (currentLine.endsWith("GET /stop")) {
                 digitalWrite(in_1, LOW);
                 digitalWrite(in_2, LOW);
                 digitalWrite(in_3, LOW);
                 digitalWrite(in_4, LOW);               
             }

           if (currentLine.endsWith("GET /left")) {
                 digitalWrite(in_1, LOW);
                 digitalWrite(in_2, HIGH);
                 digitalWrite(in_3, HIGH);
                 digitalWrite(in_4, LOW);               
             }
           if (currentLine.endsWith("GET /right")) {
                 digitalWrite(in_1, HIGH);
                 digitalWrite(in_2, LOW);
                 digitalWrite(in_3, LOW);
                 digitalWrite(in_4, HIGH);               
             }
             if (currentLine.endsWith("GET /on")) {
                  digitalWrite(eye, HIGH);               
              }
             if (currentLine.endsWith("GET /off")) {
               digitalWrite(eye, LOW);                
               }               
                   
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
