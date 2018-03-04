#include <StackArray.h>

/*************************************************************
 *                CONSTANTS & GLOBAL VARIABLES               *
 *************************************************************/

/******** MAZE ********/

int      size = 33;       // size of maze array
boolean maze[33][33];     // change if size variable changes

/******** STACK ********/

StackArray <T> branchNodes; // Stack of the coordinates of each
                            // decision branch

/******** OTHER ********/

int dirFacing = 0;        // direction robot is facing
                          // 0: x+, 1: y+, 2: x-, 3: y-

int         n = 1;        // nth update of grid

/*************************************************************
 *                           SETUP                           *
 *************************************************************/

void setup() {

  /******** MAZE ARRAY INITIALIZATION ********/

  /* Notes:
   *  
   *  maze[x][y] --> x is horizontal (E-W), y is vertical (N-S).
   *  1 is closed (posts, walls), 0 is open.
   *  All walls considered open until noted otherwise.
   *  Maze is visually rotated 180 degrees, meaning starting 
   *    square is at maze[1][1], and first open wall is at
   *    maze[1][2].
   *  Ending locations are: 
   *    maze[15][15], maze[17][15], maze[15][17], maze[17][17]
   *  
   */

  /******** SET MAZE TO 0 ********/
  
  for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++)
      maze[i][j] = 0;
  
  for (int i = 0; i < size; i++) {
    
    /******** BORDER WALLS ********/
    
    maze[i][0]        = 1; // top
    maze[i][size - 1] = 1; // bottom
    maze[0][i]        = 1; // left
    maze[size - 1][i] = 1; // right   

    /******** POSTS ********/
    if ((i % 2) == 0)
      for (int j = 2; j < size - 1; j = j + 2)
        maze[i][j]    = 1;

    /******** OPEN SQUARES ********/
    else
      for (int j = 1; j < size - 1; j = j + 2)
        maze[i][j]   = 0;
    
  }

  /******** PRINT OUTPUT ********/
  
  Serial.begin(9600);

  Serial.println("Maze Grid:");
  Serial.println();
  
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++)
      Serial.print(maze[i][j]);
    Serial.println();
  }
  Serial.println();
  
  delay(5000);

}

/*************************************************************
 *                           LOOP                            *
 *************************************************************/

void loop() {

  /******** TRAVERSE MAZE ********/

  /* Whitney's thoughts:
   *  
   *  Implement DFS? BFS? A*?
   *  
   *  [Not in loop, initial case]
   *  Start at maze[1][1].
   *  Go down to maze[1][3] (maze[1][2] is open).
   *  
   *  AT maze[1][3]
   *  Observe if maze[2][3] has wall. If so, mark as 1.
   *  Observe if maze[1][4] has wall. If so, mark as 1.
   *  
   *  [General case]
   *  AT maze[i][j]
   *  dirFacing -> determines how i and j are adjusted. 
   *               adjust to face x+.
   *  maze[i + 1][j] = wallAhead();
   *  maze[i][j - 1] = wallRight();
   *  maze[i][j + 1] = wallLeft();
   *  
   *  
   *  
   */

  /******** CALCULATE SHORTEST PATH ********/



  /******** FINAL SOLVE ********/



  /******** PRINT OUTPUT ********/
  
  Serial.print("Maze Grid ");
  Serial.println(n);
  Serial.println();
  
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++)
      Serial.print(maze[i][j]);
    Serial.println();
  }
  Serial.println();

  n++;
  
  delay(5000);
  
}

/*************************************************************
 *                   HELPER/PSEUDO FUNCTIONS                 *
 *************************************************************/

/******** MOVEMENT ********/

void goStraight()    { }   // Goes one square straight
void turnRight()     { }   // Makes a 90 degree right turn
void turnLeft()      { }   // Makes a 90 degree left turn

/******** SENSORS ********/

boolean wallAhead()  { }   // Detects if there is a wall ahead. 
                           // Returns 1 if yes, 0 if no.
boolean wallRight()  { }   // Detects if there is a right wall. 
                           // Returns 1 if yes, 0 if no.
boolean wallLeft()   { }   // Detects if there is a left wall.
                           // Returns 1 if yes, 0 if no.

/******** LOGIC ********/

//int      dirFacing() { }   // What direction is the robot facing?
                           // Returns 0: x+, 1: y+, 2: x-, 3: y-.
                           // *Maybe better as global variable.
boolean isFinished() { }   // Is the robot in the finish square?
                           // Returns 1 if yes, 0 if no. 

/******** MATH ********/

/******** OTHER ********/




