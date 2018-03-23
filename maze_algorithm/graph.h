#include "node.h"

struct Graph;

/* creates a graph that points to starting position node */
struct Graph* Graph_new(struct Node* startNode);

/* has this node been visited before? */
int beenVisited(struct Graph* oGraph, int x, int y);
