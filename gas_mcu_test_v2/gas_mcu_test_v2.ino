#define de1 A0
#define fan_control 3

#define coefficient_A 19.32
#define coefficient_B -0.64

int pir = 0;
int detect1 = 0;
float gas_data = 0;
float detect_data = 0;
float total_data = 0;
float ave_gasdata = 0;
float fan_start = 1.5;
unsigned long myTime;                //  系統時間計數器
int millisCheck = 0;                 //  量測
int system_reset_time = 1820;         //  系統重啟頻率(固定30min+20秒，總計1820秒)
int N_minute_aveReset = 5;              //  系統上傳頻率(固定5min)
void(* resetFunc) (void) = 0;         //  宣告系統重置參數
int fan_start_control = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(fan_control, OUTPUT);
  analogWrite(fan_control, 100);
  ave_get();
  delay(5000);
}

void loop() {
  // put your main code here, to run repeatedly:
  myTime = millis();
  system_reset(system_reset_time);

  if ((myTime) / 1000 / N_minute_aveReset / 60 > millisCheck ) {
    millisCheck++;
    ave_get();
  }
  
  gas_data = sensor_GAS();

  if (int(gas_data) >= int(fan_start * ave_gasdata)) {
    fan_start_control++;
    if (fan_start_control > 5000) {
      digitalWrite(fan_control, HIGH);
      Serial.print("Gas_data is larger than ave_gasdata.");
      Serial.print("Gas_data : ");
      Serial.print(gas_data);
      Serial.print(" ; 1.5 * ave_gasdata : ");
      Serial.println(float(fan_start * ave_gasdata));
      delay(1000);
    }

  }else {
    fan_start_control--;
    if(fan_start_control != 0){
      fan_start_control--;
    }else{
      fan_start_control = 0;
    }
    
    if (fan_start_control < 5) {
      analogWrite(fan_control, 100);
      fan_start_control = 0;
    }
    Serial.print("Gas data : ");
    Serial.println(gas_data);
  }
}

void ave_get() {
  ave_gasdata = 0;
  
  for (int i = 0; i < 30 ; i++) {
    gas_data = sensor_GAS();
    total_data += gas_data;
    Serial.print(i);
    Serial.println(" second");
    delay(1000);
  }
  ave_gasdata = float(total_data / 30);
  Serial.print("ave_data = ");
  Serial.println(ave_gasdata);
  total_data = 0;
}

void system_reset(int reset_time) {
  if (myTime / 1000 > reset_time) {
    resetFunc();
  }
}

/***************氣味感測*******************/
float get_voltage() {
  detect1 = analogRead(de1);
  detect_data = detect1 * (5.0 / 1023.0);
  return detect_data;
}

float TGS2602_ratio(float v) {
  return (5.0 - v) / v;
}

float sensor_GAS() {         // 臭味資料換算function
  detect_data = (float)(coefficient_A * pow(TGS2602_ratio(get_voltage()), coefficient_B));
  return detect_data;
}
