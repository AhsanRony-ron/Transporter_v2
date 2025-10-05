void lcd(int A, int B, int C, int D) {
  u8g2.clearBuffer();
  u8g2.setDrawColor(1);
  u8g2.setFont(u8g2_font_5x7_tr);


  u8g2.drawStr(0, 32, Ps3.isConnected() ? "Connect" : "Disconnect");

  u8g2.setCursor(87, 12);
  u8g2.print(lx);

  u8g2.setCursor(109, 12);
  u8g2.print(ly);

  u8g2.setCursor(87, 22);
  u8g2.print(rx);

  u8g2.setCursor(109, 22);
  u8g2.print(ry);

  // u8g2.drawLine(127, 9, 87, 9);

  // u8g2.drawLine(107, 1, 107, 18);

  u8g2.drawStr(1, 64, "128");

  u8g2.sendBuffer();
}

// void LCDMotor(int a, int b, int c, int d) {


// }
