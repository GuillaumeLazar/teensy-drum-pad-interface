

// A0 on Teensy 3.1
const int pin = 0;

// Please select: Tools -> USB Type -> MIDI
#define IS_USB_TYPE_MIDI 1

// MIDI note played
int channel = 0x90;
int note = 36; //36 = C1    //60 = C4
int velocity = 0;

// Configuration
const int IS_DYNAMIC_VELOCITY = false;
const int MIN_VELOCITY = 25;
const int MAX_VELOCITY = 127;
const int FIXED_VELOCITY = 100;

const int NOISE_FILTER_VALUE = 80; //100
const int BOUNCE_VALUE = 80; // in ms
const int MIN_VALUE = 80; //100

const int MAX_PLAYING_NOTE = 2;

// Internal varas
long lastTimeTrigged = 0;
int analogVal = 0;
int hitCounter = 0;
int isPlayingNote = false;
int playingNoteCounter = 0;


void setup() {
  #if not defined (IS_USB_TYPE_MIDI)
    Serial.begin(57600);
  #endif
}

void loop() {
  
  // Read current sensor value
  int currentValue = analogRead(pin);
  
  // Apply noise & bounce filter
  if (currentValue > NOISE_FILTER_VALUE
    && ((millis() - lastTimeTrigged) >= BOUNCE_VALUE)){
    analogVal += currentValue;
  }
  
  // If filtered value is above a threshold
  if (analogVal > MIN_VALUE){
   
    lastTimeTrigged = millis();
    hitCounter++;
    isPlayingNote = true;
    playingNoteCounter = 0;
    
    // Process tghe velocity
    if (IS_DYNAMIC_VELOCITY){
      velocity = analogVal / 7;
    }else{
      velocity = FIXED_VELOCITY;
    }
    
    if (velocity < MIN_VELOCITY){
      velocity = MIN_VELOCITY;
    }
    
    if (velocity > MAX_VELOCITY){
      velocity = MAX_VELOCITY;
    }
    
    #if defined (IS_USB_TYPE_MIDI)
      usbMIDI.sendNoteOn(note, velocity, channel);
    #else
      Serial.print("# ");
      Serial.print(hitCounter);
      Serial.print(" -> velocity=");
      Serial.print(velocity);
      Serial.print(" (raw=");
      Serial.print(analogVal);
      Serial.println(")");
    #endif
      
    analogVal = 0;
    
  }else{
     if (isPlayingNote){
       playingNoteCounter++;
       
       if (playingNoteCounter > MAX_PLAYING_NOTE){
         isPlayingNote = false;
         
         #if defined (IS_USB_TYPE_MIDI)
           usbMIDI.sendNoteOff(note, 0, channel);
         #else
           Serial.println("...sendNoteOff\n");
         #endif
         
       }
     } 
  }
  
  delay(1);
}