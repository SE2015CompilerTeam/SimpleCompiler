#include "driver.h"
#include <iostream>
using namespace std;



SymbolMap idMap;
//enum Value_Type { type_int = 10, type_char, type_double, type_string/* char* */, type_void, type_bool };
//记录当前声明语句的变量类型
Value_Type type;
bool defining = false;



void checkNodeType(Node* n, Node_Type type) {
	if (n == nullptr) {
		cout << endl << "这个结点是空的啊，那他妈就无所谓类型对不对了" << endl << endl;
	}
	if (n->getNodeType() != type)
		cout << endl << "类型不匹配啊这还玩毛啊" << endl << endl;
		//throw new exception("节点类型不匹配（Node => %d）", type);
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
	if (this->find(name) != nullptr) {
		return;
	}
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
	case Node_Type::node_array:
		ArrayNode::printNode(n);
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

void ArrayNode::printNode(Node* n) {
	try {
		if (n = nullptr) { //如果当前是空结点就不打印
			return;
		}
		checkNodeType(n, Node_Type::node_array); //这个地方如果最后不是arraynode会误报不匹配
		ValueNode* nodeTmp = (ValueNode*)n;
		ArrayNode* node = (ArrayNode*)nodeTmp; //转换成ArrayNode才能获取维数和每维空间大小
	    cout << "ARRAY    " << node->getDimension() << "  ";//打印维数
		vector<int> vector = node->getSize();
		for (int i = 0; vector.size(); i++) {//打印每维空间大小
			cout << vector[i] << "  ";
		}
		Node* child = node->getChildren(); //获取子节点
		while (child != nullptr) {
			Node::printNode(child);
			child = child->getBrother(); //获取兄弟结点
		}
	}
	catch (exception e) {
		Node::printNode(n);
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
		//case Value_Type::type_array:
		//	printf("TYPE    ARRAY\n");
		//	break;
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


void setIDType(IDNode* node){
	node->setValueType(type);
}

//enum Node_Type { node_norm, node_value, node_id, node_opt, node_type };
//这个地方还有一个问题，就是int a = b;的话会把b也获取到，我猜测左值全都是child结点，不可能是brother结点，如果是这样的话就可以只获取child
void getIDs(vector<Node*> ids, Node* now) {
	if (now == NULL)
		return;
	Node* child = now->getChildren();
	while (child != NULL) {
		if (child->getNodeType() == node_id) {
			ids.push_back(child);
		}
		getIDs(ids, child);
		child = child->getBrother();
	}
}

//判断条件有问题，设置的默认是type_int，但是这显然不合理啊
bool isRedefined(IDNode* node) {
	if (node->getValueType() == type_int) {
		return false;
	}
	else {
		return true;
	}
}

//判断条件有问题，设置的默认是type_int，但是这显然不合理啊
bool isUndefined(IDNode* node) {
	if (node->getValueType() == type_int) {
		return true;
	}
	else {
		return false;
	}
}

int ArrayNode::getDimension() {
	return this->dimension;
}

void ArrayNode::setDimension(int i) {
	this->dimension = i;
}

vector<int> ArrayNode::getSize() {
	return this->size;
}

void ArrayNode::addSize(int tmp) {
	this->size.push_back(tmp);
}

void ArrayNode::addCount() {
	this->count += 1;
}

Node* ArrayNode::getChild(int i) {
	Node* child = this->getChildren();
	if (child == nullptr) { //如果没有孩子结点
		cout << "数组越界" << endl;
		return nullptr;
	}
	else {
		if (i == 0) { //如果返回下标为0
			return child;
		}
		else {
			for (int k = 0; k < i; i++) {
				child = child->getBrother(); //获取兄弟结点
				if (child == nullptr) {
					cout << "数组越界" << endl;
					return nullptr;
					break;
				}
			}
			return child;
		}
	}
}

void convert2Pointer(Node* n) {
	ValueNode* tmp = (ValueNode*)n;
	IDNode* node = (IDNode*)tmp;
	node->setValueType(Value_Type::type_pointer);
}

void setStatus(bool status) {
	defining = status;
}

bool isDefining() {
	return defining;
}

bool hasID(string name) {
	if (idMap.find(name) != nullptr) {
		return true;
	}
	else {
		return false;
	}
}

IDNode* getID(string name) {
	cout << name << endl;
	return idMap.find(name);
}