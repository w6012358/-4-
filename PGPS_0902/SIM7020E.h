#include <SoftwareSerial.h>
//SoftwareSerial mySerial(12, 13);
SoftwareSerial mySerial(8, 9);

/*****************各項感測模組數量*********************/
int paper_sensors = 0;                //  日月潭_男廁: 1 ; 日月潭_女廁: 1
int gas_sensors = 1;                  //  日月潭_男廁: 1 ; 日月潭_女廁: 1
int soap_sensors = 0;                 
int Alcohol_sensors = 0;              
/*****************各項感測模組起始ID*******************/
int paper_start_id = 110;              //  日月潭_男廁: 109 ; 日月潭_女廁:110
int gas_start_id = 2 ;                //  日月潭_男廁: 87  ; 日月潭_女廁: 88
int soap_start_id = 0;               
int Alcohol_start_id = 0;            
/*****************感測模組對應主板Port起始位置********************/
int paper_start_port = 1;             //  廁紙模組起始埠號
int gas_start_port = 3;               //  氣味模組起始埠號
int soap_start_port = 9;              //  給皂機模組起始埠號
int alcohol_start_port = 11;          //  給皂機模組起始埠號
/*****************雲端平台IP相關資訊********************/
String server = "68.183.176.88";         //  網頁平台IP
String api = "/Testzone/";                   //  API字串

/**********************以下參數不更動，視專案調整**************************/
int N_minute_upload = 5;              //  系統上傳頻率(固定5min)
int system_reset_time = 1820;         //  系統重啟頻率(固定30min+20秒，總計1820秒)
String data = "";                     //  儲存來自模組回覆的字串參數
boolean Net_signed = false;
int sta = 1;                          //  迴圈的status參數判斷
float time_count = 0;
int count = 0;                        //  模組的回覆時間間隔計數器
int IP_count = 0;
String api_pass = "";
void(* resetFunc) (void) = 0;         //  宣告系統重置參數

/***************NBIOT相關副程式Block，不更動(有運作上狀況再行調整)*******************/

/***************NBIOT模組訊息接收Block*******************/
void serial_read(){
  while (mySerial.available()){  //Check if there is an available byte to read,
    delay(3); //Delay added to make thing stable 
    char c = mySerial.read(); //Conduct a serial read
    if (c == '\n') {break;} //Exit the loop when the "" is detected after the word
    data += c; //Shorthand for data = data + c
  }

  if (data.length() > 0) {          // 判斷data內有值在更換
    data.trim();
    Serial.print(">>  ");
    Serial.println(data);
    if(sta == 1){                   //  對模組進行重啟
      if(data == "+CPIN: READY"){
        sta = 4;
      }else if(data == "ERROR"){
        resetFunc();
      }
    }else if(sta == 4){             //  檢查註網情形AT+CGATT=1
      if(data == "OK"){
        Net_signed = true;
        sta++;
      }else if(data == "ERROR"){
        Net_signed = false;
      }
    }else if(sta == 5){             //  設定目標server
      if(data == "OK" || data[0] == "O"){
        sta++;
      }
    }else if(sta == 6){             //  與目標server建立連線
      if(data == "OK" || data[0] == "O"){
        sta++;
      }else if(data == "ERROR"){
        delay(1000);
        resetFunc();
      }
    }
    else if(sta == 7){              //  以GET方式上傳API
      if(data == "OK"){
        sta++;
      }
    }
    else if(sta == 8){              //  完成後切斷連線
      if(data == "OK"){
        sta++;
      }
    }else if(sta == 9){              //  關閉與目標server連線
      if(data == "OK"){
        sta = 5;
      }
    }
    data = "";
  }
}

/***************NBIOT模組重啟指令Block*******************/
void SIM7020E_reset(){        //  對NB模組進行reset
  mySerial.println("AT+CRESET");
  delay(1000);
  while (sta == 1) {          //  等待模組訊息回覆
    serial_read();
  }
}
/***************NBIOT模組註網狀況查詢Block*******************/
void ask_for_IP(){            //  查詢註網情況
  mySerial.println("AT+CGATT=1");
  while (sta == 4) {          //  等待模組訊息回覆
    serial_read();
    delay(1*1000);
    count++;
    if (count > 20) {       //  每20秒問一次IP狀況
      mySerial.println("AT+CGATT=1");
      count = 0;
      IP_count++;
    }
    if(IP_count == 6){     //  若超過2min未註網，則重新啟動系統/模組
      resetFunc();
    }
  }
  count = 0;
  IP_count =0;
}
/***************HTTP伺服器連線Block*******************/
void HTTP_server(String server){                 //  與目標伺服器建立通道
  String AT_http = "AT+CHTTPCREATE=\"http://";
  AT_http.concat(server);
  AT_http.concat("/\"");
  mySerial.println(AT_http);
  while (sta == 5) {          //  等待模組訊息回覆
    serial_read();
    delay(1*1000);
    count++;
    if (count > 10) {       //  若10秒後sta仍未變動
      sta++;
      count = 0;
    }
  }
}
/***************HTTP連線通道設定Block*******************/
void HTTP_connection(){                         //  透過通道建立連線
  mySerial.println("AT+CHTTPCON=0");
  while (sta == 6) {          //  等待模組訊息回覆
    serial_read();
    delay(1*1000);
    count++;
    if (count > 10) {       //  若10秒後sta仍未變動
      sta++;
      count = 0;
    }
  }
}
/***************人流資料API字串內容組成Block*******************/
void GET_data_PIR(String local, int id, int data, int mount){
  api_pass = "AT+CHTTPSEND=0,0,\"";        //  AT+CHTTPSEND=0,0,"/testZone/index.php?data=99&id=1"
  api_pass.concat(api);
  api_pass.concat(local);
  api_pass.concat(".php?id=");
  api_pass.concat(id);
  api_pass.concat("&data=");
  api_pass.concat(data);
  api_pass.concat("\"");
  mySerial.println(api_pass);
  while (sta == 7) {          //  等待模組訊息回覆
    serial_read();
    count++;
    if(count > 5){
      sta++;
      count = 0;
    }
  }
}
/***************給皂/酒精資料API字串內容組成Block*******************/
void GET_data_Soap(String local, int id, int data[], int mount){
  api_pass = "AT+CHTTPSEND=0,0,\"";        //  AT+CHTTPSEND=0,0,"/testZone/index.php?data=99&id=1"
  api_pass.concat(api);
  api_pass.concat(local);
  api_pass.concat(".php?id=");
  api_pass.concat(id);
  api_pass.concat("&at=");
  api_pass.concat(mount);
  api_pass.concat("&data=");
  for(int i = 0; i < mount; i++){
    api_pass.concat(String(data[i]));
    if(i != mount-1){
      api_pass.concat("/");
    }
  }
  api_pass.concat("\"");
  mySerial.println(api_pass);
  while (sta == 7) {          //  等待模組訊息回覆
    serial_read();
    count++;
    if(count > 5){
      sta++;
      count = 0;
    }
  }
}
/***************廁紙、氣味資料API字串上傳指令組成Block*******************/
void GET_data(String local, int id, float data[], int mount){
  api_pass = "AT+CHTTPSEND=0,0,\"";        //  AT+CHTTPSEND=0,0,"/testZone/index.php?data=99&id=1"
  api_pass.concat(api);
  api_pass.concat(local);
  api_pass.concat(".php?id=");
  api_pass.concat(id);
  api_pass.concat("&at=");
  api_pass.concat(mount);
  api_pass.concat("&data=");
  for(int i = 0; i < mount; i++){
    api_pass.concat(String(data[i]));
    if(i != mount-1){
      api_pass.concat("/");
    }
  }
  api_pass.concat("\"");
  mySerial.println(api_pass);
  while (sta == 7) {          //  等待模組訊息回覆
    serial_read();
    count++;
    if(count > 5){
      sta++;
      count = 0;
    }
  }
}
/***************HTTP伺服器連線斷開Block*******************/
void HTTP_disconnect(){                         //  
  mySerial.println("AT+CHTTPDISCON=0");
  while (sta == 8) {          //  等待模組訊息回覆
    serial_read();
    delay(1*1000);
    count++;
    if (count > 10) {       //  若10秒後sta仍未變動
      sta++;
      count = 0;
    }
  }
}
/***************通訊通道關閉Block*******************/
void connect_Release(){                         //  
  mySerial.println("AT+CHTTPDESTROY=0");
  while (sta == 9) {          //  等待模組訊息回覆
    serial_read();
    delay(1*1000);
    count++;
    if (count > 10) {       //  若10秒後sta仍未變動
      sta = 5;
      count = 0;
    }
  }
}
/***************資料上傳Block(廁紙、氣味模組)*******************/
void data_update(String local, int id, float data[], int mount){     //  資料上傳function
  Serial.println("Sending data......\n");
  
  HTTP_server(server);
  delay(1000);
  HTTP_connection();
  delay(1000);
  GET_data(local, id, data, mount);
  delay(1000);
  HTTP_disconnect();
  delay(1000);
  connect_Release();
  delay(1000);
  Serial.println();
  Serial.print("Data server IP: ");
  Serial.println(server);
  Serial.print("Platfrom data api: ");
  Serial.println(api_pass);
  Serial.println();
  Serial.println("**************************************************");
  Serial.println();
  sta = 5;
}
/***************資料上傳Block(人流模組專用)*******************/
void data_update_PIR(String local, int id, int data, int mount){     //  資料上傳function
  Serial.println("Sending data......\n");
  
  HTTP_server(server);
  delay(1000);
  HTTP_connection();
  delay(1000);
  GET_data_PIR(local, id, data, mount);
  delay(1000);
  HTTP_disconnect();
  delay(1000);
  connect_Release();
  delay(1000);
  Serial.println();
  Serial.print("Data server IP: ");
  Serial.println(server);
  Serial.print("Platfrom data api: ");
  Serial.println(api_pass);
  Serial.println();
  Serial.println("**************************************************");
  Serial.println();
  sta = 5;
}
/***************資料上傳Block(給皂/酒精模組專用)*******************/
void data_update_Soap(String local, int id, int data[], int mount){     //  資料上傳function
  Serial.println("Sending data......\n");
  
  HTTP_server(server);
  delay(1000);
  HTTP_connection();
  delay(1000);
  GET_data_Soap(local, id, data, mount);
  delay(1000);
  HTTP_disconnect();
  delay(1000);
  connect_Release();
  delay(1000);
  Serial.println();
  Serial.print("Data server IP: ");
  Serial.println(server);
  Serial.print("Platfrom data api: ");
  Serial.println(api_pass);
  Serial.println();
  Serial.println("**************************************************");
  Serial.println();
  sta = 5;
}
/***************NBIOT模組初始設定Block*******************/
void NBModule_initail(){
  SIM7020E_reset();
  Serial.println();
  Serial.println("Waiting for IP......");
  Serial.println();
  delay(20*1000);       //  等待20秒連線
  ask_for_IP();         //  查詢IP狀況
  delay(3000);
}
