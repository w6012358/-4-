#include "SIM7020E.h"
#include "paper.h"

/***************參數設定Block*******************/

unsigned long myTime;                //  系統時間計數器
int millisCheck = 0;                 //
int printCheck = 0;
boolean print_data = false;
int pass_data = 1;


/***************系統重啟Block*******************/
void system_reset(int reset_time) {
  if (myTime / 1000 > reset_time) {
    resetFunc();
  }
}

/***************數據印出Block*******************/

void data_print() {             //  感測器資料印出副程式
  if ((myTime / printTime) > printCheck) {
    printCheck++;
    print_data = true;
  }

  if (print_data == true) {
    Serial.print("Sensor Status : near-door : ");
    Serial.print(digitalRead(near_door));
    Serial.print(" , far-door : ");
    Serial.println(digitalRead(far_door));
    Serial.print("millis(): ");
    Serial.println(myTime / 1000);
    Serial.print("System restart countdown is = ");
    Serial.println(system_reset_time - (myTime / 1000));
    Serial.println();
    Serial.println("**************************************************");

    traffic_counting();
    print_data = false;
  }
  /**************系統時間顯示Block******************/

}

/***************全部數據上傳Block*******************/
void All_data_upload(int sta) {
  if (sta == 1) {
    if (PIR_sensors != 0) {
      Serial.println("(PIR) uploading......");
      data_update_Soap("Toiletpaper", PIR_start_id, PIR, PIR_sensors);
    }
  }


}


/***************定時數據上傳Block*******************/
void All_data_upload_Nmin(int minute_N, int sta) {
  if ((myTime) / 1000 / minute_N / 60 > millisCheck ) {
    millisCheck++;
    All_data_upload(sta);
  }
}
/***************腳位設定Block*******************/
void pinMode_setting() {                  //  宣告腳位
  pinMode(near_door, INPUT);
  pinMode(far_door, INPUT);
}

