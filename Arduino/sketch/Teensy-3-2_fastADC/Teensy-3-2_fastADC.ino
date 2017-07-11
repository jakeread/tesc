#define RES   12         // 16 (13 useable)
#define AVG   4         // HW AVG: 1, 2, 4, 8, 16, 32
#define CNT   100000

#define PIN_SENSA_A                 A7
#define PIN_SENSA_B                 A8
#define PIN_SENSA_C                 A9  

void setup() {
  Serial.begin(115200); // teensy does usb direct, baudrate is ignored

  analogReadResolution(RES);
  analogReadAveraging(AVG);
  analogReference(0); // describes top of range, 0 for default, which is 3.3v from teensy regulator

}

void loop() {
  int t = micros();
  int a_sens_a;
  int a_sens_b;
  int a_sens_c;

  for(int i = 0; i < CNT; i++){
    a_sens_a = analogRead(PIN_SENSA_A);
    a_sens_b = analogRead(PIN_SENSA_B);
    a_sens_c = analogRead(PIN_SENSA_C);
  }

  t = micros() - t;
  Serial.print("ADC Sampling Rate: ");
  Serial.print(CNT * 1000 / t);
  Serial.print(" kSPS at AVG: ");
  Serial.print(AVG);
  Serial.print(" and RES: ");
  Serial.println(RES);
}
