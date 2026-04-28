FUNC	=	arg_internal arg_init arg_parse arg_getters arg_free arg_help

SRC		=	$(addprefix src/, $(addsuffix .c, $(addprefix ft_, $(FUNC))))

OBJS	=	$(SRC:.c=.o)

CC		=	gcc

CFLAGS	=	-Wall -Wextra -Werror

RM		=	rm -f

NAME	=	libarg.a


all: 		$(NAME)

.c.o:
			${CC} ${CFLAGS} -c $< -o $(<:.c=.o)

$(NAME):	$(OBJS)
			ar -rcs $(NAME) $^

clean:
			$(RM) $(OBJS) $(BOBJS)

fclean:		clean
			$(RM) $(NAME)

re:			fclean all

.PHONY:		clean fclean all re