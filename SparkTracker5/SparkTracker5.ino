#define M5_BRD

#ifdef M5_BRD
#include <M5Core2.h> 
#else
#include "heltec.h"
#endif

#include "Spark.h"
#include "SparkIO.h"

#define PGM_NAME "SparkTracker4"


char *amps[]={"RolandJC120", "Twin", "ADClean", "94MatchDCV2", "Bassman","AC Boost", "Checkmate", "TwoStoneSP50", "Deluxe65",
              "Plexi", "OverDrivenJM45", "OverDrivenLuxVerb", "Bogner", "OrangeAD30", "AmericanHighGain", "SLO100", "YJM100",
              "Rectifier", "EVH", "SwitchAxeLead", "Invader", "BE101", "Acoustic", "AcousticAmpV2", "FatAcousticV2", "FlatAcoustic",
              "GK800", "Sunny3000", "W600", "Hammer500", "ODS50CN", "JH.DualShowman", "JH.Sunn100", "BluesJrTweed", "JH.JTM45", 
              "JH.Bassman50Silver", "JH.SuperLead100", "JH.SoundCity100", "6505Plus" };

char *drives[]={"Booster","DistortionTS9","Overdrive","Fuzz","ProCoRat","BassBigMuff","GuitarMuff","MaestroBassmaster","SABdriver",
                 "KlonCentaurSilver","JH.AxisFuzz","JH.SupaFuzz","JH.Octavia","JH.FuzzTone"};

char *comps[] = {"LA2AComp","BlueComp","Compressor","BassComp","BBEOpticalComp","JH.Vox846"};
char *mods[] = {"Tremolo","ChorusAnalog","Flanger","Phaser","Vibrato01","UniVibe","Cloner","MiniVibe","Tremolator",
                "TremoloSquare","JH.VoodooVibeJr","GuitarEQ6","BassEQ6"};

char *delays[]={"DelayMono","DelayEchoFilt","VintageDelay","DelayReverse","DelayMultiHead","DelayRe201"};

int this_effect = 0;
int this_type = 2;

/*
#define EFF amps
#define EFF_MAX 38
#define EFF_NO 3
#define EFF_BASE "Twin"
*/ 
#define EFF drives
#define EFF_MAX 13
#define EFF_NO 2
#define EFF_BASE "DistortionTS9"
/*
#define EFF comps
#define EFF_MAX 5
#define EFF_NO 1
#define EFF_BASE "LA2AComp"

#define EFF delays
#define EFF_MAX 5
#define EFF_NO 5
#define EFF_BASE "DelayMono"


#define EFF mods
#define EFF_MAX 12
#define EFF_NO 4
#define EFF_BASE "Tremolo"
*/

// variables required to track spark state and also for communications generally
unsigned int cmdsub;
SparkMessage msg;
SparkPreset preset;
SparkPreset presets[6];
int selected_preset;
bool got_presets;

bool ui_update;
SparkPreset fake_preset;

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
  selected_preset = 0;
  got_presets = false;

  // send commands to get preset details for all presets and current state (0x0100)
  spark_msg_out.get_preset_details(0x0000);
  spark_msg_out.get_preset_details(0x0001);
  spark_msg_out.get_preset_details(0x0002);
  spark_msg_out.get_preset_details(0x0003);
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
      Serial.printf("Unprocessed in sync: %x\n", cmdsub);
    }  
//    if (got_presets) dump_preset(presets[5]);
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
  
  connect_to_all();             // sort out bluetooth connections
  spark_start(true);            // set up the classes to communicate with Spark and app
  spark_state_tracker_start();  // set up data to track Spark and app state

  ui_update = false;
}


void loop() {
  // put your main code here, to run repeatedly:
#ifdef M5_BRD
  M5.update();


  if (M5.BtnA.wasPressed()) {
    Serial.println("BUTTON A PRESS");

    fake_preset = presets[5];  
    fake_preset.preset_num = 0x03;
    fake_preset.curr_preset = 0x00;
        
    spark_msg_out.create_preset(&fake_preset);

    sp_bin.pass_through = false;
    app_bin.pass_through = false;

    long t  = millis();
    while (millis() - t <700) {
      spark_process();
    }
    
    sp_bin.pass_through = true;
    app_bin.pass_through = true;
    
    app_msg_out.save_hardware_preset(0x00, 0x03);
    app_process();

    ui_update = false;
  }

    
 /*   
    app_msg_out.save_hardware_preset(0x00, 0x03);
    
    sp_bin.pass_through = false;
    app_bin.pass_through = false;

    app_process();
    
    ui_update = true;
 */


  if (M5.BtnB.wasPressed()) {
    Serial.println("BUTTON B PRESS");
    bool ison = presets[5].effects[1].OnOff;
    if (ison) ison = false; else ison = true;

    sp_bin.pass_through = false;
    app_bin.pass_through = false;
/*
    spark_msg_out.turn_effect_onoff(presets[5].effects[1].EffectName, ison);
    app_msg_out.turn_effect_onoff(presets[5].effects[1].EffectName, ison);
    presets[5].effects[1].OnOff = ison;
    spark_process();
    app_process();

    app_msg_out.change_effect(presets[5].effects[2].EffectName, "Fuzz");
    spark_msg_out.change_effect(presets[5].effects[2].EffectName, "Fuzz");
    strcpy(presets[5].effects[2].EffectName, "Fuzz");
    spark_process();
    app_process();
*/

    app_msg_out.change_effect(EFF_BASE, EFF[this_effect]);
    spark_msg_out.change_effect(EFF_BASE, EFF[this_effect]);

    Serial.print("Fome ");
    Serial.print(EFF_BASE);
    Serial.print(" -> ");
    Serial.println(EFF[this_effect]);

    strcpy(presets[5].effects[EFF_NO].EffectName, EFF[this_effect]);
    this_effect++;
    if (this_effect >EFF_MAX) this_effect = 0;
    
    spark_process();
    app_process();
        
    ui_update = false;
  }

  if (M5.BtnC.wasPressed()) {
    Serial.println("BUTTON C PRESS");
    sp_bin.pass_through = false;
    app_bin.pass_through = false;

    spark_msg_out.change_effect(presets[5].effects[3].EffectName, "Twin");
    strcpy(presets[5].effects[3].EffectName, "Twin");
    spark_process();

    app_msg_out.save_hardware_preset(0x00, 0x03);
    app_process();
    
    ui_update = true;
  }

  
#endif


  // do your own checks and processing here
  if (update_spark_state()) {    // just by calling this we update the local stored state which can be used here
    
      
     switch (cmdsub) {
      case 0x0201:    // just an example
        if (ui_update) {
          Serial.println("Updating UI");
          fake_preset = presets[5];  
          fake_preset.preset_num = 0x03;
          fake_preset.curr_preset = 0x00;
        
          app_msg_out.create_preset(&fake_preset);
          app_process();
          app_msg_out.change_hardware_preset(0x00, 0x03);
          app_process();
          
          sp_bin.pass_through = true;
          app_bin.pass_through = true;        
          ui_update = false;
        }       
        break;
    }
    
  }
  // do your own checks and processing here
}
