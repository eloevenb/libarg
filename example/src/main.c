#include "../../libarg.h"

#include <stdio.h>

static const t_arg_spec	g_specs[] = {
	{"help", 'h', ARG_BOOL, 0, "Print this help message", NULL, NULL},
	{"verbose", 'v', ARG_BOOL, 0, "Enable verbose output", NULL, NULL},
	{"intercept", 'i', ARG_BOOL, 0, "Enable intercept mode", NULL, NULL},
	{"interval", 't', ARG_FLOAT, 0, "Poisoning interval in seconds", "SECONDS",
		"2.0"},
	{"iface", 'I', ARG_STRING, 0, "Interface name override", "IFACE", NULL},
};

static const t_positional_spec	g_positional_specs[] = {
	{"source_ip", "IPv4 address of sender to impersonate", ARG_REQUIRED},
	{"source_mac", "Sender hardware address", ARG_REQUIRED},
	{"target_ip", "IPv4 address of target host", ARG_REQUIRED},
	{"target_mac", "Target hardware address (required in intercept mode)", 0},
	{"spoofed_mac", "MAC used for spoof ownership", ARG_REQUIRED},
};

static void	print_values(const t_arg_parser *parser)
{
	int			verbose;
	int			intercept;
	double		interval;
	const char	*iface;
	const char	**positionals;
	size_t		positional_count;
	size_t		i;

	if (arg_get_bool(parser, "verbose", &verbose) != 0
		|| arg_get_bool(parser, "intercept", &intercept) != 0
		|| arg_get_float(parser, "interval", &interval) != 0
		|| arg_get_string(parser, "iface", &iface) != 0
		|| arg_get_positionals(parser, &positionals, &positional_count) != 0)
	{
		printf("Failed to retrieve parsed values.\n");
		return ;
	}
	printf("verbose : %s\n", verbose ? "true" : "false");
	printf("intercept: %s\n", intercept ? "true" : "false");
	printf("interval : %.2f\n", interval);
	printf("iface    : %s\n", iface ? iface : "(default)");
	if (positional_count > 0)
	{
		printf("positionals (%zu):\n", positional_count);
		i = 0;
		while (i < positional_count)
		{
			printf("  - %s\n", positionals[i] ? positionals[i] : "(null)");
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
	if (arg_set_positional_specs(&parser, g_positional_specs,
			sizeof(g_positional_specs) / sizeof(g_positional_specs[0]),
			"Values consumed after all options") != 0)
		return (printf("arg_set_positional_specs failed\n"), arg_free(&parser), 1);
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
