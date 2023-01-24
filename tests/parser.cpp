#include "parserAndExecutor.h"
#include "gtest/gtest.h"
#include <fstream>
#include <iostream>
#include <vector>

extern "C" {
#include "../libft/libft.h"
#include "../parser/parser.c"
#include "../parser/parse_command.c"
#include "../parser/parser.h"
#include "../parser/parser_utils.c"
#include <stdio.h>
#include <stdlib.h>
}

void print_token_type(int token_type) {
  if (token_type == OPEN_PARENTHESES)
    std::cout << "OPEN_PARENTHESES" << std::endl;
  else if (token_type == CLOSE_PARENTHESES)
    std::cout << "CLOSE_PARENTHESES" << std::endl;
  else if (token_type == PIPELINE)
    std::cout << "PIPE" << std::endl;
  else if (token_type == LOGIC_AND)
    std::cout << "AND" << std::endl;
  else if (token_type == LOGIC_OR)
    std::cout << "OR" << std::endl;
  else if (token_type == REDIR_IN)
    std::cout << "REDIRECT_IN" << std::endl;
  else if (token_type == REDIR_OUT)
    std::cout << "REDIRECT_OUT" << std::endl;
  else if (token_type == REDIR_APPEND)
    std::cout << "REDIRECT_APPEND" << std::endl;
  else if (token_type == COMMAND)
    std::cout << "WORD" << std::endl;
}

t_list *generate_tokens(std::vector<std::string> tokens) {
  t_list *tok = NULL;

  for (std::vector<std::string>::iterator it = tokens.begin();
       it != tokens.end(); ++it) {
    ft_lstadd_back(&tok, ft_lstnew(ft_strdup((char *)it->c_str())));
  }
  return tok;
}

void compare_str_list(char **actual, char **expected) {
  int i = 0;
  while (actual[i] != NULL && expected[i] != NULL) {
    ASSERT_STREQ(actual[i], expected[i]);
    // std::cout << "actual: " << actual[i] << " expected: " << expected[i]
    //           << std::endl;
    i++;
  }
  ASSERT_EQ(actual[i], expected[i]); // check if both are NULL
}

void compare_ast(t_cmd *ast, t_cmd *expected) {
  if (ast == NULL || expected == NULL)
    FAIL() << "AST is NULL";
  ASSERT_EQ(ast->type, expected->type);
  if (ast->type == COMMAND) {
    compare_str_list(ast->cmd.argv, expected->cmd.argv);
    if (ast->cmd.next != NULL)
      compare_ast(ast->cmd.next, expected->cmd.next);
    else
      ASSERT_EQ(ast->cmd.next, expected->cmd.next);
  } else if (ast->type == PIPELINE) {
    ASSERT_EQ(ast->pipeline.pipe_count, expected->pipeline.pipe_count);
    ASSERT_NE(ast->pipeline.first_cmd, (t_cmd *)nullptr);
    compare_ast(ast->pipeline.first_cmd, expected->pipeline.first_cmd);
  } else if (ast->type == LOGIC_AND || LOGIC_OR) {
    ASSERT_NE(ast->pipe.left, (t_cmd *)NULL);
    ASSERT_NE(ast->pipe.right, (t_cmd *)NULL);
    compare_ast(ast->pipe.left, expected->pipe.left);
    compare_ast(ast->pipe.right, expected->pipe.right);
  } else if (ast->type == REDIR_IN || REDIR_OUT || REDIR_APPEND) {
    ASSERT_STREQ(ast->redir.filename, expected->redir.filename);
    ASSERT_NE(ast->redir.cmd, (t_cmd *)NULL);
    compare_ast(ast->redir.cmd, expected->redir.cmd);
  } else {
    FAIL() << "Invalid cmd->type";
  }
}

void testParser(std::vector<std::string> tokens, t_cmd *expected) {
  t_list *tok = generate_tokens(tokens);
  t_cmd *cmd = parser(tok);
  compare_ast(cmd, expected);
}

/*********************************** PARSER ***********************************/
/*********** command *********/
TEST(ParserCmd, Simple) {
  t_list *tokens = generate_tokens({"ls"});
  t_cmd *cmd = new t_cmd;
  parse_command(tokens, cmd);

  EXPECT_EQ(cmd->type, COMMAND);
  compare_str_list(cmd->cmd.argv, setup_argv({"ls"}));
  EXPECT_EQ(cmd->cmd.next, (t_cmd *)NULL);
}

TEST(ParserCmd, WithArgs) {
  t_list *tokens = generate_tokens({"ls", "-l", "-a"});
  t_cmd *cmd = new t_cmd;
  parse_command(tokens, cmd);

  EXPECT_EQ(cmd->type, COMMAND);
  compare_str_list(cmd->cmd.argv, setup_argv({"ls", "-l", "-a"}));
  EXPECT_EQ(cmd->cmd.next, (t_cmd *)NULL);
}

TEST(ParserCmd, WithManyArgs) {
  t_list *tokens = generate_tokens({"ls", "-l", "-a", "-h", "-t", "-r", "-S"});
  t_cmd *cmd = new t_cmd;
  parse_command(tokens, cmd);

  EXPECT_EQ(cmd->type, COMMAND);
  compare_str_list(cmd->cmd.argv,
                   setup_argv({"ls", "-l", "-a", "-h", "-t", "-r", "-S"}));
  EXPECT_EQ(cmd->cmd.next, (t_cmd *)NULL);
}

TEST(ParserCmd, SimpleParentesesError) {

  t_list *tokens = generate_tokens({"echo", "(", "ls", ")"});
  t_cmd *cmd = new t_cmd;
  std::string expected = "minishell: syntax error near unexpected token `ls'\n";
  testing::internal::CaptureStderr();
  parse_command(tokens, cmd);
  std::string stderr_res = testing::internal::GetCapturedStderr();
  EXPECT_EQ(stderr_res, expected);
}

TEST(ParserCmd, RightParentesesError) {
  t_list *tokens = generate_tokens({"echo", "ls", ")"});
  t_cmd *cmd = new t_cmd;
  std::string expected = "minishell: syntax error near unexpected token `)'\n";
  testing::internal::CaptureStderr();
  parse_command(tokens, cmd);
  std::string stderr_res = testing::internal::GetCapturedStderr();
  EXPECT_EQ(stderr_res, expected);
}

/*********** pipeline ********/
TEST(ParserPipeline, NoPipeline) {
  t_list *tokens = generate_tokens({"ls", "-l", "-a"});
  t_cmd *cmd = new t_cmd;
  ASSERT_FALSE(pipeline(tokens, cmd));
}

TEST(ParserPipeline, Simple) {
  t_list *tokens = generate_tokens({"ls", "|", "grep", "a"});
  t_cmd *cmd = new t_cmd;
  ASSERT_TRUE(pipeline(tokens, cmd));
  ASSERT_EQ(cmd->type, PIPELINE);
  ASSERT_EQ(cmd->pipeline.pipe_count, 1);
  ASSERT_NE(cmd->pipeline.first_cmd, (t_cmd *)NULL);
  ASSERT_EQ(cmd->pipeline.first_cmd->type, COMMAND);
  compare_str_list(cmd->pipeline.first_cmd->cmd.argv, setup_argv({"ls"}));
  ASSERT_NE(cmd->pipeline.first_cmd->cmd.next, (t_cmd *)NULL);
  ASSERT_EQ(cmd->pipeline.first_cmd->cmd.next->type, COMMAND);
  compare_str_list(cmd->pipeline.first_cmd->cmd.next->cmd.argv,
                   setup_argv({"grep", "a"}));
  EXPECT_EQ(cmd->pipeline.first_cmd->cmd.next->cmd.next, (t_cmd *)NULL);
}

TEST(ParserPipeline, ManyPipes) {
  t_list *tokens = generate_tokens({"ls", "|", "grep", "a", "|", "wc", "-l"});
  t_cmd *cmd = new t_cmd;
  ASSERT_TRUE(pipeline(tokens, cmd));
  ASSERT_EQ(cmd->type, PIPELINE);
  ASSERT_EQ(cmd->pipeline.pipe_count, 2);
  ASSERT_NE(cmd->pipeline.first_cmd, (t_cmd *)NULL);
  ASSERT_EQ(cmd->pipeline.first_cmd->type, COMMAND);
  compare_str_list(cmd->pipeline.first_cmd->cmd.argv, setup_argv({"ls"}));
  ASSERT_NE(cmd->pipeline.first_cmd->cmd.next, (t_cmd *)NULL);
  ASSERT_EQ(cmd->pipeline.first_cmd->cmd.next->type, COMMAND);
  compare_str_list(cmd->pipeline.first_cmd->cmd.next->cmd.argv,
                   setup_argv({"grep", "a"}));
  ASSERT_NE(cmd->pipeline.first_cmd->cmd.next->cmd.next, (t_cmd *)NULL);
  ASSERT_EQ(cmd->pipeline.first_cmd->cmd.next->cmd.next->type, COMMAND);
  compare_str_list(cmd->pipeline.first_cmd->cmd.next->cmd.next->cmd.argv,
                   setup_argv({"wc", "-l"}));
  EXPECT_EQ(cmd->pipeline.first_cmd->cmd.next->cmd.next->cmd.next,
            (t_cmd *)NULL);
}

/*********** redir ***********/
/*********** logic ***********/
TEST(ParserLogic, NoLogic) {
  std::vector<std::string> tokens = {"ls", "echo", "hello"};
  t_cmd *cmd = new t_cmd;
  bool result = logic(generate_tokens(tokens), cmd);
  EXPECT_EQ(result, false);
}

/*********** parser **********/
/*
TEST(Parser, EchoHello) {
  t_cmd *cmd = new_cmd(COMMAND);
  cmd->cmd.argv = setup_argv({"echo", "hello"});
  cmd->cmd.next = NULL;
  testParser({"echo", "hello"}, cmd);
}

TEST(Parser, EchoPipeCat) {
  t_cmd *cmd = new_cmd(PIPELINE);
  cmd->pipeline.pipe_count = 1;
  cmd->pipeline.first_cmd = new_cmd(COMMAND);
  cmd->pipeline.first_cmd->cmd.argv = setup_argv({"echo", "hello"});
  cmd->pipeline.first_cmd->cmd.next = new_cmd(COMMAND);
  cmd->pipeline.first_cmd->cmd.next->cmd.argv = setup_argv({"cat"});
  cmd->pipeline.first_cmd->cmd.next->cmd.next = NULL;
  testParser({"echo", "hello", "|", "cat"}, cmd);
}
*/

/************************* ParserUtils, get_token_type ************************/

TEST(ParserUtils, get_token_type) {
  EXPECT_EQ(get_token_type((char *)"&&"), LOGIC_AND);
  EXPECT_EQ(get_token_type((char *)"||"), LOGIC_OR);
  EXPECT_EQ(get_token_type((char *)"|"), PIPELINE);
  EXPECT_EQ(get_token_type((char *)">"), REDIR_OUT);
  EXPECT_EQ(get_token_type((char *)">>"), REDIR_APPEND);
  EXPECT_EQ(get_token_type((char *)"<"), REDIR_IN);
  EXPECT_EQ(get_token_type((char *)"("), OPEN_PARENTHESES);
  EXPECT_EQ(get_token_type((char *)")"), CLOSE_PARENTHESES);

  EXPECT_EQ(get_token_type((char *)") "), COMMAND);
  EXPECT_EQ(get_token_type((char *)"ls"), COMMAND);
}

/****************** ParserUtils, are_we_in_parentheses ************************/

TEST(ParserUtils, are_we_in_parentheses_SimpleTrue) {
  t_list *tok = generate_tokens({"(", "&&", "ls", "echo", "hello", ")"});
  EXPECT_EQ(are_we_in_parentheses(tok), true);
}

TEST(ParserUtils, are_we_in_parentheses_SimpleFalse) {
  t_list *tok = generate_tokens({"ls", "&&", "echo", "hello"});
  EXPECT_EQ(are_we_in_parentheses(tok), false);
}

TEST(ParserUtils, are_we_in_parentheses_TrickyTrue) {
  t_list *tok = generate_tokens({"(",                             //
                                 "ls", "&&",                      //
                                 "(", "echo", "hello", ")", "||", //
                                 "(", "echo", "world", ")",       //
                                 ")"});
  EXPECT_EQ(are_we_in_parentheses(tok), true);
}

TEST(ParserUtils, are_we_in_parentheses_TrickyFalse) {
  t_list *tok = generate_tokens({"ls", "&&", "(", "echo", "hello", ")", "||",
                                 "(", "echo", "world", ")", ")"});
  EXPECT_EQ(are_we_in_parentheses(tok), false);
}

TEST(ParserUtils, are_we_in_parentheses_RandomTrue) {
  t_list *tok = generate_tokens({"(", "ls", "&&", "(", "echo", "hello", ")"});
  // TODO ca devrait etre une erreur
  EXPECT_EQ(are_we_in_parentheses(tok), false);
}

/********************** ParserUtils, skip_parentheses *************************/

void test_skip_parentheses(std::vector<std::string> tokens, int expected) {
  t_list *tok = generate_tokens(tokens);
  t_list *new_tok = skip_parentheses(tok);
  for (int i = 0; i < expected && new_tok != NULL; i++) {
    tok = tok->next;
  }
  EXPECT_EQ(new_tok, tok);
  EXPECT_STREQ((char *)new_tok->content, (char *)tok->content);
}

TEST(ParserUtils, skip_parentheses_Simple) {
  test_skip_parentheses(
      {"(", "ls", "&&", "(", "echo", "hello", ")", ")", "&&", "echo", "world"},
      7);
}

TEST(ParserUtils, skip_parentheses_NoParentheses) {
  test_skip_parentheses(
      {"ls", "&&", "(", "echo", "hello", ")", "&&", "echo", "world"}, 0);
}

/******************* ParserUtils, lst_cut_first_and_last **********************/

TEST(ParserUtils, lst_cut_first_and_last_Simple) {
  t_list *tok = generate_tokens({"(", "echo", "hello", ")"});
  t_list *new_tok = lst_cut_first_and_last(tok);
  EXPECT_EQ(new_tok, tok->next);
  EXPECT_STREQ((char *)new_tok->content, (char *)tok->next->content);
  tok = tok->next;
  while (new_tok->next != NULL) {
    // std::cout << (char *)tok->content << std::endl;
    tok = tok->next;
    new_tok = new_tok->next;
    EXPECT_STREQ((char *)new_tok->content, (char *)tok->content);
  }
  // std::cout << (char *)new_tok->content << std::endl;
  EXPECT_EQ(new_tok, tok);
}
