unsigned long myArr[]={1,123,5,115,7,99,12,100,101,122,83,4392,12,1246};
int c=0;
void setup() {
  Serial.begin(9600);
}

void loop() {
  while (c < 1) {
    for ( int i = 0; i < 14; i++) {
    Serial.println((String)"myArr element " + i + " = " + myArr[i]);
    }
    c++;
  }
}
