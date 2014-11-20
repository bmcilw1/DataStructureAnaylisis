//Brian McIlwain && Hunter Crossett
//CSC 3102
//Linked List Trie

#include<iostream>
#include<iomanip>
#include<fstream>
#include"timer.cpp"
#include<cstring>
#include<cstdlib>
#include<string>
#include<sys/sysinfo.h>
#include<sys/types.h>

using namespace std;

unsigned long long n = 0;

struct Node {
    string label;
    Node *rSib;
    Node *fChild;
    bool isWord;
    int id;
};

class Hash{
public:
    Node ** A;
    int length;

    Hash(){ A = NULL; length = 0;}
    ~Hash(){ ; }
    void InitializeHash(){
        A = new Node * [27 * n]; 
        length = 0; 
        for (int i = 0; i < (27 * n); i++)
            A[i] = NULL;
    }
    void HashInsert(int pID, Node * x){ 
        int key = ((x->label[0] - 'a') + (pID * 26));
        if(A[key])
        cout<< "Key: "<< key << " Label: "<< x->label <<" Collision occured!\n";
        else if (key > 27 *n)
        cout<< "Key: "<< key <<" Hash function stepped out of bounds!\n";
        else{
            A[key] = x;
            length++;
        }
    }
    Node * HashSearch(int pID, char ch){
        int key = ((ch - 'a') + (pID * 26));
        return A[key];
    }
};


class llTrie{
private:
    Node * root;
    Hash H;

void DestroySubTrie(Node *& x){
    if(x){
        Node *sib = x->rSib;
        Node *ch = x->fChild;
        
        delete x;
        x = NULL;

        if(sib)
            DestroySubTrie(sib);
        if(ch)
            DestroySubTrie(ch);
    }
}

void PreOrder(Node * p, int & ID){
    if(p){
        p->id = ID;
        ID++;
        if(p->fChild)
            PreOrder(p->fChild, ID);
        if (p->rSib)
            PreOrder(p->rSib, ID);
    }
}
void Insert(Node *& p, string s){
    int i = 0;
    while(p && i < p->label.length() && i < s.length() && p->label[i] == s[i])
        i++;
    
    if(i == s.length() && i == p->label.length()){
        p->isWord = true;
        return;
    }

    if (i == 0){
        if (!p->rSib){
            Node * q3 = new Node;
            q3->label = s;
            q3->fChild = q3->rSib = NULL;
            q3->isWord = true;
            p->rSib = q3;
            n++;
        }
        else
            Insert(p->rSib, s);
    }

    else if (p->label.length() == i){//everything in p mathes origial s, and s has more characters
        s = s.substr(i, s.length());

        if (!p->fChild){
            Node * q = new Node;
            q->label = s;
            q->fChild = q->rSib = NULL;
            q->isWord = true;
            p->fChild = q;
            n++;
        }
        else
            Insert(p->fChild, s);
    }
    else if (s.length() == i){ //this else means that s.length() == i and p has more characters
    //some piece is a mismatch: ex 1: p->l = asdf and original s = as
    //case 2: p->l = asdf and original s = s
            string s1, s2;
            
            s1 = s;
            s2 = p->label.substr(i, p->label.length());

            Node *q1 = new Node;
            q1->label = s1;
            q1->isWord = true;

            Node *q2 = new Node;
            q2->label = s2;
            q2->rSib = NULL;

            q2->fChild = p->fChild;
            q2->isWord = p->isWord;
            
            q1->rSib = p->rSib;
            q1->fChild = q2;
            
            p = q1;
    }
    else { //i<p && i<s
    //1) put p overflow into a child of p, check if there is one with fist char matching else make it first child
    //2) make s overflow a sib of p->fChild. check if there is one with first char matching else make it fist sib
        string pStart = p->label.substr(0, i);
        string pEnd = p->label.substr(i, p->label.length());

        p->label = pStart;
        
        Node * q4 = new Node;
        q4->label = pEnd;
        q4->fChild = p->fChild;
        q4->rSib = NULL;
        q4->isWord = p->isWord;
        p->fChild = q4;
        p->isWord = false;
        n++;

        s = s.substr(i, s.length());
        Insert(p->fChild, s);
    }
}

bool Query(Node *& p, string s){
    if(!p)
        return false;

    int i = 0;
    while(p && i < p->label.length() && i < s.length() && p->label[i] == s[i])
        i++;
    
    if(i == s.length() && i == p->label.length())
        return p->isWord;

    else if (i == 0 && p->rSib)
        return Query(p->rSib, s);

    else if (p->label.length() == i && p->fChild){
        s = s.substr(i, s.length());
        return Query(p->fChild, s);
    }

    else
        return false;
}

bool HashQuery(Node *& p, string s, int & pID){
    Node * x = H.HashSearch(pID, s[0]);
    
    if (!x)
        return false;
    
    int i = 0;
    while(x && i < x->label.length() && i < s.length() && x->label[i] == s[i])
        i++;

    if(i == s.length() && i == x->label.length())
        return x->isWord;
    
    else if (x->label.length() == i && x->fChild){
        s = s.substr(i, s.length());

        if(x->fChild)
            return HashQuery(x->fChild, s, x->id);
        else
            return false;
    }

    else
        return false;
}

void HashFill(Node *& x, int & pID){
    if(x){
        if(x != root)
            H.HashInsert(pID, x);

        if(x->fChild)
            HashFill(x->fChild, x->id);

        if(x->rSib)
            HashFill(x->rSib, pID);
    }
}

public:

    llTrie(){ 
        root = new Node;
        root->fChild = root->rSib = NULL;
        root->label = "";
        root->isWord = false;
        root->id = 0;
    }
    ~llTrie(){ DestroySubTrie(root); }
    void Add(string word){
        if(!root->fChild){
            Node *n = new Node;
            n->rSib = n->fChild = NULL;
            n->label = word;
            n->isWord = true;
            root->fChild = n;
        }
        else
            Insert(root->fChild, word);
    }
    bool Search(string word){
        if (root)
            return Query(root->fChild, word);
    }
    void AssignID(){
        int i = 0;
        PreOrder(root, i);
    }
    void FillHash(){
        AssignID();
        H.InitializeHash();
        int i = 0;
        HashFill(root, i);
        //cout<<"\nHash length: "<< H.length<<endl;
    }
    bool SearchHash(string s){
        int pID = 0;
        return HashQuery(root, s, pID);
    }

};

void InputD(llTrie & trie, string fDic);
void InputQ(llTrie & trie, string fQuery);
void OpenInput(fstream & fin, string inName);
void InputHQ(llTrie & trie, string fQuery);

int main(){
    CStopWatch t;
    llTrie trie;
    string fDic;
    string fQuery;
    struct sysinfo memInfo;
    long long memInit, memEnd;

    cout<<"Enter name of dictionary file: ";
    cin>> fDic;
    cout<<"Enter name of query file: ";
    cin>> fQuery;
    cout<<endl;

    t.startTimer();
    InputD(trie, fDic);
    t.stopTimer();
    cout<<"Insering into the linked list took "<<t.getElapsedTime()<<" micro-seconds."<<endl;
    memEnd = memInfo.totalram - memInfo.freeram;
    cout<< "Total memory used: "<< memEnd <<" bytes."<<endl<<endl;

    t.startTimer();
    InputQ(trie, fQuery);
    t.stopTimer();
    cout<<"Linked list queries took "<<t.getElapsedTime()<<" micro-seconds."<<endl<<endl;

    t.startTimer();
    trie.FillHash();
    t.stopTimer();
    cout<<"Insering into the Hash took "<<t.getElapsedTime()<<" micro-seconds."<<endl;
    memEnd = memInfo.totalram - memInfo.freeram;
    cout<< "Total memory used: "<< memEnd <<" bytes."<<endl<<endl;

    t.startTimer();
    InputHQ(trie, fQuery);
    t.stopTimer();
    cout<<"Hash queries took "<<t.getElapsedTime()<<" micro-seconds."<<endl<<endl;

    cout<< "Number of nodes: "<< n <<endl<<endl;

    return 0;
}

void InputD(llTrie & trie, string fDic){
    fstream finD;
    OpenInput(finD, fDic);
    string word;

    finD >> word;
    while(!finD.eof())
    {   
        trie.Add(word);
        finD >> word;
    }
    
    finD.close();
}

void InputQ(llTrie & trie, string fQuery){
    fstream fin;
    OpenInput(fin, fQuery);
    string word;
    
    cout<<"Linked list queries: \n";
    fin >> word;
    while(!fin.eof())
    {   
    //    trie.Search(word);
        cout<<left<<setw(25)<< word <<" "<< trie.Search(word) << endl;
        fin >> word;
    }
    cout<<endl;

    fin.close();

}

void InputHQ(llTrie & trie, string fQuery){
    fstream fin;
    OpenInput(fin, fQuery);
    string word;

    cout<<"Hash queries: \n";
    fin >> word;
    while(!fin.eof())
    {   
    //    trie.SearchHash(word);
        cout<<left<<setw(25)<< word <<" "<< trie.SearchHash(word) << endl;
        fin >> word;
    }
    cout<< endl;

    fin.close();
}

void OpenInput(fstream & fin, string inName){
//For debugging, remove before posting
    if (inName.compare("a") == 0)
        inName = "WORD.LST";
    else if (inName.compare("s") == 0)
        inName = "QUERY.LST";
//end of debugging stuff

    fin.open(inName.c_str(), ios::in);

    if(fin.fail())
    {
        cout<<"Unable to open "<<inName<<" for input."<<endl;
        exit(1);
    }
}
