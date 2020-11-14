#include <iostream>
#include "BPTree.hpp"

using namespace std;


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

void searchMethod(BPTree* bPTree) {
    int rollNo;
    cout << "What's the RollNo to Search? ";
    cin >> rollNo;

    bPTree->search(rollNo);
}

void printMethod(BPTree* bPTree) {

    cout << "\nHere is your File Structure" << endl;
    bPTree->display(bPTree->getRoot());
}

void deleteMethod(BPTree* bPTree) {
    cout << "Showing you the Tree, Choose a key from here: " << endl;
    bPTree->display(bPTree->getRoot());
 
    int tmp;
    cout << "Enter a key to delete: " << endl;
    cin >> tmp;
    bPTree->removeKey(tmp);

    //Displaying
    bPTree->display(bPTree->getRoot());
}

int main() {

    cout << "\n***Welcome to DATABASE SERVER**\n"
         << endl;

    bool flag = true;
    int option;

    int maxChildInt = 4, maxNodeLeaf = 3;
    cout << "Please provide the value to limit maximum child Internal Nodes can have: ";
    cin >> maxChildInt;
    cout << "\nAnd Now Limit the value to limit maximum Nodes Leaf Nodes can have: ";
    cin >> maxNodeLeaf;

    BPTree* bPTree = new BPTree(maxChildInt, maxNodeLeaf);

    do {
        cout << "\nPlease provide the queries with respective keys : " << endl;
        cout << "\tPress 1: Insertion \n\tPress 2: Search \n\tPress 3: Print Tree\n\tPress 4: Delete Key In Tree\n\tPress 5: ABORT!" << endl;
        cin >> option;

        switch (option) {
            case 1:
                insertionMethod(&bPTree);
                break;
            case 2:
                searchMethod(bPTree);
                break;
            case 3:
                printMethod(bPTree);
                break;
            case 4:
                deleteMethod(bPTree);
                break;
            default:
                flag = false;
                break;
        }
    }while (flag);

    return 0;
}