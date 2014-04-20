/*
  Authors: 
  Antonio Fernandes, Lidiane Monteiro, Maria Palloma Oliveira, Yelken Heckman
*/

//Prototype code

//Library
#include <Client.h> 
#include <Ethernet.h>
#include <Server.h>
#include <Udp.h>
#include <SPI.h>
#define echoPin 5
#define trigPin 4
 
//Declaration of variables

byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 
  10, 160, 24, 99 };
EthernetServer server(80);
int conta_caracter=0 ;
int max_linha = 80; 
String linha_de_entrada = String(max_linha) ;
boolean LEDON = false ;
const int ledUmRele =  7;
const int ledDoisRele =  6;
int temperatura;

//function setup port ??
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

void loop()
{
  EthernetClient client = server.available();
  if (client) {

    boolean current_line_is_blank = true;
    conta_caracter=0 ;
    linha_de_entrada="" ;

    while (client.connected()) {
      if (client.available()) {
      
        // recebe um caracter enviado pelo browser
      
        char c = client.read();
       
        //if the line did not reach the maximum storage then adds input line character */
      
        if(linha_de_entrada.length() < max_linha) {
          linha_de_entrada.concat(c) ; 
        }  

        /*If has received a linefeed character - LF. 
        And the line is blank, the http request ended.
        So it is possible to start sending the response */

        if (c == '\n' && current_line_is_blank) {
          
          // envia uma resposta padrão ao header http recebido
          
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          // começa a enviar o formulário
          client.print("<html>") ;
          client.print("<body bgcolor = \"#FF8800\">");

          client.println("<h1><center><font color=\"#FFFFFF\" size=\"10\">SkyON</font></center></h1>"); 
          
          client.println("<h3><center><font color=\"#FFFFFF\" size=\"5\">Equipe</font></center></h3><hr/>");
          client.println("Yelken Heckman");
          client.println("Antonio Fernandes");
          client.println("Maria Palloma");
          client.println("Lidiane Monteiro");
          client.println("<hr/>");
          
          
          client.println("<h3><center><font color=\"#FFFFFF\" size=\"5\">Dados coletados no prototipo</font></center></h3>");
          temperatura = analogRead(0);          
          client.print("<h3> Temperatura: ");
          client.print(temperatura);
          
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
          client.print("<h3> Vento forte: ");
          client.print(digitalRead(8));
          client.print("</h3>");                  
       
          //          client.println("<br>") ;
          client.println("<hr/>");
          client.println("<h3><center><font color=\"#FFFFFF\" size=\"5\">Controle dos sensores</font></center></h3>");
          client.println("<br>") ;
          client.println("<h2>SENSOR 1</h2>");
          client.println("<br>") ;
          client.println("<form method=get name=LED>") ;

          client.println("LIGA <input ")  ;
          // verifica o status do led e ativa o radio button 
          // correspondente
          if(LEDON) {
            client.println("checked='checked'") ;
          }
          client.println("name='LED' value='ON' type='radio' >");

          client.println("DESLIGA <input ")  ;
          if(!LEDON) {
            client.println("checked='checked'") ;
          }
          client.println("name='LED' value='OFF' type='radio' >");
          // exibe o botão do formulário
          client.println("<br><br><br><hr/> <br><input type=submit value='ATUALIZA'></form>") ;
          
          //Led 2
          client.println("<br>") ;
          client.println("<h2>SENSOR 2</h2>");
          client.println("<br>") ;
          client.println("<form method=get name=LED>") ;
          client.println("LIGA <input ")  ;
          // verifica o status do led e ativa o radio button 
          // correspondente
          if(LEDON) {
            client.println("checked='checked'") ;
          }
          client.println("name='LEDDOIS' value='ON' type='radio' >");

          client.println("DESLIGA <input ")  ;
          if(!LEDON) {
            client.println("checked='checked'") ;
          }
          client.println("name='LEDDOIS' value='OFF' type='radio' >");
          // exibe o botão do formulário
          client.println("<br><br><br><hr/> <br><input type=submit value='ATUALIZA'></form>") ;
          // Fim Led 2
          
          
          client.println("</body>") ;
          client.println("</html>");

          break;
        }

        if (c == '\n') {
          // se o caracter recebido é um linefeed então estamos começando a receber uma 
          // nova linha de caracteres

          // Analise aqui o conteudo enviado pelo submit
          if(linha_de_entrada.indexOf("GET") != -1 ){
            // se a linha recebida contem GET e LED=ON enão guarde o status do led
            if(linha_de_entrada.indexOf("LED=ON") != -1 ){ 
              digitalWrite(ledUmRele, LOW);
              Serial.print('1');

              LEDON=true;
            }
            if(linha_de_entrada.indexOf("LED=OFF") != -1 ){ 
              // se a linha recebida contem GET e LED=OFF enão guarde o status do led
              digitalWrite(ledUmRele, HIGH);
              Serial.print('0');
              LEDON=false ;
            }
            
            if(linha_de_entrada.indexOf("LEDDOIS=ON") != -1 ){ 
              digitalWrite(ledDoisRele, LOW);
              Serial.print('1');

              LEDON=true;
            }
            if(linha_de_entrada.indexOf("LEDDOIS=OFF") != -1 ){ 
              // se a linha recebida contem GET e LED=OFF enão guarde o status do led
              digitalWrite(ledDoisRele, HIGH);
              Serial.print('0');
              LEDON=false ;
            }
          }

          current_line_is_blank = true;
          linha_de_entrada="" ;

        } 
        else if (c != '\r') {
          // recebemos um carater que não é linefeed ou retorno de carro 
          // então recebemos um caracter e a linha de entrada não está mais vazia
          current_line_is_blank = false;
        }
      }
    }
    // dá um tempo para  o browser receber os caracteres
    delay(1);
    client.stop();
  }
}
