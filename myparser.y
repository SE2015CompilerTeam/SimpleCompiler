%{
#pragma warning( disable : 4996)
#include <fstream>
#include "mylexer.h"
#include "driver.h"
#include "myparser.h"
extern int yylex();
extern int yylineno;
   
extern char* yytext;
extern int yyleng;
using namespace std;
class A{};
Node* node;
%}
%union {
    class Node *nodes;
    class TypeNode *type_n;
    class IntNode *int_n;
    class DoubleNode *double_n;
    class CharNode *char_n;
    class StringNode *str;
    class IDNode *id_n;
    char* name;
    int token;
}

%name myparser
/*
%include {
#ifndef YYSTYPE
#define YYSTYPE char*
#endif
}
*/
%token<int_n>            INTEGER TRUE FALSE NUL
%token<double_n>         DBL
%token<char_n>           CHR
%token<id_n>			 ID
%token<str>              STR
%token<token>            PROC_CONTROL  ACCESS_CONTROL ERR_CONTROL USING NAMESPACE DEFINE
%token<nodes>            READ WRITE 
%token<token>            CLASS RETURN NEW DEL THIS
%token<token>            ';'
%token<token>            '#'

%token<token>             FUNC_PRE
%token<token>            '{' '}'
%left <token>           '(' ')'
%left<token>            ','
%right<token>           '=' AA MNA MA DA MOA ORA XORA PA
%right<token>           '?' ':'
%left<token>            OR
%left<token>            AND
%left<token>            '|'
%left<token>            '^'
%left<token>            '&'
%left<token>            EQ NEQ
%left<name>             CMP
%left<token>            '+' '-'
%left<token>            '*' '/' '%'
%left<token>            '!' PP MM '~' LL RR
%left<token>            RA NSP '[' ']'

%token<type_n>            INT DOUBLE CHAR FLOAT BOOL VOID STRUCT MAIN
%token<nodes>            IF ELSE THEN WHILE DO FOR GOTO SWITCH CASE DEFAULT
%type <nodes> vals
%type <nodes> stmts block
//%type <nodes> func_def_args
%type <nodes> stmt def_stmt if_stmt while_stmt do_while_stmt for_stmt return_stmt
%type <nodes> for_init_args for_action_args cond_args
%type <nodes> expr expritem
%type <nodes> types
%type <nodes> initlist
%type <nodes> var varexpr
%type <nodes> ids exprlist
//%type <nodes> func_def_stmt

%start program
%%
program : //types MAIN '(' ')' block {Node::printTree($5, 0);printf("main\n");} // TODO: main args
         block {Node::printTree($1, 0);}
         | program block {Node::printTree($2, 0);}
        ;
types: INT  {$$ = $1; printf("types INT\n"); setTypes(Value_Type::type_int); setStatus(true);}
     | DOUBLE   {$$ = $1; setTypes(Value_Type::type_double); setStatus(true);}
     | FLOAT    {$$ = $1; setTypes(Value_Type::type_double); setStatus(true);}
     | CHAR {$$ = $1; setTypes(Value_Type::type_char); setStatus(true);}
     | VOID {$$ = $1; setTypes(Value_Type::type_void); setStatus(true);}
     | BOOL {$$ = $1; setTypes(Value_Type::type_bool); setStatus(true);}
     //| types '*' { }
     ;
/*
func_def_args:    {}
    | def_stmt    {printf("func_def_args types ID%s\n", $1);}
    | func_def_args ',' def_stmt   {printf("func_def_args lists\n");}
    ;*/
/*mainarg :  {}
        | VOID {}
        | INT ID COMMA CHAR MUL ID LBRACK RBRACK {}
        | INT ID COMMA CHAR MUL MUL ID {}
        ;*/
block : '{' stmts '}' {$$ = Node::createNode(new Node("Block"), $2);}
      ;
stmts   : /* empty */ {$$ = new Node("NULL");}
        | stmt {$$ = Node::createNode(new Node("Stmts"), $1);}
        | stmts stmt {$$ = $1; $$->addChildren($2);}
        ;
stmt    : def_stmt ';'{$$ = $1;  printf("stmt def_stmt ;\n");}
        | if_stmt {$$ = $1; printf("stmt if_stmt\n");}
        | while_stmt {$$ = $1; printf("stmt while_stmt\n");}
        | do_while_stmt {$$ = $1;}
        | for_stmt {$$ = $1; printf("stmt for_stmt\n");}
        | return_stmt ';'{$$ = $1; printf("stmt return_stmt ;\n");}
        //| error SEMICOLON { $$ = new GeneralNode(INVALID); yyerrok(); }
        | exprlist ';' {$$ = $1; printf("stmt exprlist ;\n");  }
        | block {$$ = $1; printf("stmt block \n"); }
        | ';' {$$ = new Node("Empty Stmt"); printf("stmt just a ;\n"); }
        ;
initlist: '{' exprlist '}' { $$ = Node::createNode(new Node("Initlist"), $2); printf("initlist {list}\n"); }
        | '{' exprlist ',' '}' {$$ = Node::createNode(new Node("Initlist"), $2);printf("initlist {list ,}\n");}
        ;
exprlist: exprlist ',' expritem {$$ = $1; $$->addChildren($3); ArrayNode* tmp = (ArrayNode*)$$; tmp->addCount(); printf("exprlist , item\n"); }
        | expritem {$$ = new ArrayNode(); $$->addChildren($1);printf("exprlist item\n");}
        ;
expritem: expr {$$ = $1; printf("expritem : expr\n");}
        | initlist { $$ = $1; }
        ;
vals:     INTEGER   {$$ = $1; printf("vals INTEGER %d name %s type %d\n", $1->value,$1->name, $1->getNodeType());}
        | DBL   {$$ = $1; printf("vals DBL %f\n", $1->value);}
        | CHR   {$$ = $1;printf("vals CHR %c\n", $1->value);}
        | STR   {$$ = $1;}
        | TRUE  {$$ = $1;}
        | FALSE {$$ = $1;}
        | NUL   {$$ = $1;}
        ;
/*func_def_stmt: types ID '(' def_stmt ')' block   {printf("func_def\n");}
        ;*/
expr    : '(' expr ')' { $$ = $2; printf("(expr)\n");}
        | expr PA  expr {
                            $$ = Node::createNode(3,new Node("+=", Node_Type::node_opt), $1, $3);
                            }
        | expr MNA expr {$$ = Node::createNode(3,new Node("-=", Node_Type::node_opt), $1, $3);}
        | expr MA  expr {$$ = Node::createNode(3,new Node("*=", Node_Type::node_opt), $1, $3);}
        | expr DA  expr {$$ = Node::createNode(3,new Node("/=", Node_Type::node_opt), $1, $3);}
        | expr MOA expr {$$ = Node::createNode(3,new Node("%=", Node_Type::node_opt), $1, $3);}
        | expr ORA expr {$$ = Node::createNode(3,new Node("|=", Node_Type::node_opt), $1, $3);}
        | expr XORA expr{$$ = Node::createNode(3,new Node("^=", Node_Type::node_opt), $1, $3);}
        | expr AA  expr {$$ = Node::createNode(3,new Node("&=", Node_Type::node_opt), $1, $3);}
        | expr '+' expr {$$ = Node::createNode(3,new Node("+", Node_Type::node_opt), $1, $3); }
        | expr '-' expr {$$ = Node::createNode(3,new Node("-", Node_Type::node_opt), $1, $3);}
        | expr '*' expr {$$ = Node::createNode(3,new Node("*", Node_Type::node_opt), $1, $3); }
        | expr '/' expr {$$ = Node::createNode(3,new Node("/", Node_Type::node_opt), $1, $3); }
        | expr '%' expr {$$ = Node::createNode(3,new Node("%", Node_Type::node_opt), $1, $3); }
        | expr LL expr  {$$ = Node::createNode(3,new Node("<<", Node_Type::node_opt), $1, $3); }
        | expr RR expr  {$$ = Node::createNode(3,new Node(">>", Node_Type::node_opt), $1, $3); }
        | expr '|' expr {$$ = Node::createNode(3,new Node("|", Node_Type::node_opt), $1, $3);}
        | expr '^' expr {$$ = Node::createNode(3,new Node("^", Node_Type::node_opt), $1, $3); }
        | expr '&' expr {$$ = Node::createNode(3,new Node("&", Node_Type::node_opt), $1, $3); }
        | expr EQ expr  {$$ = Node::createNode(3,new Node("==", Node_Type::node_opt), $1, $3);}
        | expr NEQ expr {$$ = Node::createNode(3,new Node("!=", Node_Type::node_opt), $1, $3); }
        | expr CMP expr {$$ = Node::createNode(3,new Node($2, Node_Type::node_opt), $1, $3); }
        | expr OR expr  {$$ = Node::createNode(3,new Node("||", Node_Type::node_opt), $1, $3); }
        | expr AND expr {$$ = Node::createNode(3,new Node("&&", Node_Type::node_opt), $1, $3); }
        | expr '=' expr {$$ = Node::createNode(3,new Node("=", Node_Type::node_opt), $1, $3);}
        | PP expr  %prec '!' {$$ = $2;}
        | MM expr  %prec '!' {$$ = $2;}
        | '-' expr %prec '!' {$$ = $2;}
        | expr PP  %prec '*' {
                                $$ = Node::createNode(new Node("++", Node_Type::node_opt), $1);
                                printf("expr PP\n");
                             }
        | expr MM  %prec '*' {$$ = Node::createNode(new Node("--", Node_Type::node_opt), $1);}
        | '!' expr {$$ = $2;} // TODO: temp..
        | '~' expr {$$ = Node::createNode(new Node("~", Node_Type::node_opt), $2);}
        | vals  {$$ = $1;}
        | var {$$ = $1;printf("expr ID \n");}
        //| {}
        //| MUL expr %prec UDEREF { }
        //| BITAND expr %prec UREF { }
        //| expr LBRACK expr RBRACK %prec SUB {}
        ;
ids     : varexpr {
                    $$ = Node::createNode(new Node("ID List"), $1);
                    printf("ids varexpr\n");
                }
        | ids ',' varexpr {$$ = $1; $$->addChildren($3);  printf("ids , varexpr\n");}
        ;
varexpr : var { $$ = $1;printf("varexpr var\n");}
        | var '=' expr { /*可能是IDNode或ExprNode*/
                            // 这里IDNode 直接SetValue 无需再创建新节点
                            $$ = Node::createNode(new Node("varexpr"), Node::createNode(3, new Node("=", Node_Type::node_opt),$1, $3)); 
                            printf("varexpr: var = expr\n");
                        }
        | var '=' initlist {
                            // SetValue
                            $$ = Node::createNode(new Node("varexpr"), Node::createNode(3, new Node("=", Node_Type::node_opt),$1, $3));
                            printf("varexpr: var = initlist\n");
                        }
        ;
var     : ID {
				printf("var ID %s\n", $1->getName());
				string name = $1->getName();
				IDNode* node = getID(name);
				if(isDefining()){
					if(isRedefined(node)){
						cout<<endl<<"fuck redefined!"<<endl<<endl;
					}
					else{
						setIDType(node);
					}
				}
				else{
					if(isUndefined(node)){
						cout<<endl<<"fuck undefined!"<<endl<<endl;
					}
				}
				$$ = node;
			 }
        //| '*' var %prec '!' {}
        | var '[' INTEGER ']' {
								//把每一维空间大小压入进去
								IDNode* tmp = (IDNode*)$1;//先转换成IDNode*
								ArrayNode* node = (ArrayNode*)tmp->getValue();//获取IDNode成员变量ValueNode*
								cout<<"向下类型转换没出问题"<<endl;
								node->addSize($3->value);//压入当前维度的空间大小
								cout<<"压入空间大小没出问题"<<endl;
								$$ = (ValueNode*)tmp;//给$$赋值
							  }
        | var '[' ']' {}
        | '(' var ')' { $$ = $2; }
        ;
def_stmt: types ids  { 
                        //Node *nodes[] = {new Node("Def_Stmt"), $1, $2};
                        //$$ = Node::createNode(new Node("Def_Stmt"), new Node("TEST"));
						setStatus(false);
                        $$ = Node::createNode(3, new Node("Def_Stmt"), $1, $2);
                        printf("def_stmt types ids\n");
                     }
        //| def_stmt ',' def_stmt    {printf("def_stmt second\n");}
        ;
if_stmt      : IF '(' cond_args ')' stmt {
                    Node *cond = Node::createNode(new Node("If Condition"), $3);
                    Node *body = Node::createNode(new Node("If Body"), $5);
                    $$ = Node::createNode(3, new Node("IF"), cond , body );
                    }
        | IF '(' cond_args ')' stmt ELSE stmt { 
                    Node *cond = Node::createNode(new Node("IF Condition"), $3);
                    Node *ifbody = Node::createNode(new Node("IF Body"), $5);
                    Node *elsebody = Node::createNode(new Node("ELSE Body"), $7);
                    $$ = Node::createNode(4, new Node("IF"), cond, ifbody, elsebody);
                 }
        //| expr '?' stmt ':' stmt {} put it in expr
        ;
while_stmt   : WHILE '(' cond_args ')' stmt  {
                    Node *cond = Node::createNode(new Node("While Condition"), $3);
                    Node *whilebody = Node::createNode(new Node("While body"), $5);
                    $$ = Node::createNode(3, new Node("WHILE"), cond, whilebody);
                }
        ;
do_while_stmt: DO stmt WHILE '(' cond_args ')'{
                    Node *body = Node::createNode(new Node("Do While Body"), $2);
                    Node *cond = Node::createNode(new Node("Do While Condition"), $5);
                    $$ = Node::createNode(3, new Node("DO WHILE"), body, cond);
                }
        ;
for_stmt: FOR '(' for_init_args ';' cond_args ';' for_action_args ')' stmt {
                    Node *init = Node::createNode(new Node("FOR args1"), $3);
                    Node *cond = Node::createNode(new Node("FOR args2"), $5);
                    Node *action = Node::createNode(new Node("FOR arg3"), $7);
                    Node *body = Node::createNode(new Node("FOR body"), $9);
                    $$ = Node::createNode(5, new Node("FOR"), init, cond, action, body);
                }
        ;
for_init_args: { $$ = new Node("NULL"); }
        | def_stmt {$$ = $1;}
        | ids   {$$ = $1;}
        ;
cond_args: expr         {$$ = $1;}
        | for_init_args {$$ = $1;} // TODO: def_stmt must have '='
        ;
for_action_args:    {$$ = new Node("NULL");}
        | expr      {$$ = $1;}
        ;
return_stmt  : RETURN expr {$$ = Node::createNode(new Node("RETURN"), $2);}
        ; 

%%

/////////////////////////////////////////////////////////////////////////////
// programs section

int main(void)
{
    int n = 1;
    mylexer lexer;
    myparser parser;
    freopen("d:\\read.txt", "r", stdin);
    freopen("d:\\out.txt", "w", stdout);
    if (parser.yycreate(&lexer)) {
        if (lexer.yycreate(&parser)) {
            n = parser.yyparse();
        }
    }
    return n;
}

