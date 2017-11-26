#include "driver.h"
#include <iostream>
#include <iomanip>
using namespace std;

#define PRTSPC 10
extern void yyerror(const char* msg);

static const char* ErrorMSG[] = {
	"ֻ�ܼ������������",
	"��֧�ֵĲ���������",
	"���ʽ�����ǿ��޸ĵ���ֵ",
};

bool isSimpleOpt(const char* name){
	if (name == "+="
		|| name == "-="
		|| name == "*="
		|| name == "/="
		|| name == "%="
		|| name == "|="
		|| name == "^="
		|| name == "&=")
		return false;
	return true;
}

class myOptFault :public exception
{
public:
	myOptFault(char* msg = "defualt msg\n") :exception(msg){}
};

void ThrowERR(int index){ // ������Ϣ����
	//throw new exception(ErrorMSG[index]);
	//yyerror(ErrorMSG[index]);
	fprintf(stderr, ErrorMSG[index]);
	fprintf(stderr,"\n");
}
void ThrowERR(char* msg){ // ������Ϣ����
	fprintf(stderr, msg);
	fprintf(stderr, "\n");
}

bool Node::checkNodeType(Node* n1, Node_Type type){
	return n1->getNodeType() == type;
}
bool Node::checkNodeType(Node* n1, Node *n2){
	return checkNodeType(n1, n2->getNodeType());
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
	while (cur->brother != NULL)
		cur = cur->brother;
	cur->brother = bro;
}

void Node::addChildren(Node *child){
	if (this->children == NULL){
		this->children = child;
	}
		
	else
		this->children->addBrother(child);
}
void SymbolMap::insert(string name, IDNode* sym){
	this->Map.insert(pair<string, IDNode>(name, *sym));
}
IDNode* SymbolMap::find(string name){
	map<string, IDNode>::iterator it = Map.find(name);
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
		IDNode::printNode(n);
		break;
	case Node_Type::node_value:
		ValueNode::printNode(n);
		break;
	case Node_Type::node_type:
		TypeNode::printNode(n);
		break;
	case Node_Type::node_opt:
		ExprNode::printNode(n);
		break;
	case Node_Type::node_norm:
		cout << n->getName() << endl;
		break;
	default:
		break;
	}
}

void ExprNode::printNode(Node* n){
	if (checkNodeType(n, Node_Type::node_opt)){
		cout.setf(ios::left);
		ExprNode *en = (ExprNode*)n;
		cout << setw(PRTSPC) << "EXPR" << en->getName();
		cout << setw(PRTSPC) << en->getValue()->getValue() << endl;
	}
}

void ValueNode::printNode(Node* n){
	ValueNode *node = extractInterValue((ValueNode*)n);
	cout.setf(ios::left);
	cout << setw(PRTSPC) << "Value" << node->getValue() << endl;
}
void IDNode::printNode(Node* n){
	cout.setf(ios::left);
	cout << setw(PRTSPC) << "VAR" << n->getName() << endl;
}

// ��ı�id��ֵ,ʹ������(��)1
void IDNode::setAuto(bool isIncre = true){
	ValueNode* invisible_one = new IntNode(1); // ��ʱ�� 1
	ValueNode* first_result = nullptr;
	if (isIncre)
		first_result = ExprNode::calcSimpleOpt("+", getValue(), invisible_one);
	else
		first_result = ExprNode::calcSimpleOpt("-", getValue(), invisible_one);
	ExprNode::calculate("=", this, first_result);
	delete invisible_one;
}

void IDNode::updateValue(){
	while (this->autoFlag != 0){
		if (this->autoFlag > 0){// ����1
			setAuto(true);
			this->autoFlag -= 1;
		}
		else if (this->autoFlag < 0){// �Լ�1
			setAuto(false);
			this->autoFlag += 1;
		}
	}
}

void IDNode::setAutoFlag(bool needIncre){ //��׺++
	this->autoFlag += needIncre;
}

ValueNode* IDNode::getValue(){
	updateValue();
	return this->value;
}

ValueNode* IDNode::setValue(ValueNode* n){ // �Ȳ������ͼ��(��������ֵ�����ֵ��) ֱ�Ӹ�ֵ TODO
	if (n != nullptr){
		if (this->value != nullptr)
			delete this->value; // �����ԭ��������Value
		ValueNode* v = ValueNode::extractInterValue(n);
		switch (this->value_type)
		{	 // ���.  TODO: ָ��Ļ���ǳ����
		case Value_Type::type_char: 	this->value = new CharNode(*(CharNode*)v);		break;
		case Value_Type::type_int:		this->value = new IntNode(*(IntNode*)v);		break;
		case Value_Type::type_float:	this->value = new FloatNode(*(FloatNode*)v);	break;
		case Value_Type::type_double:	this->value = new DoubleNode(*(DoubleNode*)v);	break;
		case Value_Type::type_string:	this->value = new StringNode(*(StringNode*)v);	break;
		case Value_Type::type_pointer:	this->value = new StringNode(*(StringNode*)v);	break;
		default:																		break;
		}
		return this;
	}
	else
		return nullptr;
}

void TypeNode::printNode(Node *n){
	cout.setf(ios::left);
	TypeNode* node = (TypeNode*)n;
	cout << setw(PRTSPC) << "TYPE";
	switch (node->type_type)
	{
		case Value_Type::type_char:		cout << "CHAR"		<< endl;		break;
		case Value_Type::type_int:		cout << "INT"		<< endl;		break;
		case Value_Type::type_double:	cout << "DOUBLE"	<< endl;		break;
		case Value_Type::type_float:	cout << "FLOAT"		<< endl;		break;
		case Value_Type::type_string:	cout << "STRING"	<< endl;		break;
		case Value_Type::type_pointer:	cout << "PINTER"	<< endl;		break;
		default:	cout << "Un F**king known.Fuck Me.."	<< endl;		break;
	}
}
void Node::printTree(Node* node, int level){
	if (node == NULL)
		return;
	for (int i = 0; i < level; i++)
		cout << "    ";
	Node::printNode(node);
	Node *child = node->getChildren();
	while (child != NULL)
	{
		Node::printTree(child, level + 1);
		child = child->getBrother();
	}
}

ValueNode* ValueNode::extractInterValue(ValueNode *n){
	ValueNode* res = n;
	if (n->getNodeType() == Node_Type::node_id)
		res = ((IDNode*)n)->getValue();
	else if (n->getNodeType() == Node_Type::node_opt)
		res = ((ExprNode*)n)->getValue();
	return res;
}

ValueNode* ExprNode::calcSimpleOpt(const char* name, ValueNode* node1, ValueNode* node2){ // TODO: ��������Ͽ϶���ȫ..
	if (!isSimpleOpt(name)){
		return nullptr;
	}
	ValueNode *n1 = ValueNode::extractInterValue(node1), *n2 = ValueNode::extractInterValue(node2);
	if (name == "=="){
		// ��double��תdouble��doubleת��stringʱ����0��
		if (n1->getValueType() == Value_Type::type_double || n1->getValueType() == Value_Type::type_float
			|| n2->getValueType() == Value_Type::type_double || n2->getValueType() == Value_Type::type_float){
			return new IntNode(atof(n1->getValue()) == atof(n1->getValue()));
		}
		return new IntNode(n1->getValue() == n2->getValue() );
	}
	if (false){ // TODO: ���ж��Ƿ���ָ�� �� ����
	}
	else if (n1->getValueType() == Value_Type::type_double
		|| n2->getValueType() == Value_Type::type_double) // �κα��ʽ ֻҪ��double �ǽ������ double
	{
		DoubleNode* dn = nullptr;
		if		(name == "*")  dn = new DoubleNode(atof(n1->getValue()) * atof(n2->getValue()));
		else if (name == "/")  dn = new DoubleNode(atof(n1->getValue()) / atof(n1->getValue()));
		else if (name == "+")  dn = new DoubleNode(atof(n1->getValue()) + atof(n1->getValue()));
		else if (name == "-")  dn = new DoubleNode(atof(n1->getValue()) - atof(n1->getValue()));
		else if (name == "&&") dn = new DoubleNode(atof(n1->getValue()) && atof(n1->getValue()));
		else if (name == "||") dn = new DoubleNode(atof(n1->getValue()) || atof(n1->getValue()));
		else ThrowERR(1); // ��֧�ֵĲ���������
		return dn;
	}
	else if (n1->getValueType() == Value_Type::type_float
		|| n2->getValueType() == Value_Type::type_float)
	{
		FloatNode* dn = nullptr;
		if (name == "*")	  dn = new FloatNode(atof(n1->getValue()) * atof(n2->getValue()));
		else if (name == "/") dn = new FloatNode(atof(n1->getValue()) / atof(n2->getValue()));
		else if (name == "+") dn = new FloatNode(atof(n1->getValue()) + atof(n2->getValue()));
		else if (name == "-") dn = new FloatNode(atof(n1->getValue()) - atof(n2->getValue()));
		else if (name == "&&") dn = new FloatNode(atof(n1->getValue()) && atof(n1->getValue()));
		else if (name == "||") dn = new FloatNode(atof(n1->getValue()) || atof(n1->getValue()));
		else ThrowERR(1); // ��֧�ֵĲ���������
		return dn;
	}
	else if (n1->getValueType() == Value_Type::type_int
		|| n2->getValueType() == Value_Type::type_int)
	{
		IntNode* dn = nullptr;
		if (name == "*")	   dn = new IntNode(atoi(n1->getValue()) * atoi(n2->getValue()));
		else if (name == "/")  dn = new IntNode(atoi(n1->getValue()) / atoi(n2->getValue()));
		else if (name == "+")  dn = new IntNode(atoi(n1->getValue()) + atoi(n2->getValue()));
		else if (name == "-")  dn = new IntNode(atoi(n1->getValue()) - atoi(n2->getValue()));
		else if (name == "%")  dn = new IntNode(atoi(n1->getValue()) % atoi(n2->getValue()));
		else if (name == "|")  dn = new IntNode(atoi(n1->getValue()) | atoi(n2->getValue()));
		else if (name == "&")  dn = new IntNode(atoi(n1->getValue()) & atoi(n2->getValue()));
		else if (name == "^")  dn = new IntNode(atoi(n1->getValue()) ^ atoi(n2->getValue()));
		else if (name == "<<") dn = new IntNode(atoi(n1->getValue()) >> atoi(n2->getValue()));
		else if (name == ">>") dn = new IntNode(atoi(n1->getValue()) >> atoi(n2->getValue()));
		else if (name == "&&") dn = new IntNode(atoi(n1->getValue()) && atoi(n1->getValue()));
		else if (name == "||") dn = new IntNode(atoi(n1->getValue()) || atoi(n1->getValue()));
		else ThrowERR(1); // ��֧�ֵĲ���������
		return dn;
	}
	else if (n1->getValueType() == Value_Type::type_char
		|| n2->getValueType() == Value_Type::type_char)
	{
		CharNode* dn = nullptr;
		if (name == "*")	   dn = new CharNode(atoi(n1->getValue()) *  atoi(n2->getValue()));
		else if (name == "/")  dn = new CharNode(atoi(n1->getValue()) /  atoi(n2->getValue()));
		else if (name == "+")  dn = new CharNode(atoi(n1->getValue()) +  atoi(n2->getValue()));
		else if (name == "-")  dn = new CharNode(atoi(n1->getValue()) -  atoi(n2->getValue()));
		else if (name == "%")  dn = new CharNode(atoi(n1->getValue()) %  atoi(n2->getValue()));
		else if (name == "|")  dn = new CharNode(atoi(n1->getValue()) |  atoi(n2->getValue()));
		else if (name == "&")  dn = new CharNode(atoi(n1->getValue()) &  atoi(n2->getValue()));
		else if (name == "^")  dn = new CharNode(atoi(n1->getValue()) ^  atoi(n2->getValue()));
		else if (name == "<<") dn = new CharNode(atoi(n1->getValue()) << atoi(n2->getValue()));
		else if (name == ">>") dn = new CharNode(atoi(n1->getValue()) >> atoi(n2->getValue()));
		else if (name == "&&") dn = new CharNode(atoi(n1->getValue()) && atoi(n1->getValue()));
		else if (name == "||") dn = new CharNode(atoi(n1->getValue()) || atoi(n1->getValue()));
		else ThrowERR(1); // ��֧�ֵĲ���������
		return dn;
	}
}

ValueNode* ExprNode::calculate(const char* name, ValueNode* n1, ValueNode* n2 = nullptr){
	ValueNode *v1 = ValueNode::extractInterValue(n1) , *v2 = ValueNode::extractInterValue(n2); // ������ȡID Expr��ֵ
	if (n2 == nullptr){
		// ��Ŀ����� ����  ! ȡ��ַ ǰ++ -- ��++ -- ~(ȡ��)
		if (name == "-"){
			IntNode* zero = new IntNode(0); // ������ʱ��Ϊ 0 - ID
			ValueNode* res = calcSimpleOpt("-", zero, v2);
			delete zero;
			return res;
		}else if (name == "!"){
			switch (v1->getValueType())
			{
				case Value_Type::type_char: 	return  new CharNode(!atoi(v1->getValue()));		break;
				case Value_Type::type_int:		return  new IntNode(!atoi(v1->getValue()));		break;
				case Value_Type::type_float:	return  new FloatNode(!atof(v1->getValue()));	break;
				case Value_Type::type_double:	return  new DoubleNode(!atof(v1->getValue()));	break;
				case Value_Type::type_string:	
				case Value_Type::type_pointer:
				default:
							ThrowERR(2);
			}
		}else if (name == "~"){
			switch (v1->getValueType())
			{
			case Value_Type::type_char: 	return  new CharNode(~atoi(v1->getValue()));		break;
			case Value_Type::type_int:		return  new IntNode(~atoi(v1->getValue()));		break;
			case Value_Type::type_float:
			case Value_Type::type_double:
			case Value_Type::type_string:
			case Value_Type::type_pointer:
			default:
						ThrowERR(2);
			}
			// �������㷵�ص���ID����(ȡ��ֵַ��֤��..)
		}else if (name == "+++"){ // ǰ׺ Ҳ֮ͬǰһ����Ҫ�����ڵ㲢����, ֻ�Ǵ�������ֵ��Ҫ��������(+1)
			//  ++a => a+=1 => a = a + 1
			if (n1->getNodeType() == Node_Type::node_id){ // ֻ��id�������� ���Ƹ�ֵ����
				((IDNode*)n1)->setAuto();
				return new IDNode(*(IDNode*)n1);
			}
			else
				//ThrowERR(1);
				return nullptr;
		}
		else if (name == "---"){
			if (n1->getNodeType() == Node_Type::node_id){
				((IDNode*)n1)->setAuto(false);
				return new IDNode(*(IDNode*)n1);
			}
			else
				return nullptr;
				//ThrowERR(1);
		}
		else if (name == "++"){
			if (n1->getNodeType() == Node_Type::node_id){
				((IDNode*)n1)->setAutoFlag(true);//��
				return new IDNode(*(IDNode*)n1);
			}
			else
				return nullptr;
				//ThrowERR(1);
		}
		else if (name == "--"){
			if (n1->getNodeType() == Node_Type::node_id){
				((IDNode*)n1)->setAutoFlag(false);
				return new IDNode(*(IDNode*)n1);
			}
			else
				return nullptr;
				//ThrowERR(1);
		}
	}else{
		if (name == "="){
			cout << "===========" << n1->getName()<<"="<< v2->getName() << ":"<< v2->getValue() <<endl;
			if (n1->getNodeType() != Node_Type::node_id)
				return nullptr;
				//ThrowERR(3);
			// �ȼ�� ָ��/���� ��ֵ��ϵ
			return n1 == n2 ? n1: ((IDNode*)n1)->setValue(v2);
		}
		else{
			if (isSimpleOpt(name)){
				return calcSimpleOpt(name, v1, v2);
			}
			else{
				if (n1->getNodeType() != Node_Type::node_id) // TODO:����Ҳ��������ֵ
					ThrowERR(3);
				ValueNode* first_result = nullptr;
				if (name == "+=")			first_result = calculate("+", n1, n2);
				else if (name == "-=")		first_result = calculate("-", n1, n2);
				else if (name == "*=")		first_result = calculate("*", n1, n2);
				else if (name == "/=")		first_result = calculate("/", n1, n2);
				else if (name == "%=")		first_result = calculate("%", n1, n2);
				else if (name == "&=")		first_result = calculate("&", n1, n2);
				else if (name == "|=")		first_result = calculate("|", n1, n2);
				else if (name == "^=")		first_result = calculate("^", n1, n2);
				else {
					cout << "��֧�ֵ������" << endl;
					return nullptr; //throw new myOptFault("��֧�ֵ���������");
				}
				return calculate("=", n1, first_result);
			}
		}
	}
}

ValueNode* ExprNode::setValue(ValueNode* node){
	if (node != nullptr){
		if (this->tvalue != nullptr)
			delete this->tvalue;//�����ԭ����ֵ
		this->tvalue = node;
		this->value_type = node->getValueType();
		return this;
	}
	else{
		return nullptr;
	}
}