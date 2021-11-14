# SparkTracker
Tracks the state of the app and amp

Updated with v5 to see which effects can be selected with the 0x0306 command (from the Spark to app)   

Interesting things previously unknown to me:

Given the command is a change from effect x to effect y, it seems that effect x does not need to be the effect currently in use.   
For example, changing from amp Twin to RolandJC120, the command could be  EVH to RolandJC120 and it seems to work.   
Which means perhaps we don't need to track the amp current state at all!!   

Findings for IOS and Android apps below.    

Interestingly the IOS app now gives a pop-up error when an effect is chosen it doesn't like. But Android accepts it, and generally updates the UI and the internal state.   
So in this regard the Android app is more friendly to pedals than the IOS app.   

Pop-up error means IOS gives a pop up saying effect invalid   
Y means the effect works and the UI (in part) is updated - and the app updates itself for that effect   
NO EFFECT means the app ignores this but shows no error   

  
|Amps | IOS | Android |   
|-|-|-| 
| RolandJC120                  | Y | Y |
| Twin | Y | Y |
| ADClean | Y | Y |
| 94MatchDCV2                  | Y | Y |
| Bassman | Y | Y |
| AC Boost                     | Y | Y |
| Checkmate | Y | Y |
| TwoStoneSP50 | Y | Y |
| Deluxe65 | Y | Y |
| Plexi | Y | Y |
| OverDrivenJM45 | Y | Y |
| OverDrivenLuxVerb | Y | Y |
| Bogner | Y | Y |
| OrangeAD30 | Y | Y |
| AmericanHighGain | Y | Y |
| SLO100 | Y | Y |
| YJM100 | Y | Y |
| Rectifier | Y | Y |
| EVH | Y | Y |
| SwitchAxeLead | Y | Y |
| Invader | Y | Y |
| BE101                        | Y | Y |
| Acoustic | Y | Y |
| AcousticAmpV2 | Y | Y |
| FatAcousticV2 | Y | Y |
| FlatAcoustic | Y | Y |
| GK800 | Y | Y |
| Sunny3000 | Y | Y |
| W600 | Y | Y |
| Hammer500 | Y | Y |
| ODS50CN | Y | Y |
| JH.DualShowman | Y | Y |
| JH.Sunn100 | Y | Y |
| BluesJrTweed | Y | Y |
| JH.JTM45 | Y | Y |
| JH.Bassman50Silver | NO EFFECT | NO EFFECT |
| JH.SuperLead100 | Y | Y |
| JH.SoundCity100 | Y | Y |
| 6505Plus | Y | Y |



| Mods | IOS | Android |   
|-|-|-| 
| Tremolo | Pop-up error | Y |	
| ChorusAnalog | Pop-up error | Y |	
| Flanger | Pop-up error | Y |	
| Phaser | Pop-up error | Y |	
| Vibrato01 | Pop-up error | Y |	
| UniVibe | Pop-up error | Y |	
| Cloner | Pop-up error | Y |	
| MiniVibe | Pop-up error | Y |	
| Tremolator | Pop-up error | Y |	
| TremoloSquare | Pop-up error | Y |	
| JH.VoodooVibeJr | NO EFFECT | NO EFFECT |	
| GuitarEQ6 | Pop-up error | Y |	
| BassEQ6 | Pop-up error | Y |	


| Delays | IOS | Android |   
|-|-|-| 
| DelayMono | Pop-up error | Y |
| DelayEchoFilt | NO EFFECT | NO EFFECT |
| VintageDelay | Pop-up error | Y |
| DelayReverse | Pop-up error | Y |
| DelayMultiHead | NO EFFECT | NO EFFECT |
| DelayRe201 | Pop-up error | Y |

| Drive | IOS | Android |   
|-|-|-| 
| Booster | Pop-up error | Y |
| DistortionTS9 | NO EFFECT | NO EFFECT |
| Overdrive | NO EFFECT | NO EFFECT |
| Fuzz | Pop-up error | Y |
| ProCoRat | Pop-up error | Y |
| BassBigMuff | NO EFFECT | NO EFFECT |
| GuitarMuff | NO EFFECT | NO EFFECT |
| MaestroBassmaster | NO EFFECT | NO EFFECT |
| SABdriver | NO EFFECT | NO EFFECT |
| KlonCentaurSilver | NO EFFECT | NO EFFECT |
| JH.AxisFuzz | NO EFFECT | NO EFFECT |
| JH.SupaFuzz | NO EFFECT | NO EFFECT |
| JH.Octavia | NO EFFECT | NO EFFECT |
| JH.FuzzTone | NO EFFECT | NO EFFECT |

