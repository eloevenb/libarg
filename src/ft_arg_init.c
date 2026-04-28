#include "ft_arg_internal.h"

#include <stdlib.h>
#include <string.h>

static int	parse_default_value(t_arg_item *item)
{
	char	*endptr;

	if (!item->spec->default_str)
		return (0);
	if (item->spec->type == ARG_BOOL)
	{
		if (strcmp(item->spec->default_str, "1") == 0
			|| strcmp(item->spec->default_str, "true") == 0)
			item->value.b = 1;
		return (0);
	}
	if (item->spec->type == ARG_INT)
	{
		item->value.i = strtol(item->spec->default_str, &endptr, 10);
		return (*endptr != '\0');
	}
	if (item->spec->type == ARG_FLOAT)
	{
		item->value.f = strtod(item->spec->default_str, &endptr);
		return (*endptr != '\0');
	}
	if (item->spec->type == ARG_STRING)
		item->value.s = item->spec->default_str;
	return (0);
}

int	arg_init(t_arg_parser *parser, const t_arg_spec *specs, size_t spec_count)
{
	size_t	i;

	if (!parser || !specs || spec_count == 0)
		return (1);
	memset(parser, 0, sizeof(*parser));
	parser->specs = specs;
	parser->spec_count = spec_count;
	parser->items = calloc(spec_count, sizeof(t_arg_item));
	if (!parser->items)
		return (1);
	i = 0;
	while (i < spec_count)
	{
		parser->items[i].spec = &specs[i];
		if (parse_default_value(&parser->items[i]) != 0)
		{
			parser->error_msg = "Invalid default value";
			parser->error_spec = &specs[i];
			return (1);
		}
		i++;
	}
	return (0);
}
