//define ultrasonic   
const int midtrigPin = 1;
const int midechoPin = 2;
const int righttrigPin=4;
const int rightechoPin = 0;
const int lefttrigPin =13;
const int leftechoPin = 8;

//define IR sensor    
int irSenReadleft=7;   // 7 is LEFT output
int irSenReadmiddle=8;   //  is middle output
int irSenReadright=12;   //  is RIGHT output
int isObstacle = HIGH;

int byteSensorStatus=0;


//ultrasonic sensor 
long duration;
int distance;   //left
long duration2;
int distance2;   //right 
long duration3;
int distance3;    //mid

int nIRStatus,nIRStatus2;


//counter 
int count=0;
int *counter=&count;

 
void setup() {
  Serial.begin(9600); 
  //Ultrasonic setup
  pinMode(midtrigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(midechoPin, INPUT); // Sets the echoPin as an Input
  
  pinMode(righttrigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(rightechoPin, INPUT); // Sets the echoPin as an Input
  
  pinMode(lefttrigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(leftechoPin, INPUT); // Sets the echoPin as an Input

  //IR set up
  pinMode(irSenReadright ,INPUT);
  pinMode(irSenReadleft ,INPUT);
  
  // Starts the serial communication  控制馬達和轉速(PWM)
  pinMode(5, OUTPUT);     //   in1
  pinMode(6, OUTPUT);     //   in2
  pinMode(9, OUTPUT);     //   in3
  pinMode(10, OUTPUT);    //   in4
  pinMode(3,OUTPUT);      //   right PWM
  pinMode(11,OUTPUT);     //   left PWM
}


void backward() { 
 analogWrite(3,70);
 analogWrite(11,75);  
 digitalWrite(5, LOW);
 digitalWrite(6, HIGH);
 digitalWrite(9, HIGH);
 digitalWrite(10, LOW);
}
 
void turnLeft() { 
 	analogWrite(3,89);
 digitalWrite(5, HIGH);
 digitalWrite(6, LOW);
 digitalWrite(9, LOW);
 digitalWrite(10, LOW);
}
 
void forward() {  
 analogWrite(3,89);
 analogWrite(11,100);  
 digitalWrite(5, HIGH);
 digitalWrite(6, LOW);
 digitalWrite(9, LOW);
 digitalWrite(10, HIGH);
}
void right()
{ 
 analogWrite(11,100); 
 digitalWrite(5, LOW);
 digitalWrite(6, LOW);
 digitalWrite(9, LOW);
 digitalWrite(10, HIGH);
}

void stopcar()
{
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
}

//用來判斷黑線還白線 並決定要直走還轉彎
void drivemotor(int nStatus,int *counter)
{
  switch(nStatus)
  { 
    // 　　sensor black:1 　white:0
    case 3: // SL:1 SR:1 // Left black Right Black
      if (*counter==0) // 如果不是在繞過障礙物後回到黑線
      forward();
      else // 如果是繞過障礙物後回到黑線
      {
        right();
        delay(300);
        stopcar();
        delay(1000);
        *counter=0;
      } 
      break;
    case 2: // SL:0 SR:1 //白黑
      right();
      break;                                                                                                                                                  
    case 1: // SL:1 SR:0 // 黑白
      turnLeft();
      break;
    case 0: // SL:0 SR:0 // 白白
      stopcar();
      break;
   }
}

void loop() {


  byteSensorStatus = 0; //清除感測器狀態值

  nIRStatus = digitalRead(irSenReadright); 
  if(nIRStatus == 1)   //讀到黑線
  byteSensorStatus = 2;
  nIRStatus2 = digitalRead(irSenReadleft);
  if(nIRStatus2 == 1)   //讀到黑線
  byteSensorStatus ++;
     
  drivemotor(byteSensorStatus,counter);//紅外線感應器副程式
  //Serial.println( nIRStatus);

//超聲波  
  digitalWrite(lefttrigPin, LOW);// Clears the trigPin
  delayMicroseconds(2);
  digitalWrite(lefttrigPin, HIGH);// Sets the trigPin on HIGH state for 10 micro seconds
  delayMicroseconds(10);
  digitalWrite(lefttrigPin, LOW);
  duration = pulseIn(leftechoPin, HIGH);//Reads the echoPin, returns the sound wave travel time in microseconds

  digitalWrite(righttrigPin, LOW);// Clears the trigPin
  delayMicroseconds(2);
  digitalWrite(righttrigPin, HIGH);// Sets the trigPin on HIGH state for 10 micro seconds
  delayMicroseconds(10);
  digitalWrite(righttrigPin, LOW);
  duration2 = pulseIn(rightechoPin, HIGH);// Reads the echoPin, returns the sound wave travel time in microseconds

  digitalWrite(midtrigPin, LOW);// Clears the trigPin
  delayMicroseconds(2);
  digitalWrite(midtrigPin, HIGH);// Sets the trigPin on HIGH state for 10 micro seconds
  delayMicroseconds(10);
  digitalWrite(midtrigPin, LOW);
  duration3 = pulseIn(midechoPin, HIGH);// Reads the echoPin, returns the sound wave travel time in microseconds

//Calculating the distance
  distance  = duration  *0.034/2;
  distance2 = duration2 *0.034/2;
  distance3 = duration3 *0.034/2;
//超聲波結束


// Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);
  Serial.print("Distance2: ");
  Serial.println(distance2);
  Serial.print("Distance3: ");
  Serial.println(distance3);
  
  if(distance>30) //如果左邊的超音波測距儀前面沒有障礙物(>30cm)
  { 
    if(0<distance3 && distance3<30)   //如果中間的超音波測距儀前面有障礙物
    {							 //0~30cm
      stopcar();					 //進行右繞
      delay(1000);
      backward();
      delay(500);
      stopcar();
      delay(1000);
      right();
      delay(200);
      stopcar();
      delay(1000);
      forward();
      delay(600);
      stopcar();
      delay(1000);
      turnLeft();
      delay(300);
      stopcar();
      delay(1000);
      *counter=1;  //有進行過繞障礙物時counter+1
    }
    else if(*counter == 1 && distance3!=0)// 如果正在繞障礙物
    {
      forward();
      delay(300);
      stopcar();
      delay(1000);
      turnLeft();
      delay(200);
      stopcar();
      delay(500);  
    }
    /*else
    {
      nIRStatus = digitalRead(irSenReadright); 
      if(nIRStatus == 1)   //讀到黑線
      {byteSensorStatus = 2;}
      
      nIRStatus2 = digitalRead(irSenReadleft);
      if(nIRStatus2 == 1)   //讀到黑線
      {byteSensorStatus ++;}
      
      drivemotor(byteSensorStatus,counter);
     //Serial.println( nIRStatus); 
    }*/
  }
  else if(10<distance && distance<30)   //左邊對障礙物的距離剛好
  {
    stopcar();
    delay(1000);
    forward();
    delay(300);
    stopcar();
    delay(1000);
    turnLeft();
    delay(250);
    stopcar();
    delay(1000);
    *counter=1;
  }
  else if(0<distance && distance<10)  //左邊距離障礙物太近  
  {
    stopcar();
    delay(1000);
    backward();
    delay(700);
    stopcar();
    delay(1000);
    right();
    delay(200);
    stopcar();
    delay(1000);
    forward();
    delay(300);
    stopcar();
    delay(1000);
    turnLeft();
    delay(300);
    stopcar();
    delay(1000);
    *counter=1;
  }
}
