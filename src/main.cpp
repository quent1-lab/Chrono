#include <Arduino.h>

#include <esp_now.h>
#include <WiFi.h>
#include "rgb_lcd.h"

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t simonAddress[] = {0x10, 0x97, 0xBD, 0xD2, 0x9B, 0x54};
uint8_t labyrintheAddress[] = {0xC8, 0xF0, 0x9E, 0x2C, 0x12, 0xC8};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message
{
  char a[32];
  int erreur;
  int timer;
  int timer_value;
  int module;
  int difficulty;
  int level[2];
  int condo;
  char num_serie[6];
  bool start;
  bool game_over;
  bool victory;
} struct_message;

// Create a struct_message called myData
struct_message myData;
esp_now_peer_info_t peerInfo;
rgb_lcd lcd;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&myData, incomingData, sizeof(myData));
}

// Variables of the pins
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
const int A_4 = 15;
const int B4 = 2;
const int C4 = 0;
const int D4 = 4;

// Array of the pins
const int pin_segment[4][5] = {
    {A1, B_1, C1, D1, dp1},
    {A2, B2, C2, D2, 0},
    {D3, C3, B3, A_3, 0},
    {A_4, B4, C4, D4, 0}};

// Prototypes of the functions
void number_digit(int num_segment, int numeral);
void timer(int timer_value);
void test_number(bool activate);
void test_place(bool activate);
void printLCD(int ligne, int colonne, String message, bool effacer);
void random_numero_serie();

// Variables of the timer
unsigned long previousMillis = 0;
unsigned long timeLCD = 0;
int sec_unite = 0;
int sec_dizaine = 0;
int minute = 0;
int minute_dizaine = 0;
int erreur_avant = 0;

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

  // Init Serial Monitor
  Serial.begin(115200);

  lcd.begin(16, 2);
  lcd.setRGB(0, 255, 0);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  Serial.println(WiFi.macAddress());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, simonAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }

  test_number(true);
  test_place(true);

  digitalWrite(dp1, HIGH);
  random_numero_serie();
  printLCD(0, 3, "ErtinaKaze", true);
  printLCD(1, 6, "BOMB", false);
  delay(1500);
  myData.erreur = 0;
  myData.timer = 0;
  myData.timer_value = 0;
  myData.module = 0;
  myData.difficulty = 0;
  myData.level[0] = 0;
  myData.level[1] = 0;
  myData.condo = 0;
  myData.start = 0;
  myData.game_over = 0;
  sec_unite = 0;
  sec_dizaine = 0;
  minute = 0;
  minute_dizaine = 0;
  previousMillis = millis();
}

void loop()
{
  OnDataRecv(simonAddress, (uint8_t *)&myData, sizeof(myData));
  OnDataRecv(labyrintheAddress, (uint8_t *)&myData, sizeof(myData));

  if (myData.start)
  {
    timer(myData.timer_value);
  }

  if (millis() - timeLCD > 400)
  {
    if (myData.start)
    {
      printLCD(0, 0, "Erreur : ", true);
      printLCD(0, 9, String(myData.erreur), false);

      printLCD(1, 0, "Serie :", false);
      printLCD(1, 8, String(myData.num_serie), false);
    }
    else
    {
      if (myData.game_over)
      {
        printLCD(0, 0, "Game Over!", true);
      }
      else
      {
        if (myData.victory)
        {
          printLCD(0, 0, "Victoire", true);
        }
        else
        {
          printLCD(0, 1, "Attente depart", true);
        }
      }
    }
    timeLCD = millis();
  }

  number_digit(0, minute);
  number_digit(1, sec_dizaine);
  number_digit(3, sec_unite);
  number_digit(2, minute_dizaine);

  if (myData.erreur != erreur_avant)
  {
    erreur_avant = myData.erreur;
    lcd.setRGB(255, 0, 0);
    delay(500);
    lcd.setRGB(0, 255, 0);
  }
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

/*
void number_digit(int num_segment, int numeral)
{
  byte num = byte(numeral);

  digitalWrite(pin_segment[num_segment][0], (num & 0b0001));

  digitalWrite(pin_segment[num_segment][1], (num & 0b0010));

  digitalWrite(pin_segment[num_segment][2], (num & 0b0100));

  digitalWrite(pin_segment[num_segment][3], (num & 0b1000));

}
*/

void timer(int timer_value)
{
  /*
  This function allows you to change the value of the variables:
  minutes, sec_ten and sec_unit in order to make the timer work
  It takes as parameter the value of the timer in minutes and counts down the time to 0
  */

  if (millis() > previousMillis + 1000)
  {
    previousMillis = millis();
    if (sec_unite == 0)
    {
      if (sec_dizaine == 0)
      {
        if (minute == 0)
        {
          if (minute_dizaine == 0)
          {
            minute = timer_value - 10;
            minute_dizaine = (timer_value - minute) / 10;
            if(!myData.victory){
              myData.game_over = 1;
              myData.start = 0;
              esp_now_send(simonAddress, (uint8_t *)&myData, sizeof(myData));
              esp_now_send(labyrintheAddress, (uint8_t *)&myData, sizeof(myData));
            }
            sec_dizaine = 0;
            sec_unite = 0;
          }
          else
          {
            minute_dizaine--;
            minute = 9;
            sec_dizaine = 5;
            sec_unite = 9;
          }
        }
        else
        {
          minute--;
          sec_dizaine = 5;
          sec_unite = 9;
        }
      }
      else
      {
        sec_dizaine--;
        sec_unite = 9;
      }
    }
    else
    {
      sec_unite--;
    }
    previousMillis = millis();
  }
}

void test_number(bool activate)
{
  if (activate)
  {
    for (int i = 0; i < 10; i++)
    {
      /* code */
      number_digit(0, i);
      number_digit(1, i);
      number_digit(2, i);
      number_digit(3, i);
      delay(400);
    }
  }
}

void test_place(bool activate)
{
  if (activate)
  {
    number_digit(0, 0);
    number_digit(1, 1);
    number_digit(2, 2);
    number_digit(3, 3);
    delay(5000);
  }
}

void printLCD(int ligne, int colonne, String message, bool effacer)
{
  if (effacer)
  {
    lcd.clear();
  }
  lcd.setCursor(colonne, ligne);
  lcd.print(message);
}

void random_numero_serie()
{
  // Un numéro de série est généré aléatoirement avec des lettres et des chiffres
  // Il est ensuite envoyé à l'arduino de Simon
  // Il est aussi affiché sur l'écran LCD

  // Génération du numéro de série
  for (int i = 0; i < 4; i++)
  {
    // Génération d'un chiffre aléatoire
    int chiffre = random(0, 10);
    // conversion du chiffre en ASCII
    chiffre = chiffre + 48;
    // Ajout du chiffre à la fin du numéro de série
    myData.num_serie[i] = chiffre;
  }

  for (int i = 4; i < 6; i++)
  {
    // Génération d'une lettre aléatoire
    int lettre = random(65, 91);
    // Ajout de la lettre à la fin du numéro de série
    myData.num_serie[i] = lettre;
  }
}