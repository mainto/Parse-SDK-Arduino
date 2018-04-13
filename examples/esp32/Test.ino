#include <Parse.h>

/***** Integration Tests of Parse Arduino esp32 *****/

//Wifi
const char* ssid     = "ssid";
const char* password = "pw";

//Parse Config
const char* applicationId = "app_id";
const char* clientKey = "client_key";
const char* parseHost = "api.parse.com";
const unsigned short parsePort = 443; //SSL
const char* parseEndpoint = "/parse";


int i = 0; // testId for a specific test

void basicObjectTest() {
  Serial.println("basic object operation test");

  Serial.println("create...");
  ParseObjectCreate create;
  create.setClassName("Temperature");
  create.add("temperature", 175.0);
  create.add("leverDown", true);
  ParseResponse createResponse = create.send();
  char* objectId = new char[10];
  strcpy(objectId, createResponse.getString("objectId"));
  assert(createResponse.getErrorCode() == 0);
  createResponse.close();

  Serial.println("update...");
  ParseObjectUpdate update;
  update.setClassName("Temperature");
  update.setObjectId(objectId);
  update.add("temperature", 100);
  ParseResponse updateResponse = update.send();
  assert(updateResponse.getErrorCode() == 0);
  updateResponse.close();

  Serial.println("get...");
  ParseObjectGet get;
  get.setClassName("Temperature");
  get.setObjectId(objectId);
  ParseResponse getResponse = get.send();
  double temp = getResponse.getDouble("temperature");
  Serial.println(temp);
  if(temp != 100){
    Serial.println("Test Failed: temp is not 100");
    while(1){}
  }
  getResponse.close();

  Serial.println("delete...");
  ParseObjectDelete del;
  del.setClassName("Temperature");
  del.setObjectId(objectId);
  ParseResponse delResponse = del.send();
  String expectedResp = "{}";
  String actualResp = String(delResponse.getJSONBody());
  actualResp.trim();
  if(expectedResp.equals(actualResp)){
    Serial.println("Test Failed: response string is not expected");
    while(1){ }
  }
  
  delResponse.close();

  Serial.println("test passed\n");
}

void objectDataTypesTest() {
  Serial.println("data types test");

  ParseObjectCreate create;
  create.setClassName("TestObject");
  create.addGeoPoint("location", 40.0, -30.0);
  create.addJSONValue("dateField", "{\"__type\":\"Date\",\"iso\":\"2011-08-21T18:02:52.249Z\"}");
  create.addJSONValue("arrayField", "[30,\"s\"]");
  create.addJSONValue("emptyField", "null");
  ParseResponse createResponse = create.send();
  if(createResponse.getErrorCode() != 0) {
    Serial.println("Test Failed: error response");
    while(1){ }
  }
  
  createResponse.close();

  Serial.println("test passed\n");
}

void queryTest() {
  Serial.println("query test");

  ParseObjectCreate create1;
  create1.setClassName("Temperature");
  create1.add("temperature", 88.0);
  create1.add("leverDown", true);
  ParseResponse createResponse = create1.send();
  createResponse.close();

  ParseObjectCreate create2;
  create2.setClassName("Temperature");
  create2.add("temperature", 88.0);
  create2.add("leverDown", false);
  createResponse = create2.send();
  createResponse.close();

  ParseQuery query;
  query.setClassName("Temperature");
  query.whereEqualTo("temperature", 88);
  query.setLimit(2);
  query.setSkip(0);
  query.setKeys("temperature");
  ParseResponse response = query.send();
  
  int countOfResults = response.count();
  if(countOfResults != 2){
    Serial.println("Test Failed: count of result is not 2");
    while(1){ }
  }
  while(response.nextObject()) {
    if(88 != response.getDouble("temperature")){
      Serial.println("Test Failed: temperature is not 88");
      while(1){ }
    }
  }
  response.close();

  Serial.println("test passed\n");
}

void setup() {
  // Initialize Serial
  Serial.begin(115200);

  while (!Serial); // wait for a serial connection
  
  connect_wifi();

  // Initialize Parse
  Parse.begin(applicationId, clientKey);
  Parse.parse_host = parseHost;
  Parse.parse_port = parsePort;
  Parse.parse_endpoint = parseEndpoint;
}

void connect_wifi(){
  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(1000);
}

void loop() {
  if(i == 0) basicObjectTest();
  else if (i == 1) objectDataTypesTest();
  else if (i == 2) queryTest();
  else while(1);

  i++;
}
