#include <stdlib.h>
#include "parse.h"

static t_parse_ast_list	**get_ast_list(void)
{
	static t_parse_ast_list	*list;

	return (&list);
}

static t_parse_ast_list	*create_ast_on_list(void)
{
	t_parse_ast_list	*list;

	list = malloc(sizeof(t_parse_ast_list));
	if (!list)
		parse_fatal_error();
	list->next = *get_ast_list();
	*get_ast_list() = list;
	return (list);
}

void	parse_free_all_ast(void)
{
	t_parse_ast_list	*list;
	t_parse_ast_list	*next;

	list = *get_ast_list();
	while (list)
	{
		next = list->next;
		if (list->ast.type == ASTNODE_STRING)
			free(list->ast.content.string->text);
		free(list->ast.content.void_ptr);
		free(list);
		list = next;
	}
	*get_ast_list() = NULL;
}

t_parse_ast	*parse_new_ast_node(t_parse_ast_type type, void *content)
{
	t_parse_ast_list	*list;

	if (!(type > ASTNODE_NONE && type < ASTNODE_INVALID))
		parse_fatal_error();
	if (!content)
		parse_fatal_error();
	list = create_ast_on_list();
	list->ast.error = 0;
	list->ast.type = type;
	list->ast.content.void_ptr = content;
	return (&list->ast);
}
