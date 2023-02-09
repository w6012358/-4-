#include "SIM7020E.h"
#include "paper.h"
#define S0 2            //  設定74HC4051控制腳位(S0)
#define S1 3            //  設定74HC4051控制腳位(S1)
#define S2 4            //  設定74HC4051控制腳位(S2)

/***************參數設定Block*******************/
float ICA_gas[2] = {0, 0};
float ICA_paper[4] = {0, 0, 0, 0};
int ICA_soap[2] = {0, 0};
int ICA_alcohol[2] = {0, 0};
float gas_ave[2] = {0, 0};
int ADC_delay = 250;                 //  設定74HC4051的通道數值穩定時間(msec)
float smell_tempt = 0;               //  偵測值暫存參數
unsigned long myTime;                //  系統時間計數器
int millisCheck = 0;                 //  量測

/***************系統重啟Block*******************/
void system_reset(int reset_time){
  if(myTime / 1000 > reset_time){
    resetFunc();
  }
}
/***************74HC4051通道切換Block*******************/
void read_4051(int i) {
  if (i == 0) {
    digitalWrite(S0, LOW);
    digitalWrite(S1, LOW);
    digitalWrite(S2, LOW);
  } else if (i == 1) {
    digitalWrite(S0, HIGH);
    digitalWrite(S1, LOW);
    digitalWrite(S2, LOW);
  } else if (i == 2) {
    digitalWrite(S0, LOW);
    digitalWrite(S1, HIGH);
    digitalWrite(S2, LOW);
  } else if (i == 3) {
    digitalWrite(S0, HIGH);
    digitalWrite(S1, HIGH);
    digitalWrite(S2, LOW);
  } else if (i == 4) {
    digitalWrite(S0, LOW);
    digitalWrite(S1, LOW);
    digitalWrite(S2, HIGH);
  } else if (i == 5) {
    digitalWrite(S0, HIGH);
    digitalWrite(S1, LOW);
    digitalWrite(S2, HIGH);
  } else if (i == 6) {
    digitalWrite(S0, LOW);
    digitalWrite(S1, HIGH);
    digitalWrite(S2, HIGH);
  } else if (i == 7) {
    digitalWrite(S0, HIGH);
    digitalWrite(S1, HIGH);
    digitalWrite(S2, HIGH);
  }
}

/***************數據讀取Block*******************/
void dataRead() {             //  感測器讀取副程式
/***************氣味感測Block******************/
  if (gas_sensors != 0) {
    for (int i = gas_start_port; i < (gas_sensors + gas_start_port); i++) {
      read_4051(i - 1);
      delay(ADC_delay);
      smell_tempt = sensor_GAS();
      if (String(smell_tempt) == "inf" || String(smell_tempt) == "INF") {       //  若數值為字串形式，則將偵測值定為999，表示感測器有問題
        ICA_gas[i - gas_start_port] = float(999);
      } else if (String(smell_tempt) == "nan" || String(smell_tempt) == "NAN") {//  若數值為字串形式，則將偵測值定為999，表示感測器有問題
        ICA_gas[i - gas_start_port] = float(999);
      } else {                                                                  //  若數值為一般小數，則儲存該值
        ICA_gas[i - gas_start_port] = smell_tempt;
      }
    }
  }
/**************廁紙感測Block******************/
  if(paper_sensors != 0){
    for (int i = paper_start_port; i < (paper_start_port + paper_sensors); i++) {
      read_4051(i - 1);
      delay(ADC_delay);
      ICA_paper[i - 1] = sensor_paper(i);
      if(int(ICA_paper[i - 1]) >= 15){
        ICA_paper[i - 1] = 99.99;
      }
    }
  }
/**************給皂感測Block******************/
  if(soap_sensors != 0){
    for (int i = soap_start_port; i < (soap_start_port + soap_sensors); i++) {
      ICA_soap[i - soap_start_port] = sensor_soap(i);
    }
  }
/**************酒精感測Block******************/
  if(Alcohol_sensors != 0){
    for (int i = alcohol_start_port; i < (alcohol_start_port + Alcohol_sensors); i++) {
      ICA_alcohol[i - alcohol_start_port] = sensor_Alcohol(i);
    }
  }
}

/***************數據印出Block*******************/

void data_print() {             //  感測器資料印出副程式
/**************氣味感測Block******************/
  if (gas_sensors != 0) {
    Serial.print("(Gas) Now collected data: ");
    for (int i = 1; i <= gas_sensors; i++) {
      Serial.print("Gas" + String(i) + ": ");
      Serial.print(ICA_gas[i - 1]);
      Serial.print(", ");
    }
    Serial.println();
  }
/**************廁紙感測Block******************/
  if (paper_sensors != 0) {
    Serial.print("(Paper) Now collected data: ");
    for (int i = 1; i <= paper_sensors; i++) {
      Serial.print("Paper" + String(i) + ": ");
      Serial.print(ICA_paper[i - 1]);
      Serial.print(", ");
    }
    Serial.println();
  }
/**************給皂感測Block******************/
  if(soap_sensors != 0){
    Serial.print("(Soap) Now collected data: ");
    for (int i = 1; i <= soap_sensors; i++) {
      Serial.print("Soap" + String(i) + ": ");
      Serial.print(ICA_soap[i - 1]);
      Serial.print(", ");
    }
    Serial.println();
  }
/**************酒精感測Block******************/
  if(Alcohol_sensors != 0){
    Serial.print("(Alcohol) Now collected data: ");
    for (int i = 1; i <= Alcohol_sensors; i++) {
      Serial.print("Alcohol" + String(i) + ": ");
      Serial.print(ICA_alcohol[i - 1]);
      Serial.print(", ");
    }
    Serial.println();
  }
/**************系統時間顯示Block******************/  
  Serial.print("System restart countdown is = ");
  Serial.println(system_reset_time - (myTime/1000));
  Serial.println();
  Serial.println("**************************************************");
  Serial.println();
}

/***************全部數據上傳Block*******************/
void All_data_upload() {
/**************廁紙感測Block******************/
  if (paper_sensors != 0) {
    delay(5000);
    Serial.println("(Paper) uploading......");
    data_update("Toiletpaper", paper_start_id, ICA_paper, paper_sensors);
  }
/**************氣味感測Block******************/
  if (gas_sensors != 0) {
    delay(5000);
    Serial.println("(Gas) uploading......");
    data_update("Smelly", gas_start_id, ICA_gas, gas_sensors);
  }
/**************給皂感測Block******************/
  if (soap_sensors != 0) {
    delay(5000);
    Serial.println("(Soap) uploading......");
    data_update_Soap("Handlotion", soap_start_id, ICA_soap, soap_sensors);
  }
/**************酒精感測Block******************/
  if (Alcohol_sensors != 0) {
    delay(5000);
    Serial.println("(Alcohol) uploading......");
    data_update_Soap("Alcohol", Alcohol_start_id, ICA_alcohol, Alcohol_sensors);
  }
}


/***************定時數據上傳Block*******************/
void All_data_upload_Nmin(int minute_N) {
  if ((myTime) / 1000 / minute_N / 60 > millisCheck ) {
    millisCheck++;
    All_data_upload();
  }
}
/***************腳位設定Block*******************/
void pinMode_setting() {                  //  宣告腳位
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  digitalWrite(S0, LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S2, LOW);
}
/***************偵測頻率設定Block*******************/
void delay_time() {
  if ((paper_sensors + gas_sensors) == 1) {
    delay(4750);
  } else if ((paper_sensors + gas_sensors) == 2) {
    delay(4500);
  } else if ((paper_sensors + gas_sensors) == 3) {
    delay(4250);
  } else if ((paper_sensors + gas_sensors) > 3) {
    delay(4000);
  }else if((paper_sensors + gas_sensors) == 0){
    delay(5000);
  }
}
