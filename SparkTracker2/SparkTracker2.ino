#define M5_BRD

#ifdef M5_BRD
#include <M5Core2.h> 
#else
#include "heltec.h"
#endif

#include "Spark.h"
#include "SparkIO.h"

#define PGM_NAME "SparkTracker"
  
SparkPreset preset1{0x0,0x7f,"F577F7F3-E8E0-4D35-8975-0427C2054DCE","Dancing in the room","0.7","Description for Blues Preset 1","icon.png",120.000000,{ 
  {"bias.noisegate", false, 2, {0.283019, 0.304245}}, 
  {"Compressor", true, 2, {0.325460, 0.789062}}, 
  {"Booster", false, 1, {0.666735}}, 
  {"Twin", true, 5, {0.613433, 0.371715, 0.453167, 0.676660, 0.805785}}, 
  {"ChorusAnalog", true, 4, {0.185431, 0.086409, 0.485027, 0.567797}}, 
  {"DelayEchoFilt", false, 5, {0.533909, 0.275554, 0.455372, 0.457702, 1.000000}}, 
  {"bias.reverb", true, 7, {0.508871, 0.317935, 0.461957, 0.349689, 0.339286, 0.481753, 0.700000}} },0x48 };



unsigned int cmdsub;
SparkMessage msg;
SparkPreset preset;

SparkPreset presets[6];

int selected_preset;
int i, j, p;
int ind;
bool got_presets;


void dump_preset(SparkPreset preset) {
  int i,j;

  Serial.print(preset.curr_preset); Serial.print(" ");
  Serial.print(preset.preset_num); Serial.print(" ");
  Serial.print(preset.Name); Serial.print(" : ");
  Serial.println(preset.Description);

  for (j=0; j<7; j++) {
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
  
  connect_to_all(); 

  spark_start(true);

  spark_msg_out.get_preset_details(0x0000);
  spark_msg_out.get_preset_details(0x0001);
  spark_msg_out.get_preset_details(0x0002);
  spark_msg_out.get_preset_details(0x0003);
  spark_msg_out.get_preset_details(0x007f);
  spark_msg_out.get_preset_details(0x0100);
}


void loop() {
  // put your main code here, to run repeatedly:
#ifdef M5_BRD
  M5.update();
#endif

  if (M5.BtnA.wasPressed() ) {
    spark_msg_out.get_preset_details(0x0100);
  }

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
  }
}
