#ifndef __DRIVER_HPP__
#define __DRIVER_HPP__ 1

#include <deque>
#include <map>
#include <iostream>
#include <string>
#include <stdarg.h>
#include <vector>
#include <iomanip>
#define LEN 1024

enum Node_Type { node_norm, node_value, node_id, node_opt, node_type, node_array };
enum Value_Type { type_int = 10, type_char, type_double, type_float, type_string/* char* */, type_void, type_bool, type_array, type_pointer };
//vector<

//map<Value_Type, ValueVec*> TypeMap;
class Node { // �ڵ����
protected:
	Node *children = nullptr; // �������ֵ�
	Node *brother = nullptr;
	Node_Type type;
public:
	char* name;
	Node() {
		name = "";
		type = Node_Type::node_norm;
	}
	Node(char name) : Node(name, Node_Type::node_norm) { }
	Node(char* name) : Node(name, Node_Type::node_norm) { }
	Node(char name, Node_Type type) {
		this->name = new char[2];
		this->name[0] = name; this->name[1] = '\0';
		this->type = Node_Type::node_norm;
	}
	Node(char* name, Node_Type type) { 
		this->name = new char[std::strlen(name)+1];
		strcpy_s(this->name, std::strlen(name) + 1, name);
		this->type = type; 
	}
	static Node* createNode(int num, Node* nodes[]); // ����һ������
	static Node* createNode(Node* root, Node* node); // ֱ�Ӹ���������
	static Node* createNode(int num, ...);
	static void printNode(Node* node);
	static void printTree(Node *tree, int level);
	void addChildren(Node* child);
	void addBrother(Node *brother);
	char* getName() { return this->name; }
	Node* getBrother() { return this->brother; }
	Node* getChildren() { return this->children; }
	bool isLeaf() { return children == nullptr; }
	Node_Type getNodeType() {
		if (this != nullptr) { return this->type; }
		else {
			std::cout << std::endl << "�����û��nodetype ��������˿�ָ�벢���Դ�ӡ����nodetype" << std::endl << std::endl;
			return Node_Type::node_id;
		}
	}
};

class ValueNode : public Node{ // �洢Value���ͽڵ� 
protected:
	char* value;
	Value_Type value_type;
public:
	ValueNode() :Node("Value", Node_Type::node_value){
		this->value_type = Value_Type::type_int;
	}
	// ��ֵ������ ��ô�����ٴ�ֵ����
	ValueNode(char* name) :Node(name, Node_Type::node_value){
		this->value = "";
		this->value_type = Value_Type::type_int;
	}
	ValueNode(const char* value, Value_Type value_type = Value_Type::type_int) :Node("Value", Node_Type::node_value){
		this->value_type = value_type;
		this->value = new char[std::strlen(value) + 1];
		strcpy_s(this->value, std::strlen(value) + 1, value);
	}
	ValueNode(const char* value, char* name, Value_Type value_type = Value_Type::type_int) :Node(name, Node_Type::node_value){
		this->value_type = value_type;
		this->value = new char[std::strlen(value) + 1];
		strcpy_s(this->value, std::strlen(value) + 1, value);
	}
	ValueNode(const char* value, char name, Value_Type value_type = Value_Type::type_int) :Node(name, Node_Type::node_value){
		this->value_type = value_type;
		this->value = new char[std::strlen(value) + 1];
		strcpy_s(this->value, std::strlen(value) + 1, value);
	}
	void setValueType(Value_Type type){ this->value_type = type; }
	void setValue(const char* val, Value_Type type);
	Value_Type getValueType(){ return this->value_type; }
	const char* getValue(){ return this->value; }
	static void printNode(Node* n);
	static bool checkValueType(ValueNode* n1, ValueNode* n2);
	static bool checkValueType(Value_Type t1, Value_Type t2);
	static bool checkValueType(char* opt, Value_Type t1, Value_Type t2);
	static bool checkValueType(char* opt, ValueNode* t1, ValueNode* t2);
	static bool checkValid(ValueNode* node);
	static ValueNode* extractInterValue(ValueNode* n);
	static ValueNode* ErrorNode(){
		return new ValueNode("Error Node");
	}
};

class DoubleNode : public ValueNode {
public:
	double tvalue;
	DoubleNode(double value) :ValueNode(std::to_string(value).c_str(), Value_Type::type_double) { // ֻ��double������Ҫ�ٽ��ַ���תΪ������
		this->tvalue = value;
	}
	double getValue(){ return this->tvalue; }
};
class FloatNode :public ValueNode{
	float tvalue;
public:
	float getValue(){ return this->tvalue; }
	FloatNode(float val) :ValueNode(std::to_string(val).c_str(), Value_Type::type_float) { // ֻ��double������Ҫ�ٽ��ַ���תΪ������
		this->tvalue = val;
	}
};
class StringNode : public ValueNode{
	char* tvalue;
public:
	char* getValue(){ return tvalue; }
	StringNode(char name) : ValueNode(std::to_string(name).c_str(), Value_Type::type_string) {
		this->tvalue = new char[2];
		this->tvalue[0] = name; this->tvalue[1] = '\0';
	}
	StringNode(char* name) :ValueNode(name, Value_Type::type_string) {
		this->tvalue = new char[std::strlen(name) + 1];
		strcpy_s(this->tvalue, std::strlen(name) + 1, name);
	}
};
class IntNode :public ValueNode{
	int tvalue;
public:
	int getValue(){ return this->tvalue; }
	IntNode(int val) :ValueNode(std::to_string(val).c_str(), Value_Type::type_int) {
		this->tvalue = val;
	}
};
class CharNode : public ValueNode{
	char tvalue;
public:
	char getValue(){ return this->tvalue; }
	CharNode(char val) : ValueNode(std::to_string(val).c_str(), Value_Type::type_char) {
		this->tvalue = val;
	}
};

class TypeNode : public Node{
	Value_Type type_type;
public:
	TypeNode(Value_Type type = Value_Type::type_int) : Node("Type", Node_Type::node_type){
		this->type_type = type;
	}
	Value_Type getTypeType(){ return this->type_type; }
	static void printNode(Node* node);
};

class IDNode : public ValueNode{ // ID�����Ͷ���ú�Ͳ����ٱ�,�ʿ�ֱ�Ӽ̳�
	int linenum;
	ValueNode *tvalue = nullptr;
	int autoFlag = 0; // ��׺++ �ı�־λ
	void updateValue();
public:
	IDNode(char* name, Value_Type type = Value_Type::type_int, int linenum = 1) : ValueNode("", name, type){
		this->linenum = linenum;
		this->type = Node_Type::node_id;
		autoFlag = 0;
		//��ʱ�ӵ� ֮��Ҫ�����ű� TODO:
		//this->value = new IntNode(999);
	}
	int getLineNum();
	//void setAutoFlag(bool needIncre);
	ValueNode* setValue(ValueNode* n);
	void mySetValue(ValueNode* n){ // ��ʱ������ӵ�
		//this->value_type = n->getValueType();
		this->tvalue = n;
	}
	//ValueNode* setValue(char* val, Value_Type type);
	ValueNode* getValue();
	static void printNode(Node* n);
	ValueNode*  setAuto(bool isIncre, bool prefix);
	void Increment(bool incre);
};

class ExprNode : public ValueNode{
private:
	ValueNode *tvalue = nullptr; // ������ʽ��ֵ(����typeǿ��ת���ɸ�������)
	//void calculate(ValueNode* n1, ValueNode* n2);
public:
	ExprNode(char* name, ValueNode* n1, ValueNode* n2 = nullptr) :ValueNode(){ // ���ʽ����� ������1 ������2��optional��
		this->name = name;
		this->type = Node_Type::node_opt;
		setValue(ExprNode::calculate(name, n1, n2));
	}
	ExprNode(ValueNode* n) :ValueNode(*n){
		setValue(n);
	}
	ValueNode* getValue(){ return this->tvalue; }
	ValueNode* setValue(ValueNode* n);
	void setNegative();  // ��Ϊȡ������Ҫ�����½ڵ�// ��������Ƿ������Ϊ��
	void setNot();
	void autoIncre(int type = 0); // 0: ++a 1: --a 2: a++ 3: a--
	static ValueNode* calculate(const char* name, ValueNode* n1, ValueNode* n2);
	static ValueNode* calcSimpleOpt(const char* name, ValueNode* n1, ValueNode* n2); // �������������
	static ValueNode* calcBoolOpt(const char* type, ValueNode* n1, ValueNode *n2);
	static void printNode(Node* n);
};

class ArrayNode : public ValueNode {
private:
	int dimension; //���ڼ�¼�ж���ά��
	int count; //���ڵ�����¼��ǰά���ж��
	std::vector<int> size; //���ڼ�¼ÿһά�ж��
public:
	int getDimension();
	void setDimension(int i);
	std::vector<int> getSize();
	void addSize(int tmp); //�������ά��ʱ��ά�ȵĴ�С��ӽ�ȥ
	void addCount(); //��ǰά�ȿռ��һ
	static void printNode(Node* n);
	Node* getChild(int i); //��ȡ��ǰ����ĵ�i��Ԫ�أ���0��ʼ��
	ArrayNode(int tmpCount = 1) : ValueNode("", Value_Type::type_array) { // value, Value_Type
		this->type = Node_Type::node_array;
		this->count = tmpCount;
		this->dimension = dimension;
	}
};

class SymbolMap {
public:
	IDNode* find(std::string name);
	void insert(std::string name, IDNode* symbol);
private:
	std::map<std::string, IDNode> Map;
};

class SymbolTable {
public:
	IDNode* find(std::string name);
	void push(SymbolMap& map) {
		MapStack.push_back(map);
	}
	void pop() {
		MapStack.pop_back();
	}
private:
	std::deque<SymbolMap> MapStack;
};


IDNode* handleVarExpr(IDNode* node);
//��lex�е��ã�������IDNode����
void addID(char* chrName, IDNode* sym, int index = 0);
//��yaccƥ��types�е��ã����õ�ǰ�������ı�������
void setTypes(Value_Type tp);
//����������е��ã����õ�ǰID��Value_Type
void setIDType(IDNode* node);
//�������ȡ��Node*
void getIDs(std::vector<char*> ids, Node* now);
//����IDNode*����Ƿ��ض���
bool isRedefined(IDNode* node);
//����IDNode*����Ƿ�δ����
bool isUndefined(IDNode* node);
//ת����ָ������
void convert2Pointer(Node* n);
//���������Ƿ����ڽ�������
void setStatus(bool status);
//��ȡ���ڵ�״̬
bool isDefining();
//�жϷ��ű�����û�д����ID
bool hasID(std::string name);
//���ط��ű���ID��IDNode
IDNode* getID(std::string name);
#endif /* END __DRIVER_HPP__ */
