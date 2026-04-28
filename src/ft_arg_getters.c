#include "ft_arg_internal.h"

static int	get_item(const t_arg_parser *parser, const char *name,
				t_arg_type expected, const t_arg_item **out_item)
{
	ssize_t	index;

	if (!parser || !name || !out_item)
		return (1);
	index = arg_find_spec_index(parser, name);
	if (index < 0 || parser->items[index].spec->type != expected)
		return (1);
	*out_item = &parser->items[index];
	return (0);
}

int	arg_get_bool(const t_arg_parser *parser, const char *name, int *out)
{
	const t_arg_item	*item;

	if (!out || get_item(parser, name, ARG_BOOL, &item) != 0)
		return (1);
	*out = item->value.b;
	return (0);
}

int	arg_get_int(const t_arg_parser *parser, const char *name, long *out)
{
	const t_arg_item	*item;

	if (!out || get_item(parser, name, ARG_INT, &item) != 0)
		return (1);
	*out = item->value.i;
	return (0);
}

int	arg_get_float(const t_arg_parser *parser, const char *name, double *out)
{
	const t_arg_item	*item;

	if (!out || get_item(parser, name, ARG_FLOAT, &item) != 0)
		return (1);
	*out = item->value.f;
	return (0);
}

int	arg_get_string(const t_arg_parser *parser, const char *name, const char **out)
{
	const t_arg_item	*item;

	if (!out || get_item(parser, name, ARG_STRING, &item) != 0)
		return (1);
	*out = item->value.s;
	return (0);
}

int	arg_get_string_list(const t_arg_parser *parser, const char *name,
			const char ***out, size_t *count)
{
	const t_arg_item	*item;

	if (!out || !count || get_item(parser, name, ARG_STRING_LIST, &item) != 0)
		return (1);
	*out = item->list;
	*count = item->count;
	return (0);
}

int	arg_get_positionals(const t_arg_parser *parser,
		const char ***out, size_t *count)
{
	if (!parser || !out || !count)
		return (1);
	*out = parser->positionals;
	*count = parser->positional_count;
	return (0);
}
