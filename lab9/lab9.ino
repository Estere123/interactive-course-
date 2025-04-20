int redPin=9;
  int bright =255;
int myVoltPin = A0;
int readVal;
float V1;
int timeDelay = 500;

void setup() {
  // put your setup code here, to run once:
  pinMode(redPin, OUTPUT);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(redPin, bright);
  analogWrite(redPin, 255);

  readVal = analogRead(myVoltPin);
  V1 = (5./1023)*readVal;
  Serial.println(V1);
  
  delay(timeDelay);
}
