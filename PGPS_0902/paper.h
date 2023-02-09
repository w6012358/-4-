#define de1 A0  //  接到74HC4051 COM IN/OUT腳位的類比腳位(P1～P8)
#define de2 A2  //  接到74HC4051 COM IN/OUT腳位的類比腳位(P33)
#define de3 A3  //  接到74HC4051 COM IN/OUT腳位的類比腳位(P34)
#define de4 A4  //  接到74HC4051 COM IN/OUT腳位的類比腳位(P34)
#define de5 A5  //  接到74HC4051 COM IN/OUT腳位的類比腳位(P34)

#define coefficient_A 19.32
#define coefficient_B -0.64

int pir = 0;
int detect1 = 0;
float distance = 0;
float v = 0;
float detect_data = 0;
float ave = 0;
float percent = 0;
const float a = 48.375;
const float b = 0.0675;
/***************給皂感測*******************/
int sensor_soap(int ch){
  int soap = 0;
  if(ch == 9){
    detect1 = analogRead(de2);
  }else if(ch == 10){
    detect1 = analogRead(de3);
  }
  
//  Serial.print("(Soap) analog data : ");
//  Serial.println(detect1);
  if(detect1 >= 500){ 
    soap = 1;
  }else if(detect1 < 500){
    soap = 0;
  }
  return soap;
}
/***************酒精感測*******************/
int sensor_Alcohol(int ch){
  if(ch == 11){
    detect1 = analogRead(de4);
  }else if(ch == 12){
    detect1 = analogRead(de5);
  }
//  Serial.print("(Soap) analog data : ");
//  Serial.println(detect1);
  if(detect1 >= 500){ 
    pir = 1;
  }else if(detect1 < 500){
    pir = 0;
  }
  return pir;
}
/***************廁紙感測*******************/
float sensor_paper(int p_sensor){
  ave = 0;
  for(int k = 0; k < 100 ; k++){
    detect1 = analogRead(de1);
    ave = ave + detect1;
  }
  distance = ave/100;
  v = (5.0 * distance)/1024.0;
  if ( v > b ) {
    detect_data = a / (v - b);
  }else{
    detect_data = 20;
  }
  return detect_data/10;
}
/***************氣味感測*******************/
float get_voltage(){
  detect1 = analogRead(de1);
  detect_data = detect1*(5.0/1023.0);
  return detect_data;
}

float TGS2602_ratio(float v){
  return (5.0-v)/v;
}

float sensor_GAS(){          // 臭味資料換算function
  detect_data = (float)(coefficient_A * pow(TGS2602_ratio(get_voltage()), coefficient_B));
  return detect_data;
}
