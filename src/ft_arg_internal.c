#include "ft_arg_internal.h"

#include <string.h>

ssize_t	arg_find_spec_index(const t_arg_parser *parser, const char *name)
{
	size_t	i;

	if (!parser || !name)
		return (-1);
	i = 0;
	while (i < parser->spec_count)
	{
		if (parser->specs[i].long_name
			&& strcmp(parser->specs[i].long_name, name) == 0)
			return ((ssize_t)i);
		i++;
	}
	return (-1);
}
