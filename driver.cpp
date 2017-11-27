#include "driver.h"
#include <iostream>
using namespace std;



SymbolMap idMap;
//enum Value_Type { type_int = 10, type_char, type_double, type_string/* char* */, type_void, type_bool };
//��¼��ǰ�������ı�������
Value_Type type;
bool defining = false;



void checkNodeType(Node* n, Node_Type type) {
	if (n == nullptr) {
		cout << endl << "�������ǿյİ��������������ν���ͶԲ�����" << endl << endl;
	}
	if (n->getNodeType() != type)
		cout << endl << "���Ͳ�ƥ�䰡�⻹��ë��" << endl << endl;
		//throw new exception("�ڵ����Ͳ�ƥ�䣨Node => %d��", type);
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
		throw new exception("�ڵ����Ͳ�ƥ�䣨Node => ValueNode�� ");
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
		if (n = nullptr) { //�����ǰ�ǿս��Ͳ���ӡ
			return;
		}
		checkNodeType(n, Node_Type::node_array); //����ط���������arraynode���󱨲�ƥ��
		ValueNode* nodeTmp = (ValueNode*)n;
		ArrayNode* node = (ArrayNode*)nodeTmp; //ת����ArrayNode���ܻ�ȡά����ÿά�ռ��С
	    cout << "ARRAY    " << node->getDimension() << "  ";//��ӡά��
		vector<int> vector = node->getSize();
		for (int i = 0; vector.size(); i++) {//��ӡÿά�ռ��С
			cout << vector[i] << "  ";
		}
		Node* child = node->getChildren(); //��ȡ�ӽڵ�
		while (child != nullptr) {
			Node::printNode(child);
			child = child->getBrother(); //��ȡ�ֵܽ��
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
		// ��Ŀ�����
	}
	else{
		int type = n1->getValueType() & n2->getValueType(); // �����������Ļ� ���ں�˭����ֵ
		if (name == "*"){
			// �г�����֧�ֳ˷�������
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


//��index���ֲ�ͬ���������µı�,����չ
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
//����ط�����һ�����⣬����int a = b;�Ļ����bҲ��ȡ�����Ҳ²���ֵȫ����child��㣬��������brother��㣬����������Ļ��Ϳ���ֻ��ȡchild
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

//�ж����������⣬���õ�Ĭ����type_int����������Ȼ������
bool isRedefined(IDNode* node) {
	if (node->getValueType() == type_int) {
		return false;
	}
	else {
		return true;
	}
}

//�ж����������⣬���õ�Ĭ����type_int����������Ȼ������
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
	if (child == nullptr) { //���û�к��ӽ��
		cout << "����Խ��" << endl;
		return nullptr;
	}
	else {
		if (i == 0) { //��������±�Ϊ0
			return child;
		}
		else {
			for (int k = 0; k < i; i++) {
				child = child->getBrother(); //��ȡ�ֵܽ��
				if (child == nullptr) {
					cout << "����Խ��" << endl;
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