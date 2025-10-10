void capit(){
  capiton = !capiton;

  if (capiton) {servocapit.write(110);}
  else {servocapit.write(10);}

  Serial.println("capit");
  delay(200);
}

unsigned long lastReposTime = 0;   // waktu terakhir reset
const unsigned long reposDelay = 500; // delay 500 ms antar reset

void ReposisionGripper() {
  unsigned long now = millis();
  if (now - lastReposTime < reposDelay) return; // cegah double trigger

  lastReposTime = now; // update waktu terakhir

  if (G1 > 0 && G1 <= 72 || G1 == 144) {
    // kalau di bawah tengah atau posisi bawah, pindah ke atas
    G1 = 0;
  } 
  else if (G1 < 144 && G1 > 72 || G1 == 0) {
    // kalau di atas tengah atau posisi atas, pindah ke bawah
    G1 = 144;
  }
  
  Serial.print("Reposisi ke: ");
  Serial.println(G1);
}
// void resetposisiongriper(){G1 = 0;}

// void resetposisiYongriper(){G1 = 144;}

void griperhead1() {
  if (Up && G1 <= 140) {G1+=4; Serial.println(G1); delay(servoSpeed);}
  if (Down && G1 >= 0) {G1-=4; Serial.println(G1); delay(servoSpeed);
  if (G1 < 0) {G1 = 0;}}

  servoangkat.write(G1); 
}
