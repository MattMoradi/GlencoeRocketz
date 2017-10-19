/* Keep track of gyro angle over time
  Â * Connect Gyro to Analog Pin 0
  Â *
  Â * Sketch by eric barch / ericbarch.com
  Â * v. 0.1 - simple serial output
  Â *
  Â */

int gyroPin = 0; Â  Â  Â  Â  Â  Â  Â  //Gyro is connected to analog pin 0
float gyroVoltage = 5; Â  Â  Â  Â  //Gyro is running at 5V
float gyroZeroVoltage = 2.5; Â  //Gyro is zeroed at 2.5V
float gyroSensitivity = .007; Â //Our example gyro is 7mV/deg/sec
float rotationThreshold = 1; Â  //Minimum deg/sec to keep track of - helps with gyro drifting

float currentAngle = 0; Â  Â  Â  Â  Â //Keep track of our current angle

void setup() {
  Â  Serial.begin (9600);
}

void loop() {
  Â  //This line converts the 0-1023 signal to 0-5V
  Â  float gyroRate = (analogRead(gyroPin) * gyroVoltage) / 1023;

  Â  //This line finds the voltage offset from sitting still
  Â  gyroRate -= gyroZeroVoltage;

  Â  //This line divides the voltage we found by the gyro's sensitivity
  Â  gyroRate /= gyroSensitivity;

  Â  //Ignore the gyro if our angular velocity does not meet our threshold
  Â  if (gyroRate >= rotationThreshold || gyroRate <= -rotationThreshold) {
    Â  Â  //This line divides the value by 100 since we are running in a 10ms loop (1000ms/10ms)
    Â  Â  gyroRate /= 100;
    Â  Â  currentAngle += gyroRate;
    Â 
  }

  Â  //Keep our angle between 0-359 degrees
  Â  if (currentAngle < 0)
    Â  Â  currentAngle += 360;
  Â  else if (currentAngle > 359)
    Â  Â  currentAngle -= 360;

  Â  //DEBUG
  Â  Serial.println(currentAngle);

  Â  delay(10);
}


