#define near_door A4  //  接到74HC4051 COM IN/OUT腳位的類比腳位(P34)
#define far_door A5  //  接到74HC4051 COM IN/OUT腳位的類比腳位(P34)

int door_traffic[3] = {9, 9, 9};
int PIR[2] = {0, 0};
int near_tempt = 0;
int far_tempt = 0;
int traffic_count = 0;
int people_count_in = 0;
int people_count_out = 0;
int sta_door = 0;
int loop_count = 0;
int printTime = 1000;
int stayTime = 5000;
boolean change = false;

void traffic_counting();

/***************給皂感測*******************/
void traffic_detect() {

  if (change == true) {
    traffic_counting();
    change = false;
    for (int i = 0; i < 4; i++) {
      door_traffic[i] = 9;
    }
  }

  /******************************初始動點***********************************************/
  if (digitalRead(near_door) == 0 && digitalRead(far_door) == 1) {            //  視為入內，進入進門判斷式
    door_traffic[0] = 1;
    sta_door = 1;
  } else if (digitalRead(near_door) == 1 && digitalRead(far_door) == 0) {     //  視為外出，進入出門判斷式
    door_traffic[2] = 0;
    sta_door = 3;
  }
  /******************************進門判斷式***********************************************/
  while (sta_door == 1) {                                                     //  前一動為0, 1
    Serial.print("At stage : ");
    Serial.println(sta_door);
    if (digitalRead(near_door) == 1 && digitalRead(far_door) == 0) {          //  判定為進入
      door_traffic[1] = 1;
      sta_door = 2;
    } else if (digitalRead(near_door) == 0 && digitalRead(far_door) == 1) {   //  判定為滯留，不動作
      
    } else {                                                                  //  誤觸發保險機制，(stayTime/1000)秒後回到sta_door = 0重新判定
      loop_count++;
    }
    if (loop_count > stayTime) {
      door_traffic[0] = 9;
      door_traffic[1] = 9;
      door_traffic[2] = 9;
      sta_door = 0;
      loop_count = 0;
    }
  }

  while (sta_door == 2) {                                                     //  前一動為1,0
    Serial.print("At stage : ");
    Serial.println(sta_door);//  前一動為0, 0
    if (digitalRead(near_door) == 1 && digitalRead(far_door) == 1) {          //  判定為完整進入，回到sta_door = 0重新判定
      door_traffic[2] = 1;
      sta_door = 0;
    } else if (digitalRead(near_door) == 1 && digitalRead(far_door) == 0) {   //  判定為滯留，不動作
      
    } else {                                                                  //  誤觸發保險機制，(stayTime/1000)秒後回到sta_door = 0重新判定
      loop_count++;
    }
    if (loop_count > stayTime) {
      door_traffic[0] = 9;
      door_traffic[1] = 9;
      door_traffic[2] = 9;
      sta_door = 0;
      loop_count = 0;
    }
  }

  //  /******************************出門判斷式***********************************************/
  while (sta_door == 3) {                                                     //  前一動為1, 0
    Serial.print("At stage : ");
    Serial.println(sta_door);
    if (digitalRead(near_door) == 0 && digitalRead(far_door) == 1) {          //  視為離開
      door_traffic[1] = 0;
      sta_door = 4;
    } else if (digitalRead(near_door) == 1 && digitalRead(far_door) == 0) {   //  判定為滯留，不動作
      
    } else {                                                                  //  誤觸發保險機制，(stayTime/1000)秒後回到sta_door = 0重新判定
      loop_count++;
    }
    if (loop_count > stayTime) {
      door_traffic[0] = 9;
      door_traffic[1] = 9;
      door_traffic[2] = 9;
      sta_door = 0;
      loop_count = 0;
    }
  }

  while (sta_door == 4) {                                                     //  前一動為0, 1
    Serial.print("At stage : ");
    Serial.println(sta_door);
    if (digitalRead(near_door) == 1 && digitalRead(far_door) == 1) {          //  判定為完整離開，回到sta_door = 0重新判定
      door_traffic[0] = 0;
      sta_door = 0;
    } else if (digitalRead(near_door) == 0 && digitalRead(far_door) == 1) {   //  判定為滯留，不動作
      
    } else {                                                                  //  誤觸發保險機制，(stayTime/1000)秒後回到sta_door = 0重新判定
      loop_count++;
    }
    if (loop_count > stayTime) {
      door_traffic[0] = 9;
      door_traffic[1] = 9;
      door_traffic[2] = 9;
      sta_door = 0;
      loop_count = 0;
    }
  }
}

void traffic_counting() {
  for (int i = 0; i < 3; i++) {
    traffic_count += door_traffic[i];
  }
  if (traffic_count == 3) {
    people_count_in++;
    PIR[0] = people_count_in;
  } else if (traffic_count == 0) {
    people_count_out++;
    PIR[1] = people_count_out;
  }

  for (int i = 0; i < 3; i++) {
    door_traffic[i] = 9;
  }

  traffic_count = 0;

  Serial.print("Get in count : ");
  Serial.println(PIR[0]);
  Serial.print("Get out count : ");
  Serial.println(PIR[1]);
  Serial.println("**********************************************");
}
