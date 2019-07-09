#include <TimerOne.h>
#include <Wire.h>
#include <MultiFuncShield.h>

enum CountDownModeValues
{
  COUNTING_STOPPED,
  COUNTING
};
//variables
byte countDownMode = COUNTING_STOPPED;

byte tenths = 0;
char seconds = 0;
char minutes = 0;


void setup() {
  //initialize timer
  Timer1.initialize();
  MFS.initialize(&Timer1);
  MFS.write(0);
  Serial.begin(9600);
  MFS.initLM35(SMOOTHING_MODERATE); // For the Temperature messaure
}

void loop() {
  byte btn = MFS.getButton();
  //
  switch (countDownMode)
  {
    case COUNTING_STOPPED:
        // Start the countdow
        if (btn == BUTTON_1_SHORT_RELEASE && (minutes + seconds) > 0)
        {
          countDownMode = COUNTING;
        }
        // set every time variable and display to 0
        else if (btn == BUTTON_1_LONG_PRESSED)
        {
          // reset the countdow
          tenths = 0;
          seconds = 0;
          minutes = 0;
          MFS.write(minutes*100 + seconds);
        }
        // add 1 minute per klick!!
        else if (btn == BUTTON_2_PRESSED || btn == BUTTON_2_LONG_PRESSED)
        {
          minutes++;
          if (minutes > 60)
          {
            minutes = 0;
          }
          MFS.write(minutes*100 + seconds);
        }
        // add 10 seconds per klick
        else if (btn == BUTTON_3_PRESSED)
        {
          seconds += 10;
          if (seconds >= 60)
          {
            seconds = 0;
          }
          MFS.write(minutes*100 + seconds);
        }
        // Displey temperature wihle holding the button and when it's released
        else if (btn == BUTTON_3_LONG_PRESSED)
        {
          int tempCentigrade = MFS.getLM35Data();
          MFS.write((float)tempCentigrade / 10, 1);
          tenths = 0;
          seconds = 0;
          minutes = 0;
        }
        break;

    case COUNTING:
        // stop the countdow
        if (btn == BUTTON_1_SHORT_RELEASE || btn == BUTTON_1_LONG_RELEASE)
        {
          countDownMode = COUNTING_STOPPED;
        }
        else
        {
          // continue the countdown
          tenths++;

          if (tenths == 10)
          {
            tenths = 0;
            seconds--;

            if (seconds < 0 && minutes > 0)
            {
              seconds = 59;
              minutes--;
            }
            // activate all LED's
            if (seconds == 5)
            {
              MFS.writeLeds(LED_ALL, ON);
            }
            // set all LED's to blink
            else if (seconds == 4)
            {
              MFS.blinkLeds(LED_1 | LED_2 | LED_3 | LED_4, ON);
            }
            //stop blink on led 1 but it's still active
            else if (seconds == 3)
            {
              MFS.blinkLeds(LED_1, OFF);
            }
            //stop blink on led 1 & 2 but they are still active
            else if (seconds == 2)
            {
               MFS.blinkLeds(LED_1 | LED_2, OFF);
            }
            //stop blink on led 1 & 2 & 3 but they are still active
            else if (seconds == 1)
            {
              MFS.blinkLeds(LED_1 | LED_2 | LED_3, OFF);
            }
            //deactivate all LED's and give the signal sound
            else if (minutes == 0 && seconds == 0)
            {
              MFS.writeLeds(LED_ALL, OFF);
              // timer has reached 0, so sound the alarm
              MFS.beep(50, 50, 3);  // beep 3 times, 500 milliseconds on / 500 off
              countDownMode = COUNTING_STOPPED;
            }

            MFS.write(minutes*100 + seconds);
          }
          delay(100);
        }
        break;
  }
}
