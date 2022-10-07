/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
/*
 * Name: Richard Marquez Cortes
 * ASU ID: 1214775731
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include <set>
#include "lexer.h"
using namespace std;

// Structure holding node information.
struct REG_node
{
    struct REG_node* first_neighbor; //next node
    string first_label; //a for example
    struct REG_node* second_neighbor; //second next node
    string second_label; //b for example
    int nodeNum; //node number
    friend bool operator< (const REG_node& a, const REG_node& b);
};
// This function allows int ['function'] type arrays
bool operator< (const REG_node& a, const REG_node& b)
{
    return a.nodeNum < b.nodeNum;
}
// Structure holding REG information.
struct REG
{
    struct  REG_node* start; //starting node
    struct  REG_node* accept; //final accepting node
};
// Structure holding token information.
struct REG_token
{
    string name; //token name
    REG *nfa; //token's REG
};

class Parser {
  public:
    void ConsumeAllInput();
    void input();
    void tokens_section();
    vector<REG_token*> token_list();
    REG_token * token();
    struct REG* expr();
    void epsilon();

  private:
    LexicalAnalyzer lexer;
    void syntax_error();
    Token expect(TokenType expected_type);
};

class myLexicalAnalyzer {
public:
    set<REG_node> Match_One_Char(set<REG_node> S, char c);
    int match(REG* r, string s, int p);
    void my_getToken();
    bool compareNodes(set<REG_node> S, REG_node *r);
    bool compareSets(set<REG_node> S, set<REG_node> S2);
    vector<string> removeDelimiter(string s, char c);
};

#endif

