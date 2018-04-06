#include "stack.h"
#include "graph.h"
#include "node.h"

/*************************************************************
 *                CONSTANTS & GLOBAL VARIABLES               *
 *************************************************************/
 enum open_positions {
   LEFT,
   RIGHT,
   FRONTRIGHT,
   FRONTLEFT,
   LEFTRIGHT,
   WALL
 };

 boolean turnAroundMode = false;

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

// TODO FIGURE OUT HOW TO TURN ROBOT
void setup() {
  boolean maze_not_done = false;
  Stack_T oStack = Stack_new(void);
  Node* startNode = Node_new(void);
  roundCoordinates();
  setXandY(startNode, xLocation, yLocation);
  setNorth(startNode, NULL);
  setSouth(startNode, NULL);
  setWest(startNode, NULL);
  // Create new oGraph that contains start Node
  Graph* oGraph = Graph_new(startNode);
  // Create a temp Node that the starting position points to. Set its field to “not visited”
  Node* tempNode = Node_new(void);
  setEast(startNode, tempNode);
  // Add starting Node to the Stack.
  Stack_push(oStack, startNode);

  boolean turnAroundMode = false;
  maze_not_done = false;
  while (maze_not_done) {
    // Go straight, visit that node.
    int open_position = goStraight();
    switch (open_position) {
      case LEFT:
        if (turnAroundMode) {
          Node* currentNode = Stack_pop();
          setNodeBacktrackDirection(facing, currentNode, NULL);
        }
        else {
          Node* currentNode = Stack_pop();
          Node* tempNode = Node_new(void);
          switch (facing) {
            case NORTH:
              currentNode->west = tempNode;
              setXandY(currentNode, xLocation, yLocation);
              break;
            case SOUTH:
              currentNode->east = tempNode;
              setXandY(currentNode, xLocation, yLocation);
              break;
            case WEST:
              currentNode->south = tempNode;
              setXandY(currentNode, xLocation, yLocation);
              break;
            case EAST:
              currentNode->north = tempNode;
              setXandY(currentNode, xLocation, yLocation);
              break;
          }
        }
        brake();
        turn();
      case RIGHT:
        if (turnAroundMode) {
          Node* currentNode = Stack_pop();
          setNodeBacktrackDirection(facing, currentNode, NULL);
        }
        else {
          Node* currentNode = Stack_pop();
          Node* tempNode = Node_new(void);
          switch (facing) {
            case NORTH:
              currentNode->east = tempNode;
              setXandY(currentNode, xLocation, yLocation);
              break;
            case SOUTH:
              currentNode->west = tempNode;
              setXandY(currentNode, xLocation, yLocation);
              break;
            case WEST:
              currentNode->north = tempNode;
              setXandY(currentNode, xLocation, yLocation);
              break;
            case EAST:
              currentNode->south = tempNode;
              setXandY(currentNode, xLocation, yLocation);
              break;
          }
        }
        brake();
        turn();
      case LEFTRIGHT:
        if (turnAroundMode) {
          Node* currentNode = Stack_pop();
          setNodeBacktrackDirection(facing, currentNode, NULL);
          switch (facing) {
            case NORTH:
              if (!currentNode->west.visited) {
                turnAroundMode = false;
                Stack_push(oStack, currentNode);
                break();
                turn();
              }
              else if (!currentNode->east.visited) {
                turnAroundMode = false;
                Stack_push(oStack, currentNode);
                break();
                turn();
              }
              else {
                Node* previousNode = Stack_pop();
                if (currentNode->north == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else if (currentNode->west == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else if (currentNode->east == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
              }
              break;
            case SOUTH:
              if (!currentNode->west.visited) {
                turnAroundMode = false;
                Stack_push(oStack, currentNode);
                break();
                turn();
              }
              else if (!currentNode->east.visited) {
                turnAroundMode = false;
                Stack_push(oStack, currentNode);
                break();
                turn();
              }
              else {
                Node* previousNode = Stack_pop();
                if (currentNode->north == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else if (currentNode->west == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else if (currentNode->east == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
              }
              break;
            case WEST:
              if (!currentNode->north.visited) {
                turnAroundMode = false;
                Stack_push(oStack, currentNode);
                break();
                turn();
              }
              else if (!currentNode->south.visited) {
                turnAroundMode = false;
                Stack_push(oStack, currentNode);
                break();
                turn();
              }
              else {
                Node* previousNode = Stack_pop();
                if (currentNode->north == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else if (currentNode->west == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else if (currentNode->east == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
              }
            break;
            case EAST:
              if (!currentNode->north.visited) {
                turnAroundMode = false;
                Stack_push(oStack, currentNode);
                break();
                turn();
              }
              else if (!currentNode->south.visited) {
                turnAroundMode = false;
                Stack_push(oStack, currentNode);
                break();
                turn();
              }
              else {
                Node* previousNode = Stack_pop();
                if (currentNode->north == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else if (currentNode->west == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else if (currentNode->east == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
              }
            break;
          }
        }
        else {
          Node* currentNode = Stack_pop();
          Node* tempNode = Node_new(void);
          Node* tempNode2 = Node_new(void);
          switch (facing) {
            case NORTH:
              currentNode->east = tempNode;
              currentNode->west = tempNode2;
              setXandY(currentNode, xLocation, yLocation);
              break;
            case SOUTH:
              currentNode->east = tempNode;
              currentNode->west = tempNode2;
              setXandY(currentNode, xLocation, yLocation);
              break;
            case WEST:
              currentNode->south = tempNode;
              currentNode->north = tempNode2;
              setXandY(currentNode, xLocation, yLocation);
              break;
            case EAST:
              currentNode->south = tempNode;
              currentNode->north = tempNode2;
              setXandY(currentNode, xLocation, yLocation);
              break;
          }
        }
        break();
        turn();
        break;
      case FRONTLEFT:
        if (turnAroundMode) {
          Node* currentNode = Stack_pop();
          setNodeBacktrackDirection(facing, currentNode, NULL);
          switch (facing) {
            case NORTH:
              if (!currentNode->north.visited) {
                turnAroundMode = false;
                Stack_push(oStack, currentNode);
              }
              else if (!currentNode->west.visited) {
                turnAroundMode = false;
                Stack_push(oStack, currentNode);
                break();
                turn();
              }
              else {
                Node* previousNode = Stack_pop();
                if (currentNode->north == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else if (currentNode->west == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else if (currentNode->east == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
              }
              break;
            case SOUTH:
              if (!currentNode->south.visited) {
                turnAroundMode = false;
                Stack_push(oStack, currentNode);
              }
              else if (!currentNode->east.visited) {
                turnAroundMode = false;
                Stack_push(oStack, currentNode);
                break();
                turn();
              }
              else {
                Node* previousNode = Stack_pop();
                if (currentNode->north == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else if (currentNode->west == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else if (currentNode->east == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
              }
              break;
            case WEST:
              if (!currentNode->west.visited) {
                turnAroundMode = false;
                Stack_push(oStack, currentNode);
              }
              else if (!currentNode->south.visited) {
                turnAroundMode = false;
                Stack_push(oStack, currentNode);
                break();
                turn();
              }
              else {
                Node* previousNode = Stack_pop();
                if (currentNode->north == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else if (currentNode->west == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else if (currentNode->east == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
              }
            break;
            case EAST:
              if (!currentNode->east.visited) {
                turnAroundMode = false;
                Stack_push(oStack, currentNode);
                break();
                turn();
              }
              else if (!currentNode->north.visited) {
                turnAroundMode = false;
                Stack_push(oStack, currentNode);
                break();
                turn();
              }
              else {
                Node* previousNode = Stack_pop();
                if (currentNode->north == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else if (currentNode->west == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else if (currentNode->east == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
              }
            break;
          }
        }
        else {
          Node* currentNode = Stack_pop();
          Node* tempNode = Node_new(void);
          Node* tempNode2 = Node_new(void);
          switch (facing) {
            case NORTH:
              currentNode->north = tempNode;
              currentNode->west = tempNode2;
              setXandY(currentNode, xLocation, yLocation);
              break;
            case SOUTH:
              currentNode->south = tempNode;
              currentNode->east = tempNode2;
              setXandY(currentNode, xLocation, yLocation);
              break;
            case WEST:
              currentNode->west = tempNode;
              currentNode->south = tempNode2;
              setXandY(currentNode, xLocation, yLocation);
              break;
            case EAST:
              currentNode->east = tempNode;
              currentNode->north = tempNode2;
              setXandY(currentNode, xLocation, yLocation);
              break;
          }
        }
        break;
      case FRONTRIGHT:
        if (turnAroundMode) {
          Node* currentNode = Stack_pop();
          setNodeBacktrackDirection(facing, currentNode, NULL);
          switch (facing) {
            case NORTH:
              if (!currentNode->north.visited) {
                turnAroundMode = false;
                Stack_push(oStack, currentNode);
              }
              else if (!currentNode->east.visited) {
                turnAroundMode = false;
                Stack_push(oStack, currentNode);
                break();
                turn();
              }
              else {
                Node* previousNode = Stack_pop();
                if (currentNode->north == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else if (currentNode->west == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else if (currentNode->east == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
              }
              break;
            case SOUTH:
              if (!currentNode->south.visited) {
                turnAroundMode = false;
                Stack_push(oStack, currentNode);
              }
              else if (!currentNode->west.visited) {
                turnAroundMode = false;
                Stack_push(oStack, currentNode);
                break();
                turn();
              }
              else {
                Node* previousNode = Stack_pop();
                if (currentNode->north == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else if (currentNode->west == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else if (currentNode->east == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
              }
              break;
            case WEST:
              if (!currentNode->west.visited) {
                turnAroundMode = false;
                Stack_push(oStack, currentNode);
              }
              else if (!currentNode->north.visited) {
                turnAroundMode = false;
                Stack_push(oStack, currentNode);
                break();
                turn();
              }
              else {
                Node* previousNode = Stack_pop();
                if (currentNode->north == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else if (currentNode->west == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else if (currentNode->east == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
              }
            break;
            case EAST:
              if (!currentNode->east.visited) {
                turnAroundMode = false;
                Stack_push(oStack, currentNode);
                break();
                turn();
              }
              else if (!currentNode->south.visited) {
                turnAroundMode = false;
                Stack_push(oStack, currentNode);
                break();
                turn();
              }
              else {
                Node* previousNode = Stack_pop();
                if (currentNode->north == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else if (currentNode->west == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else if (currentNode->east == previousNode) {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
                else {
                  Stack_push(oStack, previousNode);
                  break();
                  turn();
                }
              }
            break;
          }
        }
        else {
          Node* currentNode = Stack_pop();
          Node* tempNode = Node_new(void);
          Node* tempNode2 = Node_new(void);
          switch (facing) {
            case NORTH:
              currentNode->north = tempNode;
              currentNode->east = tempNode2;
              setXandY(currentNode, xLocation, yLocation);
              break;
            case SOUTH:
              currentNode->south = tempNode;
              currentNode->west = tempNode2;
              setXandY(currentNode, xLocation, yLocation);
              break;
            case WEST:
              currentNode->west = tempNode;
              currentNode->north = tempNode2;
              setXandY(currentNode, xLocation, yLocation);
              break;
            case EAST:
              currentNode->east = tempNode;
              currentNode->south = tempNode2;
              setXandY(currentNode, xLocation, yLocation);
              break;
          }
        }
        break;
      case WALL:
        turnAroundMode = true;
        break();
        turn();
    }
  }
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

Node* setNodeBacktrackDirection(int facing, Node* intersection, Node* fromNode) {
  switch(facing) {
    case NORTH:
      setSouth(intersection, fromNode);
      return intersection;
    case WEST:
      setEast(intersection, fromNode);
      return intersection;
    case SOUTH:
      setNorth(intersection, fromNode);
      return intersection;
    case EAST:
      setWest(intersection, fromNode);
      return intersection;
  }
  return NULL;
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
