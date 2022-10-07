/*
 * Copyright (C) Rida Bazzi, 2020
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 */
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
#include <set>
#include "parser.h"

using namespace std;
vector<string> nodeNames; //Holds node names.
vector<REG_token*> tokens; //Holds token pointers.
vector<REG_token> tokens2; //Holds tokens.
string inputString; //Input string.
string inputString2; //Copy of input string.
int nodeCount; //Node number.

void Parser::syntax_error()
{
    cout << "SYNTAX ERROR\n";
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
// you will not need it for your project and you can delete it
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
// This function takes the input and parses it.
void Parser::input()
{
    tokens_section();
    inputString = expect(INPUT_TEXT).lexeme; // The input string is saved to a global string.
    inputString2 = inputString.substr(1, inputString.size() - 2); // A copy of the input string is also saved without the quotation marks.
    expect(END_OF_FILE);
}
// This function parses the token section portion of the input.
void Parser::tokens_section()
{
    token_list();
    expect(HASH);
}
// This function parses the list of tokens within the input.
vector<REG_token*> Parser::token_list()
{
    REG_token* tok2 = token(); //A REG_token is created to save the token.
    tokens.push_back(tok2); //The token is saved to the global tokens vector.
    tokens2.push_back(*tok2); //The token is also saved to the global tokens2 vector.

    Token tok;
    tok = lexer.peek(1);
    if(tok.token_type == COMMA)
    {
        expect(COMMA);
        token_list();
    }
    return tokens; //returns the vector full of token pointers.
}
// This function parses single tokens within the input.
REG_token* Parser::token()
{
    REG_token* tok = new REG_token; //A REG_token is created.
    tok->name = expect(ID).lexeme; //Saves the token's lexeme.
    tok->nfa = expr(); //Saves the token's REG.
    return tok; //Returns the token.
}
// This function constructs a REG for each token.
struct REG* Parser::expr()
{
    Token tok;
    tok = lexer.peek(1);
    if(tok.token_type == CHAR)
    {
        expect(CHAR);
        REG *R1 = new REG(); //Creates a new REG.
        REG_node *node1 = new REG_node(); //Creates new nodes for the REG.
        REG_node *node2 = new REG_node();

        //Sets node1 and node2's values.
        node1->first_label = tok.lexeme[0];
        node1->first_neighbor = node2;
        node1->nodeNum = nodeCount;
        nodeCount++;
        node2->nodeNum = nodeCount;
        nodeCount++;

        R1->start = node1; //Sets REG's starting state.
        R1->accept = node2; //Sets REG's accepting state.
        return R1; //Returns the REG.
    }
    else if(tok.token_type == UNDERSCORE)
    {
        expect(UNDERSCORE);
        REG *R1 = new REG(); //Creates a new REG.
        REG_node *node1 = new REG_node(); //Creates new nodes for the REG.
        REG_node *node2 = new REG_node();

        //Sets node1 and node2's values.
        node1->first_label = "_";
        node1->first_neighbor = node2;
        node1->nodeNum = nodeCount;
        nodeCount++;
        node2->nodeNum = nodeCount;
        nodeCount++;

        R1->start = node1; //Sets REG's starting state.
        R1->accept = node2; //Sets REG's accepting state.
        return R1; //Returns the REG.
    }
    else if(tok.token_type == LPAREN)
    {
        expect(LPAREN);
        REG *R2 = expr(); //Creates a new REG.
        expect(RPAREN);
        Token tok2;
        tok2 = lexer.peek(1);
        if(tok2.token_type == DOT)
        {
            expect(DOT);
            expect(LPAREN);
            REG *R3 = expr(); //Creates a new REG.
            expect(RPAREN);

            R2->accept->first_label = "_"; //Updates first REG's accepting state label.
            R2->accept->first_neighbor = R3->start; //Updates first REG's neighbor to second REG's starting state.
            R2->accept = R3->accept; //Updates REG's accepting state.
            return R2; //Returns the REG.
        }
        else if(tok2.token_type == OR)
        {
            expect(OR);
            expect(LPAREN);
            REG *R3 = expr(); //Creates a new REG.
            expect(RPAREN);
            REG_node *newStart = new REG_node(); //Creates new nodes for the REG.
            REG_node *newAccept = new REG_node();

            //Sets the new starting state and accepting state values.
            newStart->first_label = "_";
            newStart->first_neighbor = R2->start;
            newStart->second_label = "_";
            newStart->second_neighbor = R3->start;
            newStart->nodeNum = nodeCount;
            nodeCount++;
            newAccept->nodeNum = nodeCount;
            nodeCount++;

            //Updates REGs' accepting state labels and neighbors.
            R2->accept->first_label = "_";
            R2->accept->first_neighbor = newAccept;
            R3->accept->second_label = "_";
            R3->accept->second_neighbor = newAccept;

            R2->start = newStart; //Sets REG's starting state.
            R2->accept = newAccept; //Sets REG's accepting state.
            return R2; //Returns the REG.
        }
        else if(tok2.token_type == STAR)
        {
            expect(STAR);
            REG_node *newStart = new REG_node(); //Creates new nodes for the REG.
            REG_node *newAccept = new REG_node();

            //Sets the new starting state and accepting state values.
            newStart->first_label = "_";
            newStart->first_neighbor = R2->start;
            newStart->second_label = "_";
            newStart->second_neighbor = newAccept;
            newStart->nodeNum = nodeCount;
            nodeCount++;
            newAccept->nodeNum = nodeCount;
            nodeCount++;

            //Updates REGs' accepting state labels and neighbors.
            R2->accept->first_label = "_";
            R2->accept->first_neighbor = newAccept;
            R2->accept->second_label = "_";
            R2->accept->second_neighbor = R2->start;

            R2->start = newStart; //Sets REG's starting state.
            R2->accept = newAccept; //Sets REG's accepting state.
            return R2; //Returns the REG.
        }
        else
        {
            syntax_error();
        }
    }
    else
    {
        syntax_error();
    }
    return NULL;
}
// This function checks for epsilon cases.
void Parser::epsilon()
{
    for(auto& it : tokens) //Traverses token vector.
    {
        bool flag = false; //Boolean check.
        vector<REG_node*> current; //Node vectors.
        vector<REG_node*> next;
        current.push_back(it->nfa->start); //Inserts token into current vector.
        while(true)
        {
            for(auto& it2 : current) //Traverses all nodes inside current vector.
            {
                if(it2->first_label == "_")
                {
                    next.push_back(it2->first_neighbor); //Inserts node into next vector.
                }
                if(it2->second_label == "_")
                {
                    next.push_back(it2->second_neighbor); //Inserts node in to next vector.
                }
            }
            if(next.empty()) //Checks for empty vector.
            {
                flag = false;
                break;
            }
            for(auto& it3 : next) //Traverses all nodes inside next vector.
            {
                if(it3 == it->nfa->accept) //Checks for same accepting state.
                {
                    flag = true;
                    break;
                }
            }
            if(flag == true) //Checks flag.
            {
                break;
            }
            current.clear();
            current = next;
            next.clear();
        }
        if(flag == true)
        {
            nodeNames.push_back(it->name); //Inserts node's name into nodeNames vector.
        }
    }
    if(!nodeNames.empty())
    {
        cout << "EPSILON IS NOOOOOT A TOKEN !!!"; //Prints error.
        for(int i = 0;i < nodeNames.size();i++) //Traverses nodeNames vector.
        {
            if(i == nodeNames.size()) //Checks to see if its the last node in vector.
            {
                cout << nodeNames.at(i); //Prints node's name without extra space.
                break;
            }
            cout << nodeNames.at(i) << " "; //Prints node's name followed by a space.
        }
        cout << endl;
        exit(1);
    }
}
// This function returns the set of nodes that can be reached with one char.
set<REG_node> myLexicalAnalyzer::Match_One_Char(set<REG_node> S, char c)
{
    set<REG_node> emptySet; //Empty set of nodes.
    string str = string(1,c); //Casts the passed in char to a string.
    set<REG_node>::iterator it; //Creates iterator to traverse set.
    bool flag, flag2; //Booleans.

    if(!emptySet.empty()) //Checks for an empty set.
    {
        flag = compareNodes(emptySet, it->first_neighbor); //Checks for it's first neighbor in the empty set.
        flag2 = compareNodes(emptySet, it->second_neighbor); //Checks for it's second neighbor in the empty set.
    }
    for(it = S.begin();it != S.end();++it)
    {
        //Checks for node in empty set, then inserts.
        if(it->first_label == str && !flag)
        {
            emptySet.insert(*it->first_neighbor);
        }
        if(it->second_label == str && !flag2)
        {
            emptySet.insert(*it->second_neighbor);
        }
    }
    if(emptySet.empty()) //Checks for empty set.
    {
        return emptySet;
    }
    bool changed = true;
    set<REG_node> temp; //Temp set of nodes.
    while (changed)
    {
        changed = false;
        for(it = emptySet.begin();it != emptySet.end();++it)
        {
            temp.insert(*it); //Inserts nodes into temp set.
            //Checks for node in empty set, then inserts
            if(it->first_label == "_" && !flag)
            {
                temp.insert(*it->first_neighbor);
            }
            if(it->second_label == "_" && !flag2)
            {
                temp.insert(*it->second_neighbor);
            }
        }
        if (!compareSets(emptySet, temp)) //Checks if sets are identical.
        {
            changed = true;
            emptySet.insert(temp.begin(),temp.end());
            temp.clear();
        }
    }
    return emptySet; //Returns set.
}
// This function matches and returns the length of the longest possible string in the REG.
int myLexicalAnalyzer::match(REG* r, string s, int position)
{
    int currentLength = 0, finalLength = 0; //Current and final string length.
    set<REG_node> set1; //Sets of nodes.
    set<REG_node> set2;
    set1.insert(*r->start); //Inserts REG's starting state into first set.
    set2 = Match_One_Char(set1, '_'); //Sets set2 to the reachable nodes.
    set2.insert(set1.begin(),set1.end());
    set1.insert(set2.begin(),set2.end());

    //Traverses through the string one char at a time.
    for(std::string::iterator it = s.begin() + position, end = s.end(); it != end; ++it)
    {
        currentLength++; //Increments current string length.
        set1 = Match_One_Char(set1, *it); //Calculates reachable nodes using new char.
        if(set1.find(*r->accept) != set1.end())
        {
            finalLength = currentLength; //Sets final length equal to current length.
        }
    }
    return finalLength; //Returns final string length.
}
// This function prints the token name and the longest possible string.
void myLexicalAnalyzer::my_getToken()
{
    int start = 0; //Starting index.
    string temp = "";

    vector<REG_token>::iterator it;
    vector<string> str = removeDelimiter(inputString2, ' '); //Removes delimiters from input string and inserts to vector.
    for(int i = 0; i < str.size();i++) //Traverses string vector.
    {
        start = 0;
        while(start != str[i].size())
        {
            int end = 0; //Ending index.
            for(it = tokens2.begin(); it != tokens2.end(); it++) //Traverses token vector.
            {
                REG_token& tok = *it; //Copies a token from the vector.
                int length = match(tok.nfa,str[i],start); //Sets length of longest str in the token's REG.

                if(length > end) //Checks if its greater than end ending index.
                {
                    end = length; //Sets end equal to length.
                    temp = tok.name; //Copies token's lexeme into string.
                }
            }
            if(end == 0) //Error.
            {
                cout << "ERROR";
                exit(1);
            }
            //Prints out token name and string.
            cout << temp << " , \"" << str[i].substr(start, end) << "\"" << endl;
            start = start + end;
        }
    }
}
// This function compares nodes and returns a boolean.
bool myLexicalAnalyzer::compareNodes(set<REG_node> S, REG_node* r)
{
    set<REG_node>::iterator it;
    for(it = S.begin(); it != S.end(); ++it)
    {
        if(r->nodeNum == it->nodeNum)
        {
            return true; //node exists
        }
    }
    return false; //node does not exist
}
// This function compares sets and returns a boolean.
bool myLexicalAnalyzer::compareSets(set<REG_node> Set1, set<REG_node> Set2)
{
    int index1 = 0, index2 = 0;
    if(Set1.size() != Set2.size())
    {
        return false; //Set sizes do not equal.
    }
    int s1[Set1.size()]; //Two arrays the sizes of the sets.
    int s2[Set2.size()];
    set<REG_node>::iterator it;
    for(it = Set1.begin();it != Set1.end(); ++it) //Traverses set1.
    {
        s1[index1] = it->nodeNum; //Copies node numbers into array.
        index1++;
    }
    for(it = Set2.begin();it != Set2.end(); ++it) //Traverses set2.
    {
        s2[index2] = it->nodeNum; //Copies node numbers into array.
        index2++;
    }
    sort(s1,s1+index1); //Sorts array 1.
    sort(s2,s2+index2); //Sorts array 2.
    for(int i = 0; i < index2; i++)
    {
        if(s1[i] != s2[i])
        {
            return false; //Sets do not equal.
        }
    }
    return true; //Sets do equal.
}
// This function removes the delimiters from the input string.
vector<string> myLexicalAnalyzer::removeDelimiter(string inputStr, char delimiter)
{
    string str; //a temp string
    vector<string> brokenString; //vector that holds the string without the delimiters.
    stringstream ss(inputStr); //creates a stream using input string.

    while(getline(ss,str,delimiter))
    {
        brokenString.push_back(str); //saves string to vector.
    }
    return brokenString; //returns vector
}

int main()
{
    // note: the parser class has a lexer object instantiated in it. You should not be declaring
    // a separate lexer object. You can access the lexer object in the parser functions as shown in the
    // example method Parser::ConsumeAllInput
    // If you declare another lexer object, lexical analysis will not work correctly
    Parser parser;
    myLexicalAnalyzer analyzer;

	//parser.ConsumeAllInput();
	parser.input();
	parser.epsilon();
    analyzer.my_getToken();
}