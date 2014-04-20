/*  Skyon Colaborative Platform and Weather Station, consists of three key parts: 
    (1) reading equipment of meteorological data, 
    (2) central data processing and data mining, and most importantly, 
    (3) arbitrary community participation.
    Copyright (C) 2014 Antonio Fernandes, Lidiane Monteiro, Palloma Oliveira, Yelken Heckman

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

//Prototype code

//Library
#‪include‬ <Client.h> //server that provides the page to user
#include <Ethernet.h> //control the packets that the Network Interface Card (NIC) receives
#include <Server.h> //manage the server data
#include <Udp.h> //sends packets that do not need answers 
#include <SPI.h>


//statement of the constants 
#define‬ echoPin 5 //constant for altitude 
#define trigPin 4 //constant for altitude
const float CELSIUS_BASE = 0.4887585532746823069403714565; //graus conversion base 
const int LM35 = A0; // analog pin which will be connected to pin 2 of LM35

//statement of the variables
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 25, 10 };
EthernetServer server(80);
int conta_caracter=0 ;
int max_linha = 80; 
String linha_de_entrada = String(max_linha) ;
boolean LEDON = false ;
const int ledUmRele = 7;
const int ledDoisRele = 6;
int temperatura;

void setup()
{
  Serial.begin(9600) ;
  Ethernet.begin(mac, ip);
  server.begin();
  pinMode(ledUmRele, OUTPUT);
  pinMode(ledDoisRele, OUTPUT);
  pinMode(echoPin,INPUT);
  pinMode(trigPin, OUTPUT);
}


void loop() {
    EthernetClient client = server.available();
    if (client) {

        boolean current_line_is_blank = true;
        conta_caracter = 0;
        linha_de_entrada = "";

        while (client.connected()) {
            if (client.available()) {
                // receives a character sent by the browser
                char c = client.read();
		//if the line did not reach the maximum storage then adds input line character
                if (linha_de_entrada.length() < max_linha) {
                    linha_de_entrada.concat(c);
                }
		/*If has received a linefeed character - LF and the line is blank, the http request ended. 
		So it is possible to start sending the response */

                if (c == '\n' && current_line_is_blank) {
                    //send a standard response for http header received 
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println();
                    //starts sending the form 
                    client.print("<html>") ;
		    client.print("<body bgcolor = \"‪#‎FF8800‬\">");
		    client.println("<h1><center><font color=\"‪#‎FFFFFF‬\" size=\"10\">SkyON</font></center></h1>");
		    client.println("<h3><center><font color=\"#FFFFFF\" size=\"5\">Team</font></center></h3><hr/>");
		    client.println("Yelken Heckman");
		    client.println("Antonio Fernandes");
		    client.println("Maria Palloma");
		    client.println("Lidiane Monteiro");
		    client.println("<hr/>");

                    client.println("<h3><center><font color=\"#FFFFFF\" size=\"5\">Data collected in the prototype</font></center></h3>");
		    temperatura = analogRead(0); 
		    client.print("<h3> Temperature: ");
		    client.print(readTemperature());
		    client.print("</h3>");
		    digitalWrite(trigPin, LOW);
		    delayMicroseconds(2);
		    digitalWrite(trigPin, HIGH);
		    delayMicroseconds(10);
		    digitalWrite(trigPin, LOW);
		    long duration = pulseIn(echoPin, HIGH);
		    long distancia = duration / 29 /2;

                    client.print("<h3> Altitude: ");
		    client.print(distancia);
		    client.print("m");
		    client.print("</h3>"); 
		    client.print("<h3> Strong wind: ");
	            client.print(digitalRead(8));
                    client.print("</h3>");
		    client.print("</h3>"); 
		    client.print("<h3> Luminosity: ");
		    client.print(analogRead(1));
		    client.print("</h3>");

		    client.println("<hr/>");
		    client.println("<h3><center><font color=\"#FFFFFF\" size=\"5\">Control of the sensors</font></center></h3>");
		    client.println("<br>") ;
		    client.println("<h2>SENSOR 1</h2>");
		    client.println("<br>") ;
		    client.println("<form method=get name=LED>") ;
		    client.println("LIGA <input ") ;
		    /* checks the status of the LED and activates the radio button corresponding */
		    if(LEDON) {
			client.println("checked='checked'") ;
		    }

		    client.println("name='LED' value='ON' type='radio' >");
		    client.println("DESLIGAR <input ") ;
		    if(!LEDON) {
		      client.println("checked='checked'") ;
		    }
                    client.println("name='LED' value='OFF' type='radio' >");
                    // displays the form button
                    client.println(
                            "<br><br><br><hr/> <br><input type=submit value='UPDATE'></form>");

                    //Led 2
                    client.println("<br>");
                    client.println("<h2>SENSOR 2</h2>");
                    client.println("<br>");
                    client.println("<form method=get name=LED>");
                    client.println("LIGA <input ");
                    /* checks the status of the LED and activates the radio button corresponding */
                    if (LEDON) {
                        client.println("checked='checked'");
                    }
                    client.println("name='LEDDOIS' value='ON' type='radio' >");

                    client.println("DESLIGA <input ");
                    if (!LEDON) {
                        client.println("checked='checked'");
                    }
                    client.println("name='LEDDOIS' value='OFF' type='radio' >");
                    //displays the form button 
                    client.println(
                            "<br><br><br><hr/> <br><input type=submit value='UPDATE'></form>");
                    // End LED 2

                    client.println("</body>");
                    client.println("</html>");

                    break;
                }

                if (c == '\n') {
                    /*If the received character is a linefeed so we're starting to get a new line character 
analysis  here the content by submit */
                    if (linha_de_entrada.indexOf("GET") != -1) {
                        // If the line contains incoming GET and LED = ON and then save the status LED
                        if (linha_de_entrada.indexOf("LED=ON") != -1) {
                            digitalWrite(ledUmRele, LOW);
                            Serial.print('1');

                            LEDON = true;
                        }
                        if (linha_de_entrada.indexOf("LED=OFF") != -1) {
                            // If the line contains incoming GET and LED = ON and then save the status LED
                            digitalWrite(ledUmRele, HIGH);
                            Serial.print('0');
                            LEDON = false;
                        }

                        if (linha_de_entrada.indexOf("LEDDOIS=ON") != -1) {
                            digitalWrite(ledDoisRele, LOW);
                            Serial.print('1');

                            LEDON = true;
                        }
                        if (linha_de_entrada.indexOf("LEDDOIS=OFF") != -1) {
                            // If the line contains incoming GET and LED = ON and then save the status LED
                            digitalWrite(ledDoisRele, HIGH);
                            Serial.print('0');
                            LEDON = false;
                        }
                    }

                    current_line_is_blank = true;
                    linha_de_entrada = "";

                } else if (c != '\r') {
                    /*received character that is not linefeed or carriage return then we have received a character 
			and the input line is not empty*/
                    current_line_is_blank = false;
                }
            }
        }
        // give time for the browser receive the characters 
        delay(1);
        client.stop();
    }
}

float readTemperature() {
    return (analogRead(LM35) * CELSIUS_BASE);
}
