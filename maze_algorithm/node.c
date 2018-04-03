#include <stdlib.h>
#include "node.h"

struct Node {
  int x;
  int y;
  int visited;
  struct Node* north;
  struct Node* south;
  struct Node* west;
  struct Node* east;
  int search_visit;
};

struct Node* Node_new(void) {
  struct Node* node =
              (struct Node*) malloc(sizeof(struct Node));
  node->x = -1;
  node->y = -1;
  oNode->visited = 0;
  node->north = NULL;
  node->south = NULL;
  node->west = NULL;
  node->east = NULL;
  oNode->search_visit = 0;

  return node;
}

void Node_free(struct Node* oNode) {
  free(oNode);
}

void setNorth(struct Node* oNode, struct Node* oNorthNode) {
  oNode->north = oNorthNode;
}

void setSouth(struct Node* oNode, struct Node* oSouthNode) {
  oNode->south = oSouthNode;
}

void setWest(struct Node* oNode, struct Node* oWestNode) {
  oNode->west = oWestNode;
}

void setEast(struct Node* oNode, struct Node* oEastNode) {
  oNode->east = oEastNode;
}

void setXandY(struct Node* oNode, int xCoord, int yCoord) {
  oNode->x = xCoord;
  oNode->y = yCoord;
}

void setVisited(struct Node* oNode) {
  oNode->visited = 1;
}
