#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>

using namespace std;

class Node{

public:
    vector<int> keys;
    Node* nextLeaf;
    vector<Node*> childPtr;
    vector<FILE*> dataPtr;
    bool isLeaf;
    Node();
    friend class BPTree;
};


Node::Node(){
    this->isLeaf = false;
    this->nextLeaf = NULL;
}

class BPTree{

private:
    int maxIntChild;
    int maxLeafNode;
    Node* root;

public:
    BPTree();
//    BPTree(int,int);
    Node* getRoot();
    int getMaxIntChild();
    int getMaxLeafNode();
    void setRoot(Node*);
    void insert(int,FILE*);
    void insertInternal(int,Node**,Node**);

};

BPTree::BPTree(){


    this->maxIntChild = 4;
    this->maxLeafNode = 3;
    this->root = NULL;
}

int BPTree::getMaxIntChild(){
    return maxIntChild;
}

int BPTree::getMaxLeafNode(){
    return maxLeafNode;
}

Node* BPTree::getRoot(){
    return this->root;
}

void BPTree::setRoot(Node* node){
    this->root = node;
}

void BPTree::insert(int key,FILE* filePtr){

    if(root == NULL){
        root = new Node();
        root->isLeaf = true;
        root->keys.push_back(key);
        root->dataPtr.push_back(filePtr);
        cout << "ROOT INSERTED " << key << endl;
        return;
    } else {

        Node* cursor = root;
        Node* parent = NULL;
        
        //search for the postion to insert
        //same procedure as search
        while(cursor->isLeaf == false){
            parent = cursor;
            int index = std::upper_bound(cursor->keys.begin(),cursor->keys.end(),key) - cursor->keys.begin();
            cursor = cursor->childPtr[index];
        }

        //cursor now at the leaf node to insert key
        //insert the key there

        if(cursor->keys.size() < maxLeafNode){
            //if node is not full find the position to insert
            //else partition the node

            int i = std::upper_bound(cursor->keys.begin(),cursor->keys.end(),key) - cursor->keys.begin();
            cursor->keys.push_back(key);
            cursor->dataPtr.push_back(filePtr);

            if(i != cursor->keys.size() - 1){
                for(int j = cursor->keys.size() - 1; j > i; j--){
                    cursor->keys[j] = cursor->keys[j - 1];
                    cursor->dataPtr[j] = cursor->dataPtr[j - 1];
                }
            }

            cursor->keys[i] = key;
            cursor->dataPtr[i] = filePtr;

            cout << "Successfully Inserted " << key << endl;
            //inserting at the end and then shifting the data to the 
            //correct index
        }
        else{
            //splitting the nodes

            vector<int> virtualNode(cursor->keys);
            vector<FILE*> virtualDataNode(cursor->dataPtr);

            //finding the place to virtually insert the key
            int i = upper_bound(cursor->keys.begin(),cursor->keys.end(),key) - cursor->keys.begin();

            virtualNode.push_back(key);
            virtualDataNode.push_back(filePtr);

            if(i != virtualNode.size() - 1){
                for(int j = virtualNode.size() - 1; j > i; j--){
                    virtualNode[j] = virtualNode[j - 1];
                    virtualDataNode[j] = virtualDataNode[j - 1];
                }

                virtualNode[i] = key;
                virtualDataNode[i] = filePtr;
            }

            Node* newNode = new Node;
            newNode->isLeaf = true;

            //swapping the next ptr
            Node* temp = cursor->nextLeaf;
            cursor->nextLeaf = newNode;
            newNode->nextLeaf = temp;

            //we have to copy the keys and data ptr to the old node
            //before that we need to resize the nodes
            cursor->keys.resize((maxLeafNode)/2 + 1);
            cursor->dataPtr.reserve((maxLeafNode)/2 + 1);
            for(int i = 0; i <= (maxLeafNode)/2; i++){
                cursor->keys[i] = virtualNode[i];
                cursor->dataPtr[i] = virtualDataNode[i];
            }

            //pushing new keys and data ptr to the new node
            for(int i = (maxLeafNode)/2 + 1; i < virtualNode.size(); i++){
                newNode->keys.push_back(virtualNode[i]);
                newNode->dataPtr.push_back(virtualDataNode[i]);
            }

            //if cursor is root node create a new root

            if(cursor == root){
                Node* newRoot = new Node;
                newRoot->keys.push_back(newNode->keys[0]);
                newRoot->childPtr.push_back(cursor);
                newRoot->childPtr.push_back(newNode);
                root = newRoot;
                cout << "New Root created " << endl;
            }
            else{
                //Insert new key in the parent
                insertInternal(newNode->keys[0],&parent,&newNode);
            }

        }


    }

    //completed inserted

}

//incomplete

void BPTree::insertInternal(int x,Node** cursor,Node** child){
    if((*cursor)->keys.size() < maxIntChild - 1){
        //the node is not full
        //find the postiton and insert

        int i = std::upper_bound((*cursor)->keys.begin(),(*cursor)->keys.end(),x) - (*cursor)->keys.begin();
        (*cursor)->keys.push_back(x);
        (*cursor)->childPtr.push_back(*child);

        if(i != (*cursor)->keys.size() - 1){
            //if there are more than one element

            for(int j = (*cursor)->keys.size() - 1; j > i; j--){
                (*cursor)->keys[j] = (*cursor)->keys[j - 1];
            }

            for(int j = (*cursor)->childPtr.size() - 1; j > (i + 1); j--){
                (*cursor)->childPtr[j] = (*cursor)->childPtr[j - 1];
            }

            (*cursor)->keys[i] = x;
            (*cursor)->childPtr[i + 1] = *child;

            cout << "Inserted Internal " << endl;

        }else{
            //splitting
            cout << "OverFlow Splitting Happens " << endl;

            vector<int> virtualkeyNode((*cursor)->keys);
            vector<Node*> virtualTreePtrNode((*cursor)->childPtr);

            int i = std::upper_bound((*cursor)->keys.begin(),(*cursor)->keys.end(),x) - (*cursor)->keys.begin();
            virtualkeyNode.push_back(x);
            virtualTreePtrNode.push_back(*child);

            if(i != virtualkeyNode.size() - 1){
                for(int j = virtualkeyNode.size() - 1; j > i; j--){
                    virtualkeyNode[j] = virtualkeyNode[j - 1];
                }

                for(int j = virtualTreePtrNode.size() - 1; j > (i + 1); j--){
                    virtualTreePtrNode[j] = virtualTreePtrNode[j - 1];
                }

                virtualkeyNode[i] = x;
                virtualTreePtrNode[i + 1] = *child;
            }

            int partitionKey = virtualkeyNode[(virtualkeyNode.size()/2)];
            int partitionIdx = (virtualkeyNode.size()) / 2;

            //resizing and copying
            (*cursor)->keys.resize(partitionIdx);
            (*cursor)->childPtr.resize(partitionIdx + 1);
            (*cursor)->childPtr.reserve(partitionIdx + 1);

            for(int i = 0; i < partitionIdx; i++){
                (*cursor)->keys[i] = virtualkeyNode[i];
            }

            for(int i = 0; i < partitionIdx + 1; i++){
                (*cursor)->childPtr[i] = virtualTreePtrNode[i];
            }

            Node* newInternalNode = new Node;
        
            for(int i = partitionIdx + 1; i < virtualkeyNode.size(); i++){
                newInternalNode->keys.push_back(virtualkeyNode[i]);
            }

            for(int i = partitionIdx + 1; i < virtualTreePtrNode.size(); i++){
                newInternalNode->childPtr.push_back(virtualTreePtrNode[i]);
            }

            if((*cursor) == root){

                Node* newRoot = new Node;
                newRoot->keys.push_back(partitionKey);
                newRoot->childPtr.push_back(*cursor);
                newRoot->childPtr.push_back(newInternalNode);

                root = newRoot;
                cout << "New ROOT " << endl;

            }else{
                //recursion
                //implementation pending
            }

        }


    }
}


void insertionMethod(BPTree** bpTree){
        int rollNo;
        int age,marks;
        string name;

        cout << "Enter the roll no " << endl;
        cin >> rollNo;

        cout << "Enter the name age and marks " << endl;
        cin >> name >> age >> marks;

        string fileName = "DBFiles/";
        fileName += to_string(rollNo) + ".txt";
        FILE* filePtr = fopen(fileName.c_str(),"w");
        string userTuple = name + " " + to_string(age) + " " + to_string(marks) + "\n";
        fprintf(filePtr,userTuple.c_str());

        (*bpTree)->insert(rollNo,filePtr);
        fclose(filePtr);
        cout << "Inserted " << endl;

    }

int main(){

    BPTree* bpTree = new BPTree();
    insertionMethod(&bpTree);
    insertionMethod(&bpTree);
    insertionMethod(&bpTree);
    insertionMethod(&bpTree);
    insertionMethod(&bpTree);

    return 0;
}