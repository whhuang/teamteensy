#include <stdlib.h>
#include "node.h"
#include "graph.h"
#include "stack.h"
#include <Arduino.h>

struct Graph {
  struct Node* start;
};

//struct Stack_T oStack;

/*
struct Graph* Graph_new(struct Node* startNode) {
  struct Graph* graph =
              (struct Graph*) malloc(sizeof(struct Graph));
  graph->start = startNode;
<<<<<<< Updated upstream
  oStack = Stack_new();
}
=======
  struct Stack_T oStack = Stack_new(void);
}*/
>>>>>>> Stashed changes

/*
int Graph_beenVisited(struct Graph* oGraph, int xCoord, int yCoord) {
  struct StackNode* current = oStack->psFirstNode;
  while (current) {
    struct Node* heldNode = (struct Node*) current->pvItem;
    if (heldNode->x == xCoord && heldNode->y == yCoord)
      return 1;
    current = current->psNextNode;
  }
<<<<<<< Updated upstream
  return 0;
}
=======
  return false;
}*/
>>>>>>> Stashed changes

/*
void Graph_addNode(struct Node* validNode) {
  Stack_push(oStack, (const void *) validNode);
}*/

/*
struct Stack_T Graph_Nodes(void) {
  return oStack;
}*/
