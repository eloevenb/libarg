#include "ft_arg_internal.h"

#include <stdlib.h>
#include <string.h>

static ssize_t	find_long_opt(const t_arg_parser *parser, const char *arg)
{
	size_t	i;

	i = 0;
	while (i < parser->spec_count)
	{
		if (parser->specs[i].long_name
			&& strcmp(parser->specs[i].long_name, arg) == 0)
			return ((ssize_t)i);
		i++;
	}
	return (-1);
}

static ssize_t	find_short_opt(const t_arg_parser *parser, char short_name)
{
	size_t	i;

	i = 0;
	while (i < parser->spec_count)
	{
		if (parser->specs[i].short_name == short_name)
			return ((ssize_t)i);
		i++;
	}
	return (-1);
}

static int	push_string_list(t_arg_item *item, const char *value)
{
	const char	**new_list;

	new_list = realloc(item->list, sizeof(char *) * (item->count + 1));
	if (!new_list)
		return (1);
	item->list = new_list;
	item->list[item->count] = value;
	item->count++;
	return (0);
}

static int	set_option_value(t_arg_parser *parser, t_arg_item *item, const char *value)
{
	char	*endptr;

	if (item->spec->type == ARG_BOOL)
	{
		item->value.b = 1;
		return (0);
	}
	if (!value)
		return (1);
	if (item->spec->type == ARG_INT)
	{
		item->value.i = strtol(value, &endptr, 10);
		return (*endptr != '\0');
	}
	if (item->spec->type == ARG_FLOAT)
	{
		item->value.f = strtod(value, &endptr);
		return (*endptr != '\0');
	}
	if (item->spec->type == ARG_STRING)
	{
		item->value.s = value;
		return (0);
	}
	if (item->spec->type == ARG_STRING_LIST)
		return (push_string_list(item, value));
	parser->error_msg = "Unsupported option type";
	return (1);
}

static int	parse_option_at(t_arg_parser *parser, ssize_t spec_index, const char *value)
{
	t_arg_item	*item;

	item = &parser->items[spec_index];
	if (item->is_set && !(item->spec->flags & ARG_MULTIPLE)
		&& item->spec->type != ARG_STRING_LIST)
	{
		parser->error_msg = "Option cannot be repeated";
		parser->error_spec = item->spec;
		return (1);
	}
	if (set_option_value(parser, item, value) != 0)
	{
		if (!parser->error_msg)
			parser->error_msg = "Invalid option value";
		parser->error_spec = item->spec;
		return (1);
	}
	item->is_set = 1;
	if (item->spec->type != ARG_STRING_LIST)
		item->count++;
	return (0);
}

static int	push_positional(t_arg_parser *parser, const char *value)
{
	const char	**new_positional;

	new_positional = realloc(parser->positionals,
			sizeof(char *) * (parser->positional_count + 1));
	if (!new_positional)
		return (1);
	parser->positionals = new_positional;
	parser->positionals[parser->positional_count] = value;
	parser->positional_count++;
	return (0);
}

static int	check_required(const t_arg_parser *parser)
{
	size_t	i;

	i = 0;
	while (i < parser->spec_count)
	{
		if ((parser->specs[i].flags & ARG_REQUIRED) && !parser->items[i].is_set)
			return ((int)i + 1);
		i++;
	}
	return (0);
}

static int	check_positionals(t_arg_parser *parser)
{
	size_t	i;
	size_t	min_required;
	int		has_variadic;

	if (parser->positional_spec_count == 0)
		return (0);
	min_required = 0;
	has_variadic = 0;
	i = 0;
	while (i < parser->positional_spec_count)
	{
		if (parser->positional_specs[i].flags & ARG_REQUIRED)
			min_required++;
		if (parser->positional_specs[i].flags & ARG_MULTIPLE)
		{
			has_variadic = 1;
			break ;
		}
		i++;
	}
	if (parser->positional_count < min_required)
		return (parser->error_msg = "Missing required positional argument", 1);
	if (!has_variadic && parser->positional_count > parser->positional_spec_count)
		return (parser->error_msg = "Too many positional arguments", 1);
	return (0);
}

static size_t	count_trailing_required(const t_arg_parser *parser)
{
	size_t	count;
	size_t	i;

	count = 0;
	i = parser->positional_spec_count;
	while (i > 0)
	{
		if (!(parser->positional_specs[i - 1].flags & ARG_REQUIRED))
			break ;
		count++;
		i--;
	}
	return (count);
}

static int	has_variadic_positional(const t_arg_parser *parser)
{
	size_t	i;

	i = 0;
	while (i < parser->positional_spec_count)
	{
		if (parser->positional_specs[i].flags & ARG_MULTIPLE)
			return (1);
		i++;
	}
	return (0);
}

static int	build_resolved_positionals(t_arg_parser *parser)
{
	size_t	trailing_required_count;
	size_t	prefix_value_count;
	size_t	i;

	free((void *)parser->resolved_positionals);
	parser->resolved_positionals = NULL;
	parser->resolved_positional_count = 0;
	if (!parser->positional_specs || parser->positional_spec_count == 0)
		return (0);
	if (has_variadic_positional(parser))
		return (0);
	parser->resolved_positionals = calloc(parser->positional_spec_count,
			sizeof(char *));
	if (!parser->resolved_positionals)
		return (1);
	parser->resolved_positional_count = parser->positional_spec_count;
	trailing_required_count = count_trailing_required(parser);
	prefix_value_count = parser->positional_count - trailing_required_count;
	i = 0;
	while (i < prefix_value_count)
	{
		parser->resolved_positionals[i] = parser->positionals[i];
		i++;
	}
	i = 0;
	while (i < trailing_required_count)
	{
		parser->resolved_positionals[parser->positional_spec_count
			- trailing_required_count + i] = parser->positionals[prefix_value_count + i];
		i++;
	}
	return (0);
}

int	arg_parse(t_arg_parser *parser, int argc, char **argv)
{
	int		i;
	ssize_t	spec_index;
	int		required_index;

	if (!parser || !argv || argc < 1)
		return (1);
	i = 1;
	while (i < argc)
	{
		if (strncmp(argv[i], "--", 2) == 0 && argv[i][2] != '\0')
		{
			spec_index = find_long_opt(parser, argv[i] + 2);
			if (spec_index < 0 || (parser->specs[spec_index].type != ARG_BOOL && ++i >= argc)
				|| parse_option_at(parser, spec_index,
					parser->specs[spec_index].type == ARG_BOOL ? NULL : argv[i]) != 0)
				return (parser->error_msg = spec_index < 0 ? "Unknown long option" : parser->error_msg, 1);
		}
		else if (argv[i][0] == '-' && argv[i][1] != '\0')
		{
			spec_index = find_short_opt(parser, argv[i][1]);
			if (spec_index < 0 || (parser->specs[spec_index].type != ARG_BOOL && ++i >= argc)
				|| parse_option_at(parser, spec_index,
					parser->specs[spec_index].type == ARG_BOOL ? NULL : argv[i]) != 0)
				return (parser->error_msg = spec_index < 0 ? "Unknown short option" : parser->error_msg, 1);
		}
		else if (push_positional(parser, argv[i]) != 0)
			return (parser->error_msg = "Out of memory", 1);
		i++;
	}
	required_index = check_required(parser);
	if (required_index > 0)
	{
		parser->error_msg = "Missing required option";
		parser->error_spec = &parser->specs[required_index - 1];
		return (1);
	}
	if (check_positionals(parser) != 0)
		return (1);
	if (build_resolved_positionals(parser) != 0)
		return (parser->error_msg = "Out of memory", 1);
	return (0);
}
