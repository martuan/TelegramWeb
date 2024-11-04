
#ifndef defines_h
#define defines_h

#define MASKINESTABLE	0x80	// Para orear a los estados de parser donde sea
								// necesaria la llamada al parser, sin la entrada 
								// desde teclado. Para unsigned char.

#define SI "SI"
#define NO "NO"

// Estados posibles set Temperatura Humedad--------------------------------------------------------------
#define INICIO 0
#define ALARMA_POR_TEMP 1
#define ESCRIBIR_TEMP_MAX 2
#define ESCRIBIR_TEMP_MIN 3
#define ALARMA_POR_HUM 4
#define ESCRIBIR_HUM_MAX 5
#define ESCRIBIR_HUM_MIN 6
#define CONFIRMAR_CON_PULSADOR 7
//#define VALORES_GUARDADOS 8
#define VALORES_GUARDADOS INESTABLE_TH(8)
#define TIME_OUT INESTABLE_TH(9)
//*************************************************************

#define WDT_TIMEOUT 15 // Tiempo de espera en segundos
#endif