void capit(){
  capiton = !capiton;

  if (capiton) {servocapit.write(110);}
  else {servocapit.write(10);}

  Serial.println("capit");
  delay(200);
}

void resetposisiongriper(){G1 = 0;}

void resetposisiYongriper(){G1 = 144;}

void griperhead1() {
  if (Up && G1 <= 140) {G1+=4; Serial.println(G1); delay(servoSpeed);}
  if (Down && G1 >= 0) {G1-=4; Serial.println(G1); delay(servoSpeed);
  if (G1 < 0) {G1 = 0;}}

  servoangkat.write(G1); 
}
