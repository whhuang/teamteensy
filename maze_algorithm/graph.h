#include "node.h"
#include "stack.h"

struct Graph;

Stack_T oStack;

/* creates a graph that points to starting position node */
struct Graph* Graph_new(struct Node* startNode);

/* has this node been visited before? */
int Graph_beenVisited(struct Graph* oGraph, int xCoord, int yCoorx);

/* add node to stack */
void Graph_addNode(struct Node* validNode);

/* Return iterable of all nodes in graph */
Stack_T Graph_Nodes(void);
