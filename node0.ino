#include "painlessMesh.h" //librería para que funcione la red mesh
#include <Arduino_JSON.h> //librería para utilizar el formato json y permita la comunicación por medio del mismo formato

// MESH Details
#define   MESH_PREFIX     "mesh" //nombre de la red (SSID)
#define   MESH_PASSWORD   "0123456789" //contraseña de la red
#define   MESH_PORT       5555 //puerto (lo dejamos por default)


int idUwu = 5; //definimos el id
double emergencyUwu = 1; //definimos el tipo de emergencia
int nodeNumber = 0; //definimos el número del nodo
int cookiesUwu = 8109094751044559; //definimos las cookies
double latiUwu = 20.6974157; //definimos la latitud
double lngiUwu = -103.3278227; //definimos la longitud

String readings;  //el string que se va a mandar a los demás nodos 

Scheduler userScheduler; // para controlar la tarea 
painlessMesh  mesh; //definimos la red mesh con el tipo de dato painlessMesh

void sendMessage() ; //mandar elmensaje
String getReadings(); // obtenemos las lecturas de las variables por medio del formato json


Task taskSendMessage(TASK_SECOND * 5 , TASK_FOREVER, &sendMessage); //generamos las tareas, es decir, enviar mensajes y obtener lecturas

String getReadings () { // obtenemos las lecturas de las variables por medio del formato json
  JSONVar jsonReadings;
  jsonReadings["id"] = idUwu;
  jsonReadings["emergency"] = emergencyUwu;
  jsonReadings["node"] = nodeNumber;
  jsonReadings["cookies"] = cookiesUwu;
  jsonReadings["lati"] = latiUwu;
  jsonReadings["lngi"] = lngiUwu;
  readings = JSON.stringify(jsonReadings); //parseamos a string
  return readings; //retornamos el string con todo el json
}

void sendMessage () { //mandamos el mensaje
  String msg = getReadings();
  mesh.sendBroadcast(msg);
}



void receivedCallback( uint32_t from, String &msg ) { //esrto se necesita para la librería de painless, simplemente definimos lo que vamos a mandar en el objeto JSONVar
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  JSONVar myObject = JSON.parse(msg.c_str());
  
  int id = myObject["id"];
  double emergency = myObject["emergency"];
  int node = myObject["node"];
  int cookies = myObject["cookies"];
  double lati = myObject["lati"];
  double lngi = myObject["lngi"];
  
  Serial.print("Id: ");
  Serial.println(id);

  Serial.print("Emergency: ");
  Serial.println(emergency);

  Serial.print("Node: ");
  Serial.println(node);

  Serial.print("Cookies: ");
  Serial.println(cookies);

  Serial.print("Latitude: ");
  Serial.println(lati);

  Serial.print("Longitude: ");
  Serial.println(lngi);
}

void newConnectionCallback(uint32_t nodeId) { //se imprime cuando se detecta una nueva conexión
  Serial.printf("New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() { //se imprime cuando cambian las conexiones
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) { //se imprime cuando se ajusta el temporizador
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
  Serial.begin(115200); //115200 baud
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // se hace antes del init para que podamos ver los mensajes
  //ahora simplemente utilizamos todas nuestras funciones
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
}

void loop() {
  mesh.update(); //se realiza un update para que siempre se sigan comunicando
}
