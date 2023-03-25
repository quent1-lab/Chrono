#include <Arduino.h>

const int A1 = 16;
const int B_1 = 13;
const int C1 = 32;
const int D1 = 33;
const int dp1 = 25;
const int D2 = 26;
const int C2 = 27;
const int B2 = 14;
const int A2 = 12;
const int A_3 = 19;
const int B3 = 18;
const int C3 = 5;
const int D3 = 17;
const int dp4 = 16;
const int A_4 = 15;
const int B4 = 2;
const int C4 = 0;
const int D4 = 4;

const int pin_segment[4][5] = {
    {A1, B_1, C1, D1, dp1},
    {A2, B2, C2, D2, 0},
    {D3, C3, B3, A_3, 0},
    {A_4, B4, C4, D4, dp4}};

void number_digit(int num_segment, int numeral);
void timer(int timer_value);

unsigned long previousMillis = 0;

int sec_unite = 0;
int sec_dizaine = 0;
int minute = 0;
int minute_dizaine = 0;

void setup()
{
  pinMode(16, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(33, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(25, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(0, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(18, OUTPUT);
  pinMode(19, OUTPUT);
for (int i = 0; i < 10; i++)
{
  /* code */
  number_digit(0, i);
  number_digit(1, i);
  number_digit(2, i);
  number_digit(3, i);
  delay(1000);
}

  previousMillis = millis();
}

void loop()
{
  digitalWrite(dp1, HIGH);
  digitalWrite(dp4, HIGH);

  timer(12);

  number_digit(3, minute);
  number_digit(0, sec_dizaine);
  number_digit(1, sec_unite);
  number_digit(2, minute_dizaine);

  delay(2000);//Ajouté pour test
}

void number_digit(int num_segment, int numeral)
{
  byte num = byte(numeral);

  if (num & 0b0001)
  {
    digitalWrite(pin_segment[num_segment][0], HIGH);
  }
  else
  {
    digitalWrite(pin_segment[num_segment][0], LOW);
  }

  if (num & 0b0010)
  {
    digitalWrite(pin_segment[num_segment][1], HIGH);
  }
  else
  {
    digitalWrite(pin_segment[num_segment][1], LOW);
  }

  if (num & 0b0100)
  {
    digitalWrite(pin_segment[num_segment][2], HIGH);
  }
  else
  {
    digitalWrite(pin_segment[num_segment][2], LOW);
  }

  if (num & 0b1000)
  {
    digitalWrite(pin_segment[num_segment][3], HIGH);
  }
  else
  {
    digitalWrite(pin_segment[num_segment][3], LOW);
  }
}

void timer(int timer_value)
{
  /*
  Cette fonction permet de modifier la valeur des variables: minutes, sec_dizaine et sec_unite
  Afin de faire fonctionner le timer
  Elle prend en paramètre la valeur du timer en minutes
  Et décompte le temps jusqu'à 0
  */

  if(millis() > previousMillis + 1000){
    previousMillis = millis();
    if(sec_unite == 0){
      if(sec_dizaine == 0){
        if(minute == 0){
          if(minute_dizaine == 0){
            minute = timer_value-10;
            minute_dizaine = (timer_value-minute)/10;
            
            sec_dizaine = 0;
            sec_unite = 0;
          }
          else{
            minute_dizaine--;
            minute--;
            sec_dizaine = 5;
            sec_unite = 9;
          }
        }
        else{
          minute--;
          sec_dizaine = 5;
          sec_unite = 9;
        }
      }
      else{
        sec_dizaine--;
        sec_unite = 9;
      }
    }
    else{
      sec_unite--;
    }
    previousMillis = millis();
  }
}