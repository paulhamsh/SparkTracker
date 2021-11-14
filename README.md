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


|-|-|
|Amps | IOS | Android |
|-|-|


