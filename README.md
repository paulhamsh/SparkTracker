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



