#include <Ethernet.h>
// Configuracion del Ethernet Shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Direccion MAC
byte ip[] = { 192,168,1,100 }; // Direccion IP del Arduino
byte server[] = { 192,168,1,5 }; // Direccion IP del servidor
EthernetClient client; 

#include "BMP280.h"
#include <SPI.h>
#include <Wire.h>
#define P0 1021.8

 float h; //Se lee la humedad
 float tempDHT;

  
#include "DHT.h" //cargamos la librería DHT
#define DHTPIN 2 //Seleccionamos el pin en el que se conectará el sensor
#define DHTTYPE DHT22 //Se selecciona el DHT22(hay otros DHT)
DHT dht(DHTPIN, DHTTYPE); //Se inicia una variable que será usada por Arduino para comunicarse con el sensor

//LM35
const int pinLM35 = A1;

//BMP280
BMP280 bmp;

// LDR
const int rOscuridad = 1000;     //Resistencia en oscuridad KO
const int rLuz = 15;        //Resistencia a la luz (10 Lux) KO
const int rCal = 10;       //Resistencia calibracion KO
const int LDRPin = A3;   //Pin del LDR

int VLDR;
int ilum;

//TWITTER
#include <Twitter.h>
Twitter twitter("734366065271144448-7BnNdJwKfbQlxXLBxRnds5gRDNviHuJ");
char msgtwitter[120];
char str_temp[6];
char str_tempBMP[6];
char str_presion[6];
char str_humedad[6];

void setup() {
  Serial.begin(9600); //Se inicia la comunicación serial 
  dht.begin(); //Se inicia el sensor
  bmp.begin();    //Begin the sensor
  bmp.setOversampling(4);
  
  Ethernet.begin(mac, ip); // Inicializamos el Ethernet Shield
  delay(1000); // Esperamos 1 segundo de cortesia
  Serial.print("Humedad"); 
  Serial.print("\t");
  Serial.print(" Temp");
  Serial.print("\t");
  Serial.print("DHT\tLM35\tBMP280");
  Serial.print("\t\t");
  Serial.print("Presion");
  Serial.print("\t\t");
  Serial.print("Altitud");
  Serial.print("\t\t");
  Serial.println("Luz");
}
void loop() {

  /*------------------------------------------------------
   *                     DHT22
   *-----------------------------------------------------*/
  h = dht.readHumidity(); //Se lee la humedad
  tempDHT = dht.readTemperature(); //Se lee la temperatura
  //Se imprimen las variables

  /*------------------------------------------------------
   *                     LDR
   *-----------------------------------------------------*/

   VLDR = analogRead(LDRPin);         

   ilum = ((long)VLDR*rOscuridad*10)/((long)rLuz*rCal*(1024-VLDR));
  


  /*------------------------------------------------------
   *                     BMP280
   *-----------------------------------------------------*/
   
  double tempBMP,P,A;
  char result = bmp.startMeasurment();
 
  if(result!=0){
    result = bmp.getTemperatureAndPressure(tempBMP,P);
    
      if(result!=0)
      {
        A = bmp.altitude(P,P0);
       
      }
      else {
        Serial.println("Error.");
      }
  }
  else {
    Serial.println("Error.");
  }

  

  /*------------------------------------------------------
   *                     LM35
   *-----------------------------------------------------*/
   
  int value = analogRead(pinLM35);
  float millivolts = (value / 1024.0) * 5000;
  float tempLM35 = millivolts / 10; 
  

  /*------------------------------------------------------
   *             REPRESENTACION DATOS
   *-----------------------------------------------------*/
   
  Serial.print(h);Serial.print("\t\t");
  Serial.print(tempDHT); Serial.print("\t");
  Serial.print(tempLM35);Serial.print("\t");
  Serial.print(tempBMP,2);Serial.print("\t\t");
  Serial.print(P,2);Serial.print("\t\t");
  Serial.print(A,2);Serial.print("\t\t");
  Serial.println(ilum); 
  
  /*------------------------------------------------------
   *             ENVIO A LA BASE DE DATOS
   *-----------------------------------------------------*/

Serial.println("Connecting...");
 if (client.connect(server, 80)>0) {  // Conexion con el servidor
    client.print("GET /estacion/conexion.php?tempLM35="); // Enviamos los datos por GET
    client.print(tempLM35);
    client.print("&tempBMP=");
    client.print(tempBMP,2);
    client.print("&tempDHT=");
    client.print(tempDHT);
    client.print("&altitud=");
    client.print(A,2);    
    client.print("&luz=");
    client.print(ilum);
    client.print("&humedad=");
    client.print(h);
    client.print("&presion=");
    client.print(P,2);    
    client.println(" HTTP/1.0");
    client.println("User-Agent: Arduino 1.0");
    client.println();
    Serial.println("Conectado");
  } else {
    Serial.println("Fallo en la conexion");
  }
  if (!client.connected()) {
    Serial.println("Disconnected!");
  }
  client.stop();
  client.flush();


  //TWITTER
  //Convertir a strings los floats para poder meterlos en el mensaje de twitter
  dtostrf(tempBMP, 4, 2, str_tempBMP); //Poner arriba de todos los dtostrf uno que no se vaya a usar porque sino no funciona bien -> bug arduino?
  dtostrf(P, 4, 2, str_presion);
  dtostrf(tempDHT, 4, 2, str_temp);
  dtostrf(h, 4, 2, str_humedad);
  sprintf(msgtwitter, "Temperatura: %s ºC  |  Humitat: %s%%   |  Presió: %s mbar", str_temp, str_humedad, str_presion);
  Serial.println(msgtwitter);
  Serial.println(tempDHT);
  Serial.println(str_temp);

  Serial.println("CONNECTING TWITTER ...");
  if (twitter.post(msgtwitter)) {
    // Specify &Serial to output received response to Serial.
    // If no output is required, you can just omit the argument, e.g.
    // int status = twitter.wait();
    int status = twitter.wait(&Serial);
    if (status == 200) {
      Serial.println("OK.");
    } else {
      Serial.print("failed : code ");
      Serial.println(status);
    }
  } else {
    Serial.println("connection failed.");
  }

  
  delay(300000); //Se espera 2 segundos para seguir leyendo //datos

}
