#ifndef TRIE_INCLUDED
#define TRIE_INCLUDED

#include <string>
#include <vector>
#include <iostream>

struct Node;

template<typename ValueType>
class Trie{
public:
    Trie();
    ~Trie();
    void reset();
    void insert(const std::string& key, const ValueType& value);
    std::vector<ValueType> find(const std::string& key, bool exactMatchOnly) const;
    
    // C++11 syntax for preventing copying and assignment
    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;
private:
    struct Child{
        Node* n;
        char label;
    };
    struct Node{
        std::vector<ValueType> values;
        std::vector<char>      labels;
        std::vector<Node*>     children;
    };
    void destroyAll(Node* n);
    void findMatch(Node* n, const std::string& key, std::vector<ValueType>& searchResult) const;
    Node* getChild(Node* n, const char& ch) const;
    Node* root;
};

template<typename ValueType>
Trie<ValueType>::Trie(){
    root = new Node;
}

template<typename ValueType>
Trie<ValueType>::~Trie(){
    destroyAll(root);
}

template<typename ValueType>
void Trie<ValueType>::reset(){
    destroyAll(root);
    root = new Node;
}

template<typename ValueType>
void Trie<ValueType>::insert(const std::string& key, const ValueType& value){
    Node* n = root;
    for(int i=0; i<key.size(); i++){
        
        char ch = key[i];
        bool foundChar = false;
        
        // Follow path if it exists=
        for(int j=0; j<n->children.size(); j++){
            if(ch == n->labels[j]){
                foundChar = true;
                n = n->children[j];
                break;
            }
        }
        
        if(!foundChar){
            (n->labels).push_back(ch);
            (n->children).push_back(new Node);
            n = n->children.back();
        }
        
        if(i == key.size()-1){
            (n->values).push_back(value);
        }
    }
}

template<typename ValueType>
std::vector<ValueType> Trie<ValueType>::find(const std::string& key, bool exactMatchOnly) const{
    std::vector<ValueType> searchResult;
    if(!exactMatchOnly){
        std::string replacedChar = key.substr(1);
        Node* n = getChild(root, key[0]);
        for(int i=1; i<key.size(); i++){
            if(n==nullptr) break;
            for(int j=0; j<n->children.size(); j++){
                replacedChar[0] = n->labels[j];
                if(replacedChar == key.substr(i)) continue;
                findMatch(n, replacedChar, searchResult);
            }
            n = getChild(n, key[i]);
            replacedChar = key.substr(1+i);
        }
    }
    findMatch(root, key, searchResult);
    return searchResult;
}

template<typename ValueType>
void Trie<ValueType>::destroyAll(Node* n){
    if(n == nullptr) return;
    for (int i = 0; i < n->children.size(); i++){
        destroyAll(n->children[i]);
    }
    if (n != nullptr)
        delete n;
}

// RUNS IN O(LC + V)
template<typename ValueType>
void Trie<ValueType>::findMatch(Node* n, const std::string& key, std::vector<ValueType>& searchResult) const{
    for(int i=0; i<key.size(); i++){
        if(n == nullptr) return;
        Node* x = getChild(n, key[i]);
        if(x == nullptr) return;
        if(i == key.size()-1){
            for(int j=0; j<x->values.size(); j++){
                searchResult.push_back(x->values[j]);
            }
            return;
        }
        n = x;
    }
}

// RUNS IN O(C)
template<typename ValueType>
typename Trie<ValueType>::Node* Trie<ValueType>::getChild(Node* n, const char& ch) const{
    for(int i=0; i<n->children.size(); i++){
        if(n->labels[i] == ch){
            return n->children[i];
        }
    }
    return nullptr;
}

#endif // TRIE_INCLUDED
