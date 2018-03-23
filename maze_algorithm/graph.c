#include <stdlib.h>
#include "node.h"
#include "graph.h"

struct Graph {
  struct Node* start;
};

/* creates a graph that points to starting position node */
struct Graph* Graph_new(struct Node* startNode) {
  struct Graph* graph =
              (struct Graph*) malloc(sizeof(struct Graph));
  graph->start = startNode;
}

/* has this node been visited before? */
int beenVisited(struct Graph* oGraph, int x, int y);
