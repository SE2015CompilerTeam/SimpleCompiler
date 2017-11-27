%{
#pragma warning( disable : 4996)
#include "mylexer.h"
#include "myparser.h"
#include "driver.h"
#include <map>
#include <stdlib.h>

%}

/////////////////////////////////////////////////////////////////////////////
// declarations section

// lexical analyser name
%name mylexer

// class definition
{
    // place any extra class members here
}

// constructor
{
    // place any extra initialisation code here
}

// destructor
{
    // place any extra cleanup code here
}

// place any declarations here
str  "\""[^"]*"\""

comment "/*"([^\*]|(\*)*[^\*/])*(\*)*"*/"|"//".*

integer ([+-]?[0-9]+)|('[.]')
double  ([+-]?[0-9]*\.[0-9]*)|('[.]')

space   [" "\t\n]+

bit     \&|\||\^|\~|\>\>|\<\<

cmp     \>=?|\<=?|==

chr    '(\\)?[^']'

id      [a-zA-Z_][a-zA-Z0-9_]*

kexue    ({integer}|{double})[e]{integer}
%%

/////////////////////////////////////////////////////////////////////////////
// rules section

%{
//this code will be added into yyaction function
    YYSTYPE YYFAR& yylval = *(YYSTYPE YYFAR*)yyparserptr->yylvalptr;
%}

// place your Lex rules here
"main"      {return MAIN;}
"if"        {return IF;}

"else"        {return ELSE;}
"then"        {return THEN;}

"while"        {return WHILE;}

"do"            {return DO;}

"for"        {return FOR;}

"switch"        {return SWITCH;}

"case"        {return CASE;}

"default"        {return DEFAULT;}
"using"         {return USING;}
"namespace"     {return NAMESPACE;}
"break"|"continue"     {return PROC_CONTROL;}
"return"        {return RETURN;}
"new"           {return NEW;}
"this"          {return THIS;}
"define"        {return DEFINE;}
"try"|"catch"|"finally"|"throw"         {return ERR_CONTROL;}
"class"|"struct"                        {return CLASS;}
"public"|"private"|"protected"          {return ACCESS_CONTROL;}
"const"|"static"                        {return FUNC_PRE;}                     
"int"           {yylval.type_n = new TypeNode(); return INT;}
"double"        {yylval.type_n = new TypeNode(Value_Type::type_double);return DOUBLE;}
"char"          {yylval.type_n = new TypeNode(Value_Type::type_char);return CHAR;}    
"float"         {yylval.type_n = new TypeNode(Value_Type::type_double);return FLOAT;}
"bool"          {yylval.type_n = new TypeNode();return BOOL;}
"void"          {yylval.type_n = new TypeNode(Value_Type::type_void);return VOID;} 
                               
"read"          {yylval.str = new StringNode("read");return READ;}
"write"         {yylval.str = new StringNode("write");return WRITE;}
"++"       {return PP;}
"--"       {return MM;}
"->"       {return RA;}
"::"       {return NSP;}

"+="       {return PA;}
"-="       {return MNA;}
"*="       {return MA;}
"/="       {return DA;}
"&="       {return AA;}
"|="       {return ORA;}
"^="       {return XORA;}
"%="       {return MOA;}
"=="       {return EQ;}
"!="       {return NEQ;}
"#"        {return '#';}
"+"        {return '+';}
"-"        {return '-';}
"*"        {return '*';}
"/"        {return '/';}
"%"        {return '%';}
"="        {return '=';}
"{"        {return '{';}
"}"        {return '}';}
"("        {return '(';}
")"        {return ')';}
"["        {return '[';} 
"]"        {return ']';}
","        {return ',';}
"."        {return '.';}
":"        {return ':';}
";"        {return ';';}
"!"        {return '!';}
"?"        {return '?';}
"^"        {return '^';}
"&&"       {return AND;}
"||"       {return OR;}
"&"        {return '&';}
"|"        {return '|';}
"~"        {return '~';}
">>"       {return RR;}
"<<"       {return LL;}


{comment}   {}
{str}      {yylval.str = new StringNode(yytext);return STR;}
{chr}      {
                yylval.char_n = new CharNode(yytext[1]); //TODO: \n \t !!
                return CHR;
           }
{integer}   {yylval.int_n = new IntNode(atoi(yytext));return INTEGER;}
{double}    {yylval.double_n = new DoubleNode(atof(yytext));return DBL;}
"NULL"          {yylval.str = new StringNode("NULL");return NUL;}
"true"          {yylval.int_n = new IntNode(1); return TRUE;}
"false"         {yylval.int_n = new IntNode(0); return FALSE;}
{cmp}       {
                char *name = new char[LEN+1];
                strcpy_s(name,LEN+1, yytext);
                yylval.name = name;
                return CMP;
            }

{id}        {
                char *name = new char[LEN];
                strcpy_s(name,LEN+1,yytext);
                int i = 10;
                i++;
                yylval.id_n = new IDNode(name, Value_Type::type_int, yylineno);
               // addID(name, yylval.id_n);
                return ID;
            }

{space}     {}

{kexue}     {yylval.double_n = new DoubleNode(atof(yytext));return DBL;}
%%

/////////////////////////////////////////////////////////////////////////////
// programs section

