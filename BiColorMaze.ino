// https://learn.adafruit.com/adafruit-led-backpack/
// Binary Tree Code for AdaFruit 8x8 Matrix Backpack
// Paul J Karlik | github.com/pjkarlik 2017

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>

Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();

// Set up for Maze //
int rows = 3;
int cols = rows;
int cr = 2 * rows + 1;
int cc = 2 * cols + 1;
int tCells = cr * cc;
char *cellData = (char*)malloc(sizeof(char)*tCells);

// User and Joystick //
int joyX = 0;
int joyY = 0;
int select = 0;
int user = 8;

void setup() {
  Serial.begin(9600);
  matrix.begin(0x70);  // pass in the address
  matrix.setRotation(1);
  createDataSet();
  pinMode(4, INPUT_PULLUP);
}

void loop() {
  readJoy();
  displayMaze();
  delay(30);
}

void displayMaze() {
  for (int d = 0; d < tCells; d += 1) {
    // Take index and get x / y from d //
    int x = d % cc;
    int y = (d - x) / cc;
    matrix.writePixel(x, y, cellData[d] == 1 ? LED_GREEN : 0);
  }
  displayMe();
  matrix.writeDisplay();
}

void displayMe() {
  int x = user % cc;
  int y = (user - x) / cc;
  matrix.writePixel(x, y, LED_RED);
}

int getCell(int index) {
  return cellData[index];
}

void readJoy() {
  uint32_t jx = analogRead(1) * 0.1;
  uint32_t jy = analogRead(2) * 0.1;
  uint32_t sl = digitalRead(4);

  int cell;
  if(jx < 30) {
    cell = getCell(user - 1);
    Serial.println(cell);
    if(cell == 0){
      user -=1;
      delay(100);
    }
  }
  if(jx > 60) {
    cell = getCell(user + 1);
    Serial.println(cell);
    if(cell == 0){
      user +=1;
      delay(100);
    }
  }
  if(jy < 30) {
    cell = getCell(user - cc);
    Serial.println(cell);
    if(cell == 0){
      user -=cc;
      delay(100);
    }
  }
  if(jy > 60) {
    cell = getCell(user + cc);
    Serial.println(cell);
    if(cell == 0){
      user +=cc;
      delay(100);
    }
  }

  if (!sl) {
    createDataSet();
    delay(300);
  }
}

// Return index in array for x and y //
int getExpandex(int x, int y) {
  int index = ((cc + 1) + (x * 2)) + ((cc * 2) * y);
  return index;
}

// Populate blank dataset for baisc walls and empty rooms //
void createDataSet() {
  user = 8;
  for (int d = 0; d < tCells; d += 1) {
    // Get position from single array //
    int x = d % cc;
    int y = (d - x) / cc;
    // Checks for Data setup //
    int oddCell = d & 1;
    int evenRow = y % 2;
    
    cellData[d] = oddCell && evenRow ? 1 : oddCell ? 1 : evenRow ? 0 : 1;
  }
  generateMaze();
}

// http://weblog.jamisbuck.org/2011/2/1/maze-generation-binary-tree-algorithm
void generateMaze() {
  for (int r = 0; r < rows; r++) {
    for (int x = 0; x < cols; x++) {
      randomSeed(analogRead(0));
      int y = (rows - r) - 1;
      int canGoUp = y > 0;
      int canGoLeft = x < cols - 1;
      int coinFlip = random(1,100) > 50;
      int expandex = getExpandex(x, y);
      if ((canGoUp && !canGoLeft) || (canGoUp && coinFlip)){
        cellData[expandex - cc] = 0;
      }
      if ((!canGoUp && canGoLeft) || (canGoLeft && !coinFlip)){
        cellData[expandex + 1] = 0;
      }
    }
  }
}

