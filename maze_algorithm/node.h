struct Node;

/* creates a new Node struct */
struct Node* Node_new(void);

/* function to free unused Node pointers if they are not real intersections */
void Node_free(struct Node* oNode);

/* Sets the node north of this node to the pointer given */
void setNorth(struct Node* oNode, struct Node* oNorthNode);

/* Sets the node south of this node to the pointer given */
void setSouth(struct Node* oNode, struct Node* oSouthNode);

/* Sets the node west of this node to the pointer given */
void setWest(struct Node* oNode, struct Node* oWestNode);

/* Sets the node east of this node to the pointer given */
void setEast(struct Node* oNode, struct Node* oEastNode);

/* Sets the x coordinate of this Node */
void setX(struct Node* oNode, int xCoord);

/* Sets the Y coordinate of this Node */
void setY(struct Node* oNode, int yCoord);

/* Sets whether this node has been visited */
void setVisisted(struct Node* oNode)
