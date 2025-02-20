# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: yel-bouk <yel-bouk@student.42nice.fr>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/02/20 07:44:20 by yel-bouk          #+#    #+#              #
#    Updated: 2025/02/20 08:03:44 by yel-bouk         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = pipex
CC = cc
CFLAGS = -Werror -Wextra -Wall
AR = ar rcs
LIBFT = ft_libft.a
SRCS = 	parsing.c \
		pipex.c \
		pipex_utils.c
		
OBJS = $(SRCS:.c=.o)

# Default Rule
all: $(NAME)

# Compile Executable
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -o $(NAME)

# Rule for Compiling .c Files into .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean .o Files
clean:
	rm -f $(OBJS)

# Clean Everything
fclean: clean
	rm -f $(NAME)

# Rebuild Everything
re: fclean all
		