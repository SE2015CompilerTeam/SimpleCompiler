%{
#pragma warning( disable : 4996)
#include <fstream>
#include <stdio.h>
#include "mylexer.h"
#include "driver.h"
#include "myparser.h"
extern int yylex();
extern void yyerror(const char* msg);
extern int yylineno;
extern int lineno;
//extern char* yytext;
//extern int yyleng;
using namespace std;


%}
%union {
    class Node *nodes;
    class ValueNode *value_n;
    class ExprNode *expr_n;
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
%token<str>              STR
%token<id_n>             ID
%token<token>            PROC_CONTROL  ACCESS_CONTROL ERR_CONTROL USING NAMESPACE DEFINE
%token<str>              READ WRITE 
%token<token>            CLASS RETURN NEW DEL THIS
%token<token>            ';'
%token<token>            '#'

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

%token<token>               FUNC_PRE
%token<type_n>            INT DOUBLE CHAR FLOAT BOOL VOID STRUCT MAIN
%token<nodes>            IF ELSE THEN WHILE DO FOR GOTO SWITCH CASE DEFAULT
//%type <value_n> vals
%type <nodes> stmts block
%type <nodes> stmt def_stmt if_stmt while_stmt do_while_stmt for_stmt return_stmt
%type <nodes> for_init_args for_action_args cond_args
%type <value_n> expr expritem
%type <nodes> types
%type <nodes> initlist // �������ɶtype�ء���
%type <value_n> vals var varexpr
%type <nodes> ids  exprlist

%start program
%%
program : INT MAIN '(' ')' block {Node::printTree($5, 0);} // TODO: main args
        ;
types: INT  {$$ = $1; printf("types INT\n"); setTypes(Value_Type::type_int); setStatus(true);}
     | DOUBLE   {$$ = $1; setTypes(Value_Type::type_double); setStatus(true);}
     | FLOAT    {$$ = $1; setTypes(Value_Type::type_double); setStatus(true);}
     | CHAR {$$ = $1; setTypes(Value_Type::type_char); setStatus(true);}
     | VOID {$$ = $1; setTypes(Value_Type::type_void); setStatus(true);}
     | BOOL {$$ = $1; setTypes(Value_Type::type_bool); setStatus(true);}
     //| types '*' { }
     ;

block : '{' stmts '}' {$$ = Node::createNode(new Node("Block"), $2);}
      ;
stmts   : /* empty */ {$$ = new Node("NULL");}
        | stmt {$$ = new Node("Stmts"); $$->addChildren($1);}
        | stmts stmt {$$ = $1; $$->addChildren($2);}
        ;
stmt    : def_stmt ';'{$$ = $1; }
        | if_stmt {$$ = $1;}
        | while_stmt {$$ = $1;}
        | do_while_stmt {$$ = $1;}
        | for_stmt {$$ = $1;}
        | return_stmt ';'{$$ = $1; }
        //| error SEMICOLON { $$ = new GeneralNode(INVALID); yyerrok(); }
        | exprlist ';' {$$ = $1; }
        | block {$$ = $1;}
        | READ '(' expr ')' ';' { 
                                    $$ = $1;
                                    ValueNode* temp = ValueNode::extractInterValue((ValueNode*)$3);
                                    $1->name = new char[strlen(temp->getValue()) + 1];
                                    strcpy_s($1->name, strlen(temp->getValue()) + 1,temp->getValue());
                                    
                                }
        | WRITE '(' expr ')' ';'{ 
                                    $$ = $1;
                                    ValueNode* temp = ValueNode::extractInterValue((ValueNode*)$3);
                                    $1->name = new char[strlen(temp->getValue()) + 1];
                                    strcpy_s($1->name, strlen(temp->getValue()) + 1,temp->getValue());
                                   
                                }
        | ';' {$$ = new Node("Empty Stmt");  }
        ;
initlist: '{' exprlist '}' {    $$ = Node::createNode(new Node("Initlist"), $2);}
        | '{' exprlist ',' '}' {$$ = Node::createNode(new Node("Initlist"), $2);}
        ;
exprlist: exprlist ',' expritem {$$ = $1; $$->addChildren($3);}
        | expritem {    
                        $$ = new Node("Expr List");
                        $$->addChildren($1);
                        }
        ;
expritem: expr {$$ = $1; }
        | initlist { 
                    //$$ = $1; 
        }
        ;
vals:     INTEGER   {$$ = $1;}
        | DBL   {$$ = $1;}
        | CHR   {$$ = $1;}
        | STR   {$$ = $1;}
        | TRUE  {$$ = $1;}
        | FALSE {$$ = $1;}
        | NUL   {$$ = $1;}
        ;
/*func_def_stmt: types ID '(' def_stmt ')' block   {printf("func_def\n");}
        ;*/
expr    : '(' expr ')' {    $$ = $2; 
                            ValueNode* v = ValueNode::extractInterValue($$);
                            
                            }
        | expr PA  expr {
                            $$ = new ExprNode("+=", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                           
                            }
        | expr MNA expr {   // �ȼ���exprֵ �ٸ������ID
                            $$ = new ExprNode("-=", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                           }
        | expr MA  expr {   
                            $$ = new ExprNode("*=", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                           
                            }
        | expr DA  expr {   $$ = new ExprNode("/=", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            
                            }
        | expr MOA expr {   $$ = new ExprNode("%=", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                          
                            }
        | expr ORA expr {   $$ = new ExprNode("|=", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            
                            }
        | expr XORA expr{   $$ = new ExprNode("^=", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                           
                            }
        | expr AA  expr {   $$ = new ExprNode("&=", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            
                            }
        | expr '+' expr {  
                            $$ = new ExprNode("+", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            
                            }
                            
        | expr '-' expr {   $$ = new ExprNode("-", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                           }
                            
        | expr '*' expr {   $$ = new ExprNode("*", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                             }
                             
        | expr '/' expr {
                            $$ = new ExprNode("/", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            
                            }
                            
        | expr '%' expr {   $$ = new ExprNode("%", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                           
                            }
        | expr LL expr  {   $$ = new ExprNode("<<", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                         
                            }
        | expr RR expr  {   $$ = new ExprNode("<<", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            
                            }
        | expr '|' expr {   
                            $$ = new ExprNode("|", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            }
        | expr '^' expr {   
                            $$ = new ExprNode("^", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            }
        | expr '&' expr {   $$ = new ExprNode("&", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            }
        | expr EQ expr  {   $$ = new ExprNode("==", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            
                            }
        | expr NEQ expr {   $$ = new ExprNode("!=", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
                            }
        | expr CMP expr {   
                            $$ = new ExprNode($2, $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
         }
        | expr OR expr  {   $$ = new ExprNode("||", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
        }
        | expr AND expr {   $$ = new ExprNode("&&", $1, $3);
                            $$->addChildren($1);  $$->addChildren($3);
         }
        | PP expr  %prec RA{//$$ = $2;
                            
                            $$ = new ExprNode("+++", $2, nullptr);
                            $$->addChildren($2);
                        }
        | MM expr  %prec RA{
                            $$ = new ExprNode("---", $2, nullptr);
                            $$->addChildren($2);
                        }
        | '-' expr %prec '*' {
                            $$ = new ExprNode("-", $2, nullptr);
                            $$->addChildren($2);}
        | expr PP{      
                        $$ = new ExprNode("++", $1, nullptr);
                        $$->addChildren($1);
                       // printf("expr PP\n");
                    }
        | expr MM{      
                        $$ = new ExprNode("++", $1, nullptr);
                        $$->addChildren($1);
                    }
        | '!' expr {    
                        $$ = new ExprNode("!", $2, nullptr);
                        $$->addChildren($2);
                        } // TODO: temp..
        | '~' expr {  
                        $$ = new ExprNode("~", $2, nullptr);
                        $$->addChildren($2);
        }
        | vals  {
                    $$ = $1;
                 //   $$ = new ValueNode("1234");
        }
        //| var   {$$ = $1;printf("expr ID \n");}
        | varexpr {$$ = $1;}
        //| MUL expr %prec UDEREF { }
        //| BITAND expr %prec UREF { }
        //| expr LBRACK expr RBRACK %prec SUB {}
        ;
ids     : varexpr {
                    $$ = Node::createNode(new Node("ID List"), $1);
                   
                    }
        | ids ',' varexpr {
                    $$ = $1; $$->addChildren($3);  
                   
                    }
        ;
varexpr : var { 
                    IDNode* symbol = handleVarExpr((IDNode*)$1);
                    if(symbol == nullptr){
                        $$ = new ValueNode("ERROR NODE");
                    }else{
                        $$ = symbol;
                    }
                    
                    }
        | var '=' expritem { 
                            // �����(δ)����, �����ҵ��Ľ��(δ�ҵ�ʱ������ű����ش������)
                          //  setAssign(true);
                            IDNode* symbol = handleVarExpr((IDNode*)$1);
                            if(symbol == nullptr){
                                $$ = new ValueNode("ERROR NODE");    
                            }
                            else{
                                IDNode* idNode = (IDNode*)$1;
                                $$ = new ExprNode("=", symbol, $3);
                                $$->addChildren(symbol);  $$->addChildren($3);
                            }
                             // $3 Ҳ����δ�����δ��ʼ�����������㺯������   
                        }
        ;
var     : ID {
                $$ = $1;
               
             }
        | var '[' INTEGER ']' {
                                if(isDefining()){//�������
                                    //��ÿһά�ռ��Сѹ���ȥ
                                    IDNode* idNode = (IDNode*)$1;//��ת����IDNode*
                                    if(idNode->getValue()==nullptr){//���û�г�ʼ����Ա����ValueNode���ȳ�ʼ��
                                        ArrayNode* arrNode = new ArrayNode();
                                        idNode->mySetValue(arrNode);
                                    }
                                    ValueNode* temp = idNode->getValue();
                                    ArrayNode* arrNodeInUsed = (ArrayNode*) temp;
                                    arrNodeInUsed->addSize($3->getValue());//ѹ�뵱ǰά�ȵĿռ��С
                                    $$ = idNode;//��$$��ֵ
                                }
                                else {//��ֵ���
                                    // $$ = $1->getChild($3->getValue());
                                }
                              }
        | var '[' ']' {}
        | '(' var ')' { $$ = $2; }
        ;
def_stmt: types ids  { 
                        //Node *nodes[] = {new Node("Def_Stmt"), $1, $2};
                        //$$ = Node::createNode(new Node("Def_Stmt"), new Node("TEST"));
                        setStatus(false);
                        $$ = Node::createNode(3, new Node("Def_Stmt"), $1, $2);
                     }
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
    //freopen("d:\\read.txt", "r", stdin);
    //freopen("d:\\out.txt", "w", stdout);
    if (parser.yycreate(&lexer)) {
        if (lexer.yycreate(&parser)) {
            n = parser.yyparse();
        }
    }
    return n;
}

