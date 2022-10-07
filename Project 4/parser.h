/*
 * Name: Richard Marquez Cortes
 * ASU ID: 1214775731
 */

#ifndef PROJECT4_PARSER_H
#define PROJECT4_PARSER_H

#include <string>
#include <set>
#include "lexer.h"
#include "compiler.h"
using namespace std;

class Parser {
public:
    InstructionNode *program();
    void var_section();
    void id_list();
    InstructionNode *body();
    void stmt_list(InstructionNode*& head);
    InstructionNode *stmt();
    InstructionNode *assign_stmt();
    void expr(InstructionNode *&state);
    Token primary();
    Token op();
    InstructionNode *output_stmt();
    InstructionNode *input_stmt();
    InstructionNode *while_stmt();
    InstructionNode *if_stmt();
    InstructionNode *condition();
    Token relop();
    InstructionNode *switch_stmt(); //
    InstructionNode *for_stmt();
    void case_list(int location, InstructionNode *&head, InstructionNode *noop); //
    InstructionNode *parser_case(int location, InstructionNode *noop); //
    InstructionNode *default_case();
    void parser_inputs();
    void num_list();

    int location(string name);
private:
    LexicalAnalyzer lexer;
    void syntax_error();
    Token expect(TokenType expected_type);

    int check(Token tok);
};

#endif //PROJECT4_PARSER_H