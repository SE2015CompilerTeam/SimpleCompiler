#include "driver.h"
using namespace std;

void walkTree(Node* node, int level){

}

void checkNodeType(Node* n, Node_Type type){
	if (n->getNodeType() != type)
		throw new exception("节点类型不匹配（Node => %d）", type);
}

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
void SymbolMap::insert(string name, IDNode* sym){
	this->Map.insert(pair<string, IDNode>(name, *sym));
}
IDNode* SymbolMap::find(string name){
	map<string,IDNode>::iterator it = Map.find(name);
	if (it == Map.end())
		return nullptr;
	else
		return &(it->second);
}

IDNode* SymbolTable::find(string name){
	IDNode* symbol;
	for (deque<SymbolMap>::iterator it = MapStack.begin(); it != MapStack.end(); it++){
		symbol = it->find(name);
		if (symbol != nullptr)
			return symbol;
	}
	return nullptr;
}

void Node::printNode(Node *n){
	switch (n->type)
	{
	case Node_Type::node_id:
		break;
	case Node_Type::node_value:
		ValueNode::printNode(n);
		break;
	case Node_Type::node_type:
		TypeNode::printNode(n);
		break;
	case Node_Type::node_opt:
		printf("OPT    %s\n", n->name);
		break;
	case Node_Type::node_norm:
		printf("%s\n", n->name);
		break;
	default:
		break;
	}
}

void ValueNode::printNode(Node* n){
	if (n->getNodeType() != Node_Type::node_value){//  && n->getNodeType() != Node_Type::node_id
		throw new exception("节点类型不匹配（Node => ValueNode） ");
	}
	ValueNode *node = (ValueNode*)n;
	switch (node->value_type)
	{
	case Value_Type::type_char:
		printf("Value %c\n", ((CharNode*)node)->value);
		break;
	case Value_Type::type_int:
		printf("Value %d\n", ((IntNode*)node)->value);
		break;
	case Value_Type::type_double:
		printf("Value %f\n", ((DoubleNode*)node)->value);
		break;
	case Value_Type::type_string:
		printf("Value %s\n", ((StringNode*)node)->value);
		break;
	default:
		break;
	}
}
void IDNode::printNode(Node* n){
	try{
		checkNodeType(n, Node_Type::node_id);
		printf("VAR %s", n->name);
	}
	catch (exception e){
		printf(e.what());
	}
}

void TypeNode::printNode(Node *n){
	try{
		char* c = "Unknown, maybe is array or pointer..fuck me..";
		TypeNode* node = (TypeNode*)n;
		switch (node->type_type)
		{
		case Value_Type::type_char:
			c = "CHAR";
			break;
		case Value_Type::type_int:
			c = "INT";
			break;
		case Value_Type::type_double:
			c = "DOUBLE";
			break;
		case Value_Type::type_string:
			c = "STRING";
			break;
			//case "pointer & array":
		default:
			break;
		}
		printf("TYPE %s\n", c);
	}
	catch (exception e){
		printf(e.what());
	}
	
}