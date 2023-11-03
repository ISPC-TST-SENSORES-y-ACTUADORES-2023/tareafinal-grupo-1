/**********************************************************************************
 *  TITLE: ESP RainMaker + Bluetooth + IR + Manual Button control 4 Relays using ESP32 (Real time feedback + no WiFi control)
 *  Click on the following links to learn more. 
 *  YouTube Video: 
 *  Related Blog : https://github.com/ISPC-TST-SENSORES-y-ACTUADORES-2023/tareafinal-grupo-1
 *  by Ulises Ale
 *  Preferences--> Aditional boards Manager URLs : 
 *  http://arduino.esp8266.com/stable/package_esp8266com_index.json
 *  https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
 *  
 *  Download Board ESP32 (2.0.3): https://github.com/espressif/arduino-esp32
 *
 *  Download the libraries 
 *  AceButton Library (1.9.2): https://github.com/bxparks/AceButton
 *  IRremote Library (3.6.1): https://github.com/Arduino-IRremote/Arduino-IRremote
 **********************************************************************************/

#include "RMaker.h"
#include "WiFi.h"
#include "WiFiProv.h"
#include <IRremote.h>
#include <AceButton.h>
using namespace ace_button;

const char *service_name = "Moldescoop-Milet";
const char *pop = "guillemi379";

// define the Chip Id
uint32_t espChipId = 0;

// define the Node Name
char nodeName[] = "ESP32 Smart-Home";

// define the Device Names
char deviceName_1[] = "Switch1";
char deviceName_2[] = "Switch2";
char deviceName_3[] = "Switch3";
char deviceName_4[] = "Switch4";

// Update the HEX code of IR Remote buttons 0x<HEX CODE>
#define IR_Button_1 0x80BF49B6
#define IR_Button_2 0x80BFC936
#define IR_Button_3 0x80BF33CC
#define IR_Button_4 0x80BF718E
#define IR_All_Off 0x80BF3BC4

// define the GPIO connected with Relays and switches
static uint8_t RelayPin1 = 23;  // D23
static uint8_t RelayPin2 = 22;  // D22
static uint8_t RelayPin3 = 21;  // D21
static uint8_t RelayPin4 = 19;  // D19

static uint8_t SwitchPin1 = 32;  // D32
static uint8_t SwitchPin2 = 33;  // D33
static uint8_t SwitchPin3 = 25;  // D25
static uint8_t SwitchPin4 = 26;  // D26

static uint8_t wifiLed = 2;  // D2
static uint8_t gpio_reset = 0;  // Press BOOT for reset WiFi
static uint8_t IR_RECV_PIN = 35;  // D35 (IR receiver pin)

static uint8_t RX2Pin       = 16;  // RX2
static uint8_t TX2Pin       = 17;  // TX2

/* Variable for reading pin status */
bool toggleState_1 = LOW; // Define integer to remember the toggle state for relay 1
bool toggleState_2 = LOW; // Define integer to remember the toggle state for relay 2
bool toggleState_3 = LOW; // Define integer to remember the toggle state for relay 3
bool toggleState_4 = LOW; // Define integer to remember the toggle state for relay 4

String bt_data = ""; // variable for storing Bluetooth data

IRrecv irrecv(IR_RECV_PIN);
decode_results results;

ButtonConfig config1;
AceButton button1(&config1);
ButtonConfig config2;
AceButton button2(&config2);
ButtonConfig config3;
AceButton button3(&config3);
ButtonConfig config4;
AceButton button4(&config4);

void handleEvent1(AceButton*, uint8_t, uint8_t);
void handleEvent2(AceButton*, uint8_t, uint8_t);
void handleEvent3(AceButton*, uint8_t, uint8_t);
void handleEvent4(AceButton*, uint8_t, uint8_t);

// The framework provides some standard device types like switch, lightbulb, fan, temperature sensor.
static Switch my_switch1(deviceName_1, &RelayPin1);
static Switch my_switch2(deviceName_2, &RelayPin2);
static Switch my_switch3(deviceName_3, &RelayPin3);
static Switch my_switch4(deviceName_4, &RelayPin4);

void sysProvEvent(arduino_event_t *sys_event)
{
    switch (sys_event->event_id) {
        case ARDUINO_EVENT_PROV_START:
#if CONFIG_IDF_TARGET_ESP32
            Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n", service_name, pop);
            printQR(service_name, pop, "ble");
#else
            Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on SoftAP\n", service_name, pop);
            printQR(service_name, pop, "softap");
#endif
            break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Serial.printf("\nConnected to Wi-Fi!\n");
            digitalWrite(wifiLed, true);
            break;
    }
}

void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx)
{
    const char *device_name = device->getDeviceName();
    const char *param_name = param->getParamName();

    if (strcmp(device_name, deviceName_1) == 0) {

        Serial.printf("Switch value = %s\n", val.val.b ? "true" : "false");
        toggleState_1 = val.val.b;
        digitalWrite(SwitchPin1, toggleState_1);
    }

    if (strcmp(device_name, deviceName_2) == 0) {

        Serial.printf("Switch value = %s\n", val.val.b ? "true" : "false");
        toggleState_2 = val.val.b;
        digitalWrite(SwitchPin2, toggleState_2);
    }

    if (strcmp(device_name, deviceName_3) == 0) {

        Serial.printf("Switch value = %s\n", val.val.b ? "true" : "false");
        toggleState_3 = val.val.b;
        digitalWrite(SwitchPin3, toggleState_3);
    }

    if (strcmp(device_name, deviceName_4) == 0) {

        Serial.printf("Switch value = %s\n", val.val.b ? "true" : "false");
        toggleState_4 = val.val.b;
        digitalWrite(SwitchPin4, toggleState_4);
    }
}

void ir_remote(){
  if (irrecv.decode(&results)) {
      switch(results.value){
          case IR_Button_1:  
            digitalWrite(RelayPin1, toggleState_1);
            toggleState_1 = !toggleState_1;
            my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_1);
            delay(100);            
            break;
          case IR_Button_2:  
            digitalWrite(RelayPin2, toggleState_2);
            toggleState_2 = !toggleState_2;
            my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_2);
            delay(100);            
            break;
          case IR_Button_3:  
            digitalWrite(RelayPin3, toggleState_3);
            toggleState_3 = !toggleState_3;
            my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_3);
            delay(100);            
            break;
          case IR_Button_4:  
            digitalWrite(RelayPin4, toggleState_4);
            toggleState_4 = !toggleState_4;
            my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_4);
            delay(100);            
            break;
          case IR_All_Off:
            all_SwitchOff();  
            break;
          default : break;         
        }   
        //Serial.println(results.value, HEX);    
        irrecv.resume();   
  } 
}

void bluetooth_control()
{
  if(Serial2.available()) {
    bt_data = Serial2.readString();
    Serial.println(bt_data.substring(bt_data.lastIndexOf(",") + 1));
    
    if (bt_data.substring(bt_data.lastIndexOf(",") + 1) == "A1"){
      digitalWrite(RelayPin1, LOW);  toggleState_1 = 1; // if "A1" received Turn on Relay1
      my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_1);
    }
    else if(bt_data.substring(bt_data.lastIndexOf(",") + 1) == "A0"){
      digitalWrite(RelayPin1, HIGH);  toggleState_1 = 0; // if "A0" received Turn off Relay1
      my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_1);
    }
    else if(bt_data.substring(bt_data.lastIndexOf(",") + 1) == "B1"){
      digitalWrite(RelayPin2, LOW);  toggleState_2 = 1; // if "B1" received Turn on Relay2
      my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_2);
    }
    else if(bt_data.substring(bt_data.lastIndexOf(",") + 1) == "B0"){
      digitalWrite(RelayPin2, HIGH);  toggleState_2 = 0; // if "B0" received Turn off Relay2
      my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_2);
    }
    else if(bt_data.substring(bt_data.lastIndexOf(",") + 1) == "C1"){
      digitalWrite(RelayPin3, LOW);  toggleState_3 = 1; // if "C1" received Turn on Relay3
      my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_3);
    }
    else if(bt_data.substring(bt_data.lastIndexOf(",") + 1) == "C0"){
      digitalWrite(RelayPin3, HIGH);  toggleState_3 = 0; // if "C0" received Turn off Relay3
      my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_3);
    }
    else if(bt_data.substring(bt_data.lastIndexOf(",") + 1) == "D1"){
      digitalWrite(RelayPin4, LOW);  toggleState_4 = 1; // if "D1" received Turn on Relay4
      my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_4);
    }
    else if(bt_data.substring(bt_data.lastIndexOf(",") + 1) == "D0"){
      digitalWrite(RelayPin4, HIGH);  toggleState_4 = 0; // if "D0" received Turn off Relay4
      my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_4);
    }
    else if(bt_data.substring(bt_data.lastIndexOf(",") + 1) == "Z1"){    // if "Z1" received Turn on all Relays
      all_SwitchOn();
    }
    else if(bt_data.substring(bt_data.lastIndexOf(",") + 1) == "Z0"){    // if "Z0" received Turn off all Relays
      all_SwitchOff();
    }
  } 
}

void all_SwitchOff(){
  toggleState_1 = 0; digitalWrite(RelayPin1, HIGH); my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_1); delay(100);
  toggleState_2 = 0; digitalWrite(RelayPin2, HIGH); my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_2); delay(100);
  toggleState_3 = 0; digitalWrite(RelayPin3, HIGH); my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_3); delay(100);
  toggleState_4 = 0; digitalWrite(RelayPin4, HIGH); my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_4); delay(100);
}

void all_SwitchOn(){
  toggleState_1 = 1; digitalWrite(RelayPin1, LOW); my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_1); delay(100);
  toggleState_2 = 1; digitalWrite(RelayPin2, LOW); my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_2); delay(100);
  toggleState_3 = 1; digitalWrite(RelayPin3, LOW); my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_3); delay(100);
  toggleState_4 = 1; digitalWrite(RelayPin4, LOW); my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_4); delay(100);
}

void setup()
{   
    Serial.begin(115200);
    Serial2.begin(9600, SERIAL_8N1, RX2Pin, TX2Pin);
    
    // Set the Relays GPIOs as output mode
    pinMode(RelayPin1, OUTPUT);
    pinMode(RelayPin2, OUTPUT);
    pinMode(RelayPin3, OUTPUT);
    pinMode(RelayPin4, OUTPUT);
    pinMode(wifiLed, OUTPUT);
    
    // Configure the input GPIOs
    pinMode(SwitchPin1, INPUT_PULLUP);
    pinMode(SwitchPin2, INPUT_PULLUP);
    pinMode(SwitchPin3, INPUT_PULLUP);
    pinMode(SwitchPin4, INPUT_PULLUP);
    pinMode(gpio_reset, INPUT);
    
    // Write to the GPIOs the default state on booting
    digitalWrite(RelayPin1, !toggleState_1);
    digitalWrite(RelayPin2, !toggleState_2);
    digitalWrite(RelayPin3, !toggleState_3);
    digitalWrite(RelayPin4, !toggleState_4);
    digitalWrite(wifiLed, LOW);

    irrecv.enableIRIn(); // Enabling IR sensor

    config1.setEventHandler(button1Handler);
    config2.setEventHandler(button2Handler);
    config3.setEventHandler(button3Handler);
    config4.setEventHandler(button4Handler);
    
    button1.init(SwitchPin1);
    button2.init(SwitchPin2);
    button3.init(SwitchPin3);
    button4.init(SwitchPin4);

    Node my_node;    
    my_node = RMaker.initNode(nodeName);

    //Standard switch device
    my_switch1.addCb(write_callback);
    my_switch2.addCb(write_callback);
    my_switch3.addCb(write_callback);
    my_switch4.addCb(write_callback);

    //Add switch device to the node   
    my_node.addDevice(my_switch1);
    my_node.addDevice(my_switch2);
    my_node.addDevice(my_switch3);
    my_node.addDevice(my_switch4);

    //This is optional 
    RMaker.enableOTA(OTA_USING_PARAMS);
    //If you want to enable scheduling, set time zone for your region using setTimeZone(). 
    //The list of available values are provided here https://rainmaker.espressif.com/docs/time-service.html
    // RMaker.setTimeZone("Asia/Shanghai");
    // Alternatively, enable the Timezone service and let the phone apps set the appropriate timezone
    RMaker.enableTZService();
    RMaker.enableSchedule();

    //Service Name
    for(int i=0; i<17; i=i+4) {
      espChipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }

    Serial.printf("\nChip ID:  %d Service Name: %s\n", espChipId, service_name);

    Serial.printf("\nStarting ESP-RainMaker\n");
    RMaker.start();

    WiFi.onEvent(sysProvEvent);
#if CONFIG_IDF_TARGET_ESP32
    WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name);
#else
    WiFiProv.beginProvision(WIFI_PROV_SCHEME_SOFTAP, WIFI_PROV_SCHEME_HANDLER_NONE, WIFI_PROV_SECURITY_1, pop, service_name);
#endif

    my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
    my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
    my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
    my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
}


void loop()
{
    ir_remote();
    bluetooth_control();
}

void button1Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT1");
  switch (eventType) {
    case AceButton::kEventReleased:
      digitalWrite(RelayPin1, toggleState_1);
      toggleState_1 = !toggleState_1;
      my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_1);
      break;
  }
}
void button2Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT2");
  switch (eventType) {
    case AceButton::kEventReleased:
      digitalWrite(RelayPin2, toggleState_2);
      toggleState_2 = !toggleState_2;
      my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_2);
      break;
  }
}
void button3Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT3");
  switch (eventType) {
    case AceButton::kEventReleased:
      digitalWrite(RelayPin3, toggleState_3);
      toggleState_3 = !toggleState_3;
      my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_3);
      break;
  }
}
void button4Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT4");
  switch (eventType) {
    case AceButton::kEventReleased:
      digitalWrite(RelayPin4, toggleState_4);
      toggleState_4 = !toggleState_4;
      my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_4);
      break;
  }
}
