void capit(){
  capiton = !capiton;

  Serial.println("capit");

}



void ReposisionGripper() {
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

  if (capiton) {G2 = 11;} else {G2 = 110;}

  servoangkat.write(G1); 
  servocapit.write(G2);
}
