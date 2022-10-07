/*
 * Name: Richard Marquez Cortes
 * ASU ID: 1214775731
 */

#include <iostream>
#include <cstdlib>
#include <vector>
#include <istream>
#include <sstream>
#include <algorithm>
#include "parser.h"
#include "lexer.h"
#include "compiler.h"
using namespace std;

vector<string> variables; //vector that holds variable names

void Parser::syntax_error()
{
    cout << "SYNTAX ERROR\n";
    exit(1);
}

// this function gets a token and checks if it is
// of the expected type. If it is, the token is
// returned, otherwise, syntax_error() is generated
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

InstructionNode *Parser::program()
{
    var_section();
    InstructionNode *bod = body(); 
    parser_inputs();
    expect(END_OF_FILE);
    return bod;
}

void Parser::var_section()
{
    id_list();
    expect(SEMICOLON);
}

void Parser::id_list()
{
    Token id = expect(ID);
    variables.push_back(id.lexeme); //saves variables into vector
    mem[next_available] = 0; //saves spot in memory
    next_available++;

    Token tok = lexer.peek(1);
    if(tok.token_type == COMMA) //more than one ID
    {
        expect(COMMA);
        id_list();
    }
}

InstructionNode *Parser::body()
{
    expect(LBRACE);
    InstructionNode *head = NULL;
    stmt_list(head); //statement body
    expect(RBRACE);
    return head;
}

void Parser::stmt_list(InstructionNode *&head)
{
    InstructionNode *statement = stmt(); //statement
    if(head == NULL) //no statements
    {
        head = statement;
    }
    else //more than one statement
    {
        InstructionNode *temp = head;
        while(temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = statement;
    }

    Token tok = lexer.peek(1);
    if(tok.token_type == ID || tok.token_type == WHILE || tok.token_type == IF
    || tok.token_type == SWITCH || tok.token_type == FOR || tok.token_type == OUTPUT || tok.token_type == INPUT) //multiple statements
    {
        stmt_list(statement);
    }
}

InstructionNode *Parser::stmt()
{
    InstructionNode *statement = new InstructionNode;
    statement->next = NULL;

    Token tok = lexer.peek(1);
    if(tok.token_type == ID)
    {
        statement = assign_stmt(); //ASSIGN
    }
    else if(tok.token_type == WHILE)
    {
        statement = while_stmt(); //JMP
    }
    else if(tok.token_type == IF)
    {
        statement = if_stmt(); //CJMP
    }
    else if(tok.token_type == SWITCH)
    {
        statement = switch_stmt(); //CJMP
    }
    else if(tok.token_type == FOR)
    {
        statement = for_stmt(); //JMP
    }
    else if(tok.token_type == OUTPUT)
    {
        statement = output_stmt(); //OUT
    }
    else if(tok.token_type == INPUT)
    {
        statement = input_stmt(); //IN
    }
    return statement;
}

InstructionNode *Parser::assign_stmt()
{
    InstructionNode *statement = new InstructionNode;
    Token id = expect(ID);
    statement->assign_inst.left_hand_side_index = location(id.lexeme);

    expect(EQUAL);
    Token tok = lexer.peek(1);
    Token tok2 = lexer.peek(2);
    if((tok.token_type == ID || tok.token_type == NUM) && (tok2.token_type == PLUS ||
    tok2.token_type == MINUS || tok2.token_type == MULT || tok2.token_type == DIV)) //expression
    {
        //assigns defaults
        statement->type = ASSIGN;
        statement->assign_inst.operand1_index = 0;
        statement->assign_inst.operand2_index = 0;
        statement->assign_inst.op = OPERATOR_NONE;
        statement->next = NULL;
        expr(statement);
        //cout << "operand1 index: " << statement->assign_inst.operand1_index << endl; //testing
        //cout << "operand2 index: " << statement->assign_inst.operand2_index << endl; //testing
        //cout << statement->assign_inst.op << endl; //testing
    }
    else if((tok.token_type == ID || tok.token_type == NUM) && tok2.token_type == SEMICOLON)  //ID or NUM
    {
        //assigns ID or NUM's location in memory and defaults
        statement->type = ASSIGN;
        int memLocation = check(primary());
        statement->assign_inst.operand1_index = memLocation;
        statement->assign_inst.operand2_index = NULL;
        statement->assign_inst.op = OPERATOR_NONE;
        statement->next = NULL;
    }
    expect(SEMICOLON);
    return statement;
}

void Parser::expr(InstructionNode *&state)
{
    int memLocation = check(primary());
    state->assign_inst.operand1_index = memLocation; //first operand
    Token tok = op();
    if(tok.token_type == PLUS) // +
    {
        state->assign_inst.op = OPERATOR_PLUS;
    }
    else if(tok.token_type == MINUS) // -
    {
        state->assign_inst.op = OPERATOR_MINUS;
    }
    else if(tok.token_type == MULT) // *
    {
        state->assign_inst.op = OPERATOR_MULT;
    }
    else if(tok.token_type == DIV) // /
    {
        state->assign_inst.op = OPERATOR_DIV;
    }
    memLocation = check(primary());
    state->assign_inst.operand2_index = memLocation; //second operand
}

Token Parser::primary()
{
    Token save;
    Token tok = lexer.peek(1);
    if(tok.token_type == ID) //variable ID
    {
        save = expect(ID);
        //cout << "ID: " << save.lexeme << endl; //testing
    }
    else if(tok.token_type == NUM) //integer
    {
        save = expect(NUM);
        mem[next_available] = stoi(save.lexeme);
        //cout << "test: " << mem[next_available] << endl; //testing
        next_available++;
    }
    return save;
}

Token Parser::op()
{
    Token tok = lexer.peek(1);
    if(tok.token_type == PLUS) // +
    {
        expect(PLUS);
    }
    else if(tok.token_type == MINUS) // -
    {
        expect(MINUS);
    }
    else if(tok.token_type == MULT) // *
    {
        expect(MULT);
    }
    else if(tok.token_type == DIV) // /
    {
        expect(DIV);
    }
    return tok;
}

InstructionNode *Parser::output_stmt()
{
    InstructionNode *out = new InstructionNode;
    out->type = OUT;
    expect(OUTPUT);
    Token tok = expect(ID); //saves output token
    out->output_inst.var_index = check(tok); //output location
    out->next = NULL;
    expect(SEMICOLON);
    return out;
}

InstructionNode *Parser::input_stmt()
{
    InstructionNode *input = new InstructionNode;
    input->type = IN;
    expect(INPUT);
    Token tok = expect(ID); //saves input token
    input->input_inst.var_index = check(tok); //input location
    input->next = NULL;
    expect(SEMICOLON);
    return input;
}

InstructionNode *Parser::while_stmt()
{
    InstructionNode *noop = new InstructionNode;
    noop->type = NOOP;
    noop->next = NULL;

    expect(WHILE);
    InstructionNode *whileState = new InstructionNode; //while statement
    whileState = condition();
    whileState->type = CJMP;
    whileState->next = body();

    InstructionNode *target = new InstructionNode; //while's target
    target->type = JMP;
    target->jmp_inst.target = whileState;
    target->next = noop;

    whileState->cjmp_inst.target = noop;
    if(whileState->next == NULL) //while body is empty
    {
        whileState->next = noop;
    }
    else //while body is not empty
    {
        InstructionNode *last = new InstructionNode;
        last = whileState->next;
        while(last->next != NULL)
        {
            last = last->next;
        }
        last->next = target;
    }
    return whileState;
}

InstructionNode *Parser::if_stmt()
{
    expect(IF);
    InstructionNode *ifState = new InstructionNode; //if statement
    ifState = condition(); //if condition
    ifState->type = CJMP;
    ifState->next = body();

    InstructionNode *noop = new InstructionNode;
    noop->type = NOOP;
    noop->next = NULL;

    if(ifState->next == NULL) //if body is empty
    {
        ifState->next = noop;
    }
    else //if body is not empty
    {
        InstructionNode *last = new InstructionNode;
        last = ifState->next;
        while(last->next != NULL)
        {
            last = last->next;
        }
        last->next = noop;
    }
    ifState->cjmp_inst.target = noop; //sets if statement target
    return ifState;
}

InstructionNode *Parser::condition()
{
    InstructionNode *cond = new InstructionNode;
    cond->cjmp_inst.operand1_index = check(primary()); //first operand
    Token tok = relop();
    if(tok.token_type == GREATER) // >
    {
        cond->cjmp_inst.condition_op = CONDITION_GREATER;
    }
    else if(tok.token_type == LESS) // <
    {
        cond->cjmp_inst.condition_op = CONDITION_LESS;
    }
    else if(tok.token_type == NOTEQUAL) // !=
    {
        cond->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
    }
    cond->cjmp_inst.operand2_index = check(primary()); //second operand
    return cond;
}

Token Parser::relop()
{
    Token tok = lexer.peek(1);
    if(tok.token_type == GREATER) // >
    {
        expect(GREATER);
    }
    else if(tok.token_type == LESS) // <
    {
        expect(LESS);
    }
    else if(tok.token_type == NOTEQUAL) // !=
    {
        expect(NOTEQUAL);
    }
    return tok;
}

InstructionNode *Parser::switch_stmt()
{
    InstructionNode *noop = new InstructionNode;
    noop->type = NOOP;
    noop->next = NULL;

    InstructionNode *head = new InstructionNode;
    head = NULL;

    expect(SWITCH);
    Token tok = expect(ID);
    int index = location(tok.lexeme);
    expect(LBRACE);

    case_list(index, head, noop); //list of cases for switch

    Token tok2 = lexer.peek(1);
    if(tok2.token_type == DEFAULT) //checks for default case in switch
    {
        InstructionNode *defaultCase = default_case(); //default statement
        if(head == NULL) //empty switch points to default case
        {
            head = defaultCase;
        }
        else //default case added to the end of switch
        {
            InstructionNode *temp = head;
            while(temp->next != NULL)
            {
                temp = temp->next;
            }
            temp->next = defaultCase;
        }
    }
    expect(RBRACE);
    if(head == NULL) //switch is empty
    {
        head = noop;
    }
    else //switch is not empty
    {
        InstructionNode *temp2 = head;
        while(temp2->next != NULL)
        {
            temp2 = temp2->next;
        }
        temp2->next = noop;
    }
    return head;
}

InstructionNode *Parser::for_stmt()
{
    InstructionNode *noop = new InstructionNode;
    noop->type = NOOP;
    noop->next = NULL;

    expect(FOR);
    expect(LPAREN);

    InstructionNode *assign = assign_stmt();
    assign->type = ASSIGN;
    InstructionNode *forState = condition(); //condition statement
    forState->type = CJMP;
    forState->cjmp_inst.target = noop;
    assign->next = forState;
    expect(SEMICOLON);

    InstructionNode *assign2 = assign_stmt();
    assign2->type = ASSIGN;
    expect(RPAREN);

    InstructionNode *target = new InstructionNode; //target statement
    target->type = JMP;
    target->next = noop;
    target->jmp_inst.target = forState;

    assign2->next = target;
    forState->next = body();
    if(forState->next == NULL) //for body is empty
    {
        forState->next = assign2;
    }
    else //for body is not empty
    {
        InstructionNode *last = new InstructionNode;
        last = forState->next;
        while(last->next != NULL)
        {
            last = last->next;
        }
        last->next = assign2;
    }
    return assign;
}

void Parser::case_list(int location, InstructionNode *&head, InstructionNode *noop)
{
    InstructionNode *statement = parser_case(location,noop); //first case
    if(head == NULL) //list is empty
    {
        head = statement;
    }
    else //list is not empty
    {
        InstructionNode *temp;
        temp = head;
        while(temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = statement;
    }
    Token tok = lexer.peek(1);
    if(tok.token_type == CASE) //more than one case
    {
        case_list(location, head, noop);
    }
}

InstructionNode *Parser::parser_case(int location, InstructionNode *noop)
{
    InstructionNode *statement = new InstructionNode; //new case
    expect(CASE);

    Token tok = expect(NUM);
    int value = stoi(tok.lexeme);
    mem[next_available] = value; //save num into memory
    next_available++;
    int num = check(tok);

    expect(COLON);
    statement->next = NULL;
    InstructionNode *stateBody = body(); //save body

    //assign statement values and jump target
    statement->type = CJMP;
    statement->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
    statement->cjmp_inst.operand1_index = location;
    statement->cjmp_inst.operand2_index = num;
    statement->cjmp_inst.target = stateBody;

    if(stateBody == NULL) //body is empty
    {
        stateBody = noop;
    }
    else //body is not empty
    {
        while(stateBody->next != NULL)
        {
            stateBody = stateBody->next;
        }
        stateBody->next = noop;
    }
    return statement;
}

InstructionNode *Parser::default_case()
{
    expect(DEFAULT);
    expect(COLON);
    InstructionNode *bod = body(); //default's body
    return bod;
}

void Parser::parser_inputs()
{
    num_list();
}

void Parser::num_list()
{
    Token num = expect(NUM);
    int value = stoi(num.lexeme);
    inputs.push_back(value); //add to vector of inputs

    Token tok = lexer.peek(1);
    if(tok.token_type == NUM) //more than one num
    {
        num_list();
    }
}

struct InstructionNode *parse_generate_intermediate_representation() //from compiler
{
    Parser parse;
    InstructionNode *representation = parse.program(); //program
    return representation;
}

int Parser::location(string name) //variable name
{
    for(int i = 0; i < variables.size(); i++)
    {
        if(variables.at(i) == name)
        {
            return i; //index
        }
    }
    return -1; //variable not found
}

int Parser::check(Token tok) //var or num
{
    int num;
    if(tok.token_type == NUM)
    {
        for(int i = variables.size(); i < next_available; i++)
        {
            if(mem[i] == stoi(tok.lexeme))
            {
                return i; //index
            }
        }
    }
    else
    {
        num = location(tok.lexeme);
        return num;
    }
    return -1; //variable not found
}