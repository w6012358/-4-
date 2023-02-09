#include "sensor.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  mySerial.begin(9600);
  pinMode_setting();
  Serial.println("**************************************************");
  Serial.println();
  
  initial_setting();
  
  Serial.println("Service start.......");
  Serial.println();
  Serial.println("**************************************************");
  
  
}

void loop() {
  // put your main code here, to run repeatedly:
  myTime = millis();
  system_reset(system_reset_time);

  traffic_detect();
  data_print();

  All_data_upload_Nmin(N_minute_upload, pass_data);
}

void initial_setting(){
  traffic_counting();
  data_print();                             //  print出資訊
  Serial.println("Boot NBA-12P");
  Serial.println();
  
  NBModule_initail(pass_data);                       //  啟動與設定連線的伺服器
  All_data_upload(pass_data);                        //  資料上傳
}
