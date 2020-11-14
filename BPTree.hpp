#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <queue>
#include <cstring>

using namespace std;

class Node {
   public:
    bool isLeaf;
    vector<int> keys;
    Node* ptr2next;              //Pointer to connect next node for leaf nodes               //to make memory efficient Node
    vector<Node*> ptr2Tree;  //Array of pointers to Children sub-trees for intermediate Nodes
    vector<FILE*> dataPtr;   // Data-Pointer for the leaf node
    friend class BPTree;  // to access private members of the Node and hold the encapsulation concept
    Node();
};

class BPTree {

   private:
    int maxIntChildLimit;                                   //Limiting  #of children for internal Nodes!
    int maxLeafNodeLimit;                                   // Limiting #of nodes for leaf Nodes!!!
    Node* root;                                             //Pointer to the B+ Tree root
    void insertInternal(int x, Node** cursor, Node** child);  //Insert x from child in cursor(parent)
    Node** findParent(Node* cursor, Node* child);
    Node* firstLeftNode(Node* cursor);

   public:
    BPTree();
    BPTree(int degreeInternal, int degreeLeaf);
    Node* getRoot();
    int getMaxIntChildLimit();
    int getMaxLeafNodeLimit();
    void setRoot(Node *);
    void display(Node* cursor);
    void seqDisplay(Node* cursor);
    void search(int key);
    void insert(int key, FILE* filePtr);
    void removeKey(int key);
    void removeInternal(int x, Node* cursor, Node* child);
};


Node* parent = NULL;


Node::Node() {
    this->isLeaf = false;
    this->ptr2next = NULL;
}

BPTree::BPTree() {
    this->maxIntChildLimit = 4;
    this->maxLeafNodeLimit = 3;
    this->root = NULL;
}

BPTree::BPTree(int degreeInternal, int degreeLeaf) {
    this->maxIntChildLimit = degreeInternal;
    this->maxLeafNodeLimit = degreeLeaf;
    this->root = NULL;
}

int BPTree::getMaxIntChildLimit() {
    return maxIntChildLimit;
}

int BPTree::getMaxLeafNodeLimit() {
    return maxLeafNodeLimit;
}

Node* BPTree::getRoot() {
    return this->root;
}

void BPTree::setRoot(Node *ptr) {
    this->root = ptr;
}

Node* BPTree::firstLeftNode(Node* cursor) {
    if (cursor->isLeaf)
        return cursor;
    for (int i = 0; i < cursor->ptr2Tree.size(); i++)
        if (cursor->ptr2Tree[i] != NULL)
            return firstLeftNode(cursor->ptr2Tree[i]);

    return NULL;
}

Node** BPTree::findParent(Node* cursor, Node* child) {


    if (cursor->isLeaf || cursor->ptr2Tree[0]->isLeaf)
        return NULL;

    for (int i = 0; i < cursor->ptr2Tree.size(); i++) {
        if (cursor->ptr2Tree[i] == child) {
            parent = cursor;
        } else {
            //Commenting To Remove vector out of bound Error: 
            Node* tmpCursor = cursor->ptr2Tree[i];
            findParent(tmpCursor, child);
        }
    }

    return &parent;
}

void BPTree::insert(int key, FILE* filePtr) {  //in Leaf Node

    if (root == NULL) {
        root = new Node;
        root->isLeaf = true;
        root->keys.push_back(key);
        root->dataPtr.push_back(filePtr);

        cout << key << ": I AM ROOT!!" << endl;
        return;
    } else {
        Node* cursor = root;
        Node* parent = NULL;
        //searching for the possible position for the given key by doing the same procedure we did in search
        while (cursor->isLeaf == false) {
            parent = cursor;
            int idx = std::upper_bound(cursor->keys.begin(), cursor->keys.end(), key) - cursor->keys.begin();
            cursor = cursor->ptr2Tree[idx];
        }

        //now cursor is the leaf node in which we'll insert the new key
        if (cursor->keys.size() < maxLeafNodeLimit) {
            
			//If current leaf Node is not FULL, find the correct position for the new key and insert!
			
            int i = std::upper_bound(cursor->keys.begin(), cursor->keys.end(), key) - cursor->keys.begin();
            cursor->keys.push_back(key);
            cursor->dataPtr.push_back(filePtr);

            if (i != cursor->keys.size() - 1) {
                for (int j = cursor->keys.size() - 1; j > i; j--) {  // shifting the position for keys and datapointer
                    cursor->keys[j] = cursor->keys[j - 1];
                    cursor->dataPtr[j] = cursor->dataPtr[j - 1];
                }

                //since earlier step was just to inc. the size of vectors and making space, now we are simplying inserting
                cursor->keys[i] = key;
                cursor->dataPtr[i] = filePtr;
            }
            cout << "Inserted successfully: " << key << endl;
        } else {
        
            //splitting the nodes

            vector<int> virtualNode(cursor->keys);
            vector<FILE*> virtualDataNode(cursor->dataPtr);

            //finding the probable place to insert the key
            int i = std::upper_bound(cursor->keys.begin(), cursor->keys.end(), key) - cursor->keys.begin();

            virtualNode.push_back(key);          // to create space
            virtualDataNode.push_back(filePtr);  // to create space

            if (i != virtualNode.size() - 1) {
                for (int j = virtualNode.size() - 1; j > i; j--) {  // shifting the position for keys and datapointer
                    virtualNode[j] = virtualNode[j - 1];
                    virtualDataNode[j] = virtualDataNode[j - 1];
                }

                //inserting
                virtualNode[i] = key;
                virtualDataNode[i] = filePtr;
            }


            Node* newLeaf = new Node;
            newLeaf->isLeaf = true;
            //swapping the next ptr
            Node* temp = cursor->ptr2next;
            cursor->ptr2next = newLeaf;
            newLeaf->ptr2next = temp;

            //resizing and copying the keys & dataPtr to OldNode
            cursor->keys.resize((maxLeafNodeLimit) / 2 +1);//check +1 or not while partitioning
            cursor->dataPtr.reserve((maxLeafNodeLimit) / 2 +1);
            for (int i = 0; i <= (maxLeafNodeLimit) / 2; i++) {
                cursor->keys[i] = virtualNode[i];
                cursor->dataPtr[i] = virtualDataNode[i];
            }

            //Pushing new keys & dataPtr to NewNode
            for (int i = (maxLeafNodeLimit) / 2 + 1; i < virtualNode.size(); i++) {
                newLeaf->keys.push_back(virtualNode[i]);
                newLeaf->dataPtr.push_back(virtualDataNode[i]);
            }

            if (cursor == root) {
                Node* newRoot = new Node;
                newRoot->keys.push_back(newLeaf->keys[0]);
                newRoot->ptr2Tree.push_back(cursor);
                newRoot->ptr2Tree.push_back(newLeaf);
                root = newRoot;
                cout << "Created new Root!" << endl;
            } else {
                // Insert new key in the parent
                insertInternal(newLeaf->keys[0], &parent, &newLeaf);
            }
        }
    }
}

void BPTree::insertInternal(int x, Node** cursor, Node** child) {  //in Internal Nodes
    if ((*cursor)->keys.size() < maxIntChildLimit - 1) {

        //find the position for the new key
        int i = std::upper_bound((*cursor)->keys.begin(), (*cursor)->keys.end(), x) - (*cursor)->keys.begin();
        (*cursor)->keys.push_back(x);
        (*cursor)->ptr2Tree.push_back(*child);

        if (i != (*cursor)->keys.size() - 1) {  // if there are more than one element
            // Different loops because size is different for both (i.e. diff of one)

            for (int j = (*cursor)->keys.size() - 1; j > i; j--) {  // shifting the position for keys and datapointer
                (*cursor)->keys[j] = (*cursor)->keys[j - 1];
            }

            for (int j = (*cursor)->ptr2Tree.size() - 1; j > (i + 1); j--) {
                (*cursor)->ptr2Tree[j] = (*cursor)->ptr2Tree[j - 1];
            }

            (*cursor)->keys[i] = x;
            (*cursor)->ptr2Tree[i + 1] = *child;
        }
        cout << "Inserted key in the internal node :)" << endl;

    } else {  
        //splitting
        cout << "Inserted Node in internal node successful" << endl;
        vector<int> virtualKeyNode((*cursor)->keys);
        vector<Node*> virtualTreePtrNode((*cursor)->ptr2Tree);

        int i = std::upper_bound((*cursor)->keys.begin(), (*cursor)->keys.end(), x) - (*cursor)->keys.begin();  //finding the position for x
        virtualKeyNode.push_back(x);                                                                   // to create space
        virtualTreePtrNode.push_back(*child);                                                           // to create space

        if (i != virtualKeyNode.size() - 1) {
            for (int j = virtualKeyNode.size() - 1; j > i; j--) {  // shifting the position for keys and datapointer
                virtualKeyNode[j] = virtualKeyNode[j - 1];
            }

            for (int j = virtualTreePtrNode.size() - 1; j > (i + 1); j--) {
                virtualTreePtrNode[j] = virtualTreePtrNode[j - 1];
            }

            virtualKeyNode[i] = x;
            virtualTreePtrNode[i + 1] = *child;
        }

        int partitionKey;                                            //exclude middle element while splitting
        partitionKey = virtualKeyNode[(virtualKeyNode.size() / 2)];  //right biased
        int partitionIdx = (virtualKeyNode.size() / 2);

        //resizing and copying the keys & TreePtr to OldNode
        (*cursor)->keys.resize(partitionIdx);
        (*cursor)->ptr2Tree.resize(partitionIdx + 1);
        (*cursor)->ptr2Tree.reserve(partitionIdx + 1);
        for (int i = 0; i < partitionIdx; i++) {
            (*cursor)->keys[i] = virtualKeyNode[i];
        }

        for (int i = 0; i < partitionIdx + 1; i++) {
            (*cursor)->ptr2Tree[i] = virtualTreePtrNode[i];
        }

        Node* newInternalNode = new Node;
        //Pushing new keys & TreePtr to NewNode

        for (int i = partitionIdx + 1; i < virtualKeyNode.size(); i++) {
            newInternalNode->keys.push_back(virtualKeyNode[i]);
        }

        for (int i = partitionIdx + 1; i < virtualTreePtrNode.size(); i++) {  // because only key is excluded not the pointer
            newInternalNode->ptr2Tree.push_back(virtualTreePtrNode[i]);
        }

        if ((*cursor) == root) {
            //create a new node if the cursor is a root
            Node* newRoot = new Node;
            newRoot->keys.push_back(partitionKey);
            newRoot->ptr2Tree.push_back(*cursor);
            newRoot->ptr2Tree.push_back(newInternalNode);

            root = newRoot;
            cout << "Created new ROOT!" << endl;
        } else {

            insertInternal(partitionKey, findParent(root, *cursor), &newInternalNode);
        }
    }
}

void BPTree::removeKey(int x) {
	Node* root = getRoot();

	// If tree is empty
	if (root == NULL) {
		cout << "B+ Tree is Empty" << endl;
		return;
	}

	Node* cursor = root;
	Node* parent;
	int leftSibling, rightSibling;

	// Going to the Leaf Node, Which may contain the *key*
	// TO-DO : Use Binary Search to find the val
	while (cursor->isLeaf != true) {
		for (int i = 0; i < cursor->keys.size(); i++) {
			parent = cursor;
			leftSibling = i - 1;//left side of the parent node
			rightSibling = i + 1;// right side of the parent node

			if (x < cursor->keys[i]) {
				cursor = cursor->ptr2Tree[i];
				break;
			}
			if (i == cursor->keys.size() - 1) {
				leftSibling = i;
				rightSibling = i + 2;// CHECK here , might need to make it negative
				cursor = cursor->ptr2Tree[i+1];
				break;
			}
		}
	}

	// Check if the value exists in this leaf node
	int pos = 0;
	bool found = false;
	for (pos = 0; pos < cursor->keys.size(); pos++) {
		if (cursor->keys[pos] == x) {
			found = true;
			break;
		}
	}

	auto itr = lower_bound(cursor->keys.begin(), cursor->keys.end(), x);

	if (itr == cursor->keys.end()) {
		cout << "Key Not Found in the Tree" << endl;
		return;
	}
	
	// Delete the respective File and FILE*
	string fileName = "DBFiles/" + to_string(x) + ".txt";
	char filePtr[256];
	strcpy(filePtr, fileName.c_str());

	//delete cursor->ptr2TreeOrData.dataPtr[pos];//avoid memory leaks
	if (remove(filePtr) == 0)
		cout << "SuccessFully Deleted file: " << fileName << endl;
	else
		cout << "Unable to delete the file: " << fileName << endl;

	// Shifting the keys and dataPtr for the leaf Node
	for (int i = pos; i < cursor->keys.size()-1; i++) {
		cursor->keys[i] = cursor->keys[i+1];
		cursor->dataPtr[i] = cursor->dataPtr[i + 1];
	}
	int prev_size = cursor->keys.size();
	cursor->keys.resize(prev_size - 1);
	cursor->dataPtr.resize(prev_size - 1);

	// If it is leaf as well as the root node
	if (cursor == root) {
		if (cursor->keys.size() == 0) {
			// Tree becomes empty
			setRoot(NULL);
			cout << "Our Tree is Empty Now :(" << endl;
		}
	}
	
	cout << "Deleted " << x << " From Leaf Node successfully" << endl;
	if (cursor->keys.size() >= (getMaxLeafNodeLimit()+1) / 2) {
		//Sufficient Node available for invariant to hold
		return;
	}

	cout << "UnderFlow in the leaf Node Happended" << endl;
	cout << "Starting Redistribution..." << endl;

	//1. Try to borrow a key from leftSibling
	if (leftSibling >= 0) {
		Node* leftNode = parent->ptr2Tree[leftSibling];

		//Check if LeftSibling has extra Key to transfer
		if (leftNode->keys.size() >= (getMaxLeafNodeLimit()+1) / 2 +1) {

			//Transfer the maximum key from the left Sibling
			int maxIdx = leftNode->keys.size()-1;
			cursor->keys.insert(cursor->keys.begin(), leftNode->keys[maxIdx]);
			cursor->dataPtr
				.insert(cursor->dataPtr.begin(), leftNode->dataPtr[maxIdx]);

			//resize the left Sibling Node After Tranfer
			leftNode->keys.resize(maxIdx);
			leftNode->dataPtr.resize(maxIdx);

			//Update Parent
			parent->keys[leftSibling] = cursor->keys[0];
			printf("Transferred from left sibling of leaf node\n");
			return;
		}
	}

	//2. Try to borrow a key from rightSibling
	if (rightSibling < parent->ptr2Tree.size()) {
		Node* rightNode = parent->ptr2Tree[rightSibling];

		//Check if RightSibling has extra Key to transfer
		if (rightNode->keys.size() >= (getMaxLeafNodeLimit() + 1) / 2 + 1) {

			//Transfer the minimum key from the right Sibling
			int minIdx = 0;
			cursor->keys.push_back(rightNode->keys[minIdx]);
			cursor->dataPtr
				.push_back(rightNode->dataPtr[minIdx]);

			//resize the right Sibling Node After Tranfer
			rightNode->keys.erase(rightNode->keys.begin());
			rightNode->dataPtr.erase(rightNode->dataPtr.begin());

			//Update Parent
			parent->keys[rightSibling-1] = rightNode->keys[0];
			printf("Transferred from right sibling of leaf node\n");
			return;
		}
	}

	// Merge and Delete Node
	if (leftSibling >= 0) {// If left sibling exists
		Node* leftNode = parent->ptr2Tree[leftSibling];
		//Transfer Key and dataPtr to leftSibling and connect ptr2next
		for (int i = 0; i < cursor->keys.size(); i++) {
			leftNode->keys.push_back(cursor->keys[i]);
			leftNode->dataPtr
				.push_back(cursor->dataPtr[i]);
		}
		leftNode->ptr2next = cursor->ptr2next;
		cout << "Merging two leaf Nodes" << endl;
		removeInternal(parent->keys[leftSibling], parent, cursor);//delete parent Node Key
		//delete cursor;
	}
	else if (rightSibling <= parent->keys.size()) {
		Node* rightNode = parent->ptr2Tree[rightSibling];
		//Transfer Key and dataPtr to rightSibling and connect ptr2next
		for (int i = 0; i < rightNode->keys.size(); i++) {
			cursor->keys.push_back(rightNode->keys[i]);
			cursor->dataPtr
				.push_back(rightNode->dataPtr[i]);
		}
		cursor->ptr2next = rightNode->ptr2next;
		cout << "Merging two leaf Nodes" << endl;
		removeInternal(parent->keys[rightSibling-1], parent, rightNode);//delete parent Node Key
		//delete rightNode;
	}

}

void BPTree::removeInternal(int x, Node* cursor, Node* child) {
	Node* root = getRoot();

	// Check if key from root is to deleted
	if (cursor == root) {
		if (cursor->keys.size() == 1) {
			// If only one key is left and matches with one of the
			// child Pointers
			if (cursor->ptr2Tree[1] == child) {
				setRoot(cursor->ptr2Tree[0]);
				//delete cursor;
				cout << "New Changed Root" <<endl;
				return;
			}
			else if (cursor->ptr2Tree[0] == child) {
				setRoot(cursor->ptr2Tree[1]);
				//delete cursor;
				cout << "New Changed Root" << endl;
				return;
			}
		}
	}

	// Deleting key x from the parent
	int pos;
	for (pos = 0; pos < cursor->keys.size(); pos++) {
		if (cursor->keys[pos] == x) {
			break;
		}
	}
	for (int i = pos; i < cursor->keys.size()-1; i++) {
		cursor->keys[i] = cursor->keys[i + 1];
	}
	cursor->keys.resize(cursor->keys.size() - 1);

	// Now deleting the ptr2tree
	for (pos = 0; pos < cursor->ptr2Tree.size(); pos++) {
		if (cursor->ptr2Tree[pos] == child) {
			break;
		}
	}

	for (int i = pos; i < cursor->ptr2Tree.size() - 1; i++) {
		cursor->ptr2Tree[i] = cursor->ptr2Tree[i + 1];
	}
	cursor->ptr2Tree
		.resize(cursor->ptr2Tree.size()-1);

	// If there is No underflow. Phew!!
	if (cursor->keys.size() >= (getMaxIntChildLimit() + 1) / 2 - 1) {
		cout << "Deleted " << x << " from internal node successfully\n";
		return;
	}

	cout << "UnderFlow in internal Node! What did you do :/" << endl;

	if (cursor == root) {
		return;
	}

	Node** p1 = findParent(root, cursor);
	Node* parent = *p1;

	int leftSibling, rightSibling;

	// Finding Left and Right Siblings as we did earlier
	for (pos = 0; pos < parent->ptr2Tree.size(); pos++) {
		if (parent->ptr2Tree[pos] == cursor) {
			leftSibling = pos - 1;
			rightSibling = pos + 1;
			break;
		}
	}

	// If possible transfer to leftSibling
	if (leftSibling >= 0) {
		Node* leftNode = parent->ptr2Tree[leftSibling];

		//Check if LeftSibling has extra Key to transfer
		if (leftNode->keys.size() >= (getMaxIntChildLimit() + 1) / 2 ) {

			//transfer key from left sibling through parent
			int maxIdxKey = leftNode->keys.size() - 1;
			cursor->keys.insert(cursor->keys.begin(), parent->keys[leftSibling]);
			parent->keys[leftSibling] = leftNode->keys[maxIdxKey];

			int maxIdxPtr = leftNode->ptr2Tree.size()-1;
			cursor->ptr2Tree
				.insert(cursor->ptr2Tree.begin(), leftNode->ptr2Tree[maxIdxPtr]);

			//resize the left Sibling Node After Tranfer
			leftNode->keys.resize(maxIdxKey);
			leftNode->dataPtr.resize(maxIdxPtr);

			return;
		}
	}

	// If possible transfer to rightSibling
	if (rightSibling < parent->ptr2Tree.size()) {
		Node* rightNode = parent->ptr2Tree[rightSibling];

		//Check if LeftSibling has extra Key to transfer
		if (rightNode->keys.size() >= (getMaxIntChildLimit() + 1) / 2) {

			//transfer key from right sibling through parent
			int maxIdxKey = rightNode->keys.size() - 1;
			cursor->keys.push_back(parent->keys[pos]);
			parent->keys[pos] = rightNode->keys[0];
			rightNode->keys.erase(rightNode->keys.begin());

			//transfer the pointer from rightSibling to cursor
			cursor->ptr2Tree
				.push_back(rightNode->ptr2Tree[0]);
			cursor->ptr2Tree
				.erase(cursor->ptr2Tree.begin());
			 
			return;
		}
	}

	//Start to Merge Now, if None of the above cases applied
	if (leftSibling >= 0) {
		//leftNode + parent key + cursor
		Node* leftNode = parent->ptr2Tree[leftSibling];
		leftNode->keys.push_back(parent->keys[leftSibling]);

		for (int val : cursor->keys) {
			leftNode->keys.push_back(val);
		}

		for (int i = 0; i < cursor->ptr2Tree.size(); i++) {
			leftNode->ptr2Tree
				.push_back(cursor->ptr2Tree[i]);
			cursor->ptr2Tree[i] = NULL;
		}

		cursor->ptr2Tree.resize(0);
		cursor->keys.resize(0);

		removeInternal(parent->keys[leftSibling], parent, cursor);
		cout << "Merged with left sibling"<<endl;
	}
	else if (rightSibling < parent->ptr2Tree.size()) {
		//cursor + parentkey +rightNode
		Node* rightNode = parent->ptr2Tree[rightSibling];
		cursor->keys.push_back(parent->keys[rightSibling - 1]);

		for (int val : rightNode->keys) {
			cursor->keys.push_back(val);
		}

		for (int i = 0; i < rightNode->ptr2Tree.size(); i++) {
			cursor->ptr2Tree
				.push_back(rightNode->ptr2Tree[i]);
			rightNode->ptr2Tree[i] = NULL;
		}

		rightNode->ptr2Tree.resize(0);
		rightNode->keys.resize(0);

		removeInternal(parent->keys[rightSibling - 1], parent, rightNode);
		cout << "Merged with right sibling\n";
	}
}

void BPTree::display(Node* cursor) {

    //Level Order Display

    if (cursor == NULL) return;
    queue<Node*> q;
    q.push(cursor);

    while (!q.empty()) {
        int sz = q.size();
        for (int i = 0; i < sz; i++) {
            Node* u = q.front(); q.pop();

            //printing keys in self

            for (int val : u->keys)
                cout << val << " ";

            cout << "|| ";  //to seperate next adjacent nodes
            
            if (u->isLeaf != true) {
                for (Node* v : u->ptr2Tree) {
                    q.push(v);
                }
            }
        }
        cout << endl;
    }
}


void BPTree::search(int key) {
    if (root == NULL) {
        cout << "NO Tuples Inserted yet" << endl;
        return;
    } else {
        Node* cursor = root;
        while (cursor->isLeaf == false) {
            int idx = std::upper_bound(cursor->keys.begin(), cursor->keys.end(), key) - cursor->keys.begin();
            cursor = cursor->ptr2Tree[idx];  //upper_bound takes care of all the edge cases
        }

        int idx = std::lower_bound(cursor->keys.begin(), cursor->keys.end(), key) - cursor->keys.begin();  //Binary search

        if (idx == cursor->keys.size() || cursor->keys[idx] != key) {
            cout << "Key NOT FOUND!!!" << endl;
            return;
        }

        
		//We can fetch the data from the disc in main memory using data-ptr
		//using cursor->dataPtr[idx]
		

        string fileName = "DBFiles/";
        string data;
        fileName += to_string(key) + ".txt";
        FILE* filePtr = fopen(fileName.c_str(), "r");
        cout << "Key FOUND" << endl;
        cout << "Corresponding Tuple Data is: ";
        char ch = fgetc(filePtr);
        while (ch != EOF) {
            printf("%c", ch);
            ch = fgetc(filePtr);
        }
        fclose(filePtr);
        cout << endl;
    }
}

