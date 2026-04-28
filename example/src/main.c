#include "../../libarg.h"

#include <stdio.h>

static const t_arg_spec	g_specs[] = {
	{"help", 'h', ARG_BOOL, 0, "Print this help message", NULL, NULL},
	{"verbose", 'v', ARG_BOOL, 0, "Enable verbose output", NULL, NULL},
	{"port", 'p', ARG_INT, ARG_REQUIRED, "Target port", "PORT", NULL},
	{"timeout", 't', ARG_FLOAT, 0, "Timeout in seconds", "SECONDS", "3.5"},
	{"host", 0, ARG_STRING, ARG_REQUIRED, "Target host", "HOST", NULL},
};

static void	print_values(const t_arg_parser *parser)
{
	int			verbose;
	long		port;
	double		timeout;
	const char	*host;
	const char	**positionals;
	size_t		positional_count;
	size_t		i;

	if (arg_get_bool(parser, "verbose", &verbose) != 0
		|| arg_get_int(parser, "port", &port) != 0
		|| arg_get_float(parser, "timeout", &timeout) != 0
		|| arg_get_string(parser, "host", &host) != 0
		|| arg_get_positionals(parser, &positionals, &positional_count) != 0)
	{
		printf("Failed to retrieve parsed values.\n");
		return ;
	}
	printf("verbose : %s\n", verbose ? "true" : "false");
	printf("port    : %ld\n", port);
	printf("timeout : %.2f\n", timeout);
	printf("host    : %s\n", host);
	if (positional_count > 0)
	{
		printf("positionals (%zu):\n", positional_count);
		i = 0;
		while (i < positional_count)
		{
			printf("  - %s\n", positionals[i]);
			i++;
		}
	}
}

int	main(int argc, char **argv)
{
	t_arg_parser	parser;
	int				show_help;

	if (arg_init(&parser, g_specs, sizeof(g_specs) / sizeof(g_specs[0])) != 0)
		return (printf("arg_init failed\n"), 1);
	if (arg_parse(&parser, argc, argv) != 0)
	{
		fprintf(stderr, "%s: %s", argv[0], parser.error_msg ? parser.error_msg : "unknown");
		if (parser.error_spec && parser.error_spec->long_name)
			fprintf(stderr, " (--%s)", parser.error_spec->long_name);
		fprintf(stderr, "\n\n");
		arg_print_help(&parser, argv[0]);
		return (arg_free(&parser), 1);
	}
	show_help = 0;
	if (arg_get_bool(&parser, "help", &show_help) == 0 && show_help)
		arg_print_help(&parser, argv[0]);
	else
		print_values(&parser);
	arg_free(&parser);
	return (0);
}
