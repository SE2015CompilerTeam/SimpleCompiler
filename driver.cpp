#include "driver.h"
using namespace std;

Node* Node::createNode(int num, Node* nodes[]){
	Node* root = NULL;
	if (num > 0 && nodes[0] != NULL){
		root = nodes[0];
		for (int i = 1; i < num; i++){
			if (nodes[i] == NULL) continue;
			root->addChildren(nodes[i]);
		}
	}
	return root;
}
Node* Node::createNode(int num, ...){
	va_list argp;
	Node* node = NULL;
	Node* root = NULL;
	va_start(argp, num);
	for (int i = 0; i < num; i++){
		node = va_arg(argp, Node*);
		if (node == NULL) continue;
		if (i == 0)
			root = node;
		else{
			root->addChildren(node);
		}
	}
	va_end(argp);
	return root;
}

Node* Node::createNode(Node* root, Node* node){
	root->addChildren(node);
	return root;
}

void Node::addBrother(Node *bro){
	Node *cur = this;
	while (cur->brother)
		cur = cur->brother;
	Node *b = new Node(*bro);
	cur->brother = b;
}

void Node::addChildren(Node *child){
	if (!children)
		children = new Node();
	this->children->addBrother(child);
}
void A(Node *node){
	if (node->getNodeType() == Node_Type::node_value){
	}
}
void SymbolMap::insert(string name, Symbol* sym){
	this->Map.insert(pair<string, Symbol>(name, *sym));
}
Symbol* SymbolMap::find(string name){
	map<string,Symbol>::iterator it = Map.find(name);
	if (it == Map.end())
		return nullptr;
	else
		return &(it->second);
}

Symbol* SymbolTable::find(string name){
	Symbol* symbol;
	for (deque<SymbolMap>::iterator it = MapStack.begin(); it != MapStack.end(); it++){
		symbol = it->find(name);
		if (symbol != nullptr)
			return symbol;
	}
	return nullptr;
}