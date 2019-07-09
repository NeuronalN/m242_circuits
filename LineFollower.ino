/* 
Der Zumo Roboter kann mit diesem Code eine schwarze Lienen erkennen
mit den 5 verschiedenen Sensoren und wird dieser folgen. 
*/

#include <Wire.h>
#include <Zumo32U4.h>

//200 ist die Maximale Geschwindgkeit die der Roboter fahrt
//Beim abbiegen wird die diese jedoch langsamer angepasst
const uint16_t maxSpeed = 200;

Zumo32U4Buzzer buzzer;
Zumo32U4LineSensors lineSensors;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4LCD lcd;

int16_t lastError = 0;

#define NUM_SENSORS 5
unsigned int lineSensorValues[NUM_SENSORS];

// Hier werden die LCD Charakter definiert
// um auf der Anzeige die Sensoren anzuzeigen
void loadCustomCharacters()
{
  static const char levels[] PROGMEM = {
    0, 0, 0, 0, 0, 0, 0, 63, 63, 63, 63, 63, 63, 63
  };
  lcd.loadCustomCharacter(levels + 0, 0);  // 1 bar
  lcd.loadCustomCharacter(levels + 1, 1);  // 2 bars
  lcd.loadCustomCharacter(levels + 2, 2);  // 3 bars
  lcd.loadCustomCharacter(levels + 3, 3);  // 4 bars
  lcd.loadCustomCharacter(levels + 4, 4);  // 5 bars
  lcd.loadCustomCharacter(levels + 5, 5);  // 6 bars
  lcd.loadCustomCharacter(levels + 6, 6);  // 7 bars
}

void printBar(uint8_t height)
{
  if (height > 8) { height = 8; }
  const char barChars[] = {' ', 0, 1, 2, 3, 4, 5, 6, 255};
  lcd.print(barChars[height]);
}

void calibrateSensors()
{
  lcd.clear();


  //Es wird 1 Sekkunde gewartet danach beginnt der Roboter
  //automatisch die Kalibration der Sensoren in dem er
  //über die Linien rotiert
  delay(1000);
  for(uint16_t i = 0; i < 120; i++)
  {
    if (i > 30 && i <= 90)
    {
      motors.setSpeeds(-200, 200);
    }
    else
    {
      motors.setSpeeds(200, -200);
    }

    lineSensors.calibrate();
  }
  motors.setSpeeds(0, 0);
}

//Hier werden die gelesenen Sensoren auf der LCD angezeigt
void showReadings()
{
  lcd.clear();

  while(!buttonA.getSingleDebouncedPress())
  {
    lineSensors.readCalibrated(lineSensorValues);

    lcd.gotoXY(0, 0);
    for (uint8_t i = 0; i < NUM_SENSORS; i++)
    {
      uint8_t barHeight = map(lineSensorValues[i], 0, 1000, 0, 8);
      printBar(barHeight);
    }
  }
}

void setup()
{

  lineSensors.initFiveSensors();

  loadCustomCharacters();

  // Spielt einen kleinen Song
  buzzer.play(">g32>>c32");

  // Wartet bis der Button A gedrückt wurde
  lcd.clear();
  lcd.print(F("Press Button A"));
  lcd.gotoXY(0, 1);
  lcd.print(F("Calibrate"));
  buttonA.waitForButton();

  calibrateSensors();

  showReadings();

  // Spielt Musik und wartet bis sie fertig ist
  lcd.clear();
  lcd.print(F("Let's Go!"));
  buzzer.play("L16 cdegreg4");
  while(buzzer.isPlaying());
}

void loop()
{
 
  //Die Position wird ab gespeichert mit den Values der Sensoren
  int16_t position = lineSensors.readLine(lineSensorValues);

  //Error ist wie viel man von der Mitte weg ist
  //was von der position 2000 entspricht
  int16_t error = position - 2000;

  
  int16_t speedDifference = error / 4 + 6 * (error - lastError);

  lastError = error;

  //Es werden Inviduel die Geschwindigkeit angepasst für Links oder
  //Rechts mit SpeedDiffrence bestimmt man ob er links oder rechts abbiegt
  int16_t leftSpeed = (int16_t)maxSpeed + speedDifference;
  int16_t rightSpeed = (int16_t)maxSpeed - speedDifference;

  //Hier wird die Geschwindigkeit gesetzt für rechts und links
  //Die Geschwindigkeit ist eingeschränkt von 0 bis maxSpeed
  leftSpeed = constrain(leftSpeed, 0, (int16_t)maxSpeed);
  rightSpeed = constrain(rightSpeed, 0, (int16_t)maxSpeed);

  motors.setSpeeds(leftSpeed, rightSpeed);
}
