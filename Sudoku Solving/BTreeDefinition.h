#pragma once
#include "BTree.h"

template <class T>
Entry<T>::Entry() {
	rightPtr = nullptr;
}

template <class T>
Entry<T>::Entry(const T& _x, const T& _y) : x(_x), y(_y) {
	rightPtr = nullptr;
}

template <class T>
Entry<T>::~Entry() {
	delete rightPtr;
	rightPtr = nullptr;
}

template <class U>
Node<U>::Node() {
	firstPtr = nullptr;
	numEntries = 0;
	for (int i = 0; i < 6; i++)
		entries.push_back(nullptr);
}

template <class U>
Node<U>::~Node() {
	if (firstPtr != nullptr) {
		Clear(firstPtr);
	}
	firstPtr = nullptr;
	while (numEntries > 0) {
		if (entries[numEntries]->rightPtr != nullptr)
			Clear(entries[numEntries]->rightPtr);
		delete entries[numEntries];
		entries[numEntries] = nullptr;
		numEntries--;
	}
}

template <class U>
void Node<U>::Clear(Node*& root) {  // rewrite the Clear function wrong recursive call
	Node<U>* ptr = root->firstPtr;
	if (ptr != nullptr) {
		Clear(ptr);
	}
	root->firstPtr = nullptr;
	while (root->numEntries > 0) {
		if (root->entries[root->numEntries]->rightPtr != nullptr)
			Clear(root->entries[root->numEntries]->rightPtr);
		delete root->entries[root->numEntries];
		root->entries[root->numEntries] = nullptr;
		root->numEntries--;
	}
	delete root;
	root = nullptr;
}

template <class U>
Node<U>* Node<U>::BTreeInsert(Node<U>* root, U _x, U _y) {
	Entry<U>* upEntry = new Entry<U>();
	bool taller = insertNode(root, _x, _y, upEntry);
	if (taller == true) {
		Node* newPtr = new Node();
		newPtr->entries[1] = upEntry;
		newPtr->firstPtr = root;
		newPtr->numEntries = 1;
		root = newPtr;
	}
	return root;
}

template <class U>
bool Node<U>::insertNode(Node<U>* node, U _x, U _y, Entry<U>*& upEntry) {
	bool taller;
	if (node == nullptr) {
		upEntry->x = _x;
		upEntry->y = _y;
		upEntry->rightPtr = nullptr;
		taller = true;
	}
	else {
		int entryNdx = searchNode(node, _x); // data is key now
		Node<U>* subTree = nullptr;
		if (entryNdx > 0)
			subTree = node->entries[entryNdx]->rightPtr;
		else
			subTree = node->firstPtr;
		taller = insertNode(subTree, _x, _y, upEntry);
		if (taller == true)
			if (node->numEntries == 4) {
				upEntry = splitNode(node, entryNdx, upEntry);
				taller = true;
			}
			else {
				insertEntry(node, entryNdx, upEntry);
				taller = false;
				node->numEntries++;
			}
	}
	return taller;
}

template <class U>
int Node<U>::searchNode(Node<U>* nodePtr, U target) {
	int walker;
	if (target <= nodePtr->entries[1]->x)
		walker = 0;
	else {
		walker = nodePtr->numEntries;
		while (target <= nodePtr->entries[walker]->x) {
			walker = walker - 1;
		}
	}
	return walker;
}

template <class U>
int Node<U>::searchNode(U target) {
	int walker;
	if (target <= entries[1]->x)
		walker = 0;
	else {
		walker = numEntries;
		while (target <= entries[walker]->x) {
			walker = walker - 1;
		}
	}
	return walker;
}

template <class U>
Entry<U>* Node<U>::splitNode(Node<U>* node, int entryNdx, Entry<U>*& upEntry) {
	int minEntries = 3 / 2 - 1;
	Entry<U>* newEntry = upEntry;
	Node<U>* rightPtr = new Node<U>();
	int fromNdx, toNdx;
	insertEntry(node, entryNdx, newEntry);
	node->numEntries++;
	fromNdx = 4;
	toNdx = 1;
	while (fromNdx <= node->numEntries) {
		rightPtr->entries[toNdx] = node->entries[fromNdx];
		fromNdx++, toNdx++;
		rightPtr->numEntries++;
	}
	node->numEntries = node->numEntries - rightPtr->numEntries;
	int medianNdx = node->numEntries;
	rightPtr->firstPtr = node->entries[medianNdx]->rightPtr;
	node->entries[medianNdx]->rightPtr = rightPtr;
	upEntry = node->entries[medianNdx];
	node->numEntries--;
	return upEntry;
}

template <class U>
void Node<U>::insertEntry(Node<U>* node, int entryNdx, Entry<U>*& newEntry) {
	int shifter = node->numEntries + 1;
	while (shifter > entryNdx + 1) {
		node->entries[shifter] = node->entries[shifter - 1];
		shifter--;
	}
	node->entries[shifter] = newEntry;
}

template <class U>
void Node<U>::printTree(Node<U>* node) {
	printTree(node, node);
}

template <class U>
void Node<U>::printTree(Node<U>* node, Node<U>* root) {
	int count = 1;
	if (node == nullptr)
		return;
	if (node != root)
		std::cout << std::endl;
	while (count <= node->numEntries) {
		std::cout << "(" << node->entries[count]->x << "." << node->entries[count]->y << ") ";
		if (count != node->numEntries)
			std::cout << " ";
		count++;
	}
	printTree(node->firstPtr, node);
	if (node != root)
		std::cout << "-";
	count = 1;
	while (count <= node->numEntries) {
		printTree(node->entries[count]->rightPtr);
		std::cout << "-";
		count++;
	}
}

template <class U>
Node<U>* Node<U>::deleteEntry(Node<U>* node, Node<U>* root, U data, bool& check) {
	if (node == nullptr) { // didn't find the entry you want to delete
		check = 0;
	}
	else {
		int ptrPos = searchNode(node, data);
		if (ptrPos == 0)
			node = deleteEntry(node->firstPtr, root, data, check);
		else
			node = deleteEntry(node->entries[ptrPos]->rightPtr, root, data, check);
		bool cLeaf = checkLeaf(node);
		if (data == node->entries[ptrPos + 1]->data) { // found the entry you want to delete
			if (cLeaf == true) { // node in the leaf
				for (int i = ptrPos + 1; i < numEntries; i++) // delete the entry
					node->entries[i] = node->entries[i + 1];
				node->numEntries--; // reduce the total entries of node
				return node;
			}
		}
		int minEntry;
		Node<U>* tempNode = nullptr;
		Node<U>* leftSibL = nullptr;
		Node<U>* rightSibl = nullptr;
		if (ptrPos == 0) {
			tempNode = node->firstPtr;
			if (node != root)
				rightSibl = node->entries[1]->rightPtr;
		}
		else {
			if (ptrPos == 1)
				leftSibL = node->firstPtr;
			else
				leftSibL = node->entries[ptrPos - 1]->rightPtr;
			if (ptrPos != node->numEntries)
				rightSibl = node->entries[ptrPos + 1]->rightPtr;
			tempNode = node->entries[ptrPos]->rightPtr;
		}
		// case 1 and case 3 pretty much the same, just replace the nullptr with the ptr the node 's holding
		if (tempNode->numEntries == 0) {
			if (leftSibL != nullptr && leftSibL->numEntries > 1) {
				tempNode->entries[1]->x = node->entries[ptrPos]->x;
				tempNode->entries[1]->y = node->entries[ptrPos]->y;
				node->entries[ptrPos]->x = leftSibL->entries[leftSibL->numEntries]->x;
				node->entries[ptrPos]->y = leftSibL->entries[leftSibL->numEntries]->y;
				delete leftSibL->entries[leftSibL->numEntries];
				leftSibL->entries[leftSibL->numEntries] == nullptr;
				leftSibL->numEntries--;
				tempNode->numEntries++;
			}
			else if (rightSibl != nullptr && rightSibl->numEntries > 1) {
				tempNode->entries[1]->x = node->entries[ptrPos]->x;
				tempNode->entries[1]->y = node->entries[ptrPos]->y;
				node->entries[ptrPos]->x = rightSibl->entries[1]->x;
				node->entries[ptrPos]->y = rightSibl->entries[1]->y;
				for (int i = 2; i < rightSibl->numEntries; i++)
					rightSibl->entries[i - 1] = rightSibl->entries[i];
				delete rightSibl->entries[rightSibl->numEntries];
				rightSibl->entries[rightSibl->numEntries] == nullptr;
				rightSibl->numEntries--;
				tempNode->numEntries++;
			}
			else if (leftSibL->numEntries <= 1 && rightSibl->numEntries <= 1) {
				leftSibL->numEntries++;
				leftSibL->entries[leftSibL->numEntries]->x = node->entries[1]->x;
				leftSibL->entries[leftSibL->numEntries]->y = node->entries[1]->y;
				for (int i = 2; i < node->numEntries; i++)
					node->entries[i - 1] = node->entries[i];
				delete node->entries[node->numEntries];
				node->entries[node->numEntries] == nullptr;
				node->numEntries--;
			}
			else if (leftSibL->numEntries == 1 && rightSibl->numEntries == 1
				&& cLeaf == false) {
				Node<U>* pDel = node->entries[ptrPos];
				leftSibL->numEntries++;
				leftSibL->entries[leftSibL->numEntries] = rightSibl;
				for (int i = ptrPos; i < node->numEntries; i++)
					node->entries[i] = node->entries[i + 1];
				node->entries[numEntries] = nullptr;
				node->numEntries--;
				delete pDel;
				pDel = nullptr;
			}
		}
	}
	return node;
}

template <class U>
void Node<U>::deleteEntry(U data, bool& check) {
	int idx = searchNode(data); // search the position to find the delete node

	// the key to be removed is present in this node
	if (idx < 4 && this->entries[idx + 1]->data == data) {
		// if the node is leaf
		if (checkLeaf() == true)
			removeFromLeaf(idx + 1);
		else
			// if the node is internal node
			removeFromNonLeaf(idx + 1);
		check = true;
	}
	else {
		// if this node is leaf node then the key does not exist in tree
		if (checkLeaf() == true) {
			check = false;
			return;
		}
		// if the child where the key is supposed to exist has equal to
		// t(1) key(t means minimum entry of node)
		// We fill that child
		if (idx == 0 && firstPtr->numEntries == 1)
			fillNode(0);

		if (idx > 0 && entries[idx]->rightPtr->numEntries == 1)
			fillNode(idx);

		// if idx = 0, call the recursive on firstPtr
		// else call the recursive on entries[i]->rightPtr to delete
		// the Entry with that data 
		if (idx == 0)
			firstPtr->deleteEntry(data, check);
		else
			entries[idx]->rightPtr->deleteEntry(data, check);
	}
	return;
}

template <class U>
bool Node<U>::checkLeaf() {
	if (firstPtr != nullptr)
		return 0;
	for (int i = 1; i <= numEntries; i++)
		if (entries[i]->rightPtr != nullptr)
			return 0;
	return 1;
}

template <class U>
void Node<U>::removeFromLeaf(int idx) {
	for (int i = idx; i <= numEntries; i++)
		entries[i] = entries[i + 1];
	numEntries--;
}

template <class U>
void Node<U>::removeFromNonLeaf(int idx) {
	U k = entries[idx]->x;
	U l = entries[idx]->y;
	bool check = true;

	U predX, predY, succX, succY;

	// if the child that precedes k has atleast t keys,
	// find the predecessor 'pred' of k in the subtree rooted at
	// Replace k by pred. Recursively delete predecessor
	if (idx == 1 && firstPtr->numEntries >= 1) {
		getPred(1, predX, predY);
		entries[idx]->x = predX;
		entriex[idx]->y = predY;
		firstPtr->deleteEntry(predX, check);
	}
	else if (idx > 1 && entries[idx]->rightPtr->numEntries >= 1) {
		getPred(idx, predX, predY);
		entries[idx]->x = predX;
		entries[idx]->y = predY;
		entries[idx - 1]->rightPtr->deleteEntry(pred, check);
	}

	// if the predecessor has less than t keys, examine entries[idx]->rightPtr
	// if its has atleast t keys, find the successor 'succ' of k in 
	// the subtree rooted at entries[idx]->rightPtr
	// replace k by succ. recursively delete succ in entries[idx]->rightPtr
	else if (entries[idx]->rightPtr->numEntries >= 1) {
		U succ = getSucc(idx);
		entries[idx]->data = succ;
		entries[idx]->rightPtr->deleteEntry(succ, check);
	}

	// if both the predessor and successor has exactly t key, merge k 
	// and all of successor into predessor
	// now predessor contain 2 key
	// delete entries[idx]->rightPtr and recursively delete k from predecessor
	else {
		bool check = true;
		merge(idx - 1);
		if (idx == 1)
			firstPtr->deleteEntry(k, check);
		else
			entries[idx - 1]->rightPtr->deleteEntry(k, check);
	}
}

template <class U>
U Node<U>::getPred(int idx) {
	// Keep moving to the right most node until we reach a leaf
	Node<U>* cur = nullptr;
	if (idx == 1)
		cur = firstPtr;
	else
		cur = entries[idx - 1]->rightPtr;
	while (cur->checkLeaf() != true)
		cur = cur->entries[cur->numEntries]->rightPtr;

	return cur->entries[cur->numEntries]->data;
}

template <class U>
U Node<U>::getSucc(int idx) {
	Node<U>* cur = entries[idx]->rightPtr;
	while (cur->checkLeaf() != true)
		cur = cur->firstPtr;
	return cur->entries[1]->data;
}

template <class U>
void Node<U>::fillNode(int idx) {
	// If the previous child has more than 1 key, borrow a key from that child
	if (idx == 1 && firstPtr->numEntries > 1)
		borrowFromPrev(idx);
	else if (idx != 0 && entries[idx - 1]->rightPtr->numEntries > 1)
		borrowFromPrev(idx);

	// If the next child has more than 1 keys, borrow a key from that child
	else if (idx == 0 && entries[idx + 1]->rightPtr->numEntries > 1)
		borrowFromNext(idx);

	// if both the previous child and the next child has equal to 1 entry
	// if idx is the last entry, merge it with its previous sibling
	// otherwise merge it with its next sibling
	else {
		merge(idx);
	}
}

template <class U>
void Node<U>::borrowFromPrev(int idx) {
	Node<U>* child = entries[idx]->rightPtr;
	Node<U>* sibling = nullptr;
	if (idx == 1)
		sibling = firstPtr;
	else
		sibling = entries[idx - 1]->rightPtr;

	// the last key from sibling goes up to the parent and key before the idx
	// key is inserted as the first key in child node.
	// Thus, the sibling loses one key and child gains one key

	// moving all key in child one step ahead
	for (int i = child->numEntries + 1; i > 1; i--)
		child->entries[i] = child->entries[i - 1];

	// Setting child's first entry equal to the entry before entry idx from
	// the current node

	child->entries[1]->data = entries[idx - 1]->data;

	// Sibling's last entry rightPtr is now the rightPtr of child first entry
	child->entries[1]->rightPtr = sibling->entries[sibling->numEntries]->rightPtr;
	sibling->entries[sibling->numEntries]->rightPtr = nullptr;

	// Moving the key from the sibling to the parent
	// This reduces the  number of keys in the sibling
	entries[idx - 1]->data = sibling->entries[sibling->numEntries]->data;

	child->numEntries++;
	sibling->numEntries--;
}

template <class U>
void Node<U>::borrowFromNext(int idx) {
	Node<U>* child = entries[idx]->rightPtr;
	Node<U>* sibling = entries[idx + 1]->rightPtr;

	// entries[idx]->data is inserted as the last data in entries[idx]->rightPtr
	child->entries[numEntries + 1]->data = entries[idx]->data;

	// Sibling's first entry rightPtr is now the rightPtr of the last entries
	// of child
	child->entries[numEntries + 1]->rightPtr = sibling->entries[1]->rightPtr;
	sibling->entries[1]->rightPtr = nullptr;

	// the first entry data from sibling is inserted into entries[idx]->data
	entries[idx]->data = sibling->entries[1]->data;

	// Moving all keys in sibling one step behind
	for (int i = 1; i < sibling->numEntries; i++)
		sibling->entries[i] = sibling->entries[i - 1];
	sibling->entries[sibling->numEntries] = nullptr;

	// Increasing and decreasing the entry count of child and sibling
	// respectively
	child->numEntries++;
	sibling->numEntries--;
}

template <class U>
void Node<U>::merge(int idx) {
	Node<U>* child = entries[idx]->rightPtr;
	Node<U>* sibling = nullptr;

	if (idx == 1)
		sibling = firstPtr;
	else
		sibling = entries[idx - 1]->rightPtr;

	// Copying the entry from sibling to child at the end
	child->entries[2] = sibling->entries[1];
	sibling->entries[1] = nullptr;

	// delete the entries[idx]
	if (idx == 1)
		delete entries[idx];
	else
		delete entries[idx - 1];

	// Moving all data after idx in the current node one step before -
	// to fill the gap created by moving entry at node->entries[idx]
	// to child
	for (int i = idx; i < numEntries; i++)
		entries[idx] = entries[idx + 1];

	// Updating the key count of child and the current node
	child->numEntries++;
	numEntries--;
}