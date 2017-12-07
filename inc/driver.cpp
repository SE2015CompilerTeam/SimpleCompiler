#include "driver.h"
extern void yyerror(const char* msg);

SymbolMap idMap;
Value_Type type;
Value_Type VALUE_TYPE;
bool defining = false;

static const char* ErrorMSG[] = {
	"ֻ�ܼ������������",
	"��֧�ֵĲ���������",
	"���ʽ�����ǿ��޸ĵ���ֵ",
	"���ʽȱ����ֵ",
	"ʹ����δ��ʼ���Ļ��ʼ��Ϊ�յı�ʶ��",
	"�ض���ı�ʶ��",
	"��֧�ֵ������",
	"δ�����ʶ��"
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

void ThrowERR(int index){ // ������Ϣ����
	//throw new exception(ErrorMSG[index]);
	//yyerror(ErrorMSG[index]);
	fprintf(stderr, ErrorMSG[index]);
	fprintf(stderr, "\n");
}
void ThrowERR(char* msg){ // ������Ϣ����
	//yyerror(msg);
	fprintf(stderr, msg);
	fprintf(stderr, "\n");
}

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
	while (cur->brother != NULL){
		if (cur == bro) // ��������Լ�Ϊ�ֵ�
			return;
		cur = cur->brother;
	}
	cur->brother = bro;
}

void Node::addChildren(Node *child) {
	if (this != child){
		if (this->children == NULL)
			this->children = child;
		else
			this->children->addBrother(child);
	}
}

void Node::checkDefTree(Node* root, TypeNode* def_type){
	Node* child = root->getChildren();// ��һ������
	while (child != nullptr)
	{
		if (child->getNodeType() == Node_Type::node_id){
			IDNode* id_child = (IDNode*)child;
			if (idMap.hasDefined(id_child)){
				fprintf(stderr, "�ض��������'%s' at line: %d\n", id_child->getName(), (getID(id_child->getName()))->getLineNum());
			}
			else{
				// Ϊ�丳����
				setIDType(id_child, def_type->getTypeType());
				addID(child->getName(), id_child);
			}
		}
		child = child->getBrother();
	}
}

void Node::printNode(Node *n) {
	if (n != nullptr){
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
}
void Node::printTree(Node* node, int level){
	if (node == NULL)
		return;
	for (int i = 0; i < level; i++)
		cout << "    ";
	Node::printNode(node);
	Node *child = node->getChildren();
	while (child != NULL && child != child->getBrother())
	{
		Node::printTree(child, level + 1);
		child = child->getBrother();
	}
}


void ValueNode::printNode(Node* n){
	if (n != nullptr){
		ValueNode *node = extractInterValue((ValueNode*)n);
		cout.setf(ios::left);
		if (node->getValueType() == Value_Type::type_string){
			cout << setw(PRTSPC) << "Value" << node->getValue() << '(' << node->getName() << ')' << endl;
		}
		else
			cout << setw(PRTSPC) << "Value" << node->getValue() << endl;
	}
}

void ValueNode::setValue(const char* val, Value_Type type = Value_Type::type_int){
	this->value = new char[std::strlen(val) + 1];
	strcpy_s(this->value, std::strlen(val) + 1, val);
}

bool ValueNode::checkValid(ValueNode* node){ // �ڵ��Ƿ�Ϊ�ջ��ʶ����û�б���ֵ
	if (node != nullptr)
		return extractInterValue(node) != nullptr;
	return false;
}

ValueNode* ValueNode::extractInterValue(ValueNode *n){
	ValueNode* res = n;
	if (n != nullptr){
		if (n->getNodeType() == Node_Type::node_id){
			if (n->getValueType() == Value_Type::type_default){
				fprintf(stderr, "δ�����δ��ֵ�ı�ʶ�� %s\n", n->getName());
			}
			res = ((IDNode*)n)->getValue();
		}
			
		else if (n->getNodeType() == Node_Type::node_opt)
			res = ((ExprNode*)n)->getValue();
	}
	return res;
}

IDNode* IDNode::cp(IDNode* src){
	IDNode* target = new IDNode(src->getName());
	cp(target, src);
	return target;
}
void IDNode::cp(IDNode* target, IDNode* src){ // ���˲���ֵ���Ӻ��ֵܣ������Ҿ��ÿ���������..��
	if (src != nullptr){
		target->setLineNum(src->getLineNum());
		target->setValueType(src->getValueType());
		target->setValue(src->getValue());
	}
}

int IDNode::getLineNum(){
	return this->linenum;
}
void IDNode::setLineNum(int no){
	this->linenum = no;
}

void IDNode::Increment(bool incre){
	if (this->tvalue != nullptr){
		ValueNode* invisible_one = new IntNode(1); // ��ʱ�� 1
		ValueNode* first_res = ExprNode::calcSimpleOpt(incre ? "+" : "-", this->tvalue, invisible_one);
		ExprNode::calculate("=", this, first_res); 
		delete invisible_one;
		delete first_res;
	}
}

// ��ı�id��ֵ,ʹ������(��)1
ValueNode* IDNode::setAuto(bool isIncre = true, bool prefix = true){
	ValueNode* res = nullptr;
	if (prefix){ // �����󷵻��������
		Increment(isIncre);
		res = new ValueNode(this->tvalue->getValue(), this->value_type);
		return res;
	}
	else{ // �ȷ����������������
		res = new ValueNode(this->tvalue->getValue(), this->value_type);
		Increment(isIncre);
		return res;
	}
}

void IDNode::printNode(Node* n) {
	if (n != nullptr){
		try {
			if (n->getNodeType() == Node_Type::node_id){
				IDNode* node = (IDNode*)n;
				cout.setf(ios::left);
				cout << setw(PRTSPC) << "VAR" << n->getName() << endl;
				/*if (node->getValue()->getValueType() == Value_Type::type_array) {
					ValueNode* valNode = node->getValue();
					ArrayNode* arrNode = (ArrayNode*)valNode;
					vector<int> size = arrNode->getSize();
					cout << n->getName();
					for (int i = 0; i < size.size(); i++) {
						cout << "[" << size[i] << "]";
					}
					cout << endl;
				}
				else {
					cout << n->getName() << endl;
				}*/
			}
		}
		catch (exception e) {
			printf(e.what());
		}
	}

}


ValueNode* IDNode::getValue(){
	//updateValue(); // ������ÿ����ֵ��ʱ��update ! ����һ���������Ժ�
	return this->tvalue;
}

ValueNode* IDNode::setValue(ValueNode* n){ // �Ȳ������ͼ��(��������ֵ�����ֵ��) ֱ�Ӹ�ֵ TODO
	if (n != nullptr){
		if (this->tvalue != nullptr)
			delete this->tvalue; // �����ԭ��������Value
		ValueNode* v = ValueNode::extractInterValue(n);
		char *type; 
		switch (v->getValueType())
		{
		case Value_Type::type_char:		type = "char";		break;
		case Value_Type::type_int:		type = "int";		break;
		case Value_Type::type_float:	type = "float";		break;
		case Value_Type::type_double:	type = "double";	break;
		default:						type = "other";		break;
		}
		switch (this->value_type)
		{
		case Value_Type::type_char: 	
			switch (v->getValueType())
			{
				case Value_Type::type_char:		this->tvalue = new CharNode(((CharNode*)v)->getValue());	break;
				case Value_Type::type_int:		this->tvalue = new CharNode(((IntNode*)v)->getValue());		break;
				case Value_Type::type_float:	this->tvalue = new CharNode(((FloatNode*)v)->getValue());	break;
				case Value_Type::type_double:	this->tvalue = new CharNode(((DoubleNode*)v)->getValue());	break;
				//case Value_Type::type_pointer:	this->tvalue = new CharNode(*(StringNode*)v);	break;
				default:
					fprintf(stderr, "��֧�ֵ�����ת��,�� %s �� char\n", type);
					break;
			}
			break;
		case Value_Type::type_int:		
			switch (v->getValueType())
			{
			case Value_Type::type_char:		this->tvalue = new IntNode(((CharNode*)v)->getValue());	break;
			case Value_Type::type_int:		this->tvalue = new IntNode(((IntNode*)v)->getValue());		break;
			case Value_Type::type_float:	this->tvalue = new IntNode(((FloatNode*)v)->getValue());	break;
			case Value_Type::type_double:	this->tvalue = new IntNode(((DoubleNode*)v)->getValue());	break;
				//case Value_Type::type_pointer:	this->tvalue = new CharNode(*(StringNode*)v);	break;
			default:
				fprintf(stderr, "��֧�ֵ�����ת��,�� %s �� double\n", type);
				break;
			}
			break;
		case Value_Type::type_float:	
			switch (v->getValueType())
			{
			case Value_Type::type_char:		this->tvalue = new FloatNode(((CharNode*)v)->getValue());	break;
			case Value_Type::type_int:		this->tvalue = new FloatNode(((IntNode*)v)->getValue());		break;
			case Value_Type::type_float:	this->tvalue = new FloatNode(((FloatNode*)v)->getValue());	break;
			case Value_Type::type_double:	this->tvalue = new FloatNode(((DoubleNode*)v)->getValue());	break;
				//case Value_Type::type_pointer:	this->tvalue = new CharNode(*(StringNode*)v);	break;
			default:
				fprintf(stderr, "��֧�ֵ�����ת��,�� %s �� float\n", type);
				break;
			}
			break;
		case Value_Type::type_double:	
			switch (v->getValueType())
			{
			case Value_Type::type_char:		this->tvalue = new DoubleNode(((CharNode*)v)->getValue());	break;
			case Value_Type::type_int:		this->tvalue = new DoubleNode(((IntNode*)v)->getValue());		break;
			case Value_Type::type_float:	this->tvalue = new DoubleNode(((FloatNode*)v)->getValue());	break;
			case Value_Type::type_double:	this->tvalue = new DoubleNode(((DoubleNode*)v)->getValue());	break;
				//case Value_Type::type_pointer:	this->tvalue = new CharNode(*(StringNode*)v);	break;
			default:
				fprintf(stderr, "��֧�ֵ�����ת��,�� %s �� double\n", type);
				break;
			}
			break;
		case Value_Type::type_string:	
			switch (v->getValueType())
			{
			case Value_Type::type_char:		this->tvalue = new StringNode(((CharNode*)v)->getValue());	break;
			case Value_Type::type_int:		this->tvalue = new StringNode(((IntNode*)v)->getValue());		break;
			case Value_Type::type_float:	this->tvalue = new StringNode(((FloatNode*)v)->getValue());	break;
			case Value_Type::type_double:	this->tvalue = new StringNode(((DoubleNode*)v)->getValue());	break;
				//case Value_Type::type_pointer:	this->tvalue = new CharNode(*(StringNode*)v);	break;
			default:
				fprintf(stderr, "��֧�ֵ�����ת��,�� %s �� string\n", type);
				break;
			}
			break;
		//case Value_Type::type_pointer:	this->tvalue = new StringNode(*(StringNode*)v);	break;
		//case Value_Type::type_array:	this->tvalue = n;								break;
		default:																		break;
		}
		return this;
	}
	else
		return nullptr;
}


void ExprNode::printNode(Node* n){
	if (n != nullptr){
		if (n->getNodeType() == Node_Type::node_opt){
			cout.setf(ios::left);
			ExprNode *en = (ExprNode*)n;
			cout << setw(PRTSPC) << "EXPR" << en->getName() << "    " << en->getValue()->getValue() << endl;
		}
	}
}

ValueNode* ExprNode::setValue(ValueNode* node){
	if (node != nullptr && this->tvalue != ValueNode::extractInterValue(node)){
		if (this->tvalue != nullptr)
			delete this->tvalue;//�����ԭ����ֵ
		this->tvalue = ValueNode::extractInterValue(node);
		this->value_type = node->getValueType();
		return this;
	}
	else{
		return nullptr;
	}
}


ValueNode* ExprNode::calcBoolOpt(const char* type, ValueNode* node1, ValueNode* node2){
	// >, <, ==, !=
	if (ValueNode::checkValid(node1) && ValueNode::checkValid(node2)){
		ValueNode *n1 = ValueNode::extractInterValue(node1), *n2 = ValueNode::extractInterValue(node2);
		double res1 = atof(n1->getValue()), res2 = atof(n2->getValue());
		if (!strcmp(type, ">"))			return new IntNode(res1 > res2);
		if (!strcmp(type, "<"))			return new IntNode(res1 < res2);
		if (!strcmp(type, "=="))		return new IntNode(res1 == res2);
		if (!strcmp(type, "!="))		return new IntNode(res1 != res2);
		if (!strcmp(type, ">="))		return new IntNode(res1 >= res2);
		if (!strcmp(type, "<="))		return new IntNode(res1 <= res2);
		return nullptr;
	}
	else{
		ThrowERR(3);
		return ValueNode::ErrorNode();
	}
}


// ֻ�� ��ֵ���� ���Բ����ֵ�Ƿ�Ϊ��
ValueNode* ExprNode::calcSimpleOpt(const char* name, ValueNode* node1, ValueNode* node2){
	if (!isSimpleOpt(name)){
		return ValueNode::ErrorNode();
	}
	bool b = checkValid(node1) && checkValid(node2);
	if (checkValid(node1) && checkValid(node2)){
		ValueNode *n1 = ValueNode::extractInterValue(node1), *n2 = ValueNode::extractInterValue(node2);
		if (n1 == nullptr || n2 == nullptr){
			ThrowERR(4);
		}
		if (false){ // TODO: ���ж��Ƿ���ָ�� �� ����
		}
		else {
			ValueNode *res = calcBoolOpt(name, n1, n2);
			if (res != nullptr){
				return res;
			}
			if (n1->getValueType() == Value_Type::type_double
				|| n2->getValueType() == Value_Type::type_double) // �κα��ʽ ֻҪ��double �ǽ������ double
			{
				DoubleNode* dn = nullptr;
				if (name == "*")  dn = new DoubleNode(atof(n1->getValue()) * atof(n2->getValue()));
				else if (name == "/")  dn = new DoubleNode(atof(n1->getValue()) / atof(n2->getValue()));
				else if (name == "+")  dn = new DoubleNode(atof(n1->getValue()) + atof(n2->getValue()));
				else if (name == "-")  dn = new DoubleNode(atof(n1->getValue()) - atof(n2->getValue()));
				else if (name == "&&") dn = new DoubleNode(atof(n1->getValue()) && atof(n2->getValue()));
				else if (name == "||") dn = new DoubleNode(atof(n1->getValue()) || atof(n2->getValue()));
				else{
					ThrowERR(1);
					ValueNode::ErrorNode();
				}
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
				else if (name == "&&") dn = new FloatNode(atof(n1->getValue()) && atof(n2->getValue()));
				else if (name == "||") dn = new FloatNode(atof(n1->getValue()) || atof(n2->getValue()));
				else {
					ThrowERR(1);
					ValueNode::ErrorNode();
				}
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
				else if (name == "&&") dn = new IntNode(atoi(n1->getValue()) && atoi(n2->getValue()));
				else if (name == "||") dn = new IntNode(atoi(n1->getValue()) || atoi(n2->getValue()));
				else {
					ThrowERR(1);
					ValueNode::ErrorNode();
				}
				return dn;
			}
			else if (n1->getValueType() == Value_Type::type_char
				|| n2->getValueType() == Value_Type::type_char)
			{
				CharNode* dn = nullptr;
				if (name == "*")	   dn = new CharNode(atoi(n1->getValue()) *  atoi(n2->getValue()));
				else if (name == "/")  dn = new CharNode(atoi(n1->getValue()) / atoi(n2->getValue()));
				else if (name == "+")  dn = new CharNode(atoi(n1->getValue()) + atoi(n2->getValue()));
				else if (name == "-")  dn = new CharNode(atoi(n1->getValue()) - atoi(n2->getValue()));
				else if (name == "%")  dn = new CharNode(atoi(n1->getValue()) % atoi(n2->getValue()));
				else if (name == "|")  dn = new CharNode(atoi(n1->getValue()) | atoi(n2->getValue()));
				else if (name == "&")  dn = new CharNode(atoi(n1->getValue()) &  atoi(n2->getValue()));
				else if (name == "^")  dn = new CharNode(atoi(n1->getValue()) ^ atoi(n2->getValue()));
				else if (name == "<<") dn = new CharNode(atoi(n1->getValue()) << atoi(n2->getValue()));
				else if (name == ">>") dn = new CharNode(atoi(n1->getValue()) >> atoi(n2->getValue()));
				else if (name == "&&") dn = new CharNode(atoi(n1->getValue()) && atoi(n2->getValue()));
				else if (name == "||") dn = new CharNode(atoi(n1->getValue()) || atoi(n2->getValue()));
				else {
					ThrowERR(1);
					return ValueNode::ErrorNode();
				}
				return dn;
			}
		}
	}
	else{
		ThrowERR(4);
		return ValueNode::ErrorNode();
	}

}

ValueNode* ExprNode::calculate(const char* opt, ValueNode* n1, ValueNode* n2 = nullptr){
	if (n1 == nullptr)
		return ValueNode::ErrorNode();
	ValueNode *v1 = ValueNode::extractInterValue(n1);
	ValueNode *v2 = ValueNode::extractInterValue(n2); // ������ȡID Expr��ֵ
	if (n2 == nullptr){
		// ��Ŀ����� ����  ! ȡ��ַ ǰ++ -- ��++ -- ~(ȡ��)
		if (v1 == nullptr){
			// ��ʶ����δ��ʼ��
			ThrowERR(4);
			return ValueNode::ErrorNode();
		}
		else{
			if (opt == "-"){
				IntNode* zero = new IntNode(0); // ������ʱ��Ϊ 0 - ID
				ValueNode* res = calcSimpleOpt("-", zero, v1);
				delete zero;
				return res;
			}
			else if (opt == "!"){
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
					return ValueNode::ErrorNode();

				}
			}
			else if (opt == "~"){
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
					return ValueNode::ErrorNode();
				}
				// �������㷵�ص���ID����(ȡ��ֵַ��֤��..)
			}
			else if (opt == "+++"){ // ǰ׺ Ҳ֮ͬǰһ����Ҫ�����ڵ㲢����, ֻ�Ǵ�������ֵ��Ҫ��������(+1)
				//  ++a => a+=1 => a = a + 1
				if (n1->getNodeType() == Node_Type::node_id){ // ֻ��id�������� ���Ƹ�ֵ����
					return ((IDNode*)n1)->setAuto(true, true);
				}
				else{
					ThrowERR(1);
					return ValueNode::ErrorNode();
				}
			}
			else if (opt == "---"){
				if (n1->getNodeType() == Node_Type::node_id){
					return ((IDNode*)n1)->setAuto(false, true);
					//return new IDNode(*(IDNode*)n1);
				}
				else{
					ThrowERR(1);
					return ValueNode::ErrorNode();
				}
			}
			else if (opt == "++"){
				if (n1->getNodeType() == Node_Type::node_id){
					return ((IDNode*)n1)->setAuto(true, false);
				}
				else{
					ThrowERR(1);
					return ValueNode::ErrorNode();
				}
			}
			else if (opt == "--"){
				if (n1->getNodeType() == Node_Type::node_id){
					return ((IDNode*)n1)->setAuto(true, false);
				}
				else{
					ThrowERR(1);
					return ValueNode::ErrorNode();
				}
			}
		}
	}
	else{
		if (opt == "="){
			if (n1->getNodeType() != Node_Type::node_id){
				ThrowERR(2);
				return ValueNode::ErrorNode();
			}
			// �ȼ�� ָ��/���� ��ֵ��ϵ
			assign((IDNode*)n1, n2);
			return (IDNode*)n1;
		}
		else{
			if (v1 == nullptr){
				ThrowERR(4);
				return  ValueNode::ErrorNode();
			}
			else{
				if (isSimpleOpt(opt)){
					return calcSimpleOpt(opt, v1, v2);
				}
				else{
					if (n1->getNodeType() != Node_Type::node_id) // TODO:����Ҳ��������ֵ
						ThrowERR(3);
					ValueNode* first_result = nullptr;
					if (opt == "+=")			first_result = calculate("+", n1, n2);
					else if (opt == "-=")		first_result = calculate("-", n1, n2);
					else if (opt == "*=")		first_result = calculate("*", n1, n2);
					else if (opt == "/=")		first_result = calculate("/", n1, n2);
					else if (opt == "%=")		first_result = calculate("%", n1, n2);
					else if (opt == "&=")		first_result = calculate("&", n1, n2);
					else if (opt == "|=")		first_result = calculate("|", n1, n2);
					else if (opt == "^=")		first_result = calculate("^", n1, n2);
					else {
						ThrowERR(6);
						return ValueNode::ErrorNode(); //throw new myOptFault("��֧�ֵ���������");
					}
					return calculate("=", n1, first_result);
				}
			}
		}
	}
}

void ExprNode::assign(IDNode* target, ValueNode* src){ // �����ֵ�Ƿ�δ����
	IDNode* symbol = getID(target->getName());//����Ҫ����ֵ��name ����
	if (symbol != nullptr){
		target->setValue(src);  
	}
	else{
		fprintf(stderr, "δ�����ʶ��: ", target->getName(),'\n');
		//ThrowERR(4); // ʹ����δ�������
	}
}

void TypeNode::printNode(Node *n){
	if (n != nullptr){
		cout.setf(ios::left);
		TypeNode* node = (TypeNode*)n;
		cout << setw(PRTSPC) << "TYPE";
		switch (node->type_type)
		{
		case Value_Type::type_char:		cout << "CHAR" << endl;		break;
		case Value_Type::type_int:		cout << "INT" << endl;		break;
		case Value_Type::type_double:	cout << "DOUBLE" << endl;		break;
		case Value_Type::type_float:	cout << "FLOAT" << endl;		break;
		case Value_Type::type_string:	cout << "STRING" << endl;		break;
		case Value_Type::type_pointer:	cout << "PINTER" << endl;		break;
		default:	cout << "Un F**king known.Fuck Me.." << endl;		break;
		}
	}
}

void ArrayNode::printNode(Node* n) {
	try {
		if (n == nullptr) { //�����ǰ�ǿս��Ͳ���ӡ
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

void SymbolMap::updateSymbol(IDNode* src){
	IDNode* target = find(src->getName());
	target->setValue(ValueNode::extractInterValue(src));
}
bool SymbolMap::hasDefined(std::string name){
	IDNode* target = this->find(name);
	if (target != nullptr && target->getValueType() != Value_Type::type_default)// �ҵ���,����Ѿ���������
		return true;
	return false; // δ�ҵ�, ����δ����
}
bool SymbolMap::hasDefined(IDNode* symbol){
	return hasDefined(symbol->getName());
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


//�ж����������⣬���õ�Ĭ����type_int����������Ȼ������
bool isRedefined(IDNode* node) {
	return idMap.find(node->getName()) != nullptr;
}
//�ж����������⣬���õ�Ĭ����type_int����������Ȼ������
bool isUndefined(IDNode* node) {
	return idMap.find(node->getName()) == nullptr;
}

//��index���ֲ�ͬ���������µı�,����չ
void addID(char* chrName, IDNode* sym, int index) {
	string strNmae = chrName;
	if (idMap.find(strNmae) == nullptr && sym != nullptr) {
		idMap.insert(strNmae, sym);
	}
}

void setTypes(Value_Type tp) { // ȫ�ֵ�type
	VALUE_TYPE = tp;
}

void setIDType(IDNode* node, Value_Type type){
	node->setValueType(type);
}
void setIDType(IDNode* node){
	setIDType(node, VALUE_TYPE);
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

void updateIDInMap(ValueNode* src){
	if (src->getNodeType() == Node_Type::node_id)
		idMap.updateSymbol((IDNode*)src);
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
	return idMap.find(name) != nullptr;
}

IDNode* getID(string name) {
	return idMap.find(name);
}
IDNode* ExprNode::handleVarExpr(IDNode* node){
	IDNode* symbol = getID(node->getName());//����Ҫ����ֵ��name ����
	//IDNode* nn = idMap.find(node->getName());
	if (symbol != nullptr){ // �ҵ���,�̳��ҵ���ֵ
		IDNode::cp(node, symbol);
	}
	Value_Type t = node->getValueType();
	return node;
	/*if (isDefining()){
		if (symbol != nullptr){
			// ���ڶ���ı����Ѿ������˷��ű�  =>  �ض���
			cout << "�ض���ı�ʶ��: " << symbol->getName() << " ���� " << symbol->getLineNum() << " �ж���" << endl;
			return nullptr;
		}
		else{
			addID(node->getName(), node);
			symbol = getID(node->getName()); // �ⲻ��ֱ����node��ֵ������ �����mapȡ����
			setIDType(symbol);
			return symbol;
		}
	}
	else{
		// ���ڸ�ֵ
		if (symbol == nullptr){
			// δ�����ı���
			cout << "ʹ��δ�����ı��� " << node->getName() << endl;
			//ThrowERR(4);
			return nullptr;
		}
		return symbol;
	}*/
}