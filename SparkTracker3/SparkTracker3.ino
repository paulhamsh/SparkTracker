#define M5_BRD

#ifdef M5_BRD
#include <M5Core2.h> 
#else
#include "heltec.h"
#endif

#include "Spark.h"
#include "SparkIO.h"

#define PGM_NAME "SparkTracker"

// variables required to track spark state and also for communications generally
unsigned int cmdsub;
SparkMessage msg;
SparkPreset preset;
SparkPreset presets[6];
int selected_preset;
bool got_presets;

void dump_preset(SparkPreset preset) {
  int i,j;

  Serial.print(preset.curr_preset); Serial.print(" ");
  Serial.print(preset.preset_num); Serial.print(" ");
  Serial.print(preset.Name); Serial.print(" : ");
  Serial.println(preset.Description);

  for (j = 0; j < 7; j++) {
    Serial.printf("     %-30s", preset.effects[j].EffectName); Serial.print(" ");
    if (preset.effects[j].OnOff == true) Serial.print(" On  "); else Serial.print (" Off ");
    for (i = 0; i < preset.effects[j].NumParameters; i++) {
      Serial.print(preset.effects[j].Parameters[i]); Serial.print(" ");
    }
    Serial.println();
  }
}

int get_effect_index(char *str) {
  int ind, i;

  ind = -1;
  for (i = 0; ind == -1 && i <= 6; i++) {
    if (strcmp(presets[5].effects[i].EffectName, str) == 0) {
      ind  = i;
    }
  }
  return ind;
}

void  spark_state_tracker_start() {
  // send commands to get preset details for all presets and current state (0x0100)
  spark_msg_out.get_preset_details(0x0000);
  spark_msg_out.get_preset_details(0x0001);
  spark_msg_out.get_preset_details(0x0002);
  spark_msg_out.get_preset_details(0x0003);
  spark_msg_out.get_preset_details(0x007f);
  spark_msg_out.get_preset_details(0x0100);
}

// get changes from app or Spark and update internal state to reflect this
// this function has the side-effect of loading cmdsub, msg and preset which can be used later

bool  update_spark_state() {
  int j, p, ind;

  connect_spark();  // reconnects if any disconnects happen
  spark_process();
  app_process();
  
  // K&R: Expressions connected by && or || are evaluated left to right, and it is guaranteed that evaluation will stop as soon as the truth or falsehood is known.
  if (spark_msg_in.get_message(&cmdsub, &msg, &preset) || app_msg_in.get_message(&cmdsub, &msg, & preset)) {
    Serial.printf("Message: %4X  ", cmdsub);

    switch (cmdsub) {
      // full preset details
      case 0x0301:  
      case 0x0101:
        p = preset.preset_num;
        j = preset.curr_preset;
        if (p == 0x7f)       
          p = 4;
        if (j == 0x01)
          p = 5;
        presets[p] = preset;
        if (p==5) got_presets = true;
        Serial.printf("Send / receive new preset: %x\n", p);      
        dump_preset(preset);
        break;
      // change of amp  
      case 0x0306:
        strcpy(presets[5].effects[3].EffectName, msg.str2);
        Serial.printf("Change to new amp %s -> %s\n", msg.str1, msg.str2);
        break;
      // change of effect
      case 0x0106:
        Serial.printf("Change to new effect %s -> %s\n", msg.str1, msg.str2);
        ind = get_effect_index(msg.str1);
        if (ind >= 0) {
          strcpy(presets[5].effects[ind].EffectName, msg.str2);
        }
        break;
      // effect on/off  
      case 0x0315:
      case 0x0115:
        Serial.printf("Turn effect %s %s\n", msg.str1, msg.onoff ? "On " : "Off");
        ind = get_effect_index(msg.str1);
        if (ind >= 0) {
          presets[5].effects[ind].OnOff = msg.onoff;
        }
        break;
      // change parameter value  
      case 0x0337:
      case 0x0104:
        Serial.printf("Change model parameter %s %d %0.3f\n", msg.str1, msg.param1, msg.val);
        ind = get_effect_index(msg.str1);
        if (ind >= 0) {
          presets[5].effects[ind].Parameters[msg.param1] = msg.val;
        }
        break;  
      // change to preset  
      case 0x0338:
      case 0x0138:
        selected_preset = msg.param2;
        if (selected_preset == 0x7f) 
          selected_preset=4;
        presets[5] = presets[selected_preset];
        Serial.printf("Change to preset: %x\n", selected_preset);
        if (msg.param1 == 0x01) Serial.println("** Got a change to preset 0x100 from the app **");
        break;
      // store to preset  
      case 0x0327:
        selected_preset = msg.param2;
        if (selected_preset == 0x7f) 
          selected_preset=4;
        presets[selected_preset] = presets[5];
        Serial.printf("Store in preset: %x\n", selected_preset);
        break;
      // current selected preset
      case 0x0310:
        selected_preset = msg.param2;
        if (selected_preset == 0x7f) 
          selected_preset = 4;
        if (msg.param1 == 0x01) 
          selected_preset = 5;
        presets[5] = presets[selected_preset];
        Serial.printf("Hardware preset is: %x\n", selected_preset);
        break;
      default:
        Serial.println();
    }  
    if (got_presets) dump_preset(presets[5]);
    return true;
  }
  else
    return false;
}

void setup() {
  // put your setup code here, to run once:
  
#ifdef M5_BRD
  M5.begin();
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextSize(4);
  M5.Lcd.println(PGM_NAME);
 
#else
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Enable*/, true /*Serial Enable*/);
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, PGM_NAME);
  Heltec.display->display();
  Heltec.display->display();
#endif

  Serial.println("Started");
  
  selected_preset = 0;
  got_presets = false;
  
  connect_to_all();             // sort out bluetooth connections
  spark_start(true);            // set up the classes to communicate with Spark and app
  spark_state_tracker_start();  // set up data to track Spark and app state
}


void loop() {
  // put your main code here, to run repeatedly:
#ifdef M5_BRD
  M5.update();
#endif

  // do your own checks and processing here
  if (update_spark_state()) {    // just by calling this we update the local stored state which can be used here
    switch (cmdsub) {
      case 0x0115:    // just an example
        // do something with messages and presets[5]
        break;
    }
  }
  // do your own checks and processing here
}
