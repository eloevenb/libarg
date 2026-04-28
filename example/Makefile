SRCS = main.c
NAME = libarg_test

CC = gcc
SRC_DIR = src
INC_DIR = inc
LIBARG_DIR = ..
LIBARG = $(LIBARG_DIR)/libarg.a
CFLAGS = -Wall -Wextra -Werror -I$(INC_DIR) -I$(LIBARG_DIR)
SRC_FILES = $(addprefix $(SRC_DIR)/, $(SRCS))
OBJS = $(SRC_FILES:.c=.o)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $(<:.c=.o)


$(NAME): $(OBJS) $(LIBARG)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) -L$(LIBARG_DIR) -larg

$(LIBARG):
	$(MAKE) -C $(LIBARG_DIR)

all: $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
