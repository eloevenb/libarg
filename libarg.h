#ifndef LIBARG_H
# define LIBARG_H

# include <sys/types.h>

typedef enum    e_arg_type {    // Examples
	ARG_BOOL,                   // --verbose, -v
	ARG_INT,                    // --port 80
	ARG_FLOAT,                  // --ratio 1.25
	ARG_STRING,                 // --name file.txt
	ARG_STRING_LIST             // --skip a --skip b, ARG_MULTIPLE REQUIRED TO WORK
} t_arg_type;

typedef enum    e_arg_flags {
	ARG_REQUIRED = 1 << 0,
	ARG_MULTIPLE = 1 << 1,
} t_arg_flags;

typedef struct  		s_arg_spec {
	const char      	*long_name;     // "verbose" for --verbose
	char            	short_name;     // 'v' for -v, 0 if none
	t_arg_type      	type;
	unsigned int    	flags;          // one of t_arg_flags
	const char      	*help;          // help message for that arg
	const char      	*metavar;       // "FILE", "PORT", "HOST" etc... for the usage string
	const char      	*default_str;   // optional, parse at init
} t_arg_spec;

typedef struct			s_positional_spec {
	const char			*name;
	const char			*help;
	unsigned int		flags;		// uses t_arg_flags
} t_positional_spec;

typedef union   		s_arg_value {
	int         		b;
	long        		i;
	double      		f;
	const char  		*s;
} t_arg_value;

typedef struct          s_arg_item {
	const t_arg_spec    *spec;      // Points to argument's specs
	int                 is_set;     // If it was present in argv
	size_t              count;      // for ARG_MULTIPLE
	t_arg_value         value;      // single value only
	const char          **list;     // if ARG_STRING_LIST
} t_arg_item;

typedef struct			s_arg_parser {
	const t_arg_spec	*specs;
	size_t				spec_count;
	t_arg_item			*items;    // same length as specs
	const t_positional_spec	*positional_specs;
	size_t				positional_spec_count;
	const char			*positionals_help;
	const char			**positionals;
	size_t				positional_count;
	const char			**resolved_positionals;
	size_t				resolved_positional_count;
	const char			*error_msg;
	const t_arg_spec	*error_spec;
} t_arg_parser;


/*	
*	Initializes the parser with the given t_arg_spec list
*	
*	Arguments:
*	- parser (t_arg_parser *) will be cleared and initialized
*	- specs (const t_arg_spec *) list of spec_count elements, this is the only table that you need to edit
*	- spec_count (size_t) number of elements in specs
*
*	Returns 0 on success, 1 on failure.
*/
int		arg_init(t_arg_parser *parser, const t_arg_spec *specs, size_t spec_count);

/*
*	Configures positional arguments metadata used for validation and help output.
*
*	Arguments:
*	- parser (t_arg_parser *) should be initialized with arg_init.
*	- specs (const t_positional_spec *) list of positional specs or NULL to clear.
*	- spec_count (size_t) number of elements in specs.
*	- positionals_help (const char *) optional section help for positionals.
*
*	Returns 0 on success, 1 on failure.
*/
int		arg_set_positional_specs(t_arg_parser *parser,
			const t_positional_spec *specs, size_t spec_count,
			const char *positionals_help);

/*	
*	Parses the command line arguments with the configured parser.
* 
*	Arguments:
*	- parser (t_arg_parser *) should be initialized with arg_init.
*	- argc (int)
*	- argv (char **)
*
*	Returns 0 on success, 1 on error.
*/
int		arg_parse(t_arg_parser *parser, int argc, char **argv);

/* 
*	Frees the memory allocated for the parser.
*	
*	Arguments:
*	- parser (t_arg_parser *) should be initialized with arg_init.
*/
void	arg_free(t_arg_parser *parser);

/*
*	Gets the value of a boolean argument.
*	
*	Arguments:
*	- parser (t_arg_parser *) should be initialized with arg_init.
*	- name (const char *) the name of the argument.
*	- out (int *) the output value. (0 or 1)
*/
int		arg_get_bool(const t_arg_parser *parser, const char *name, int *out);

/*
*	Gets the value of an integer argument.
*	
*	Arguments:
*	- parser (t_arg_parser *) should be initialized with arg_init.
*	- name (const char *) the name of the argument.
*	- out (long *) the output value.
*/
int		arg_get_int(const t_arg_parser *parser, const char *name, long *out);

/*
*	Gets the value of a float argument.
*	
*	Arguments:
*	- parser (t_arg_parser *) should be initialized with arg_init.
*	- name (const char *) the name of the argument.
*	- out (double *) the output value.
*/
int		arg_get_float(const t_arg_parser *parser, const char *name, double *out);

/*
*	Gets the value of a string argument.
*	
*	Arguments:
*	- parser (t_arg_parser *) should be initialized with arg_init.
*	- name (const char *) the name of the argument.
*	- out (const char **) the output value.
*/
int		arg_get_string(const t_arg_parser *parser, const char *name, const char **out);

/*
*	Gets the value of a string list argument.
*	
*	Arguments:
*	- parser (t_arg_parser *) should be initialized with arg_init.
*	- name (const char *) the name of the argument.
*	- out (const char ***) the output value.
*	- count (size_t *) the number of elements in the list.
*/
int		arg_get_string_list(const t_arg_parser *parser, const char *name,
			const char ***out, size_t *count);

/*
*	Gets all positional arguments collected during parsing.
*
*	Arguments:
*	- parser (t_arg_parser *) should be initialized with arg_init.
*	- out (const char ***) array of positional values.
*	- count (size_t *) number of positional arguments.
*
*	When positional specs are configured without ARG_MULTIPLE, the returned
*	array is spec-aligned. Optional omitted values are returned as NULL holes.
*	In that case, count equals positional_spec_count.
*/
int		arg_get_positionals(const t_arg_parser *parser,
			const char ***out, size_t *count);

/*
*	Prints the help message for the parser.
*	
*	Arguments:
*	- parser (t_arg_parser *) should be initialized with arg_init.
*	- prog_name (const char *) the name of the program.
*/
void	arg_print_help(const t_arg_parser *parser, const char *prog_name);

#endif