#include <ESP32Servo.h>
#include "Adafruit_VL53L0X.h"

Servo PochylniaSerwo;

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
int licznik_petli = 0;
int wyslij = 0;
int pomiar = 0;
bool pisanie = false;

//EMA
float wartosc_poprzednia = 140.0;
float wartosc_obecna = 140.0;
float alpha = 0.2;

const int SerwoPin = 19;

int KatPochylenia = 1430; //O dziwo taki kąt powinien być idealnie płaski             Jeszcze dodatkowe obserwacje, te serwo reaguje na zmiany co 2 stopnie, nie wykrywa nieparzystych np 67 69
                        //1100 to jeden limit zanim się wykopyrtnie jedna strona
                        //1700 to limit w drugą stronę
int odleglosc_zadana = 140;    //Ignorować wartości powyżej 330mm i poniżej 35 to zaokrąglać jako min
                        //Minimalna wartość to 35 maksymalna 330
int Histereza = 15;
float odleglosc_rzeczywista = 140.0;
float odleglosc_poprzednia = 140.0;

//PID
float obecny_czas  = 0;
float poprzedni_czas = 0;
float dt = 0;
float uchyb = 0;
float uchyb_poprzedni = 0;
float suma_calkowania = 0;
float wynik_rozniczki = 0;
float sterowanie = 0;
float K = -0.3;
float Ti = 3;
float Td = 0.5;

int pomiar_odleglosci();
int PID(float,float);

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("odpalanie espa");
  PochylniaSerwo.attach(SerwoPin);
  PochylniaSerwo.write(KatPochylenia);

  if(!lox.begin()) Serial.println("Nie znaleziono czujnika");
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false);
  if(measure.RangeStatus != 4) {
  wartosc_obecna = measure.RangeMilliMeter;
  }
}

unsigned long poprzedni_czas_czujnika = 0;
unsigned long obecny_czas_ms = 0;
const unsigned long okres_probkowania_ms = 30;

void loop() 
{
  obecny_czas_ms = millis();
  obecny_czas = micros();

  if(obecny_czas_ms - poprzedni_czas_czujnika >= okres_probkowania_ms)
  {
    dt = (obecny_czas - poprzedni_czas)/1000000;
    poprzedni_czas = obecny_czas;

    uchyb_poprzedni = uchyb;
    odleglosc_rzeczywista = pomiar_odleglosci();
    //Uchyb
    uchyb = odleglosc_zadana - odleglosc_rzeczywista;
    if(uchyb < 0 and uchyb > -5) uchyb = 0;
    if(uchyb > 0 and uchyb < 5) uchyb = 0;

    int wartosc_sterujaca = PID(uchyb,uchyb_poprzedni) + KatPochylenia;
    wartosc_sterujaca = constrain(wartosc_sterujaca,1100,1700);
    PochylniaSerwo.writeMicroseconds(wartosc_sterujaca);

    poprzedni_czas_czujnika = obecny_czas_ms;
  }
  //Serial.print(" | Obecne dt: "); Serial.println(dt,6);
}

int pomiar_odleglosci()
{
  int odleglosc = 0;

  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure,false);
  odleglosc = measure.RangeMilliMeter;

  if(odleglosc >= 330) odleglosc = 330;
  if(odleglosc <= 35) odleglosc = 35;

  wartosc_obecna = alpha*odleglosc + (1.0 - alpha) * wartosc_poprzednia;
  wartosc_poprzednia = wartosc_obecna;

  wyslij++;
  if(wyslij == 25)
  {
    wyslij = 0;

    Serial.print("Odległośc: "); 
    if (measure.RangeStatus != 4)
    {
      Serial.print(measure.RangeMilliMeter);
      Serial.print(" mm");
      Serial.print(" | Odleglosc EMA: ");
      Serial.println(wartosc_obecna);
    } else {
      Serial.println("Poza zasięgiem");
    }
    pisanie = true;
  }

  return wartosc_obecna;
}

int PID(float uchyb, float uchyb_poprzedni)
{
  //Całkowanie
  suma_calkowania = uchyb*dt + suma_calkowania;

  float limit_calki = 150;
  suma_calkowania = constrain(suma_calkowania,-limit_calki,limit_calki);
  //Różniczkowanie
  wynik_rozniczki = (uchyb - uchyb_poprzedni)/dt;
  sterowanie = K*(uchyb + 1.0/Ti*suma_calkowania + Td*wynik_rozniczki);
  return sterowanie;
}