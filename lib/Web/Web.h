#ifndef Web_h
#define Web_h

#include "ESPAsyncWebServer.h"


extern AsyncWebServer server;
extern boolean takeNewPhoto;
extern boolean fotoContinua;
extern int r;
extern int g;
extern int b;
extern boolean encenderRuleta;
extern boolean cambiarColoresForm;

void urls(void);
void notFound(AsyncWebServerRequest);
void convertirParametrosRecibidos(String, String, int*, int*);



#endif