#include <iostream>
#include <sstream>
#include <string>

using namespace std;

template <class K, class D, int M> // K: key, D: data, M: degree of BTree
class BTree {
    /// Convention: Left sub-tree < Root's key <= Right sub-tree

public:
    class Entry;

    class Node;

private:
    Node *root;

public:
    BTree() : root(0) {};

    ~BTree() {}
public:
    class Entry {
    private:
        K key;
        D data;
        Node *rightPtr;

        friend class BTree<K, D, M>;

    public:
        Entry(K key = K{}, D value = D{}) : key(key), data(value), rightPtr(0) {}
        ~Entry() {}
        string toString() {
            stringstream ss;
            ss << "<"
                   << this->key << ","
                   << this->data
                   << ">";
            return ss.str();
        }
    };
public:
    class Node {
    private:
        Node *firstPtr;
        int numEntries;
        Entry entries[M - 1];

        friend class BTree<K, D, M>;

    public:
        Node() : firstPtr(0), numEntries(0) {};

        ~Node() { }
        string trave(Node* at){
            stringstream ss;
            ss << '[';
            if (at->numEntries!=0) ss << '(' << at->numEntries << ')';
            if (at!=nullptr){
                if (firstPtr!=nullptr) ss << trave(firstPtr);
                for (int i = 0;i<at->numEntries;++i){
                    ss << entries[i].toString();
                    if (entries[i].rightPtr!=nullptr) ss << trave(entries[i].rightPtr);
                }
            }
            ss << ']';
            return ss.str();
        }
        string toString() {
            return trave(this);
        }
        bool isFull() {
            return (numEntries >= M - 1);
        }
    };

public:
    string trav(Node* at){
        if (at == nullptr) return "";
        stringstream s;
        s << at->toString();
        if (at->firstPtr != nullptr) {
            s << ' ' << trav(at->firstPtr);
        }
        for (int i = 0; i < at->numEntries; ++i) {
            if (at->entries[i].rightPtr != nullptr) {
                s << ' ' << trav(at->entries[i].rightPtr);
            }
        }
        return s.str();
    }
    string toStringPreOrder() {
        return trav(root);
    }
    bool isNull(Entry at){
        return (at.data==0 && at.key==0 && at.rightPtr==nullptr);
    }
    /*int find(Node* at, Entry x){
        for (int i = 0;i<at->numEntries;++i) if (equal(at->entries[i],x)) return i;
        return -1;
    }*/
    Entry insert_entry(Node* at, Entry which, Node* left_hand){
        Entry ans;
        if (at->isFull()){
            if (which.key<at->entries[M-2].key){
                ans = at->entries[M-2];
                at->entries[M-2] = Entry();
                at->numEntries--;
            } else return which;
        }
        int i = at->numEntries-1;
        while (i>-1 && which.key<at->entries[i].key) {
            at->entries[i+1] = at->entries[i];
            i--;
        }
        at->entries[i+1] = which;
        if (i!=-1) at->entries[i].rightPtr = left_hand; else at->firstPtr = left_hand;
        at->numEntries++;
        return ans;
    }
    Entry split(Node* at, int branch, Entry remain){
        Node* left = (branch!=-1)?at->entries[branch].rightPtr:at->firstPtr;
        Node* right = new Node;
        for (int i = (M/2+M%2);i<M-1;++i) {
            insert_entry(right,left->entries[i],left->entries[i-1].rightPtr);
            left->entries[i] = Entry();
        }
        left->numEntries = M-(M/2+1);
        insert_entry(right,remain,left->entries[left->numEntries].rightPtr);
        left->entries[left->numEntries].rightPtr = right;
        Entry ans = insert_entry(at,left->entries[left->numEntries],left);
        left->entries[left->numEntries] = Entry();
        return ans;
    }
    Entry insert_val(Node* at, const K &key, const D &data){
        Entry ans;
        if (at->isFull()){
            if (at->entries[M-2].key>key){
                ans = at->entries[M-2];
                at->entries[M-2] = Entry();
                at->numEntries--;
            } else {
                ans = Entry(key,data);
                return ans;
            }
        }
        int i = at->numEntries-1;
        while (i>-1 && key<at->entries[i].key) {
            at->entries[i+1] = at->entries[i];
            i--;
        }
        at->entries[i+1] = Entry(key,data);
        at->numEntries++;
        return ans;
    }
    Entry insert(Node* at, const K &key, const D &data){
        Entry ans;
        if (at->firstPtr==nullptr){//Leaf confirmed
            ans = insert_val(at,key,data);
            return ans;
        } else {//Not a leaf
            int i = at->numEntries-1;
            while (i>-1 && key<at->entries[i].key) i--;
            if (i==-1) ans = insert(at->firstPtr,key,data); else ans = insert(at->entries[i].rightPtr,key,data);
            if (!isNull(ans)) return split(at,i,ans);
        }
        return ans;
    }
    void insert(const K &key, const D &data) {
        if (root==nullptr){
            root = new Node;
        }
        Entry a = insert(root,key,data);
        if (!isNull(a)){
            Node* nw = new Node;
            nw->firstPtr = root;
            root = nw;
            split(root,-1,a);
        }
    }
};

int main()
{
    BTree<int, int, 3> bTree;
    int keys[] = {78, 21, 14, 11, 97, 85, 74, 63, 45, 42, 57};
    int size = sizeof(keys) / sizeof(int);

    for (int idx = 0; idx < size; idx++) {
        bTree.insert(keys[idx], idx);
    }
}
