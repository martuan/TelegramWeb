#include <Arduino.h>
#include "defines.h"
#include "ParserBlueTooth.h"
#include <BluetoothSerial.h>
#include <Preferences.h>

extern BluetoothSerial SerialBT; //Para comunicación BlueTooth.
extern String ssid_EEPROM;
extern String psw_EEPROM;
extern Preferences preferences;
//extern bool esperaDato;
extern bool esperaDatoBT;
extern String datoRecibido;

// variables para el parser ------------------------------------------------------
//String inParserComm;		    // dato de entrada al parser
String inParserBlueTooth;		    // dato de entrada al parser
//unsigned char estadoActual; 	// estado del parser
//unsigned char estadoAnterior;	// utilizado para los casos en que se 
unsigned char estadoActualBT; 	// estado del parser
//unsigned char estadoAnteriorBT;	// utilizado para los casos en que se 
								// necesita saber de donde viene.
String auxStr;
//**************************************************************************
//
// Funciones para máquina de estados. (Parser)
//
//**************************************************************************
//void IniParserComm(void)
void IniParserBlueTooth(void)
{
   	estadoActualBT = ESPERA;
	inParserBlueTooth = "DEFAULT";

}
//void ParserComm(void)
void ParserBlueTooth(void)
{
  	//unsigned char codigoFamilia;

  	//codigoFamilia = OTRO;   // por ahora.
	estadoActualBT = estadoActualBT & ~MASKINESTABLE; //Quita posible inestabilidad.

	//if( ((inParser >= '0') && (inParser <= '9')) || inParser == COD_OT_COMUN  
  	//	|| inParser == COD_INTERLABORATORIO )	
 	//{
	//	codigoFamilia = EDICION_OT;
	//}
  
  	ptrEstadoParserBT = dirEstadoParserBT[estadoActualBT];

	while ( (ptrEstadoParserBT->entrada != inParserBlueTooth) && 
			(ptrEstadoParserBT->entrada != "DEFAULT")  /*&&
		 	(ptrEstadoParser->entrada != codigoFamilia)*/ )
	{
		ptrEstadoParserBT++;//si no es la entrada buscada pasa a la otra.
	}
	estadoActualBT = ptrEstadoParserBT->proxEstado; //actualiza estado
	
	(*ptrEstadoParserBT->Accion) (); 			//realiza acción
}
//
// Nada()
//
void Nada(void)
{
	//No hace nada.
}

void AMenu(void)
{
	SerialBT.println("1-Leer SSID.");
    SerialBT.println("2-Leer PSW.");
    SerialBT.println("3-Escribir SSID.");
    SerialBT.println("4-Escribir PSW.");
    SerialBT.println("5-Volver a Menu.");
    //SerialBT.println("5-Salir.");
}

void ALeerSSID(void)
{
	ssid_EEPROM = preferences.getString("SSID", "default");
	SerialBT.println("SSID: " + ssid_EEPROM);	
	SerialBT.println();
	inParserBlueTooth= "SI";
}

void AEscribirSSID(void)
{
	SerialBT.println("Ingrese SSID:");
	esperaDatoBT = true;//Espera recibir un dato por BlueTooth.
}

void SSIDSalvado(void)
{
	if(datoRecibido != "5"){	//Solo "5"  es para volver al menu
		preferences.putString("SSID", datoRecibido);
	}
	AMenu();	
}

void ALeerPSW(void)
{
	psw_EEPROM = preferences.getString("PSW", "default");
	inParserBlueTooth= "SI";
	SerialBT.println("Clave: " + psw_EEPROM);
	SerialBT.println();
}

void PSWSalvado(void)
{
	if(datoRecibido != "5"){
		preferences.putString("PSW", datoRecibido);
	}
	AMenu();	
}
void AEscribirPSW(void)
{
	SerialBT.println("Ingrese Clave:");
	esperaDatoBT = true;//Espera recibir un dato por BlueTooth.
}