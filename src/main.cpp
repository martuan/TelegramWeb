#include <Arduino.h>
#include <WiFi.h>              // Built-in
#include <SPI.h>

#include <BluetoothSerial.h>
#include <Preferences.h>
#include "defines.h"

#include <Adafruit_Sensor.h>
#include "DHT.h"

#include "hardware.h"
#include "credenciales.h"
#include "constantes.h"
//#include "parser_setTemperatura.h"
#include "esp_task_wdt.h"

#include "Web.h"
#include <SPIFFS.h>

DHT dht(DHTPIN, DHTTYPE);

void setup_wifi(void);
boolean set_Temperatura(String);
//void set_Temperatura(String);
void manejadorTelegram(int, float, float);
void credencialesWifiEEprom(void);
boolean  wifiConectado;
void leerValoresGuardados(void);

void preferencias1_ini(void);

void pedirTokenTelegram_ini(void); // Token y CHAT_ID de Telegram
//void preferencias2_ini(void);  
void leerTokenTelegramGuardado(void);
Preferences preferencias2;
int64_t stringToInt64(String);

float consultarTemperatura(void);
float consultarHumedad(void);
//Blue Tooth *******************************************
BluetoothSerial SerialBT; //Para comunicación BlueTooth.
Preferences preferences; //Crea un objeto para guardar info en Flash memory
bool esperaDatoBT; //Si true el parser está esperando un dato.
                 //Si false se espera inParser 
String datoRecibido;
boolean bloquearMonitorTempHum = false;

extern String inParserBlueTooth;	// dato de entrada al parser
extern unsigned char estadoActualBT; 	// estado del parser
extern void IniParserBlueTooth(void);
extern void ParserBlueTooth(void);
//extern void IniParserBlueTooth(void);

int indice;//Debug.
#define CR 0x0D
String ssid_EEPROM; //Datos wifi
String psw_EEPROM;

//********************************************************
//Set Temperatura y Humedad
extern void IniParserSetTH(void);
extern unsigned char estadoActualSetTH; 	// estado del parser
extern String inParserSetTH;		    // dato de entrada al parser
extern void ParserSetTH(void);
bool esperaDatoSetTH;
extern boolean cambioTemperatura;
extern boolean cambioHumedad;
//********************************************************
//Telegram************************************************
//extern const String botName;
extern String stringBOT;
//extern const char* BOTtoken;
extern String botNombreString;
extern int64_t chat_ID;
extern boolean usuarioValido;
String mensajeNuevoGlobal;
extern void setup_telegram(void);
extern void funcionPrueba(void);
//extern void manejadorMensajesNuevos(void);
extern String mensajesNuevosRecibidos(void);
extern void enviarTemperatura(float, float);
extern void alertaTemperaturaMax(void);
extern void alertaTemperaturaBaja(void);
extern void temperaturaEnRango(void);
extern void alertaHumedadMax(void);
extern void alertaHumedadBaja(void);
extern void humedadEnRango(void);
extern void enviarMensageTelegram(String);
extern void enviarSetTemperaturasMaxMin(float, float);
extern float corregirTemperatura(float);
extern float corregirHumedad(float);
extern void temperaturaOscilandoEnAlta(void);
extern void temperaturaOscilandoEnBaja(void);
extern void humedadOscilandoEnAlta(void);
extern void humedadOscilandoEnBaja(void);
extern void mostrarConfiguracionActual(String, String, String, String, boolean);
extern Preferences preferencias1;
//*************************************************************************************************
extern String tempMax;
extern String tempMin;
extern String humMax;
extern String humMin;

float temperaturaMax;
float temperaturaMin;
float humedadMax;
float humedadMin;
boolean histeresis;

//*************************************************************************************************
void leerValoresGuardados(){
	preferencias1.begin("Set_Temp_Hum", true); //true = read only. Espacio para guardar y leer los límites de temperatura y humedad

	tempMax = preferencias1.getString("TempMax", "27");
	tempMin = preferencias1.getString("TempMin", "20" );
	humMax = preferencias1.getString("HumMax", "90");
	humMin = preferencias1.getString("HumMin", "60");
  histeresis = preferencias1.getBool("Histeresis", "false");

	preferencias1.end();
  temperaturaMax = tempMax.toFloat();
  temperaturaMin = tempMin.toFloat();
  humedadMax = humMax.toFloat();
  humedadMin = humMin.toFloat();
}

//*************************************************************************************************

void setup_wifi(){
 //     boolean wifiConectado;
      int cuenta = 0;
      // Nos conectamos a nuestra red Wifi
      Serial.println();
      Serial.print("Conectando a ssid: ");
      Serial.println(ssid);
      
      WiFi.mode(WIFI_STA);
      credencialesWifiEEprom();
      if(wifiConectado == false){//si no logró conectarse
      
        Serial.println("No es posible conectar a WiFi");
        wifiConectado = false;
        ESP.restart();
      }else{//si logró conectarse

        Serial.println("");
        Serial.println("Conectado a red WiFi!");
        Serial.print("Dirección IP: ");
        Serial.println(WiFi.localIP());
        cuenta = 0;
        while(cuenta < 5){
          digitalWrite(LED_EXTERNO, HIGH);
          delay(300);
          digitalWrite(LED_EXTERNO, LOW);
          delay(300);
          cuenta++;
        }
      }
}

void credencialesWifiEEprom(){
//  boolean wifiConectado = false;
  int cuenta = 0;
  preferences.begin("SerialBT_App", false); // false: read-write.
  // Verifica si es la primera vez que se corre el programa luego de grabar el uC.
  //Las variables ssid , psw no están configuradas por lo que no se conectará a WiFi.
  if (preferences.getUInt("PrimeraVez", 0) == 0) 
  {
    Serial.println("Configure SSID y Clave");
    // Levanta el flag indicando que ya se ejecutó una vez.
    preferences.putUInt("PrimeraVez", 1);
  }
  else
  {
    ssid_EEPROM = preferences.getString("SSID", "default");
    psw_EEPROM = preferences.getString("PSW", "default");
    //Conexión a la red WiFi
    //WiFi.begin(ssid_EEPROM.c_str(), psw_EEPROM.c_str());
    WiFi.begin(ssid, password);
    Serial.println("Conectando a WiFi ssid_EEPROM: " + ssid_EEPROM);
    while ((WiFi.status() != WL_CONNECTED) && cuenta < 10) 
    {
      Serial.print(".");
      delay(500);
      cuenta++;
    }
    if(cuenta>=10){
      wifiConectado = false;
      Serial.println("wifiConectado = false ");
    }else{
      wifiConectado = true;
    }
    Serial.println("SSID EEPROM: " + ssid_EEPROM);
    Serial.println("PSW EEPROM: " + psw_EEPROM);
  }
 // return wifiConectado;
}
 
void preferencias1_ini(){
  // Key to check
  //const char* TempMax;
  preferencias1.begin("Set_Temp_Hum", false); //false = read/write. Espacio para guardar y leer los límites de temperatura y humedad
    String myString = "";
  // Check if the key exists
  if (preferencias1.isKey("TempMax")) {
    String myString = preferencias1.getString("TempMax");
    Serial.println("Existe TempMax, valor: " + myString);
    myString = "";
  } else {
	  preferencias1.putString("TempMax", "30");
    Serial.println("Se creado TempMax ");
  }

  if (preferencias1.isKey("TempMin")) {
    String myString = preferencias1.getString("TempMin");
    Serial.println("Existe TempMax, valor: " + myString);
    myString = "";
  } else {
	  preferencias1.putString("TempMin", "15" );
    Serial.println("Se creado TempMin");
  }

  if (preferencias1.isKey("HumMax")) {
    String myString = preferencias1.getString("HumMax");
    Serial.println("Existe HumMax, valor: " + myString);
    myString = "";
  } else {
	  preferencias1.putString("HumMax", "95");
    Serial.println("Se creado HumMax");
  }

  if (preferencias1.isKey("HumMin")) {
    String myString = preferencias1.getString("HumMin");
    Serial.println("Existe HumMin, valor: " + myString);
  } else {
	  preferencias1.putString("HumMin", "65");
    Serial.println("Se creado HumMin");
  }

  if (preferencias1.isKey("Histeresis")) {
    boolean myHisteresis = preferencias1.getBool("Histeresis");
    if(myHisteresis == true){
          Serial.println("Existe Histeresis, True");
    }else{
          Serial.println("Existe Histeresis, False");
    }

  } else {
	  preferencias1.putBool("Histeresis", "false");
    Serial.println("Se creado Histeresis");
  }
	  preferencias1.end();
}
//***********************************************************************************************
int64_t stringToInt64(String str) {
  int64_t result = 0;
  bool negative = false;

  if (str.length() == 0) return 0;

  // Chequear si el número es negativo
  if (str[0] == '-') {
    negative = true;
    str.remove(0, 1);
  }

  for (int i = 0; i < str.length(); i++) {
    char c = str[i];
    if (c >= '0' && c <= '9') {
      result = result * 10 + (c - '0');
    } else {
      break;  // Salir si hay un carácter no numérico
    }
  }

  return negative ? -result : result;
}

void leerTokenTelegramGuardado(void){
  preferencias2.begin("Set_Token", false); //false = read/write. Espacio para guardar y leer el token y chat_id de Telegram
  String stringLeido = "";
  stringLeido = preferencias2.getString("tokenBotFlash","No_hay_tokenBot");
  stringBOT =  stringLeido;
  Serial.print("Token stringBOT leido: ");
  Serial.println(stringBOT);

  stringLeido = preferencias2.getString("nombreBotFlash","No_hay_nombreBot");
  botNombreString = stringLeido;
  Serial.print("nombreBOT leido: ");
  Serial.println(botNombreString);

  stringLeido = preferencias2.getString("chatIDBotFlash","No_hay_chatIDBot");
  chat_ID =  stringToInt64(stringLeido);
  Serial.print("chatIDBotFlash leido: ");
  Serial.println(chat_ID);

  preferencias2.end();
}
void pedirTokenTelegram_ini(void){
  preferencias2.begin("Set_Token", false); //false = read/write. Espacio para guardar y leer el token y chat_id de Telegram
  String entradaStringSerie = "";
  Serial.println("Ingrese el Token del Bot de Telegram: ");
  while (Serial.available() == 0) {    // Es para lectura del puerto serie
  }
  entradaStringSerie = Serial.readStringUntil('\r'); //Con '\n' no funciona
  preferencias2.putString("tokenBotFlash", entradaStringSerie);
  if (preferencias2.isKey("tokenBotFlash"))  Serial.println("Se creo tokenBotFlash ");
  else Serial.println(":( No se pudo crear tokenBotFlash ");
  entradaStringSerie = "";
  while (Serial.available() > 0) {      Serial.read();  }// Lee y descarta los datos
 
  Serial.println("Ingrese el nombre del Bot de Telegram: ");
  while (Serial.available() == 0) {    // Es para lectura del puerto serie
  }
  entradaStringSerie = Serial.readStringUntil('\r'); //Con '\n' no funciona
  preferencias2.putString("nombreBotFlash", entradaStringSerie);
  if (preferencias2.isKey("nombreBotFlash"))  Serial.println("Se creo nombreBotFlash ");
  else Serial.println(":( No se pudo crear nombreBotFlash ");
  entradaStringSerie = "";

  while (Serial.available() > 0) {      Serial.read();  }// Lee y descarta los datos
  Serial.println("Ingrese el Chat ID del Bot de Telegram: ");
  while (Serial.available() == 0) {    // Es para lectura del puerto serie
  }
  entradaStringSerie = Serial.readStringUntil('\r'); //Con '\n' no funciona
  preferencias2.putString("chatIDBotFlash", entradaStringSerie);
  if (preferencias2.isKey("chatIDBotFlash"))  Serial.println("Se creo chatIDBotFlash ");
  else Serial.println(":( No se pudo crear chatIDBotFlash ");
  entradaStringSerie = "";  

  while (Serial.available() > 0) {      Serial.read();  }// Lee y descarta los datos
  Serial.println("Fin de gravacion de valores del Bot de Telegram");
  Serial.println("");
  preferencias2.end();
}
//**********************************************************************************************
void setup() {
//  boolean wifiConectado;
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_EXTERNO, OUTPUT);
  pinMode(PULSADOR_EXTERNO, INPUT);
  pinMode(PULSADOR_INTERNO, INPUT);

   // Iniciar el Watchdog Timer
  esp_task_wdt_init(WDT_TIMEOUT, true); // Inicializa el WDT con un timeout de WDT_TIMEOUT segundos
  esp_task_wdt_add(NULL); // Agrega la tarea actual (loop) al WDT

  boolean estadoPulsador = digitalRead(PULSADOR_EXTERNO);
 // stringBOT =  "7108639657:AAFxG2An5kHBQNmGKaLIMsjFv_K60X0Lh-0";
  /*
  if(estadoPulsador == LOW){
    pedirTokenTelegram_ini();
  }
  */
  leerTokenTelegramGuardado();
  IniParserBlueTooth();

  digitalWrite(LED_BUILTIN, HIGH);

  SerialBT.begin("GUS1_BT"); // Set your device name here
//  Serial.println("Bluetooth inicializado");
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);    
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);

   if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    ESP.restart();
  }
  else {
    delay(500);
    Serial.println("SPIFFS mounted successfully");
  }

  setup_wifi();
//  String tempMax, 	tempMin ;
  if(wifiConectado == true){
    /*
    preferencias1_ini();
    leerValoresGuardados();
    setup_telegram();
    IniParserSetTH();
    */

  }else{
    Serial.println("No se pudo conectar al WiFi ");
  } 

  dht.begin();    //Inicializar sensor de temperatura

  server.begin();

  urls();

}

float consultarTemperatura(){
  float temperaturaCorregida, temp;
  temp = dht.readTemperature(); //Leemos la temperatura en grados Celsius
  temperaturaCorregida = corregirTemperatura(temp);
  return temperaturaCorregida;
}
float consultarHumedad(){
  float humedadCorregida, humed;
  humed = dht.readHumidity(); //Leemos la temperatura en grados Celsius
  humedadCorregida = corregirHumedad(humed);

  return humedadCorregida;
}

void manejadorTelegram(int numAlerta, float temp, float humed){
 
  static boolean habilitarValorasGuardados = false; // Sirve para que parser pase por el estado inestable Valores_Guardados
  const int botRequestDelay = 1000;
  static unsigned long lastTimeBotRan;
  funcionPrueba();
 
  boolean flagMensajeNuevo = false;
  boolean estadoPulsadorConfirmacion;
  static boolean seteando_temperatura = false;
  String mensajeNuevo = "";
  if (millis() > lastTimeBotRan + botRequestDelay)
  {
    mensajeNuevo = mensajesNuevosRecibidos();
    if(usuarioValido == false)  mensajeNuevo = "";
    mensajeNuevoGlobal = mensajeNuevo;
      if(mensajeNuevo != ""){
        Serial.print("Mensaje recibido: ");
        Serial.println(mensajeNuevo);

        flagMensajeNuevo = true;
      }else{
    //    seteando_temperatura = false;
        flagMensajeNuevo = false;
      }
    lastTimeBotRan = millis();       
  }

//  if((mensajeNuevo == "/set_temp_hum")||(mensajeNuevo =="/set_temp_hum@INTI_02_bot")){
  if((mensajeNuevo == "/set_temp_hum")||(mensajeNuevo =="/set_temp_hum" + botNombreString))
  {
    Serial.println("Valor seteando_temperatura: ");
    inParserSetTH = "Set_Temp";
    ParserSetTH();
    	Serial.print("Estado: ");
	    Serial.println(estadoActualSetTH);
    seteando_temperatura = true;
    mensajeNuevo = "";
  }
  if(seteando_temperatura == true){
    seteando_temperatura = set_Temperatura(mensajeNuevo);
   //mensajeNuevo = "";
    bloquearMonitorTempHum = true;
  }else{
    bloquearMonitorTempHum = false;
  }
  if(estadoActualSetTH == CONFIRMAR_CON_PULSADOR){
    if((cambioTemperatura == false) && (cambioHumedad == false)){ //Si entra en este estado sin haber realizado cambios no espera al pulsador.
        mensajeNuevo = "PulTrue";
//        seteando_temperatura = false;
        seteando_temperatura = set_Temperatura(mensajeNuevo);
        habilitarValorasGuardados = true;
    }
    estadoPulsadorConfirmacion = digitalRead(PULSADOR_EXTERNO);
    if(estadoPulsadorConfirmacion == LOW){
      mensajeNuevo = "PulTrue";
      Serial.println("inParserSetTH = PulTrue");
      delay(100);
      esperaDatoSetTH = false;
      seteando_temperatura = set_Temperatura(mensajeNuevo);
//      seteando_temperatura = false;
      habilitarValorasGuardados = true;
    }   
    if(habilitarValorasGuardados)
    {
      seteando_temperatura = set_Temperatura(mensajeNuevo); //Se debe llamar a la función para que actualice inParserSetTH antes de llamar al parser
      //ParserSetTH();
      habilitarValorasGuardados = false;
      seteando_temperatura = false;
    } 
  }
//  if((mensajeNuevo == "/light_on")||(mensajeNuevo =="/light_on@INTI_02_bot")){
  if((mensajeNuevo == "/light_on")||(mensajeNuevo =="/light_on" + botNombreString)){
    digitalWrite(LED_EXTERNO, HIGH);  
  }
  if((mensajeNuevo == "/light_off")||(mensajeNuevo =="/light_off"  + botNombreString)){
    digitalWrite(LED_EXTERNO, LOW);  
  }
  if((mensajeNuevo == "/temperatura")||(mensajeNuevo =="/temperatura" + botNombreString)){

    temp = consultarTemperatura();
    humed = consultarHumedad();
    enviarTemperatura(temp, humed);
  }
    if((mensajeNuevo == "/histeresis_on")||(mensajeNuevo =="/histeresis_on" + botNombreString)){
    preferencias1.begin("Set_Temp_Hum", false); 
    bool miBooleano = true;
 	  preferencias1.putBool("Histeresis", miBooleano);
    histeresis = preferencias1.getBool("Histeresis");
    preferencias1.end(); 
    if(histeresis == true)  enviarMensageTelegram("Histeresis ON");
  }
    if((mensajeNuevo == "/histeresis_off")||(mensajeNuevo =="/histeresis_off" + botNombreString)){
    preferencias1.begin("Set_Temp_Hum", false); 
    bool miBooleano = false;
 	  preferencias1.putBool("Histeresis", miBooleano);
    delay(500);
    histeresis = preferencias1.getBool("Histeresis");
    preferencias1.end(); 
    if(histeresis == false)      enviarMensageTelegram("Histeresis OFF");
  }
    if((mensajeNuevo == "/config_actual")||(mensajeNuevo =="/config_actual" + botNombreString)){
      mostrarConfiguracionActual(tempMax, tempMin, humMax, humMin, histeresis);
  }

  if(bloquearMonitorTempHum == false){
    if(numAlerta == TEMPERATURA_ALTA){
      alertaTemperaturaMax();
      delay(200);
      enviarTemperatura(temp, humed);
      numAlerta = SIN_ALERTA;
    }
    if(numAlerta == TEMPERATURA_BAJA){
      alertaTemperaturaBaja();
      delay(200);
      enviarTemperatura(temp, humed);
      numAlerta = SIN_ALERTA;
    }
    if(numAlerta == TEMPERATURA_EN_RANGO){
      temperaturaEnRango();
      delay(200);
      enviarTemperatura(temp, humed);
      numAlerta = SIN_ALERTA;
    }
    if(numAlerta == HUMEDAD_ALTA){
      alertaHumedadMax();
      Serial.println("Entro a: HUMEDAD_ALTA ");
      delay(200);
      enviarTemperatura(temp, humed);
      numAlerta = SIN_ALERTA;
    }
    if(numAlerta == HUMEDAD_BAJA){
      alertaHumedadBaja();
      delay(200);
      enviarTemperatura(temp, humed);
      numAlerta = SIN_ALERTA;
    }
    if(numAlerta == HUMEDAD_EN_RANGO){
      humedadEnRango();
      delay(200);
      enviarTemperatura(temp, humed);
      numAlerta = SIN_ALERTA;
    }

    if(numAlerta == TEMPERATURA_OSCILANDO_EN_ALTA){
      temperaturaOscilandoEnAlta();
      numAlerta = SIN_ALERTA;
    }
    if(numAlerta == TEMPERATURA_OSCILANDO_EN_BAJA){
      temperaturaOscilandoEnBaja();
      numAlerta = SIN_ALERTA;
    }
    if(numAlerta == HUMEDAD_OSCILANDO_EN_ALTA){
      humedadOscilandoEnAlta();
      numAlerta = SIN_ALERTA;
    }
    if(numAlerta == HUMEDAD_OSCILANDO_EN_BAJA){
      humedadOscilandoEnBaja();
      numAlerta = SIN_ALERTA;
    }
  }
}
//**************************************************************************
void leerBlueTooth(void){
  if(esperaDatoBT)
  {
    if (SerialBT.available())
    {
      esperaDatoBT = false; //ya se recibió el dato.
      datoRecibido = SerialBT.readStringUntil(0); //Lee Hasta el NULL.
      //Elimina los caracteres desde el \n en adelante.
      indice = datoRecibido.indexOf(CR);
      datoRecibido.remove(indice);
      inParserBlueTooth = "DATO";
      ParserBlueTooth(); //LLama al parser.
    }
  }
  else //Espera inParser.
  {
    if (SerialBT.available())
    {
      inParserBlueTooth = SerialBT.readStringUntil(0); //Lee Hasta el NULL.
      
      //Elimina los caracteres desde el \n en adelante.
      indice = inParserBlueTooth.indexOf(CR);
      inParserBlueTooth.remove(indice);
      ParserBlueTooth(); //LLama al parser
    }
    while( estadoActualBT & MASKINESTABLE )//Si el estado actual es inestable, llama al parser.
      ParserBlueTooth();    
  }
}
//**************************************************************************

boolean set_Temperatura(String mensajeNuevo){
  boolean seteando_temperatura;
  static unsigned long ultimoTiempoMillis = millis();
  
  const unsigned long timeOutDelay_Set_TH = 90000;
  if(esperaDatoSetTH){
    if(mensajeNuevo != ""){
      inParserSetTH = "DATO";      
            esperaDatoSetTH = false;
      ParserSetTH();
      Serial.print("Estado_1: ");
	    Serial.println(estadoActualSetTH);
      ultimoTiempoMillis = millis();
    }

  }else{
    if(mensajeNuevo != ""){
      inParserSetTH = mensajeNuevo;
      esperaDatoSetTH = false;
      Serial.print("Estado_2: ");
	    Serial.println(estadoActualSetTH);
      ParserSetTH();
      Serial.print("Estado_3: ");
	    Serial.println(estadoActualSetTH);
      ultimoTiempoMillis = millis();
    }    
  }
seteando_temperatura = true;
if (millis() > ultimoTiempoMillis + timeOutDelay_Set_TH)
  {
    if(estadoActualSetTH != INICIO)
    {
      inParserSetTH = "Tout";
      ParserSetTH();
      inParserSetTH = "DATO";
            Serial.print("Estado_Tout1: ");
      Serial.println(estadoActualSetTH);

      seteando_temperatura = false;
//      enviarMensageTelegram("Salió de set Temperatura/Humedad por Time Out");

    }
  }
  while( estadoActualSetTH & MASKINESTABLE )//Si el estado actual es inestable, llama al parser.
    {
      ParserSetTH(); 
      Serial.print("Estado_Tout2: ");
      Serial.println(estadoActualSetTH);
    }

return seteando_temperatura;
}

void monitorTemperatura(void){
    static boolean alertaTempAltaEnviada = false; //Bandera para que no este repitiendo Alerta en cada ciclo de programa
    static boolean alertaTempBajaEnviada = false; //Bandera para que no este repitiendo Alerta en cada ciclo de programa
    static boolean alertaHumAltaEnviada = false;  //Bandera para que no este repitiendo Alerta en cada ciclo de programa
    static boolean alertaHumBajaEnviada = false;  //Bandera para que no este repitiendo Alerta en cada ciclo de programa
    const int tiempoEntreMedicionTemperatura = 5000;
    static unsigned long ultimoMillis;
    //float temperaturaSinCorregir;
   // float humedadSinCorregir;
    float temperatura;
    float humedad;
  // Variables de Histeresis Inicio *****************
    const int tiempoDeHisteresis = 180000;
    static unsigned long ultimoMillisHisteresis;
    static boolean temperaturaAlta = false;
    static boolean temperaturaRango = false;
    static boolean temperaturaBaja = false;
    static int8_t tempeCruceAlAlta = 0;
    static int8_t tempeCruceAlBaja = 0;
    static boolean oscilandoTemperaturaEnAlta = false;
    static boolean oscilandoTemperaturaEnBaja = false;
    static boolean mostroMensajeOscilandoTemperaturaAlta = false;
    static boolean mostroMensajeOscilandoTemperaturaBaja = false;

    static boolean humedadAlta = false;
    static boolean humedadRango = false;
    static boolean humedadBaja = false;
    static int8_t humCruceAlAlta = 0;
    //static boolean humHisteresisEnAlta = false;
    static int8_t humCruceAlBaja = 0;
    //static boolean humHisteresisEnBaja = false;
    static boolean oscilandoHumedadEnAlta = false;
    static boolean oscilandoHumedadEnBaja = false;
    static boolean mostroMensajeOscilandoHumedadAlta = false;
    static boolean mostroMensajeOscilandoHumedadBaja = false;

  // Variables de Histeresis Fin *****************
    static boolean flagSerialStatic = true;
  if (millis() > ultimoMillis + tiempoEntreMedicionTemperatura)
  {
    //temperaturaSinCorregir = dht.readTemperature(); //Leemos la temperatura en grados Celsius
    //humedadSinCorregir = dht.readHumidity(); //Leemos la Humedad
    //****************************************** */
    //                  Serial.println("Entro  tiempoEntreMedicionTemperatura");
          /*String temperaturaString = "";
          boolean flagSerial = false;

          if(flagSerialStatic == true){
              Serial.println("Ingrese temperatura simulada:");
              flagSerialStatic = false;
          }
          //if(Serial.available()> 0){

            temperaturaString = Serial.readStringUntil('\r'); //Con '\n' no funciona
            temperatura = temperaturaString.toFloat();
            if(temperatura > 0){
              Serial.print("Temperatura simulada ingresada: "); 
              Serial.println(temperatura);
              flagSerialStatic = true;
              flagSerial = true; // Es para lectura del puerto serie
            }
        //  }
          delay(100);
          while (Serial.available() > 0) {    
            // Serial.println("Deberia limpiar puerto serie");  
            Serial.read();  }// Lee y descarta los datos*/           
            
    boolean  flagSerial = true;
    if(flagSerial == true)
  {
     
    //****************************************** */  
    temperatura = consultarTemperatura();
    humedad = consultarHumedad();

    if((temperatura > temperaturaMax)&&(alertaTempAltaEnviada == false)&&(oscilandoTemperaturaEnAlta == false)){
      if(bloquearMonitorTempHum == false) manejadorTelegram(TEMPERATURA_ALTA, temperatura, humedad);
      Serial.println("Alerta alta temperatura ");
      alertaTempAltaEnviada = true;
      alertaTempBajaEnviada = false;
    }
    if((temperaturaMin > temperatura)&&(alertaTempBajaEnviada == false)&&(oscilandoTemperaturaEnAlta == false)){
      if(bloquearMonitorTempHum == false) manejadorTelegram(TEMPERATURA_BAJA, temperatura, humedad);
      alertaTempBajaEnviada = true;
      alertaTempAltaEnviada = false;
      Serial.println("Alerta baja temperatura ");
       delay(100);
    }  
    if((temperaturaMin < temperatura)&&(temperatura < temperaturaMax)&&(oscilandoTemperaturaEnAlta == false)){
      if((alertaTempAltaEnviada == true) || (alertaTempBajaEnviada == true)){
        if(bloquearMonitorTempHum == false) manejadorTelegram(TEMPERATURA_EN_RANGO, temperatura, humedad);
        alertaTempAltaEnviada = false;
        alertaTempBajaEnviada = false;
        Serial.println("Temperatura en rango");
      }
    }
    //******************** */

    if((humedad > humedadMax)&&(alertaHumAltaEnviada == false)&&(oscilandoHumedadEnAlta == false)){
      if(bloquearMonitorTempHum == false) manejadorTelegram(HUMEDAD_ALTA, temperatura, humedad);
      Serial.println("Alerta alta humedad ");
      alertaHumAltaEnviada = true;
      alertaHumBajaEnviada = false;
    }
 
    if((humedadMin > humedad)&&(alertaHumBajaEnviada == false)&&( oscilandoHumedadEnBaja == false)){
      if(bloquearMonitorTempHum == false) manejadorTelegram(HUMEDAD_BAJA, temperatura, humedad);
      alertaHumBajaEnviada = true;
      alertaHumAltaEnviada = false;
      Serial.println("Alerta baja humedad ");
       delay(100);
    }  
    if((humedadMin < humedad)&&(humedad < humedadMax)&&( oscilandoHumedadEnBaja == false)){
      if((alertaHumAltaEnviada == true) || (alertaHumBajaEnviada == true)){
        if(bloquearMonitorTempHum == false) manejadorTelegram(HUMEDAD_EN_RANGO, temperatura, humedad);
        alertaHumBajaEnviada = false;
        alertaHumAltaEnviada = false;
        Serial.println("Humedad en rango");
      }
    }

    //***********   Histeresis Temperatura   */ 
    if(histeresis == true){   
      if(temperatura > temperaturaMax){
        if(((temperaturaRango == true)||(temperaturaBaja == true))&&(histeresis == true)){
          if(tempeCruceAlAlta < 5){
            tempeCruceAlAlta++;
            Serial.print("****** temperatura > temperaturaMax, tempeCruceAlAlta = ");
            Serial.println(tempeCruceAlAlta);
          }  
        } 
        temperaturaAlta = true;
        temperaturaRango = false;
        temperaturaBaja = false;
      }
      if(tempeCruceAlAlta == 4){
        oscilandoTemperaturaEnAlta = true;
        if((bloquearMonitorTempHum == false)&&(mostroMensajeOscilandoTemperaturaAlta == false)){
          manejadorTelegram(TEMPERATURA_OSCILANDO_EN_ALTA, temperatura, humedad);
          mostroMensajeOscilandoTemperaturaAlta = true;
          alertaTempAltaEnviada = false;
          alertaTempBajaEnviada = false;
        }
      }  

      if(tempeCruceAlAlta <= 2){
        oscilandoTemperaturaEnAlta =  false;
        mostroMensajeOscilandoTemperaturaAlta = false;
      } 

      if((temperatura < temperaturaMax)&&(temperatura > temperaturaMin)){
        temperaturaAlta = false;
        temperaturaRango = true;
        temperaturaBaja = false;
      }
      if(temperatura < temperaturaMin){
        if(((temperaturaRango == true)||(temperaturaAlta == true))&&(histeresis == true)){
          if(tempeCruceAlBaja < 5){
            tempeCruceAlBaja++;
            Serial.print("****** temperatura < temperaturaMin,  tempeCruceAlBaja =  ");
            Serial.println(tempeCruceAlBaja);
          }  
        } 
        temperaturaAlta = false;
        temperaturaRango = false;
        temperaturaBaja = true;
      }
      if(tempeCruceAlBaja == 4){
        oscilandoTemperaturaEnBaja = true;
        if((bloquearMonitorTempHum == false) && (mostroMensajeOscilandoTemperaturaBaja == false)){
          manejadorTelegram(TEMPERATURA_OSCILANDO_EN_BAJA, temperatura, humedad);
          mostroMensajeOscilandoTemperaturaBaja = true;
          alertaTempAltaEnviada = false;
          alertaTempBajaEnviada = false;   
        }
      }
      if(tempeCruceAlBaja <= 2){
        oscilandoTemperaturaEnBaja = false;    
        mostroMensajeOscilandoTemperaturaBaja = false;
      }
      //***********  Fin Histeresis Temperatura    */    

      //***********   Histeresis Humedad   */    
      if(humedad > humedadMax){
        if((humedadRango == true)||(humedadBaja == true)){
          if(humCruceAlAlta < 5){
            humCruceAlAlta++;
            Serial.println("****** humCruceAlAlta++ ");
          }  
        } 
        humedadAlta = true;
        humedadRango = false;
        humedadBaja = false;
      }
      if(humCruceAlAlta == 4){
      //  humHisteresisEnAlta = true;
        if((bloquearMonitorTempHum == false)&& (mostroMensajeOscilandoHumedadAlta == false)){
          manejadorTelegram(HUMEDAD_OSCILANDO_EN_ALTA, temperatura, humedad);
          mostroMensajeOscilandoHumedadAlta = true;
          oscilandoHumedadEnAlta = true;
          alertaHumAltaEnviada == false;
          alertaHumBajaEnviada == false;
        }
      }
      if(humCruceAlAlta <= 1){
        oscilandoHumedadEnAlta = false;
        mostroMensajeOscilandoHumedadAlta = false;
      }

      if((humedad < humedadMax)&&(humedad > humedadMin)){
        humedadAlta = false;
        humedadRango = true;
        humedadBaja = false;
      }
      if(humedad < humedadMin){
        if((humedadRango == true)||(humedadAlta == true)){
          if(humCruceAlBaja < 5)  humCruceAlBaja++;
        } 
        humedadAlta = false;
        humedadRango = false;
        humedadBaja = true;
      }
      if(humCruceAlBaja == 4){
      //     humHisteresisEnBaja = true;
        if((bloquearMonitorTempHum == false) && (mostroMensajeOscilandoHumedadBaja = false)){
          manejadorTelegram(HUMEDAD_OSCILANDO_EN_BAJA, temperatura, humedad);
          oscilandoHumedadEnBaja = true;
          mostroMensajeOscilandoHumedadAlta = true;
          alertaHumAltaEnviada == false;
          alertaHumBajaEnviada == false;      
        }      
      }
      if(humCruceAlBaja <= 1){
        oscilandoHumedadEnBaja = false;
        mostroMensajeOscilandoHumedadBaja = false;      
      }  
    }else{
      oscilandoHumedadEnBaja = false;
      }    
  }
    //***********  Fin Histeresis Humedad    */    
    //*************Ventana de tiempo de histeresis */
    if(histeresis == true){
      if((millis() > ultimoMillisHisteresis + tiempoDeHisteresis)){
        if(tempeCruceAlAlta > 0){
          tempeCruceAlAlta--;
          Serial.println("****** tempeCruceAlAlta-- ");
        }
        if(tempeCruceAlBaja > 0){
          tempeCruceAlBaja--;
          Serial.println("****** tempeCruceAlBaja-- ");
        }
        if(humCruceAlAlta > 0){
          humCruceAlAlta--;
          Serial.println("****** humCruceAlAlta-- ");
        }
        if(humCruceAlBaja > 0){
          humCruceAlBaja--;
          Serial.println("****** humCruceAlBaja-- ");
        }

        ultimoMillisHisteresis = millis();
      }
      // *********** Fin de ventana de tiempo
      ultimoMillis = millis();
   }
} 
}
/*void primerMensajeTemperatura(void){
    static boolean primerIngreso = true;
    if(primerIngreso == true){
    float temperatura1, humedad1;
    delay(1500);
    temperatura1 =  consultarTemperatura();
    humedad1 = consultarHumedad();
    enviarTemperatura(temperatura1, humedad1);
    primerIngreso = false;
    }
}*/
void estadoWifi(void){
  static int cont = 0;
  unsigned long tiempoEntreMedicionTemperatura = 5000; // 5 segundos
  static unsigned long ultimoMillis1 = millis();

  if (millis() > ultimoMillis1 + tiempoEntreMedicionTemperatura) {
        // Tomar acción en caso de timeout, por ejemplo, reiniciar el ESP32
      if((WiFi.status() != WL_CONNECTED)){
        //WiFi.begin(ssid_EEPROM.c_str(), psw_EEPROM.c_str());
        cont++;
      }else{
        cont = 0;
      }
      ultimoMillis1 = millis();
    }

  if(cont > 9) ESP.restart();
}
void loop(){
  int alerta = SIN_ALERTA;
  estadoWifi();
  leerBlueTooth();
  if(wifiConectado == true){
    manejadorTelegram(alerta, 0 , 0);
    //if(bloquearMonitorTempHum == false)  
    monitorTemperatura();
    //primerMensajeTemperatura();
  }
  esp_task_wdt_reset();
}
 


