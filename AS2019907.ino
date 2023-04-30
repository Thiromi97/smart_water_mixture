//Variables
unsigned long prev_time = millis();
int decimalPrecision = 2, hotThermistorPin = A2, coldThermistorPin = A1;
float voltageDividerR1 = 10000, BValue = 3380, R1 = 10000, T1 = 298.15, R2, R3, hotTemp, coldTemp, a, b, c, d, e = 2.718281828 ;
float tempSampleRead = 0, tempSampleRead1 = 0, tempLastSample = 0, tempLastSample1 = 0, tempSampleSum  = 0, tempSampleSum1 = 0, tempSampleCount = 0, tempSampleCount1 = 0, tempMean, tempMean1;
int  hotWaterCount, coldWaterCount;
float coldWaterVolume, hotWaterVolume;
float finalVolume = 150.00;
float requiredTemp = 0;

void waterPump() {
  pinMode(9, OUTPUT);
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);
  digitalWrite(9, LOW);
  coldWaterCount = coldWaterVolume * 50;
  digitalWrite(9, HIGH);
  delay(coldWaterCount);
  digitalWrite(9, LOW);

  hotWaterCount = hotWaterVolume * 50;
  digitalWrite(6, HIGH);
  delay(hotWaterCount);
  digitalWrite(6, LOW);
  pinMode(9, INPUT);
  pinMode(6, INPUT);
}

void calVolume() {
  coldWaterVolume = (finalVolume * (hotTemp - requiredTemp)) / (hotTemp - coldTemp);
  hotWaterVolume = (finalVolume * (requiredTemp - coldTemp)) / (hotTemp - coldTemp);
}

void getAllTemp() {
  /* 1- Hot Temperature Measurement */

  if (millis() >= tempLastSample + 1)
  {
    tempSampleRead = analogRead(hotThermistorPin);
    tempSampleSum = tempSampleSum + tempSampleRead;
    tempSampleCount = tempSampleCount + 1;
    tempLastSample = millis();
  }

  if (tempSampleCount == 1000)
  {
    tempMean = tempSampleSum / tempSampleCount;
    R2 = (voltageDividerR1 * tempMean) / (1023 - tempMean);

    a = 1 / T1;
    b = log10(R1 / R2);
    c = b / log10(e);
    d = c / BValue ;
    hotTemp = (1 / (a - d)) - 273;
    //    Serial.print("Hot Temperature:");
    //    Serial.print(hotTemp - 273.15, decimalPrecision);
    //    Serial.println(" Â°C");

    tempSampleSum = 0;
    tempSampleCount = 0;
  }

  /* 2- Cold Temperature Measurement */

  if (millis() >= tempLastSample1 + 1)
  {
    tempSampleRead1 = analogRead(coldThermistorPin);
    tempSampleSum1 = tempSampleSum1 + tempSampleRead1;
    tempSampleCount1 = tempSampleCount1 + 1;
    tempLastSample1 = millis();
  }

  if (tempSampleCount1 == 1000)
  {
    tempMean1 = tempSampleSum1 / tempSampleCount1;
    R3 = (voltageDividerR1 * tempMean1) / (1023 - tempMean1);

    a = 1 / T1;
    b = log10(R1 / R3);
    c = b / log10(e);
    d = c / BValue ;
    coldTemp = (1 / (a - d)) - 273;
    //    Serial.print("Cold Temperature:");
    //    Serial.print(coldTemp - 273.15, decimalPrecision);
    //    Serial.println(" Â°C");

    tempSampleSum1 = 0;
    tempSampleCount1 = 0;
  }
}


void setup() {
  Serial.begin(9600);
  while (1) {
    getAllTemp();
    if (hotTemp > 0 && coldTemp > 0 ) {
      Serial.print("hotTemp: ");
      Serial.println(hotTemp);
      Serial.print("coldTemp: ");
      Serial.println(coldTemp);
      break;
    }
  }
}



void loop() {
  Serial.println("\nRequired Temperature.... ");      //Prompt User for input
  while (Serial.available() == 0) {}
  if (Serial.available() > 0) {
    requiredTemp = Serial.parseFloat();            //Read user input and hold it in a variable

  }
  if (requiredTemp != 0) {
    Serial.print("Required Temperature: ");
    Serial.println(requiredTemp);
    while (1) {
      getAllTemp();
      if (hotTemp > 0 && coldTemp > 0 ) {
        Serial.print("hotTemp: ");
        Serial.println(hotTemp);
        Serial.print("coldTemp: ");
        Serial.println(coldTemp);
        calVolume();
        Serial.print("cold  volume:");
        Serial.println( coldWaterVolume);
        Serial.print("hot  volume:");
        Serial.println( hotWaterVolume);
        if (coldWaterVolume > 0 && hotWaterVolume > 0 ) {
          waterPump();
        }
        hotTemp = 0;
        coldTemp = 0;
        break;

      }
    }

  }// wait for user input



}
