#include <Arduino.h>
#include "defines.h"
#include "parser_setTemperatura.h"
#include <Preferences.h>


//Telegram************************************************
extern String mensajesNuevosRecibidos(void);
extern void enviarTemperatura(float, float);
extern void alertaTemperaturaMax(void);
extern void alertaTemperaturaBaja(void);
extern void temperaturaEnRango(void);
extern void enviarMensageTelegram(String);
extern void enviarSetTemperaturasMaxMin(float, float);
extern String mensajeNuevoGlobal;
//*************************************************************************************************
extern boolean esperaDatoSetTH;
// variables para el parser ------------------------------------------------------
String inParserSetTH;		    // dato de entrada al parser
unsigned char estadoActualSetTH; 	// estado del parser

// Funciones para máquina de estados. (Parser)
//
//**************************************************************************
String tempMax = "";
String tempMin = "";
String humMax  = "";
String humMin  = "";
boolean cambioTemperatura;
boolean cambioHumedad;
//extern Preferences preferences;
Preferences preferencias1;
void IniParserSetTH(void)
{
   	estadoActualSetTH = INICIO;
	inParserSetTH = "DEFAULT";

}
//void ParserComm(void)
void ParserSetTH(void)
{
  	//unsigned char codigoFamilia;

  	//codigoFamilia = OTRO;   // por ahora.
	estadoActualSetTH = estadoActualSetTH & ~MASKINESTABLE; //Quita posible inestabilidad.

	  
  	ptrEstadoParserSetTH = dirEstadoParserSetTH[estadoActualSetTH];

	while ( (ptrEstadoParserSetTH->entrada != inParserSetTH) && 
			(ptrEstadoParserSetTH->entrada != "DEFAULT") )
	{
		ptrEstadoParserSetTH++;//si no es la entrada buscada pasa a la otra.
	}
	estadoActualSetTH = ptrEstadoParserSetTH->proxEstado; //actualiza estado
	
	(*ptrEstadoParserSetTH->Accion) (); 			//realiza acción

}
//
// Nada()
//
void NadaSetTH()
{
	//No hace nada.
}

void AInicio(){
	preferencias1.begin("Set_Temp_Hum", true); //true = read only. Espacio para guardar y leer los límites de temperatura y humedad

	String mensaje = "";
	tempMax = preferencias1.getString("TempMax", "27");
	tempMin = preferencias1.getString("TempMin", "20" );
	humMax = preferencias1.getString("HumMax", "90");
	humMin = preferencias1.getString("HumMin", "60");
	preferencias1.end();
	mensaje = "Temperatura máxima guardada: " + String(tempMax) + "ºC";
	enviarMensageTelegram(mensaje);
	mensaje = "Temperatura mínima guardada: " + String(tempMin) + "ºC";
	enviarMensageTelegram(mensaje);
	mensaje = "Humedad máxima guardada: " + String(humMax) + " %";
	enviarMensageTelegram(mensaje);
	mensaje = "Humedad mínima guardada: " + String(humMin) + " %";
	enviarMensageTelegram(mensaje);
	Serial.println("Entró a AInicio");
	cambioTemperatura = false;
	cambioHumedad = false;
	
}
void ASetTemp(){
	AInicio();
	enviarMensageTelegram("¿Desea Configurar rango de temperatura?");
	enviarMensageTelegram("si/no");
	Serial.println("Entro a ASetTemp");
}
/*void AMensajeSetTempHum(){

}*/
void AEscribirTempMax(){
	enviarMensageTelegram("Escriba Temperatura Máxima: ");
	esperaDatoSetTH = true;
	Serial.println("Entró a AEscribirTempMax");
}
void AEscribirTempMin(){
	//tempMax = inParserSetTH;
	tempMax = mensajeNuevoGlobal;
	enviarMensageTelegram("Escriba Temperatura Mímima: ");
	Serial.println("Entro a AEscribirTempMin");
	esperaDatoSetTH = true;
	cambioTemperatura = true;
}
void AAlarmaPorHum(){
//	tempMin = inParserSetTH;
	tempMin = mensajeNuevoGlobal;
	enviarMensageTelegram("¿Desea Configurar rango de humedad?");
	enviarMensageTelegram("si/no");
		Serial.println("Entro a ASetHum");
	esperaDatoSetTH = false;
}

void AEscribirHumMax(){
	enviarMensageTelegram("Escriba Humedad Máxima: ");
	esperaDatoSetTH = true;
	Serial.println("Entro a AEscribirHumMax");
}
void AEscribirHumMin(){

	humMax = mensajeNuevoGlobal;
	enviarMensageTelegram("Escriba Humedad Mínima: ");
	Serial.println("Entro a AEscribirHumMin");
	esperaDatoSetTH = true;
	cambioHumedad = true;
}
void AConfirmarConPulsador(){
	humMin = mensajeNuevoGlobal;
	if(cambioTemperatura || cambioHumedad){
	enviarMensageTelegram("Presione Pulsador en Gabinete para confirmar... ");
	Serial.println("Entro a AConfirmarConPulsador");
	}
}
void AGuardarValores(){
	Serial.println("Entro a AGuardarValores");
	preferencias1.begin("Set_Temp_Hum", false); //false = read/write. Espacio para guardar y leer los límites de temperatura y humedad
	if(cambioTemperatura){
	preferencias1.putString("TempMax", tempMax);
	preferencias1.putString("TempMin", tempMin);
	}
	if(cambioHumedad){
	preferencias1.putString("HumMax", humMax);
	preferencias1.putString("HumMin", humMin);
	}
	preferencias1.end();
	if(cambioTemperatura || cambioHumedad){
	enviarMensageTelegram("Nuevos valores guardados!");
	ESP.restart();
	}else{
	enviarMensageTelegram("No hubo cambios!");		
	}
//	esperaDatoSetTH = true;
      inParserSetTH = "DATO";      

	Serial.print("Valor TempMax: ");
	Serial.println(tempMax);
	Serial.print("Valor TempMin: ");
	Serial.println(tempMin);
}
void ATimeOut(){
	Serial.print("Salida por Time Out ");
	enviarMensageTelegram("Salió de set Temperatura/Humedad por Time Out");
//	enviarMensageTelegram("Salida por Time Out");
}