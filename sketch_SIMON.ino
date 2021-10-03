//improving code: use an array for the led/switch pins?

////////////////////
//define variables//
////////////////////

//arduino pins
int switchPin0 = 2;
int ledPin0 = 3;
int switchPin1 = 5;
int ledPin1 = 6;
int buzzerPin = 9;

//game variables
int buttonState0 = LOW;
int buttonState1 = LOW;
int lastButtonState = LOW;
unsigned long firstpress;
unsigned long secondpress;
unsigned long sec;
bool game = true;
int bstate, bswitch, bledpin;

//secret numbers and counters
int seq_length = 8;
int seq[8];
int n = 0;
int m = 0;
int i = 0;

//buzzer boi
int tones[] = {261, 277, 294, 311, 330, 349, 370, 392};

/////////
//setup//
/////////

void setup() {
  // put your setup code here, to run once:
  pinMode(switchPin0, INPUT);
  pinMode(switchPin1, INPUT);
  pinMode(ledPin0, OUTPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);

  //loop to determine random sequence
  randomSeed(analogRead(A0));  // We’re reading analog input 0 which should be disconnected 
}

/////////////
//functions//
/////////////

//function to determine which button state to read from
int getstate(int arr, int buttonState0, int buttonState1){
  int i;
  if(arr == 0){
    return buttonState0;
  }
  else{
    return buttonState1;
  }
}

//function to determine which led to use
int getled(int arr, int ledPin0, int ledPin1){
  int i;
  if(arr == 0){
    return ledPin0;
  }
  else{
    return ledPin1;
  }
}

//function to determine which button to read from
int getswitch(int arr, int switchpin0, int switchpin1){
  if(arr == 0){
    return switchpin0;
  }
  else{
    return switchpin1;
  }
}

//function to blink leds
void winner(){
  delay(250);
  tone(buzzerPin, 450, 250);
  digitalWrite(ledPin0, HIGH);
  digitalWrite(ledPin1, HIGH);
  delay(250);  
  digitalWrite(ledPin0, LOW);
  digitalWrite(ledPin1, LOW);
  delay(250);
  tone(buzzerPin, 450, 250);
  digitalWrite(ledPin0, HIGH);
  digitalWrite(ledPin1, HIGH);
  delay(250);  
  digitalWrite(ledPin0, LOW);
  digitalWrite(ledPin1, LOW);
}

////////
//loop//
////////

void loop() {
  buttonState0 = digitalRead(switchPin0);
  buttonState1 = digitalRead(switchPin1);
  
  if (buttonState0 != lastButtonState || buttonState1 != lastButtonState){
    //if the button is pressed, start the game
    if(buttonState0 == HIGH || buttonState1 == HIGH){  

      //find random sequence
      for (int i=0; i<seq_length; i++) {
        int randnum;     
        randnum = random(0,50); // A random number in {0, 1} 
        //use modulus to make numbers between 0 and 1
        seq[i] = randnum%2;
      }

      //print this sequence
      for (int i=0; i<seq_length; i++) {     
        Serial.print(seq[i]);
      }
      
      //loop through the sequence and flash all the correct lights
      while(i<8 && game == true){  
        while(n <= i){
          //play this tune when showing the sequence
          //increase in pitch for every new led
          tone(buzzerPin, tones[n], 250);
          
          //correct led corresponding to sequence
          bledpin = getled(seq[n], ledPin0, ledPin1);
          //flash led
          digitalWrite(bledpin, HIGH);
          delay(250);
          digitalWrite(bledpin, LOW);
          delay(250);
          n++;
        }

        game = true;
        buttonState0 = buttonState1 = LOW;
        firstpress = millis();

        while(m <= i && game==true){

          
          //read the buttons
          buttonState0 = digitalRead(switchPin0);
          buttonState1 = digitalRead(switchPin1);
          //loop until user presses either switch again
          while(buttonState0 == LOW && buttonState1 == LOW){
            buttonState0 = digitalRead(switchPin0);
            buttonState1 = digitalRead(switchPin1);
          }

          //record the correct switch in the sequence 
          bswitch = getswitch(seq[m], switchPin0, switchPin1);
          bstate = getstate(seq[m], buttonState0, buttonState1);

          //record the second button press
          secondpress = millis();
          //find how long it took to press switch
          sec = secondpress - firstpress;
          //the second button press becomes the start of the next interval
          firstpress = millis();
    
          //if interval was less than 2 seconds AND the correct switch was pressed, flash led
          if(sec < 2000 && bstate==HIGH){
            bledpin = getled(seq[m], ledPin0, ledPin1);
            digitalWrite(bledpin, HIGH);
            delay(250);  
            digitalWrite(bledpin, LOW);
          }
          //if interval was more than 2 seconds OR the wrong switch was pressed, exit
          else{
            bledpin = getled(seq[m], ledPin0, ledPin1);
            //flash the loser light
            digitalWrite(bledpin, HIGH);
            delay(2000);
            digitalWrite(bledpin, LOW);

            //play a demoralizing tone
            tone(buzzerPin, 350, 250);
            delay(250);
            tone(buzzerPin, 150, 250);
            
            //exit the game
            game = false;
          }
          m++;
        }
        //if player made it through the simon, light up both leds
        if(i==7){     
          winner();
        }
        //reset counters
        n = 0;
        m = 0;
        i++;
        delay(500);
      }
      //if an error was made, reset counters to start over
      game = true;
      i = 0;
      //debounce
      delay(50);
    }
    lastButtonState = buttonState0 = buttonState1;
  }
}
