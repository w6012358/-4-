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
  Serial.print("millis(): ");
  Serial.println(myTime/1000);

  system_reset(system_reset_time);

  dataRead();
  data_print();

  All_data_upload_Nmin(N_minute_upload);
  delay_time();
}

void initial_setting(){
  if(gas_sensors != 0){
    Serial.println("Initialing the data for 30secs......");
    Serial.println();
    for(int i = 0; i < 30; i++){
      dataRead();
      Serial.print(i+1);
      Serial.println("seconds");
      delay(1000);
    }
  }  
  data_print();                               //  print出資訊
  Serial.println("Boot NBA-12P");
  Serial.println();
  
  NBModule_initail();                       //  啟動與設定連線的伺服器
  All_data_upload();                        //  資料上傳
}
