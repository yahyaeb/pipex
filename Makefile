# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: yel-bouk <yel-bouk@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/02/20 07:44:20 by yel-bouk          #+#    #+#              #
#    Updated: 2025/03/18 13:54:26 by yel-bouk         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME = pipex
CC = gcc
CFLAGS = -g -Werror -Wextra -Wall
LIBFT_DIR = ft_libft  # Ensure this is the correct relative path
LIBFT = ./ft_libft/ft_libft.a  # Corrected path

SRCS = pipex.c \
		free.c \
		parsing.c 

OBJS = $(SRCS:.c=.o)

all: libft $(NAME)

libft:
	$(MAKE) -C $(LIBFT_DIR)

# Compile Executable
$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -o $(NAME)

# Rule for Compiling .c Files into .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean .o Files
clean:
	rm -f $(OBJS)
	$(MAKE) -C $(LIBFT_DIR) clean

# Clean Everything
fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean

# Rebuild Everything
re: fclean all
