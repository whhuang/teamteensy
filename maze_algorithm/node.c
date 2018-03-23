#include <stdlib.h>
#include "node.h"

struct Node {
  int x;
  int y;
  struct Node* north;
  struct Node* south;
  struct Node* west;
  struct Node* east;
};

struct Node* Node_new(void) {
  struct Node* node =
              (struct Node*) malloc(sizeof(struct Node));
  node->x = -1;
  node->y = -1;
  node->north = NULL;
  node->south = NULL;
  node->west = NULL;
  node->east = NULL;
  return node;
}

void Node_free(struct Node* oNode) {
  free(oNode);
}

/* Sets the node north of this node to the pointer given */
void setNorth(struct Node* oNode, struct Node* oNorthNode) {
  oNode->north = oNorthNode;
}

/* Sets the node south of this node to the pointer given */
void setSouth(struct Node* oNode, struct Node* oSouthNode) {
  oNode->south = oSouthNode;
}

/* Sets the node west of this node to the pointer given */
void setWest(struct Node* oNode, struct Node* oWestNode) {
  oNode->west = oWestNode;
}

/* Sets the node east of this node to the pointer given */
void setEast(struct Node* oNode, struct Node* oEastNode) {
  oNode->east = oEastNode;
}

/* Sets the x coordinate of this Node */
void setX(struct Node* oNode, int xCoord) {
  oNode->x = xCoord;
}

/* Sets the Y coordinate of this Node */
void setY(struct Node* oNode, int yCoord) {
  oNode->x = yCoord;
}
