NAME = webserv

GREEN = \033[1;32m
RED = \033[1;31m
MAG = \033[1;35m
EOC = \033[0m

CC = c++
CFLAGS = -Wall -Wextra -Werror  -std=c++98
RM = rm -f

SRC_DIR = src
OBJ_DIR = obj

FILE =	main										\
		Cgi													\
		Config											\
		Route												\
		server											\
		Utils												\
		Response										\
		ServerException							\
		request_parser							\
		request_parser_start_line		\
		request_parser_headers			\
		request_parser_utils				\
		class_uri										\
		launch_servers							\
		launch_servers_utils				\
		read_chunked								\
		Mime												\
		responding									\
#		request_parser_tester				\

SRCS = $(addprefix $(SRC_DIR)/, $(addsuffix .cpp, $(FILE)))
OBJS = $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(FILE)))

all : $(NAME)

$(NAME) : $(OBJS)
	@echo "$(MAG)Compiling...$(EOC)"
	@$(CC) $(CFLAGS) $^ -o $@
	@echo "$(GREEN)Compiled! ✅$(EOC)"

change:
	$(eval CFLAGS += -g3 -fsanitize=address)

sani:  change all

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR) :
	@if [ ! -d $(OBJ_DIR) ]; then mkdir -p $(OBJ_DIR); fi

clean:
	@echo "$(MAG)Removing o-files...$(EOC)"
	@$(RM) $(OBJS)
	@echo "$(RED)clean done 🧼$(EOC)\n"

fclean: clean
	@echo "$(MAG)Removing binary -files...$(EOC)"
	@$(RM) $(NAME)
	@rm -rf $(OBJ_DIR)
	@echo "$(RED)fclean done 🛁$(EOC)\n"

re: fclean all

.PHONY: clean re fclean all
