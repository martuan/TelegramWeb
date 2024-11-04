#include "Web.h"
//#include "ESPAsyncWebServer.h"
#include <SPIFFS.h>
//#include "defines.h"

AsyncWebServer server(80);
boolean takeNewPhoto = false;
boolean fotoContinua = false;
int r = 0; 
int g = 0;
int b = 0;
boolean encenderRuleta = false;
boolean cambiarColoresForm = false;

// Photo File Name to save in SPIFFS



void urls(void){

  
  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/styles.css", "text/css");
  });
 
  server.on("/test.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/test.js", "text/javascript");
  });

  server.on("/login", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/login.html", "text/html");
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    cambiarColoresForm = false;
    request->send(SPIFFS, "/index.html", "text/html");
  });


  server.on("/cambiarColoresRGB", HTTP_ANY, [](AsyncWebServerRequest *request){


    cambiarColoresForm = true;
    int pin, valor;
    String valorStr;

    //request->send(SPIFFS, "/ESP32.html", "text/html");
    Serial.println("/cambiarColoresRGB");

    //if(request->args() > 0){//chequea que tenga al menos un argumento recibido
    if(request->hasArg("redComp") || request->hasArg("greenComp") || request->hasArg("blueComp")){//chequea que tenga al menos un argumento recibido
    //request->args()
      
      Serial.println("Tiene argumentos en la solicitud");

      for(int i=0;i<3;i++){
 
        AsyncWebParameter* param = request->getParam(i);
    
        Serial.print("Param name: ");
        Serial.println(param->name());
    
        Serial.print("Param value: ");
        Serial.println(param->value());
    
        Serial.println("------");

        
        Serial.printf("%s = %s\n",param->name(), param->value());

        valor = param->value().toInt();
        if(i == 0){
          r = valor;
        }
        if(i == 1){
          g = valor;
        }
        if(i == 2){
          b = valor;
        }
        
      }
      
    }

    request->send(SPIFFS, "/cambiarColoresRGB.html", "text/html");
    
  });



  server.on("/encenderRuletaRGB", HTTP_GET, [](AsyncWebServerRequest *request){

    Serial.println("Encender Ruleta");
    encenderRuleta = true;
    
    request->send(SPIFFS, "/index.html", "text/html");

  });

}

void notFound(AsyncWebServerRequest *request) { //rutina de atención para páginas solicitadas no definidas 
      //pathDirectory = ""; //resetea el directorio para una nueva navegación
    
    Serial.print("URL = ");
    Serial.println(request->url());
    if(request->url().endsWith(".ico")){
      Serial.println("se trata de favicon.ico");
    }else{
      Serial.println("URL no encontrada: no es favicon.ico");
      Serial.println("URL no encontrada, resetea pathDirectory y redirect a /");
      //pathDirectory = ""; //resetea el directorio para una nueva navegación
      request->redirect("/");                     //Toda página solicitada no definida se redirecciona a la raíz ("/")
    }
    //Serial.println("URL no encontrada");
    //request->redirect("/");                     //Toda página solicitada no definida se redirecciona a la raíz ("/")
}


void convertirParametrosRecibidos(String paramName, String paramValue, int *pin, int *valor){

  String pinName = {};
  pinName = paramName;
  pinName.remove(0, 4);

  Serial.printf("%s\n", pinName);

  //obtiene el número de pin desde un String recibido
  *pin = pinName.toInt();


  //obtiene el valor a asignar al pin desde un String recibido (true = 1 = HIGH, false = 0 = LOW)
  if(paramValue == "true"){
    *valor = 1;
  }else if(paramValue == "false"){
    *valor = 0;
  }

  Serial.printf("%i\n", *valor);

}



