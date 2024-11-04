#include <Arduino.h>
#include <WiFi.h>              // Built-in
#include <SPI.h>

#include <ArduinoJson.h>
#include <AsyncTelegram2.h>
#include <SSLClient.h>  
#include "tg_certificate.h"
//#include "telegramINTI.h"
#include "hardware.h"
#include "defines.h"
/* Menu del Bot
light_on - Encender Led
light_off - Apagar Led
temperatura - Consultar Temperatura y Humedad
set_temp_hum - Setear Temperatura y Humedad
*/
/*const char* BOTtoken = "7108639657:AAFxG2An5kHBQNmGKaLIMsjFv_K60X0Lh-0"; // Colocar el Token de Telegram (De Nestor) INTI_MonitorTemperarura_1_bot
#define CHAT_ID -4127887228  // ID del grupo "INTI_MonitorTemperarura_1_bot" 
const char* channel = "@INTI_MonitorTemperarura_1_bot";*/
/*const char* BOTtoken = "7392446861:AAHl9FMTkgggG4XYomj3-lr4V7JuwDTTeMk"; // Colocar el Token de Telegram (De Nestor) INTI_02_MonitorTemperarura_bot
#define CHAT_ID -1004240459321  // ID del grupo "INTI_02_MonitorTemperatural" 
int64_t user_ID_salida = -4240459321;                         
const char* channel = "@INTI_02_bot"; */

/*Para saber el CHAT_ID: https://api.telegram.org/bot<YourBOTToken>/getUpdates*/

  String stringBOT; // =  "7108639657:AAFxG2An5kHBQNmGKaLIMsjFv_K60X0Lh-0";
//  const char* BOTtoken = (char*) stringBOT.c_str();
  const char* BOTtoken;
//#define CHAT_ID -1004127887228  // ID del grupo "INTI_MonitorTemperarura_1_bot" 
//int64_t user_ID_salida = -4127887228;

//**********************************************************************
//const char* channel = "@INTI_MonitorTemperarura_1_bot";
//**********************************************************************
extern float consultarTemperatura(void);
extern float consultarHumedad(void);

void mostrarConfiguracionActual(String, String, String, String, boolean);
void enviarTemperatura(float, float);

void imprimirPC(float, float);

const String botName;// = "@INTI_MonitorTemperarura_1_bot";
String botNombreString;// = botName;

WiFiClient base_client;
SSLClient INTI1_client(base_client, TAs, (size_t)TAs_NUM, A0, 1, SSLClient::SSL_ERROR);
AsyncTelegram2 INTI1_Bot(INTI1_client);
#include <time.h>
#define MYTZ "CET-1CEST,M3.5.0,M10.5.0/3"
//int64_t user_ID = CHAT_ID; 
int64_t chat_ID;// = CHAT_ID; 
//############ Reemplazar user_ID por chat_ID. Pasar "usuarioValido" al main

boolean usuarioValido;


float corregirTemperatura(float);
float corregirHumedad(float);
void setup_telegram(void){

  BOTtoken = (char*) stringBOT.c_str();

      configTzTime(MYTZ, "time.google.com", "time.windows.com", "pool.ntp.org");
      // Set the Telegram bot properies
      INTI1_Bot.setUpdateTime(2000);
      //INTI1_Bot.wifiConnect(ssid_INTI1, password_INTI1);
      INTI1_Bot.setTelegramToken(BOTtoken);

      INTI1_Bot.begin() ? Serial.println("OK") : Serial.println("Bot NOK");
      //***************************************
      char welcome_msg[128];
      snprintf(welcome_msg, 128, "BOT @%s en línea\nListo para funcionar.", INTI1_Bot.getBotName());

   
      // Send a message to specific user who has started your bot
    INTI1_Bot.sendTo(chat_ID, welcome_msg);
//    INTI1_Bot.sendTo(user_ID_salida, welcome_msg);  

}

void funcionPrueba(void){

  boolean estado_pulsador;
  static boolean flag_pulsador;
  estado_pulsador = digitalRead(PULSADOR_EXTERNO);
  if(estado_pulsador){
    flag_pulsador = true;
  }else
  {
    if(flag_pulsador){
//      INTI1_Bot.sendTo(user_ID_salida, "Presiono Pulsador ", "");
      INTI1_Bot.sendTo(chat_ID, "Presiono Pulsador ", "");
      flag_pulsador = false;
    }
  } 
}

String mensajesNuevosRecibidos(void){
  String mensajeNuevo = "";

  String bot_id = INTI1_Bot.getBotName();
  // local variable to store telegram message data
  TBMessage msg;

  // if there is an incoming message...
  if (INTI1_Bot.getNewMessage(msg)) {    
    Serial.print("Nombre chatID: ");
    Serial.println(msg.chatId);
    
    /*int64_t chatIDcorregido = msg.chatId - 1000000000000; // Se resta 1000000000000 para agregar el 100 al inicio
    Serial.print("Nombre chatID corregido: ");
    Serial.println(chatIDcorregido );*/
    //if(chatIDcorregido == user_ID)   Serial.println("Son iguales: ");

    /*String message ;
    message += "Mensage de @";
    message += INTI1_Bot.getBotName();
    message += ": ";
    message += msg.text;
    Serial.println(message);*/

   // if (chatIDcorregido == chat_ID)  
    if (msg.chatId == chat_ID)  // 
    {
      Serial.println("Usuario valido !!!");
      usuarioValido = true;
    }else
    {
      usuarioValido = false;
      INTI1_Bot.sendTo(msg.chatId, "Usuario NO autorizado");
      INTI1_Bot.sendTo(chat_ID, "Usuario NO autorizado intentó ingresar");
      INTI1_Bot.sendTo(chat_ID, "ID rechazado: ");
      String idRechazado = String(msg.chatId);
      INTI1_Bot.sendTo(chat_ID, idRechazado);
    }
    if(usuarioValido){
      if((msg.text == "/light_on")||(msg.text =="/light_on" + botNombreString)){
        INTI1_Bot.sendTo(chat_ID, "Led encendido");
      }
     if((msg.text == "/light_off")||(msg.text =="/light_off" + botNombreString)){  
        INTI1_Bot.sendTo(chat_ID, "Led apagado");
      }
    }
    mensajeNuevo = msg.text;
  }
  //******************

  //******************
  return mensajeNuevo;
}

void imprimirPC(String temp2, String humed2){
      Serial.println();
      Serial.print("Temperatura: ");
      Serial.print(temp2);
      Serial.print("        ");
      Serial.print("Humedad: ");
      Serial.print(humed2);
      Serial.println();
  }

float corregirTemperatura(float temp){
  float tempeCorregida;
  float m = 1.01556987;
  float b = -0.27751757;

  tempeCorregida = m*temp + b;
  return tempeCorregida;
}
float corregirHumedad(float humed){
  float humedCorregida;
  float m = 0.69762253;
  float b = 10.1214544;
  humedCorregida = m*humed + b;
  if(humedCorregida >= 99){
    humedCorregida = 99.9;
    Serial.println("Humedad > 100 %");
  } 
  return humedCorregida;  
}
void enviarTemperatura(float temp1, float humed1){

    /*float tempCorregida;
    float humedCorregida;
    tempCorregida = corregirTemperatura(temp);
    humedCorregida = corregirHumedad(humed);*/
    String stringTemp = String(temp1, 1);       //Interesan los valores con un solo decimal.
    String stringHumed = String(humed1, 1);
    imprimirPC(stringTemp,stringHumed);
    char temp_msg[32];
    char humed_msg[32];

    if(usuarioValido){
      snprintf(temp_msg, 32, "Temperatura: %s ºC", stringTemp);
      snprintf(humed_msg, 32, "Humedad: %s %", stringHumed);

      INTI1_Bot.sendTo(chat_ID, "Consulta Temperatura/Humedad");   
      delay(200);
  //    INTI1_Bot.sendTo(user_ID_salida , temp_msg);   
      INTI1_Bot.sendTo(chat_ID, temp_msg);   
      delay(200);
      INTI1_Bot.sendTo(chat_ID, humed_msg); 
      delay(200);
    }
//      snprintf(tempHumed_msg, 128, "BOT @%s en línea\nListo para funcionar.", INTI1_Bot.getBotName());
}

void enviarSetTemperaturasMaxMin(float tempMax, float tempMin){
    String stringTempMax = String(tempMax);
    String stringTempMin = String(tempMin);
    
    char tempMax_msg[32];
    char tempMin_msg[32];
    snprintf(tempMax_msg, 32, "Temperatura Máxima: %s ºC", stringTempMax);
    snprintf(tempMin_msg, 32, "Temperatura Mínima: %s %", stringTempMin);
    imprimirPC(tempMax, tempMin);
    INTI1_Bot.sendTo(chat_ID, tempMax_msg);   
    INTI1_Bot.sendTo(chat_ID, tempMin_msg); 
    INTI1_Bot.sendTo(chat_ID, "Presione pulsador SET (en placa)para guargar cambios");     
}

void alertaTemperaturaMax(void){
   INTI1_Bot.sendTo(chat_ID, "ALERTA: TEMPERATURA ALTA");
}
void alertaTemperaturaBaja(void){
   INTI1_Bot.sendTo(chat_ID, "ALERTA: TEMPERATURA BAJA");
}
void temperaturaEnRango(void){
   INTI1_Bot.sendTo(chat_ID, "TEMPERATURA EN RANGO");
}

void alertaHumedadMax(void){
   INTI1_Bot.sendTo(chat_ID, "ALERTA: HUMEDAD ALTA");
}
void alertaHumedadBaja(void){
   INTI1_Bot.sendTo(chat_ID, "ALERTA: HUMEDAD BAJA");
}
void humedadEnRango(void){
   INTI1_Bot.sendTo(chat_ID, "HUMEDAD EN RANGO");
}

void temperaturaOscilandoEnAlta(void){
   INTI1_Bot.sendTo(chat_ID, "TEMPERATURA OSCILANDO EN ALTA");
}
void temperaturaOscilandoEnBaja(void){
   INTI1_Bot.sendTo(chat_ID, "TEMPERATURA OSCILANDO EN BAJA");
}
void humedadOscilandoEnAlta(void){
   INTI1_Bot.sendTo(chat_ID, "HUMEDAD OSCILANDO EN ALTO");
}
void humedadOscilandoEnBaja(void){
   INTI1_Bot.sendTo(chat_ID, "HUMEDAD OSCILANDO EN BAJA");
} 
void enviarMensageTelegram(String mensaje){
//   INTI1_Bot.sendTo(user_ID_salida, mensaje);
   INTI1_Bot.sendTo(chat_ID, mensaje);
}

void mostrarConfiguracionActual(String tempMax, String tempMin, String humMax, String humMin, boolean histeresis){
  String mensaje = "Temperatura Máxima: ";
  mensaje += tempMax;
  mensaje += " °C";
  INTI1_Bot.sendTo(chat_ID, mensaje);
  
  mensaje = "Temperatura Mínima: ";
  mensaje += tempMin;
  mensaje += " °C";
  INTI1_Bot.sendTo(chat_ID, mensaje);

  mensaje = "Humedad Máxima: ";
  mensaje += humMax;
  mensaje += " %";
  INTI1_Bot.sendTo(chat_ID, mensaje);

  mensaje = "Humedad Mínima: ";
  mensaje += humMin;
  mensaje += " %";
  INTI1_Bot.sendTo(chat_ID, mensaje);

  mensaje = "Histeresis Activa: ";
  if(histeresis == true){
  mensaje += " SI";
  }else{
  mensaje += " NO";
  }
  INTI1_Bot.sendTo(chat_ID, mensaje);
}
