//
//  bplustree.cpp
//  bplustree
//
//  Created by Peiliang Bai on 3/21/19.
//  Copyright © 2019 Peiliang Bai. All rights reserved.
//

#include <iostream>
#include <bits/stdc++.h>
#include <string.h>

using namespace std;

#define MAX 10000
#define OUTPUT_FILE "output_file.txt"

int ORDER;

/* we construct a structure of Node, in this struct, it contains keys, values, pointers to parent and children respectively. */
struct Node{
    int nkey;                   // the number of keys
    int keys[MAX];              // keys in the node
    float values[MAX];          // values in the node
    
    Node *parentNode;           // pointer to parent node
    Node *childNode[MAX];       // pointer to children nodes
    
    Node(){                     // initialize node
        nkey = 0;
        parentNode = NULL;
        for(int i=0; i<MAX; ++i){
            keys[i] = INT_MAX;
            values[i] = FLT_MAX;
            childNode[i] = NULL;
        }
    }
};

// create root node
Node *rootNode = new Node();

// initialize the b+ tree with an input order
void initialize(int m){
    ORDER = m;
}

// split the leaf nodes
void splitLeafNode(Node *cNode){
    int x, i, j;
    // split the less half to the left when order is odd
    if(ORDER % 2)
        x = (ORDER - 1)/2;
    else x = ORDER / 2;
    
    // we don't create another Node for left Node, rather re-use cNode as lNode and
    // put the right half keys to the rNode
    Node *rNode = new Node();
    
    // lNode has x number of nodes
    cNode-> nkey = x;
    // rNode has order-x
    rNode-> nkey = ORDER - x;
    rNode-> parentNode = cNode-> parentNode;
    
    for(i=x, j=0; i<ORDER; i++, j++){
        // extract right-half keys from cNodes and put in the rNode
        rNode-> keys[j] = cNode-> keys[i];
        rNode-> values[j] = cNode-> values[i];
        // now delete the right-half keys and values
        cNode-> keys[i] = INT_MAX;
        cNode-> values[i] = FLT_MAX;
    }
    // for leaf node, we need to copy the first item from the rNode to their parentNode and k contains that key
    int k = rNode-> keys[0];
    
    // if the leaf itself is a parent then
    if(cNode-> parentNode==NULL){
        //it has null parent, so create a new parent
        Node *parentNode = new Node();
        //and new parent should have a null parent
        parentNode-> parentNode = NULL;
        //new parent will have only one member
        parentNode-> nkey=1;
        //and that member is k
        parentNode-> keys[0] = k;
        //so the parent has two child, so assign them (don't forget cNode is actually the lNode)
        parentNode-> childNode[0] = cNode;
        parentNode-> childNode[1] = rNode;
        //their parent of the left and right Nodes is no longer null, so assign their parent
        cNode-> parentNode = rNode-> parentNode = parentNode;
        //from now on this parentNode is the rootNode
        rootNode = parentNode;
        return;
    }else{
        // the splitted leaf node is not root
        // we put the leader key k in the parent and assign rNode to the right place in the parentNode
        // so we go to the parentNode and from now we consider the cNode as the parentNode of the splitted Node
        cNode = cNode-> parentNode;
        Node *newChildNode = new Node();
        newChildNode = rNode;
        
        // simple insertion and sorting to put k at the exact position of keys in the parentNode (cNode here)
        for(i=0; i<=cNode-> nkey; i++){
            if(k < cNode-> keys[i]){
                swap(cNode-> keys[i], k);
            }
        }
        // after putting k, the number of nodes gets increase by one
        cNode-> nkey++;
        
        // simple insertNodeion sort to put rNode at the exact position
        // of childNodes in the cNode
        for(i=0; i<cNode-> nkey; i++){
            if(newChildNode-> keys[0] < cNode-> childNode[i]-> keys[0]){
                swap(cNode->childNode[i], newChildNode);
            }
        }
        cNode->childNode[i] = newChildNode;
        
        // reordering some Nodes and pointers
        for(i=0;cNode->childNode[i]!=NULL;i++){
            cNode->childNode[i]->parentNode = cNode;
        }
    }
}

// split the non leaf nodes
void splitNonLeaf(Node *cNode){
    int x, i, j;
    
    //split the less half to the left when numberOfPointer is odd
    //else split equal equal when numberOfPointer is even.  n/2 does it nicely for us
    
    x = ORDER / 2;
    if(ORDER % 2)
        x = (ORDER - 1)/2;
    else
        x = ORDER / 2 - 1;
    
    //declare rNode and we will use cNode as the lNode
    Node *rNode = new Node();
    
    //so lNode has x number of nodes
    cNode-> nkey = x;
    //rNode has numberOfPointers-x-1 children, because we won't copy and paste
    //rather delete and paste the first item of the rNode
    rNode-> nkey = ORDER - x-1;
    //both children have their common parent
    rNode-> parentNode = cNode-> parentNode;
    
    
    for(i=x, j=0; i<=ORDER; i++, j++){
        //copy the right-half members to the rNode
        rNode-> keys[j] = cNode-> keys[i];
        //and also copy their children
        rNode-> childNode[j] = cNode-> childNode[i];
        //erase the right-half keys from cNode to make it perfect lNode
        //which won't contain only left-half keys and their children
        cNode-> keys[i] = INT_MAX;
        //erase all the right-half childNodes from cNode except the x one
        //because if left child has 3 nodes then it should have 4 childNodes, so don't delete that child
        if(i!=x)cNode-> childNode[i] = NULL;
    }
    
    //we will take a copy of the first item of the rNode
    //as we will delete that item later from the list
    int k = rNode-> keys[0];
    //just right-shift key[] and childNode[] by one from rNode
    //to have no repeat of the first item for non-leaf Node
    memcpy(&rNode-> keys, &rNode->keys[1], sizeof(int)*(rNode->nkey+1));
    memcpy(&rNode-> childNode, &rNode-> childNode[1], sizeof(rootNode)*(rNode-> nkey+1));
    
    //we reordered some keys and positions so don't forget
    //to assign the children's exact parent
    
    for(i=0;cNode-> childNode[i]!=NULL;i++){
        cNode-> childNode[i]-> parentNode = cNode;
    }
    for(i=0;rNode-> childNode[i]!=NULL;i++){
        rNode-> childNode[i]-> parentNode = rNode;
    }
    
    //if the splitted Node itself a parent
    if(cNode-> parentNode==NULL){
        //create a new parent
        Node *parentNode = new Node();
        //parent should have a null parent
        parentNode-> parentNode = NULL;
        //parent will have only one node
        parentNode-> nkey=1;
        //the only key is the k
        parentNode-> keys[0] = k;
        //it has two children, lNode and rNode
        parentNode-> childNode[0] = cNode;
        parentNode-> childNode[1] = rNode;
        
        //and both rNode and lNode has no longer null parent, they have their new parent
        cNode-> parentNode = rNode-> parentNode = parentNode;
        
        //from now on this new parent is the root parent
        rootNode = parentNode;
        return;
    }
    else{
        // this case is for the splitted leaf Node is not rootNode
        // we have to put the k and assign the rNode to the right place in the parentNode
        // so we go to the parentNode and from now we consider the cNode as the parentNode of the splitted Node
        cNode = cNode-> parentNode;
        Node *newChildNode = new Node();
        newChildNode = rNode;
        
        // simple insertion sort to put k at the exact position of keys in the parentNode
        // here we use swap function to locate the k at the exact position.
        for(i=0; i<=cNode-> nkey; i++){
            if(k < cNode-> keys[i]){
                swap(cNode-> keys[i], k);
            }
        }
        
        //after putting k number of nodes gets increase by one
        cNode-> nkey++;
        
        // simple insertNodeion sort to put rNode at the exact position
        // of childNode in the parentNode
        
        for(i=0; i<cNode-> nkey; i++){
            if(newChildNode-> keys[0] < cNode-> childNode[i]-> keys[0]){
                swap(cNode-> childNode[i], newChildNode);
            }
        }
        cNode-> childNode[i] = newChildNode;
        
        // we reordered some Nodes and pointers, so for the sake of safety
        // all childNodes' should have their parent updated
        for(i=0;cNode-> childNode[i]!=NULL;i++){
            cNode-> childNode[i]-> parentNode = cNode;
        }
    }
}

// inserting (key, value) into a node
void insertNode(Node *cNode, int k, float val){
    for(int i=0; i<=cNode->nkey; i++){
        if(k < cNode-> keys[i] && cNode-> childNode[i]!=NULL){
            insertNode(cNode-> childNode[i], k, val);
            if(cNode-> nkey == ORDER)
                splitNonLeaf(cNode);
            return;
        }
        // when reach the leaf node
        else if(k < cNode-> keys[i] && cNode-> childNode[i]==NULL){
            swap(cNode-> keys[i], k);
            swap(cNode-> values[i], val);
            //swap(cNode->childNode[i], newChildNode);
            if(i == cNode-> nkey){
                cNode-> nkey++;
                break;
            }
        }
    }
    
    if(cNode-> nkey == ORDER){
        splitLeafNode(cNode);
    }
}

// re-distribution will affect the first key of the rNode, so remember it
// for later replacement of the copy of this key somewhere in ancestor Node
void arrangeNode(Node *lNode, Node *rNode, bool isLeaf, int posOflNode, int whichOneiscNode){
    int PrevRightFirstk = rNode-> keys[0];
    
    // case: left node is current node
    if( whichOneiscNode == 0 ){
        
        // not the leaf node
        if(!isLeaf){
            //bring down the key from which it is left child in parentNode
            lNode-> keys[lNode->nkey] = lNode-> parentNode-> keys[posOflNode];
            //the right child of newly added key should be the left child of first key of rNode
            lNode-> childNode[lNode-> nkey+1] = rNode-> childNode[0];
            //increase lNode's number of nodes by one
            lNode-> nkey++;
            //send up a the first key of the rNode to the parentNode
            lNode-> parentNode-> keys[posOflNode] = rNode->keys[0];
            //shift left by one in rNode
            memcpy(&rNode->keys[0], &rNode->keys[1], sizeof(int)*(rNode->nkey+1));
            memcpy(&rNode->childNode[0], &rNode->childNode[1], sizeof(rootNode)*(rNode->nkey+1));
            rNode->nkey--;
            
        }else{
            // leaf node then we borrow the first key of rNode to the last position of lNode
            lNode->keys[lNode->nkey] = rNode->keys[0];
            lNode->values[lNode->nkey] = rNode->values[0];
            lNode->nkey++;
            //shift by one node to left of the rNode
            memcpy(&rNode->keys[0], &rNode->keys[1], sizeof(int)*(rNode->nkey+1));
            memcpy(&rNode->values[0], &rNode->values[1], sizeof(int)*(rNode->nkey+1));
            //decrease number of nodes by one
            rNode->nkey--;
            
            lNode->parentNode-> keys[posOflNode] = rNode->keys[0];
        }
        
        
        
    }
    // case: left node is not current node
    else{
        // not the leaf node
        if(!isLeaf){
            //shift right by one in rNode so that first position becomes free
            memcpy(&rNode->keys[1], &rNode->keys[0], sizeof(int)*(rNode->nkey+1));
            memcpy(&rNode->childNode[1], &rNode->childNode[0], sizeof(rootNode)*(rNode->nkey+1));
            //bring down the key from which it is left child in parentNode to first pos of rNode
            rNode->keys[0] = lNode->parentNode->keys[posOflNode];
            //and the left child of the newly first key of right child will be the last child of lNode
            rNode->childNode[0] = lNode->childNode[lNode->nkey];
            
            rNode->nkey++;
            
            //send up a the last key of the lNode to the parentNode
            lNode->parentNode->keys[posOflNode] = lNode->keys[lNode->nkey-1];
            //erase the last element and pointer of lNode
            lNode->keys[lNode->nkey-1] = INT_MAX;
            lNode->childNode[lNode->nkey] = NULL;
            lNode->nkey--;
            
        }else{
            // leaf node then shift by one node to right of the rNode so that we can free the first position
            memcpy(&rNode->keys[1], &rNode->keys[0], sizeof(int)*(rNode->nkey+1));
            memcpy(&rNode->values[1], &rNode->values[0], sizeof(int)*(rNode->nkey+1));
            //borrow the last key of lNode to the first position of rNode
            rNode->keys[0] = lNode->keys[lNode->nkey-1];
            rNode->values[0] = lNode->values[lNode->nkey-1];
            //increase number of nodes by one
            rNode->nkey++;
            
            lNode->keys[lNode->nkey-1] = INT_MAX;
            lNode->values[lNode->nkey-1] = FLT_MAX;
            lNode->nkey--;
            
            lNode->parentNode->keys[posOflNode] = rNode->keys[0];
        }
    }
}

// merge two nodes
void mergeNode(Node *lNode, Node *rNode, bool isLeaf, int posOfrNode){
    if(!isLeaf){
        lNode-> keys[lNode->nkey] = lNode->parentNode-> keys[posOfrNode-1];
        lNode->nkey++;
    }
    else if(isLeaf){
        memcpy(&lNode-> values[lNode-> nkey], &rNode-> values[0], sizeof(int)*(rNode-> nkey+1));
    }
    memcpy(&lNode-> keys[lNode-> nkey], &rNode-> keys[0], sizeof(int)*(rNode-> nkey+1));
    memcpy(&lNode-> childNode[lNode-> nkey], &rNode-> childNode[0], sizeof(rootNode)*(rNode-> nkey+1));
    
    // #left node key should be added by #right node key
    lNode-> nkey += rNode-> nkey;

    memcpy(&lNode->parentNode-> keys[posOfrNode-1], &lNode-> parentNode-> keys[posOfrNode], sizeof(int)*(lNode-> parentNode-> nkey+1));
    memcpy(&lNode->parentNode-> childNode[posOfrNode], &lNode-> parentNode-> childNode[posOfrNode+1], sizeof(rootNode)*(lNode-> parentNode-> nkey+1));
    
    // after merging, the parent of left node should decrease by 1
    lNode-> parentNode-> nkey--;
    
    for(int i=0;lNode-> childNode[i] != NULL;i++){
        lNode-> childNode[i]-> parentNode = lNode;
    }
}

// Deletion operation function
bool dataFound;          // boolean variable, indicate if we find the data.
void deleteNode(Node *cNode, int k, int cNodePosition){
    // Boolean variable to indicate if the node is leaf or not. Initialize by checking if it has children.
    bool isLeaf;
    if(cNode-> childNode[0] == NULL)
        isLeaf = true;
    else isLeaf = false;
    
    // left-most key could be changed due to merge or re-distribution later,
    // so keep it to replace it's copy from it's ancestor
    int prevLeftMostk = cNode->keys[0];
    for(int i=0;dataFound==false && i<=cNode->nkey; i++){
        if(k < cNode->keys[i] && cNode->childNode[i] != NULL){
            deleteNode(cNode->childNode[i], k, i);
        }
        //if we could find the target key at any leafNode then
        else if(k == cNode->keys[i] && cNode->childNode[i] == NULL){
            
            //delete the node by shifting all keys and pointers  by one to the left
            memcpy(&cNode->keys[i], &cNode->keys[i+1], sizeof(int)*(cNode->nkey+1));
            memcpy(&cNode->values[i], &cNode->values[i+1], sizeof(int)*(cNode->nkey+1));
            
            //decrease number of nodes by one
            cNode-> nkey--;
            dataFound = true;
            break;
        }
    }
    
    //if the root is the only leaf
    if(cNode->parentNode == NULL && cNode->childNode[0] == NULL){
        return;
    }
    
    
    //if the cNode is rootNode and it has one pointers only
    if(cNode->parentNode==NULL && cNode->childNode[0] != NULL && cNode->nkey == 0){
        rootNode = cNode->childNode[0];
        rootNode->parentNode = NULL;
        return;
    }
    
    
    // check if the cNode has less than half of the number of maximum node
    // if less than half we will try to re-distribute first

    if(isLeaf && cNode->parentNode!=NULL){
        
        if(cNodePosition==0){
            Node *rNode = new Node();
            rNode = cNode->parentNode->childNode[1];
            
            // if current node is under-full and the right one has more than half nodes of maximum capacity
            // then re-distribute
            if( rNode != NULL && rNode->nkey > (ORDER+1)/2 && cNode-> nkey < (ORDER-1)/2){
                arrangeNode(cNode, rNode, isLeaf, 0, 0);
            }
            // else there is nothing to re-distribute, thus we merge them
            else if (rNode != NULL && cNode->nkey + rNode->nkey < ORDER){
                mergeNode(cNode, rNode, isLeaf, 1);
            }
        }
        else{
            Node *lNode = new Node();
            Node *rNode = new Node();
            lNode = cNode->parentNode-> childNode[cNodePosition-1];
            rNode = cNode->parentNode-> childNode[cNodePosition+1];

            // similar to the previous one, we re-distribute when under-full.
            if( lNode != NULL && lNode->nkey > (ORDER+1)/2 && cNode-> nkey < (ORDER-1)/2 ){
                arrangeNode(lNode, cNode, isLeaf, cNodePosition-1, 1);
            }
            else if( rNode!=NULL && rNode-> nkey > (ORDER+1)/2 && cNode-> nkey < (ORDER-1)/2 ){
                arrangeNode(cNode, rNode, isLeaf, cNodePosition, 0);
            }
            else if (lNode!=NULL && cNode->nkey + lNode->nkey < ORDER){
                mergeNode(lNode, cNode, isLeaf, cNodePosition);
            }
            else if (rNode!=NULL && cNode-> nkey + rNode->nkey < ORDER){
                mergeNode(cNode, rNode, isLeaf, cNodePosition+1);
            }
        }
    }
    else if(!isLeaf && cNode->parentNode != NULL){
        
        if(cNodePosition==0){
            Node *rNode = new Node();
            rNode = cNode-> parentNode-> childNode[1];
            
            // if we see the right one has more than half nodes of maximum capacity then re-distribute
            if( rNode != NULL && rNode->nkey - 1 >= ceil((ORDER - 1)/2.0) && cNode-> nkey < (ORDER-1)/2 ){
                arrangeNode(cNode, rNode, isLeaf, 0, 0);
            }
            // else we merge two nodes instead of re-distribute.
            else if (rNode!=NULL && cNode->nkey + rNode->nkey < ORDER - 1){
                mergeNode(cNode, rNode, isLeaf, 1);
            }
        }
        // for any other case we can safely take the left one to try for re-distribution
        else{
            Node *lNode = new Node();
            Node *rNode = new Node();
            lNode = cNode->parentNode->childNode[cNodePosition-1];
            rNode = cNode->parentNode->childNode[cNodePosition+1];
    
            //if we see that left one has more than half nodes of maximum capacity then try to re-distribute
            if( lNode!=NULL && lNode->nkey - 1 >= ceil((ORDER - 1)/2.0) && cNode-> nkey < (ORDER-1)/2 ){
                arrangeNode(lNode, cNode, isLeaf, cNodePosition-1, 1);
            }
            else if(rNode!=NULL && rNode->nkey-1 >=  ceil((ORDER-1)/2.0) && cNode-> nkey < (ORDER-1)/2 ){
                arrangeNode(cNode, rNode, isLeaf, cNodePosition, 0);
            }
            // else there is nothing to re-distribute, so we merge them
            
            else if ( lNode!=NULL && cNode->nkey + lNode->nkey < ORDER-1) {
                mergeNode(lNode, cNode, isLeaf, cNodePosition);
            }
            else if ( rNode!=NULL && cNode->nkey + rNode->nkey < ORDER-1){
                mergeNode(cNode, rNode, isLeaf, cNodePosition+1);
            }
        }
    }
    // delete the duplicate if any in the ancestor Node
    Node *tempNode = new Node();
    tempNode = cNode->parentNode;
    while(tempNode!=NULL){
        for(int i=0; i<tempNode->nkey;i++){
            if(tempNode->keys[i] == prevLeftMostk){
                tempNode->keys[i] = cNode->keys[0];
                break;
            }
        }
        tempNode = tempNode->parentNode;
    }
    
}

// search node by ONE key, if we cannot find it, then we return NULL
float searchedvalue;
void searchNode(Node *cNode, int k, int cNodePosition){
    // boolean variable indicating leaf
    bool isLeaf;
    if(cNode->childNode[0]==NULL)
        isLeaf = true;
    else isLeaf = false;
    
    // recursively search key
    for(int i=0;dataFound == false && i<=cNode->nkey; i++){
        if(k < cNode-> keys[i] && cNode-> childNode[i] != NULL){
            searchNode(cNode->childNode[i], k, i);
        }
        //if we could find the target key at any leafNode then
        else if(k == cNode->keys[i] && cNode->childNode[i] == NULL){
            searchedvalue = cNode->values[i];
            // cout << searchedvalue << "\n";
            dataFound = true;
            break;
        }
    }
}

// search nodes' range by TWO keys, return all nodes within the given two keys.
vector <float> searchedvalues;    // create a vector for restoring values for searched node
void searchNodeRange(Node *cNode, int k1, int k2, int cNodePosition){
    // boolean variable indicating leaf
    bool isLeaf;
    if(cNode->childNode[0]==NULL)
        isLeaf = true;
    else isLeaf = false;
    
    // recursively search key, if the given key is satisfied in some range, we choose this range
    // and extract the nodes within this range.
    for(int i=0; i<=cNode-> nkey; i++){
        if(cNode-> childNode[i] != NULL){
            // in this case, we cannot find the range
            searchNodeRange(cNode-> childNode[i], k1, k2, i);
        }
        else if(k1 <= cNode-> keys[i] && k2 >= cNode-> keys[i] && cNode-> childNode[i] == NULL){
            // push nodes back to the result vector.
            searchedvalues.push_back(cNode-> values[i]);
            dataFound = true;
            continue;
        }
    }
}

/* main function */
int main(int argc, char *argv[]){
    string line;
    string::size_type sz;
    ifstream myfile(argv[1]);    // input file
    ofstream outputFile;         // output file
    
    if (myfile.is_open()){
        outputFile.open(OUTPUT_FILE, ios::out | ios::trunc);
        while(getline(myfile, line)){
            // line.erase(remove(line.begin(), line.end(), ' '), line.end());
            // cout << line << '\n';
            // Vector of string to save tokens
            vector <string> tokens;
            // stringstream to read lines from file
            stringstream stringStream(line);
            while(getline(stringStream, line)){
                size_t prev = 0, pos;
                while ((pos = line.find_first_of("(), ", prev)) != string::npos){
                    if (pos > prev)
                        tokens.push_back(line.substr(prev, pos-prev));
                    prev = pos + 1;
                }
            }
            
            // read tokens from line in file, to check if what function we need to use
            if (tokens[0] == "Initialize"){
                int order = stoi (tokens[1], &sz);
                initialize(order);;
            }
            else if(tokens[0] == "Insert"){
                int key = stoi(tokens[1], &sz);
                float val = stof (tokens[2], &sz);
                insertNode(rootNode, key, val);
            }
            else if(tokens[0] == "Delete"){
                dataFound = false;
                int key = stoi(tokens[1], &sz);
                deleteNode(rootNode, key, 0);
            }
            else if(tokens[0] == "Search"){
                if (tokens.size() == 2){
                    int key = stoi(tokens[1], &sz);
                    dataFound = false;
                    // cout << key << '\n';
                    searchNode(rootNode, key, 0);
                    outputFile << searchedvalue << "\n";
                    if(dataFound == false){
                        printf("Null\n");
                    }
                }
                else if(tokens.size() == 3){
                    int key1 = stoi(tokens[1], &sz);
                    int key2 = stoi(tokens[2], &sz);
                    dataFound = false;
                    searchedvalues.clear();    // clear the searched values vector before we use it.
                    searchNodeRange(rootNode, key1, key2, 0);
                    if(dataFound == true){
                        for(auto j=searchedvalues.begin(); j != searchedvalues.end(); ++j){
                            outputFile << *j;
                            if(j < searchedvalues.end()-1){
                                outputFile << ",";
                            }
                        }
                        outputFile << "\n";
                    }
                    else if(dataFound == false){
                        outputFile << "Null\n";
                    }
                }
            }
        }
        myfile.close();
        outputFile.close();
    }else
        cout << "Error in input file";
    
    return 0;
}
