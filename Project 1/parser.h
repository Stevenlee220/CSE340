/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include "lexer.h"

class Parser {
  public:
    void ConsumeAllInput();
    void parse_input();
    void parse_program();
    void parse_poly_decl_section();
    void parse_poly_decl();
    void parse_polynomial_header();
    void parse_id_list();
    void parse_polynomial_name();
    void parse_polynomial_body();
    void parse_term_list();
    void parse_term();
    void parse_monomial_list();
    void parse_monomial();
    void parse_exponent();
    void parse_add_operator();
    void parse_coefficient();
    void parse_start();
    void parse_inputs();
    void parse_statement_list();
    void parse_statement();
    void parse_poly_evaluation_statement();
    void parse_input_statement();
    void parse_polynomial_evaluation();
    void parse_argument_list();
    void parse_argument();

  private:
    LexicalAnalyzer lexer;
    void syntax_error();
    Token expect(TokenType expected_type);
};

#endif