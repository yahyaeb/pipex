# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: yel-bouk <yel-bouk@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/02/16 17:10:45 by yel-bouk          #+#    #+#              #
#    Updated: 2025/03/18 13:35:37 by yel-bouk         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ft_libft.a
CC = gcc
CFLAGS = -Wall -Wextra -Werror
AR = ar rcs

LIBFT_DIR = libft
PRINTF_DIR = ft_printf
GNL_DIR = GNL


LIBFT = $(LIBFT_DIR)/libft.a
PRINTF = $(PRINTF_DIR)/libftprintf.a
GNL = $(GNL_DIR)/libgnl.a

all: $(GNL) $(LIBFT) $(PRINTF) $(NAME) 

$(LIBFT):
	make -C $(LIBFT_DIR)

$(PRINTF):
	make -C $(PRINTF_DIR)

$(GNL):
	make -C $(GNL_DIR)

$(NAME): $(LIBFT) $(PRINTF) $(GNL)
	$(AR) $(NAME) $(LIBFT_DIR)/*.o $(PRINTF_DIR)/srcs/*.o $(GNL_DIR)/*.o
	ranlib $(NAME)  # Ensure indexing is correct

clean:
	make -C $(LIBFT_DIR) clean
	make -C $(PRINTF_DIR) clean
	make -C $(GNL_DIR) clean
	
fclean: clean
	make -C $(LIBFT_DIR) fclean
	make -C $(PRINTF_DIR) fclean
	make -C $(GNL_DIR) fclean
	rm -f $(NAME)

re: fclean all
