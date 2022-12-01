#pragma once
#include <iostream>
#include <vector>
#ifndef BTree_H
#define BTree_H
// Entry
template <class U>
class Node;

template <class T>
class Entry {
private:
	template <class U>
	friend class Node;
	T x; // using data as key
	T y;
	Node<T>* rightPtr;
public:
	Entry();
	Entry(const T&, const T&);
	~Entry();
};

template <class U>
class Node {
private:
	Node* firstPtr;
	int numEntries;
	std::vector<Entry<U>*> entries;
	void printTree(Node*, Node*);
public:
	Node();
	~Node();
	void Clear(Node*&);
	Node* BTreeInsert(Node*, U, U);
	bool insertNode(Node*, U, U, Entry<U>*&);
	int searchNode(Node*, U);
	int searchNode(U);
	Entry<U>* splitNode(Node*, int, Entry<U>*&);
	void insertEntry(Node*, int, Entry<U>*&);
	void printTree(Node*);
	Node* deleteEntry(Node*, Node*, U, bool&);
	void deleteEntry(U, bool&);
	bool checkLeaf();
	void removeFromLeaf(int);
	void removeFromNonLeaf(int);
	void getPred(int, U& x, U& y);
	void getSucc(int, U& x, U& y);
	void fillNode(int);
	void borrowFromPrev(int);
	void borrowFromNext(int);
	void merge(int);
};
#endif // !BTree_H
