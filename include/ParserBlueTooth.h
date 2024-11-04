#define INESTABLE(x) (x | MASKINESTABLE)

#define CANT_REINTENTOS 10
#define CANT_REINTENTOS_MED 5

// Estados posibles --------------------------------------------------------------
#define ESPERA 0
#define MENU 1
#define LEER_SSID INESTABLE(2)
#define ESCRIBIR_SSID 3
#define LEER_PSW INESTABLE(4)
#define ESCRIBIR_PSW 5

// Funciones para el parser -------------------------------------------------------  
void ParserBlueTooth(void);
void IniParserBlueTooth(void);
void Nada(void);
void AMenu(void);
void ALeerSSID(void);
void AEscribirSSID(void);
void SSIDSalvado(void);
void ALeerPSW(void);
void PSWSalvado(void);
void AEscribirPSW(void);

struct estadoParserBT // estructura que representa cada estado del parser
{
	String entrada;
	unsigned char proxEstado;
	void (*Accion)(void);
};

struct estadoParserBT espera[] =
	{
		"Menu",	MENU, AMenu,
		"menu",	MENU, AMenu,
		"DEFAULT", ESPERA,	Nada,
};

struct estadoParserBT menu[] =
	{
		"1", LEER_SSID,	ALeerSSID,
		"2", LEER_PSW,	ALeerPSW,
		"3", ESCRIBIR_SSID,	AEscribirSSID,
		"4", ESCRIBIR_PSW,	AEscribirPSW,
		"5", MENU, AMenu,
		"DEFAULT", MENU, Nada,
};

struct estadoParserBT leerSSID[] = // Inestable.
	{
		"SI", MENU,	AMenu,
		"DEFAULT",	LEER_SSID,	Nada,
};

struct estadoParserBT escribirSSID[] =
	{
		"DATO",	MENU, SSIDSalvado,
		"5", MENU, AMenu,
		"DEFAULT", ESCRIBIR_SSID, Nada,
};

struct estadoParserBT escribirPSW[] =
	{
		"DATO",	MENU, PSWSalvado,
		"5", MENU,	AMenu,
		"DEFAULT",	ESCRIBIR_PSW, Nada,
};

struct estadoParserBT leerPSW[] = // Inestable.
	{
		"SI", MENU,	AMenu,
		"DEFAULT", LEER_PSW, Nada,
};

// struct estadoParser * ptrEstadoParser;
struct estadoParserBT *ptrEstadoParserBT;

// struct estadoParser * dirEstadoParser[]=
struct estadoParserBT *dirEstadoParserBT[] =
	{
		espera,
		menu,
		leerSSID, // INESTABLE
		escribirSSID,
		leerPSW, // INESTABLE
		escribirPSW,
};
