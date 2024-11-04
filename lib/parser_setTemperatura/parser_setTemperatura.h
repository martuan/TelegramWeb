#ifndef parser_setTemperatura_h
#define parser_setTemperatura_h
// Estados para setear rangos de temperatura y humedad
#define INESTABLE_TH(x) (x | MASKINESTABLE)

//#define CANT_REINTENTOS 10
//#define CANT_REINTENTOS_MED 5

// Estados posibles (Se incluyo en defines.h para verlodesde el main.cpp) No me permite incluir parser_setTemperatura.h 
//en main.cpp, me devuelve doble definició a pesar de comenzar con #ifndef --------------
/*#define INICIO 0
#define ALARMA_POR_TEMP 1
#define ESCRIBIR_TEMP_MAX 2
#define ESCRIBIR_TEMP_MIN 3
#define ALARMA_POR_HUM 4
#define ESCRIBIR_HUM_MAX 5
#define ESCRIBIR_HUM_MIN 6
#define CONFIRMAR_CON_PULSADOR 7
#define VALORES_GUARDADOS INESTABLE(8)*/

void ParserSetTH(void);
void IniParserSetTH(void);
void NadaSetTH(void);
void AInicio(void);
void ASetTemp(void);
//void AMensajeSetTempHum(void);
void AEscribirTempMax(void);
void AEscribirTempMin(void);
//void ASetHum(void);
void AAlarmaPorHum(void);
void AEscribirHumMax(void);
void AEscribirHumMin(void);
void AConfirmarConPulsador(void);
void AGuardarValores(void);
void ATimeOut(void);


struct estadoParserSetTH // estructura que representa cada estado del parser
{
	String entrada;
	unsigned char proxEstado;
	void (*Accion)(void);
};

struct estadoParserSetTH inicio[] =
	{
		"Set_Temp",	ALARMA_POR_TEMP, ASetTemp,
		"DEFAULT", INICIO,	NadaSetTH,
    };

struct estadoParserSetTH alarmaPorTemp[] =
    {
        "Si", ESCRIBIR_TEMP_MAX, AEscribirTempMax,
        "si", ESCRIBIR_TEMP_MAX, AEscribirTempMax,
        "No", ALARMA_POR_HUM, AAlarmaPorHum,
        "no", ALARMA_POR_HUM, AAlarmaPorHum,
        "Tout", TIME_OUT,	ATimeOut,
		"DEFAULT", ALARMA_POR_TEMP,	NadaSetTH,

    };

struct estadoParserSetTH escribirTempMax[] =
    {
        "DATO", ESCRIBIR_TEMP_MIN, AEscribirTempMin,
//        "Tout", INICIO,	AInicio,
        "Tout", TIME_OUT,	ATimeOut,
		"DEFAULT", ESCRIBIR_TEMP_MAX, NadaSetTH,
    };

struct estadoParserSetTH escribirTempMin[] =
    {
        "DATO", ALARMA_POR_HUM, AAlarmaPorHum,
        "Tout", TIME_OUT,	ATimeOut,
//        "SI", ESCRIBIR_HUM_MIN, AEscribirHumMin,
//        "si", ESCRIBIR_HUM_MIN, AEscribirHumMin,
//        "NO", INICIO,	NadaSetTH,
//        "no", INICIO,	NadaSetTH,
		"DEFAULT", ESCRIBIR_TEMP_MIN, NadaSetTH,
    };

struct estadoParserSetTH alarmaPorHum[] =
    {
        "Si", ESCRIBIR_HUM_MAX, AEscribirHumMax,
        "si", ESCRIBIR_HUM_MAX, AEscribirHumMax,
        "No", CONFIRMAR_CON_PULSADOR, AConfirmarConPulsador,
        "no", CONFIRMAR_CON_PULSADOR, AConfirmarConPulsador,
        "Tout", TIME_OUT,	ATimeOut,
		"DEFAULT", ALARMA_POR_HUM, NadaSetTH,
    };

struct estadoParserSetTH escribirHumMax[] =
    {
        "DATO", ESCRIBIR_HUM_MIN, AEscribirHumMin,
//        "Tout", INICIO,	AInicio,
        "Tout",  TIME_OUT,	ATimeOut,
		"DEFAULT", ESCRIBIR_HUM_MAX, NadaSetTH,
    };

struct estadoParserSetTH escribirHumMin[] =
    {
        "DATO", CONFIRMAR_CON_PULSADOR, AConfirmarConPulsador,
 //       "Tout", INICIO,	AInicio,
        "Tout",  TIME_OUT,	ATimeOut,
		"DEFAULT", ESCRIBIR_HUM_MAX, NadaSetTH,
    };

struct estadoParserSetTH confirmarConPulsador[] =
    {
        "PulTrue", VALORES_GUARDADOS, AGuardarValores,       //Se presiona pulsador en plaqueta
//        "Tout", INICIO,	AInicio,
        "Tout",  TIME_OUT,	ATimeOut,
		"DEFAULT", CONFIRMAR_CON_PULSADOR, NadaSetTH,
    };

struct estadoParserSetTH guardarValores[] =
    {
        "DATO", INICIO, AInicio,
		"DEFAULT", VALORES_GUARDADOS, NadaSetTH,
    };

struct estadoParserSetTH timeOut[] =
    {
        "DATO", INICIO, AInicio,
		"DEFAULT", TIME_OUT, NadaSetTH,
    };
// struct estadoParser * ptrEstadoParser;
struct estadoParserSetTH *ptrEstadoParserSetTH;

// struct estadoParser * dirEstadoParser[]=
struct estadoParserSetTH *dirEstadoParserSetTH[] =
	{
		inicio,
		alarmaPorTemp,
		escribirTempMax, 
		escribirTempMin,
		alarmaPorHum, 
        escribirHumMax,
        escribirHumMin,
        confirmarConPulsador,
		guardarValores,// INESTABLE
        timeOut, // INESTABLE
};

#endif