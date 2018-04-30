/* Keep track of gyro angle over time
  Â * Connect Gyro to Analog Pin 0
  Â *
  Â * Sketch by eric barch / ericbarch.com
  Â * v. 0.1 - simple serial output
  Â *
  Â */

int gyroPin = 0;//Gyro is connected to analog pin 0
float gyroVoltage = 5;//Gyro is running at 5V
float gyroZeroVoltage = 2.5;//Gyro is zeroed at 2.5V
float gyroSensitivity = .007;//Our example gyro is 7mV/deg/sec
float rotationThreshold = 1;//Minimum deg/sec to keep track of - helps with gyro drifting
float currentAngle = 0;//Keep track of our current angle
void setup() {
  Serial.begin (9600);
}
void loop() {
  //This line converts the 0-1023 signal to 0-5V
  float gyroRate = (analogRead(gyroPin) * gyroVoltage) / 1023;
  //This line finds the voltage offset from sitting still
  gyroRate -= gyroZeroVoltage;
  //This line divides the voltage we found by the gyro's sensitivity
  gyroRate /= gyroSensitivity;
  //Ignore the gyro if our angular velocity does not meet our threshold
  if (gyroRate >= rotationThreshold || gyroRate <= -rotationThreshold) {
    //This line divides the value by 100 since we are running in a 10ms loop (1000ms/10ms)
    gyroRate /= 100;
    currentAngle += gyroRate;
  }
  //Keep our angle between 0-359 degrees
  if (currentAngle < 0)
    currentAngle += 360;
  else if (currentAngle > 359)
    currentAngle -= 360;

  //DEBUG
  Serial.println(currentAngle);

}


