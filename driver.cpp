#include "driver.h"
using namespace std;



SymbolMap idMap;
//enum Value_Type { type_int = 10, type_char, type_double, type_string/* char* */, type_void, type_bool };
//记录当前声明语句的变量类型
Value_Type type;



void checkNodeType(Node* n, Node_Type type) {
	if (n->getNodeType() != type)
		throw new exception("节点类型不匹配（Node => %d）", type);
}

Node* Node::createNode(int num, Node* nodes[]) {
	Node* root = NULL;
	if (num > 0 && nodes[0] != NULL) {
		root = nodes[0];
		for (int i = 1; i < num; i++) {
			if (nodes[i] == NULL) continue;
			root->addChildren(nodes[i]);
		}
	}
	return root;
}

Node* Node::createNode(int num, ...) {
	va_list argp;
	Node* node = NULL;
	Node* root = NULL;
	va_start(argp, num);
	for (int i = 0; i < num; i++) {
		node = va_arg(argp, Node*);
		if (node == NULL) continue;
		if (i == 0)
			root = node;
		else {
			root->addChildren(node);
		}
	}
	va_end(argp);
	return root;
}

Node* Node::createNode(Node* root, Node* node) {
	root->addChildren(node);
	return root;
}

void Node::addBrother(Node *bro) {
	Node *cur = this;
	while (cur->brother != NULL)
		cur = cur->brother;
	cur->brother = bro;
}

void Node::addChildren(Node *child) {
	if (this->children == NULL)
		this->children = child;
	else
		this->children->addBrother(child);
}

void SymbolMap::insert(string name, IDNode* sym) {
	this->Map.insert(pair<string, IDNode>(name, *sym));
}

IDNode* SymbolMap::find(string name) {
	map<string, IDNode>::iterator it = Map.find(name);
	if (it == Map.end())
		return nullptr;
	else
		return &(it->second);
}

IDNode* SymbolTable::find(string name) {
	IDNode* symbol;
	for (deque<SymbolMap>::iterator it = MapStack.begin(); it != MapStack.end(); it++) {
		symbol = it->find(name);
		if (symbol != nullptr)
			return symbol;
	}
	return nullptr;
}

void Node::printNode(Node *n) {
	switch (n->type)
	{
	case Node_Type::node_id:
		IDNode::printNode(n);
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

void ValueNode::printNode(Node* n) {
	if (n->getNodeType() != Node_Type::node_value) {//  && n->getNodeType() != Node_Type::node_id
		throw new exception("节点类型不匹配（Node => ValueNode） ");
	}
	ValueNode *node = (ValueNode*)n;
	switch (node->value_type)
	{
	case Value_Type::type_char:
		printf("Value    %c\n", ((CharNode*)node)->value);
		break;
	case Value_Type::type_int:
		printf("Value    %d\n", ((IntNode*)node)->value);
		break;
	case Value_Type::type_double:
		printf("Value    %f\n", ((DoubleNode*)node)->value);
		break;
	case Value_Type::type_string:
		printf("Value    %s\n", ((StringNode*)node)->value.c_str());
		break;
	default:
		break;
	}
}

void IDNode::printNode(Node* n) {
	try {
		checkNodeType(n, Node_Type::node_id);
		printf("VAR    %s\n", n->name);
	}
	catch (exception e) {
		printf(e.what());
	}
}

void TypeNode::printNode(Node *n) {
	try {
		char* c = "Unknown, maybe is array or pointer..fuck me..";
		TypeNode* node = (TypeNode*)n;
		switch (node->type_type)
		{
		case Value_Type::type_char:
			printf("TYPE    CHAR\n");
			break;
		case Value_Type::type_int:
			printf("TYPE    INT\n");
			break;
		case Value_Type::type_double:
			printf("TYPE    DOUBLE\n");
			break;
		case Value_Type::type_string:
			printf("TYPE    STRING\n");
			break;
			//case "pointer & array":
		default:
			printf("TYPE    Un F**king known. Fuck Me..\n");
			break;
		}
	}
	catch (exception e) {
		printf(e.what());
	}
}

void Node::printTree(Node* node, int level) {
	if (node == NULL)
		return;
	int a;
	(a) = 3;
	for (int i = 0; i < level; i++)
		printf("    ");
	Node::printNode(node);
	Node *child = node->getChildren();
	while (child != NULL)
	{
		Node::printTree(child, level + 1);
		child = child->getBrother();
	}
}

char* ExprNode::calculate(const char* name, ValueNode* n1, ValueNode* n2 = nullptr) {
	/*char* c = new char[LEN];
	if (n2 == nullptr){
		// 单目运算符
	}
	else{
		int type = n1->getValueType() & n2->getValueType(); // 两个操作数的话 不在乎谁是左值
		if (name == "*"){
			// 列出所有支持乘法的类型
			if (type == Value_Type::type_int & Value_Type::type_int
				|| type == Value_Type::type_int & Value_Type::type_char){
				itoa(atoi(n1->getValue().c_str()) * atoi(n2->getValue().c_str()), c, 10);
				return c;
			}
			//else if (type == )
		}
		return "test value";
	}*/
	return "test value";
}


//用index区分不同的作用域下的表,待扩展
void addID(char* chrName, IDNode* sym, int index) {
	string strNmae = chrName;
	if (idMap.find(strNmae) == nullptr) {
		idMap.insert(strNmae, sym);
	}
}


void setTypes(Value_Type tp) {
	type = tp;
}


void setIDType(IDNode* node) {
	node->setValueType(type);
}

//enum Node_Type { node_norm, node_value, node_id, node_opt, node_type };
//遍历结点获取IDNode 存的是ID的名字 应该改成IDNode*才对 调用方法如下
void getIDs(vector<char*> ids, Node* now) {
	if (now == NULL)
		return;
	Node* child = now->getChildren();
	while (child != NULL) {
		if (child->getNodeType() == node_id) {
			ids.push_back(child->getName());
		}
		getIDs(ids, child);
		child = child->getBrother();
	}
}

//判断条件不知道有没有问题，未定义不知道是不是NULL
bool isRedefined(IDNode* node) {
	if (node->getValueType() == NULL) {
		return false;
	}
	else {
		return true;
	}
}
