#include "../libarg.h"

#include <unistd.h>

static size_t	str_len(const char *s)
{
	size_t	len;

	if (!s)
		return (0);
	len = 0;
	while (s[len])
		len++;
	return (len);
}

static void	write_str(const char *s)
{
	size_t	len;
	size_t	written;
	ssize_t	ret;

	if (!s)
		return ;
	len = str_len(s);
	written = 0;
	while (written < len)
	{
		ret = write(1, s + written, len - written);
		if (ret <= 0)
			return ;
		written += (size_t)ret;
	}
}

static void	write_char(char c)
{
	(void)write(1, &c, 1);
}

static const char	*type_hint(const t_arg_spec *spec)
{
	if (spec->type == ARG_BOOL)
		return ("");
	if (spec->metavar)
		return (spec->metavar);
	if (spec->type == ARG_INT)
		return ("INT");
	if (spec->type == ARG_FLOAT)
		return ("FLOAT");
	return ("VALUE");
}

void	arg_print_help(const t_arg_parser *parser, const char *prog_name)
{
	size_t	i;

	if (!parser)
		return ;
	write_str("Usage: ");
	write_str(prog_name ? prog_name : "program");
	write_str(" [OPTIONS] [ARGS...]\n\n");
	write_str("Options:\n");
	i = 0;
	while (i < parser->spec_count)
	{
		if (parser->specs[i].short_name)
		{
			write_str("  -");
			write_char(parser->specs[i].short_name);
			write_str(", ");
		}
		else
			write_str("      ");
		write_str("--");
		write_str(parser->specs[i].long_name);
		if (parser->specs[i].type != ARG_BOOL)
		{
			write_str(" <");
			write_str(type_hint(&parser->specs[i]));
			write_char('>');
		}
		if (parser->specs[i].flags & ARG_REQUIRED)
			write_str(" (required)");
		if (parser->specs[i].help)
		{
			write_str("\n      ");
			write_str(parser->specs[i].help);
		}
		write_char('\n');
		i++;
	}
}
