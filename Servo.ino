void capit(){
  unsigned long now = millis();
  if (now - timeA < 500) return; // cegah double trigger
  timeA = now;
  capiton = !capiton;

  Serial.println("capit");
}

void ReposisionGripper() {
  unsigned long now = millis();
  if (now - timeB < 500) return; // cegah double trigger
  timeB = now;

  if (G1 > 0 && G1 <= 72 || G1 == 145) {
    G1 = 0;
  } 
  else if (G1 < 145 && G1 > 72 || G1 == 0) {
    G1 = 145;
  }
  
  Serial.print("Reposisi ke: ");
  Serial.println(G1);
}

void griper() {
  if (Up && G1 < 145) {G1+=5; Serial.println(G1);}
  if (Down && G1 > 0) {G1-=5; Serial.println(G1);
  if (G1 < 0) {G1 = 0;}}

  if (capiton) {G2 = 120;} else {G2 = 35;}

  servoangkat.write(G1); 
  servocapit.write(G2);
}
