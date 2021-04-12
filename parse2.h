#ifndef PARSE2_H
# define PARSE2_H

t_parse_ast_node	*parse_new_ast_node(t_parse_ast_type type, void *content);
void				parse_skip_spaces(t_parse_buffer *buf, t_token *tok);

#endif
