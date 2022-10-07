/*
 * Copyright (C) Rida Bazzi, 2020
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 *
 */
#include <iostream>
#include <unordered_map>
#include <vector>
#include <cstdlib>
#include "parser.h"

using namespace std;

//struct to hold info
struct argPair
{
    string name;
    int argCount;
    string variable;
    string monomial;
    int lineNum;
};

//int memory [1000];
//vector<int> input;
//unordered_map<string,int> symbolTable;
//int nextOpening = 0;

//global variables needed
vector<Token> error1, error5;
vector<int> error3, error4;
vector<argPair> polyDecls;
vector<argPair> polyEvals;
argPair tempPair;
argPair tempPair2;
int headerArgs2 = 1;
bool bool1 = true;

/*struct stmt
{
    int stmt_type;
    struct poly_eval* poly;
    int variable;
    struct stmt* next;
};*/

void errorCode1()
{
    string error = "Error Code 1: ";
    string dupes = "";
    string test = "";
    for(auto& it : error1)
    {
        Token temp = it;
        int freq = 0;

        for(auto& it2 : error1)
        {
            //counts number of appearances
            if(it.lexeme == it2.lexeme)
            {
                freq++;
            }
        }
        //appears more than once = error
        if(freq > 1)
        {
            //concats error line numbers
            string add = to_string(it.line_no);
            dupes += add + " ";
        }
    }
    //prints error w/ string containing error line numbers
    if(dupes.compare(test) != 0)
    {
        cout << error << dupes << endl;
    }
}

void errorCode2()
{
    string error = "Error Code 2: ";
    string invalid = "";
    vector<char> uniqueChars;
    for(auto& it : polyDecls)
    {
        string mono = it.monomial; //ex: var"x," mono"x,x,x,x,x,"
        string var = it.variable; //ex: var"p,q" mono"p,p,q,q,r,"
        //traverses list of monomials
        for(int i = 0; i < mono.size(); i+=2)
        {
            //looks for variable inside monomial
           if(var.find(mono[i]) == std::string::npos)
           {
               //not found
               uniqueChars.push_back(mono[i]); //vector holding non variables
               //concats error line numbers
               string add = to_string(it.lineNum);
               invalid += add + " ";
           }
        }
    }
    //prints error w/ string containing error line numbers
    if(uniqueChars.size() != 0)
    {
        cout << error << invalid << endl;
    }
}

void errorCode3()
{
    string error = "Error Code 3: ";
    string dupes = "";
    string test = "";
    bool exists = false; //does not exist by default
    //traverses poly eval structs
    for(auto& it : polyEvals)
    {
        //traverses poly decl structs
        for(auto& it2 : polyDecls)
        {
            //poly was declared
            if(it.name == it2.name)
            {
                exists = true;
            }
        }
        //poly was not declared
        if(exists == false)
        {
            error3.push_back(it.lineNum); //vector holding non declared poly line numbers
        }
        exists = false;
    }
    for(auto& it3 : error3)
    {
        //concats error line numbers
        string add = to_string(it3);
        dupes += add + " ";
    }
    //prints error w/ string containing error line numbers
    if(error3.size() != 0)
    {
        cout << error << dupes << endl;
    }
}

void errorCode4()
{
    string error = "Error Code 4: ";
    string dupes = "";
    string test = "";
    //traverses error 4 line numbers
    for(auto& it : error4)
    {
        //concats error line numbers
        string add = to_string(it);
        dupes += add + " ";
    }
    //prints error w/ string containing error line numbers
    if(dupes.compare(test) != 0)
    {
        cout << error << dupes << endl;
    }
}

void errorCode5()
{
    string error = "Error Code 5: ";
    string dupes = "";
    vector<string> lineNum; //vector holding error line numbers
    bool flag1 = false; //false by default
    //traverses poly eval structs
    for(auto& it : polyEvals)
    {
        string vars = it.variable; //saves struct's list of variables
        //traverses list of variables
        for(int i = 0;i < vars.size();i+=2)
        {
            string var = vars.substr(i,1); //saves single variable
            //traverses error5 vector
            for(auto& it2 : error5)
            {
                //checks if ID token is equal to single variable
                if(it2.lexeme == var)
                {
                    flag1 = true; //sets flag to true
                    break;
                }
            }
            //pushes error line number into vector
            if(flag1 == false)
            {
                lineNum.push_back(to_string(it.lineNum));
            }
        }
        flag1 = false;
    }
    //traverses error line number vector
    for(auto& it3 : lineNum)
    {
        //concats error line numbers
        dupes = dupes + it3 + " ";
    }
    //prints error w/ string containing error line numbers
    if(lineNum.size() != 0)
    {
        cout << error << dupes << endl;
    }
}

void Parser::syntax_error()
{
    cout << "SYNTAX ERROR !!&%!!\n";
    exit(1);
}

// this function gets a token and checks if it is
// of the expected type. If it is, the token is
// returned, otherwise, synatx_error() is generated
// this function is particularly useful to match
// terminals in a right hand side of a rule.
// Written by Mohsen Zohrevandi
Token Parser::expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();
    return t;
}

// Parsing
// This function is simply to illustrate the GetToken() function
// you will not need it for your project and you can delete it.
// the function also illustrates the use of peek()
void Parser::ConsumeAllInput()
{
    Token token;
    int i = 1;
    
    token = lexer.peek(i);
    token.Print();
    while (token.token_type != END_OF_FILE)
    {
        i = i+1;
        token = lexer.peek(i);
        token.Print();
    }
   
    token = lexer.GetToken();
    token.Print();
    while (token.token_type != END_OF_FILE)
    {
        token = lexer.GetToken();
        token.Print();
    }
}

void Parser::parse_input()
{
    parse_program();
    parse_inputs();
    expect(END_OF_FILE);
}

void Parser::parse_program()
{
    parse_poly_decl_section();
    parse_start();
}

void Parser::parse_poly_decl_section()
{
    parse_poly_decl();
    Token tok;
    tok = lexer.peek(1);
    if(tok.token_type == POLY)
    {
        parse_poly_decl_section();
    }
}

void Parser::parse_poly_decl()
{
    expect(POLY);
    parse_polynomial_header();
    expect(EQUAL);
    parse_polynomial_body();
    expect(SEMICOLON);
    polyDecls.push_back(tempPair); //saves poly decl struct
}

void Parser::parse_polynomial_header()
{
    Token tok2;
    tok2 = lexer.peek(1);
    string polyname = tok2.lexeme;

    //poly decl struct
    tempPair.name = polyname;
    tempPair.lineNum = tok2.line_no;
    tempPair.argCount = 1;
    tempPair.variable = "";

    parse_polynomial_name();
    Token tok;
    tok = lexer.peek(1);
    if(tok.token_type == LPAREN)
    {
        expect(LPAREN);
        parse_id_list();
        expect(RPAREN);
    }
    //no parenthesis sets 'x' to default variable
    if(tempPair.variable.compare("") == 0)
    {
        tempPair.variable = "x";
    }
}

void Parser::parse_id_list()
{
    Token tok2 = expect(ID);
    string firstVar = tok2.lexeme;

    tempPair.variable += firstVar + " "; //list of variables

    Token tok;
    tok = lexer.peek(1);
    if(tok.token_type == COMMA)
    {
        expect(COMMA);
        tempPair.argCount += 1; //increments number of args
        parse_id_list();
    }
}

void Parser::parse_polynomial_name()
{
    Token tokName = expect(ID);
    //error1
    if(bool1 == true)
    {
        error1.push_back(tokName);
    }
}

void Parser::parse_polynomial_body()
{
    parse_term_list();
}

void Parser::parse_term_list()
{
    parse_term();
    Token tok;
    tok = lexer.peek(1);
    if(tok.token_type == PLUS || tok.token_type == MINUS)
    {
        parse_add_operator();
        parse_term_list();
    }
}

void Parser::parse_term()
{
    Token tok;
    tok = lexer.peek(1);
    if(tok.token_type == ID)
    {
        parse_monomial_list();
    }
    else if(tok.token_type == NUM)
    {
        parse_coefficient();
        tok = lexer.peek(1);
        if(tok.token_type == ID)
        {
            parse_monomial_list();
        }
    }
    else
    {
        syntax_error();
    }
}

void Parser::parse_monomial_list()
{
    parse_monomial();
    Token tok;
    tok = lexer.peek(1);
    if(tok.token_type == ID)
    {
        parse_monomial_list();
    }
}
void Parser::parse_monomial()
{
    Token mon = expect(ID);
    tempPair.monomial = mon.lexeme + ","; //list of monomials
    Token tok;
    tok = lexer.peek(1);
    if(tok.token_type == POWER)
    {
        parse_exponent();
    }
}

void Parser::parse_exponent()
{
    expect(POWER);
    expect(NUM);
}

void Parser::parse_add_operator()
{
    Token tok;
    tok = lexer.peek(1);
    if(tok.token_type == PLUS)
    {
        expect(PLUS);
    }
    else if(tok.token_type == MINUS)
    {
        expect(MINUS);
    }
}

void Parser::parse_coefficient()
{
    expect(NUM);
}

void Parser::parse_start()
{
    expect(START);
    parse_statement_list();
}

void Parser::parse_inputs()
{
    expect(NUM);
    Token tok;
    tok = lexer.peek(1);
    if(tok.token_type == NUM)
    {
        parse_inputs();
    }
}

void Parser::parse_statement_list()
{
    parse_statement();
    Token tok;
    tok = lexer.peek(1);
    if(tok.token_type == INPUT || tok.token_type == ID)
    {
        parse_statement_list();
    }
}

void Parser::parse_statement()
{
    Token tok;
    tok = lexer.peek(1);
    if(tok.token_type == INPUT)
    {
        parse_input_statement();
    }
    else if(tok.token_type == ID)
    {
        parse_poly_evaluation_statement();
    }
    else
    {
        syntax_error();
    }
}

void Parser::parse_poly_evaluation_statement()
{
    parse_polynomial_evaluation();
    expect(SEMICOLON);
}

void Parser::parse_input_statement()
{
    //struct stmt* st = new stmt;
    //bool exists = false;
    Token tok;
    expect(INPUT);
    tok = expect(ID);
    error5.push_back(tok);
    expect(SEMICOLON);

    /*for (auto i = symbolTable.begin(); i != symbolTable.end(); i++)
    {
        if(i->first == tok.lexeme)
        {
            exists = true;
        }
    }
    if(exists == false)
    {
        symbolTable.insert({tok.lexeme, nextOpening});
        st->stmt_type = INPUT;
        nextOpening++;
    }*/
    //return st;
}

void Parser::parse_polynomial_evaluation()
{
    bool1 = false;
    //poly eval struct
    Token peekTok = lexer.peek(1);
    string peekName = peekTok.lexeme;
    int peekIndex = peekTok.line_no;
    tempPair2.name = peekName;
    tempPair2.lineNum = peekIndex;

    parse_polynomial_name();
    expect(LPAREN);
    parse_argument_list();
    Token peekTok2 = expect(RPAREN);

    polyEvals.push_back(tempPair2); //sends struct to vector

    //error4
    int tempCount = 0;
    string tempName = "";
    for(auto& it : polyDecls)
    {
        tempName = it.name;
        tempCount = it.argCount;
        if(tempName.compare(peekName) == 0)
        {
            break;
        }
    }
    if(tempCount != headerArgs2)
    {
        error4.push_back(peekIndex); //num of decl arguments do not match num of eval arguments
    }
    headerArgs2 = 1;
}

void Parser::parse_argument_list()
{
    parse_argument();
    Token tok;
    tok = lexer.peek(1);
    if(tok.token_type == COMMA)
    {
        headerArgs2++; //increments arg amount
        expect(COMMA);
        parse_argument_list();
    }
    tempPair2.argCount = headerArgs2; //sets arg amount
}

void Parser::parse_argument()
{
    Token tok, tok2;
    tok = lexer.peek(1);
    tok2 = lexer.peek(2);
    if(tok.token_type == NUM)
    {
        expect(NUM);
        return;
    }
    else if(tok.token_type == ID && tok2.token_type == LPAREN)
    {
        parse_polynomial_evaluation();
        return;
    }
    else if(tok.token_type == ID && (tok2.token_type == COMMA || tok2.token_type == RPAREN))
    {
        Token evalVar;
        evalVar = expect(ID);
        tempPair2.variable = evalVar.lexeme + " "; //list of variables
        return;
    }
    else
    {
        syntax_error();
    }
}

int main()
{
	// note: the parser class has a lexer object instantiated in it. You should not be declaring
    // a separate lexer object. You can access the lexer object in the parser functions as shown in the
    // example method Parser::ConsumeAllInput
    // If you declare another lexer object, lexical analysis will not work correctly
    Parser parser;

	//parser.ConsumeAllInput();
	parser.parse_input();
	errorCode1(); //15/15
    //errorCode2(); //conflicts with others
    //something in EC 3/4 loses a test case for EC 1
    errorCode3();
    errorCode4();
    //errorCode5(); //conflicts with others
}
