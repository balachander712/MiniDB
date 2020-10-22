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