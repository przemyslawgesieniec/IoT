#include <RedBot.h>
RedBotMotors motors;

//MyVariables
//Const timers
const int stopMotorTimer = 2000;

//milisTmpVariables
int driveTimer = 0;

/*flags*/
bool isGoForwardStop = true;
bool isGoBackToStartingPosition = false;
bool isGoBackwardsStop = true;
bool isTimeToTurn = false;

/*counters*/
int succesDriveCounter = 0;

/*Encoder Variables*/
RedBotEncoder encoder = RedBotEncoder(A2, 10); 
int buttonPin = 12;
int countsPerRev = 192;   // 4 pairs of N-S x 48:1 gearbox = 192 ticks per wheel rev
int lCount;
int rCount;

/*Ir sensor*/
int ir = A1;

/*accelerometer*/
RedBotAccel accelerometer;

/*bumpers*/
int lBumperState = HIGH;  
int rBumperState = HIGH; 
RedBotBumper lBumper = RedBotBumper(3);  
RedBotBumper rBumper = RedBotBumper(11); 

/*black line finder sensor*/
RedBotSensor center = RedBotSensor(A6);

void setup()
{
  Serial.begin(9600);
  encoder.clearEnc(BOTH);
}
void loop()
{
  accelerometerRead(); // activation flag setter ( let the car start riding :D )
  irScanDistance();
  bumpersRead();
  lineScanerRead();
  driveForward();
}
void irScanDistance()
{
  int odl = analogRead(ir); 
  if (odl > 250)
  {
    driveTimer = millis();
  }
}
void accelerometerRead()
{
  accelerometer.read();
  if (accelerometer.angleXZ < -60 )
  {
      isGoForwardStop = false;
  }
}
void bumpersRead()
{
  lBumperState = lBumper.read();  // default INPUT state is HIGH, it is LOW when bumped
  rBumperState = rBumper.read();  // default INPUT state is HIGH, it is LOW when bumped
  if(lBumperState == LOW || rBumperState == LOW )
  {
    driveTimer = millis();
    isGoBackToStartingPosition = true; // stop driving forward
    isGoBackwardsStop = false; // return to starting position
  }
}
void lineScanerRead()
{
  if(center.read() > 850)
  {
    driveTimer = millis();
    isGoBackToStartingPosition = true; // stop driving forward
    isGoBackwardsStop = false; // return to starting position
  }
}
void driveForward()
{
  if(succesDriveCounter <= 4 && isGoForwardStop == false)
  {
    if(millis()-driveTimer > stopMotorTimer)
    {
      //reset po napotkaniu każdej przeszkody
      motors.drive(50);
      lCount = encoder.getTicks(LEFT);    // read the left motor encoder
      rCount = encoder.getTicks(RIGHT);   // read the right motor encoder
      
      /*encoder debug*/
      Serial.print(lCount);
      Serial.print("\t");
      Serial.println(rCount);

      if ((lCount >= 5*countsPerRev) || (rCount >= 5*countsPerRev) )
      {
        succesDriveCounter ++;
        Serial.println("succesDriveCounter is now equal = " + succesDriveCounter);
        motors.brake();
        isTimeToTurn = true;
        driveTimer = millis();   
      }       
    }
  }
  if(isGoBackwardsStop == false)
  {
     int goBackLCount = lCount;
     int goBackRCount = rCount;
     encoder.clearEnc(BOTH);
     lCount = encoder.getTicks(LEFT);    // read the left motor encoder
     rCount = encoder.getTicks(RIGHT);   // read the right motor encoder
     motors.drive(-50);

    if ((lCount >= goBackLCount) || (rCount >= goBackRCount) )
    {
      isGoBackwardsStop = true;
      isGoForwardStop = false;
    }
  }
  if(isTimeToTurn == true )
  {
    motors.leftMotor(-SPEED);  // spin CCW (left)
    motors.rightMotor(-SPEED); // spin CCW
    delay(500);
    motors.brake();
    delay(100);
    isTimeToTurn = false;
  }
}

