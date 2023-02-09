#define near_door A5
#define far_door A4

int near_tempt = 0;
int far_tempt = 0;
int door_traffic[4] = {9, 9, 9, 9};
int sta_door = 0;
int traffic_count = 0;
int people_count_in = 0;
int people_count_out = 0;
boolean change = false;
unsigned long myTime;                //  系統時間計數器
int millisCheck = 0;                 //
int printCheck = 0;
boolean print_data = false;
int loop_count = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(near_door, INPUT);
  pinMode(far_door, INPUT);
  traffic_counting();
}

void loop() {
  // put your main code here, to run repeatedly:
  myTime = millis();
  traffic_detect();
  dataPrint();
//  delay(100);
}

void traffic_detect() {

  if (change == true) {
    traffic_counting();
    change = false;
    for (int i = 0; i < 4; i++) {
      door_traffic[i] = 9;
    }
  }



  /******************************初始動點***********************************************/
  if (digitalRead(near_door) == 0 && digitalRead(far_door) == 1) {        //  視為向內行走，進入進門判斷式
    door_traffic[0] = 1;
    sta_door = 1;
  } else if (digitalRead(near_door) == 1 && digitalRead(far_door) == 0) {  //  視為向外行走，進入出門判斷式
    door_traffic[3] = 0;
    sta_door = 5;
  }
  /******************************進門判斷式***********************************************/
  while (sta_door == 1) {                                                   //  前一動為0, 1
    Serial.print("At stage : ");
    Serial.println(sta_door);
    if (digitalRead(near_door) == 1 && digitalRead(far_door) == 1) {  //  視為折返
      door_traffic[1] = 1;
      sta_door = 2;
    }

  }

  while (sta_door == 2) {
    Serial.print("At stage : ");
    Serial.println(sta_door);//  前一動為0, 0
    if (digitalRead(near_door) == 0 && digitalRead(far_door) == 1) {        //  繼續向內行走
      door_traffic[1] = 9;
      sta_door = 1;
    } else if (digitalRead(near_door) == 1 && digitalRead(far_door) == 0) {  //  視為折返，回到sta_door = 1重新判斷
      door_traffic[2] = 1;
      sta_door = 3;
    } else if (digitalRead(near_door) == 0 && digitalRead(far_door) == 0) {  //  視為折返，回到sta_door = 1重新判斷
      door_traffic[2] = 1;
      sta_door = 3;
    } else if (digitalRead(near_door) == 1 && digitalRead(far_door) == 1) {  //  視為折返，回到sta_door = 1重新判斷
      loop_count++;
    }
    if (loop_count > 5000) {
      door_traffic[1] = 9;
      door_traffic[0] = 9;
      sta_door = 0;
      loop_count = 0;
    }
  }

  while (sta_door == 3) {                                                   //  前一動為1, 0
    Serial.print("At stage : ");
    Serial.println(sta_door);
    if (digitalRead(near_door) == 1 && digitalRead(far_door) == 1) {  //  視為折返，回到sta_door = 2重新判斷
      door_traffic[3] = 1;
      sta_door = 4;
    } else if (digitalRead(near_door) == 0 && digitalRead(far_door) == 1) {  //  視為折返，回到sta_door = 1重新判斷
      door_traffic[2] = 9;
      sta_door = 2;
    }
  }

  while (sta_door == 4) {                                                   //  前一動為1, 0
    Serial.print("At stage : ");
    Serial.println(sta_door);
    if (digitalRead(near_door) == 1 && digitalRead(far_door) == 1) {  //  視為折返，回到sta_door = 0重新判斷
      sta_door = 0;
    }
  }
  //  /******************************出門判斷式***********************************************/
  while (sta_door == 5) {                                                   //  前一動為0, 1
    Serial.print("At stage : ");
    Serial.println(sta_door);
    if (digitalRead(near_door) == 1 && digitalRead(far_door) == 1) {  //  視為折返
      door_traffic[2] = 0;
      sta_door = 6;
    }

  }

  while (sta_door == 6) {
    Serial.print("At stage : ");
    Serial.println(sta_door);//  前一動為0, 0
    if (digitalRead(near_door) == 0 && digitalRead(far_door) == 1) {        //  繼續向內行走
      door_traffic[1] = 0;
      sta_door = 7;
    } else if (digitalRead(near_door) == 1 && digitalRead(far_door) == 0) {  //  視為折返，回到sta_door = 1重新判斷
      door_traffic[2] = 9;
      sta_door = 5;
    } else if (digitalRead(near_door) == 0 && digitalRead(far_door) == 0) {  //  視為折返，回到sta_door = 1重新判斷
      door_traffic[1] = 0;
      sta_door = 7;
    } else if (digitalRead(near_door) == 1 && digitalRead(far_door) == 1) {  //  視為折返，回到sta_door = 1重新判斷
      loop_count++;
    }
    if (loop_count > 5000) {
      door_traffic[1] = 9;
      door_traffic[0] = 9;
      sta_door = 0;
      loop_count = 0;
    }
  }

  while (sta_door == 7) {                                                   //  前一動為1, 0
    Serial.print("At stage : ");
    Serial.println(sta_door);
    if (digitalRead(near_door) == 1 && digitalRead(far_door) == 1) {  //  視為折返，回到sta_door = 2重新判斷
      door_traffic[3] = 9;
      sta_door = 7;
    } else if (digitalRead(near_door) == 0 && digitalRead(far_door) == 1) {  //  視為折返，回到sta_door = 1重新判斷
      door_traffic[0] = 0;
      sta_door = 8;
    }
  }

  while (sta_door == 8) {                                                   //  前一動為1, 0
    Serial.print("At stage : ");
    Serial.println(sta_door);
    if (digitalRead(near_door) == 1 && digitalRead(far_door) == 1) {  //  視為折返，回到sta_door = 0重新判斷
      sta_door = 0;
    }
  }
}

void traffic_counting() {
  for (int i = 0; i < 4; i++) {
    traffic_count += door_traffic[i];
  }
  if (traffic_count == 4) {
    people_count_in++;
  } else if (traffic_count == 0) {
    people_count_out++;
  }

  for (int i = 0; i < 4; i++) {
    door_traffic[i] = 9;
  }

  traffic_count = 0;

  Serial.print("Get in count : ");
  Serial.println(people_count_in);
  Serial.print("Get out count : ");
  Serial.println(people_count_out);
  Serial.println("**********************************************");
}

void dataPrint() {
  if ((myTime / 1000) > printCheck) {
    printCheck++;
    print_data = true;
  }

  if (print_data == true) {
    Serial.print("millis(): ");
    Serial.println(myTime / 1000);
    Serial.println("**********************************************");
    traffic_counting();
    print_data = false;
  }
}
