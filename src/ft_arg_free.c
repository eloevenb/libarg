#include "../libarg.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void	ft_bzero(char *buffer, size_t n)
{
	for (size_t i = 0; i < n; i++)
		buffer[i] = 0;
}

void	arg_free(t_arg_parser *parser)
{
	size_t	i;

	if (!parser)
		return ;
	i = 0;
	while (parser->items && i < parser->spec_count)
	{
		if (parser->items[i].spec && parser->items[i].spec->type == ARG_STRING_LIST
			&& parser->items[i].spec->flags == ARG_MULTIPLE)
		{
			for (size_t j = 0; j < parser->items[i].count; j++) {
				free((void *)parser->items[i].list[j]);
			}
			free((void *) parser->items[i].list);
		}
		i++;
	}
	free(parser->items);
	free((void *)parser->positionals);
	free((void *)parser->resolved_positionals);
	ft_bzero((char *)parser, sizeof(*parser));
}
