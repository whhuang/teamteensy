#include <stdlib.h>
#include "node.h"
#include "graph.h"
#include "stack.h"

struct Graph {
  struct Node* start;
};

Stack_T oStack;

struct Graph* Graph_new(struct Node* startNode) {
  struct Graph* graph =
              (struct Graph*) malloc(sizeof(struct Graph));
  graph->start = startNode;
  oStack = Stack_new();
}

int Graph_beenVisited(struct Graph* oGraph, int xCoord, int yCoord) {
  struct StackNode* current = oStack->psFirstNode;
  while (current) {
    struct Node* heldNode = (struct Node*) current->pvItem;
    if (heldNode->x == xCoord && heldNode->y == yCoord)
      return 1;
    current = current->psNextNode;
  }
  return 0;
}

void Graph_addNode(struct Node* validNode) {
  Stack_push(oStack, (const void *) validNode);
}

Stack_T Graph_Nodes(void) {
  return oStack;
}
