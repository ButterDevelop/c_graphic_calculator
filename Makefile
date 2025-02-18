# Компилятор
CC = gcc

# Флаги компилятора
CFLAGS = -g -Wall

# Исходные файлы
SRC = main.c parse_input.c parser_utils.c function_normalizer.c shuntingyard.c postscriptexport.c

# Объектные файлы
OBJ = $(SRC:.c=.o)

# Исполняемый файл
EXEC = SemestralWork

# Цель по умолчанию - компиляция программы
all: $(EXEC)

# Правило компиляции программы из объектных файлов
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ) -lm

# Правило для компиляции .o файлов из .c файлов
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Цели для Valgrind с разными параметрами
test1: $(EXEC)
	valgrind --leak-check=full ./$(EXEC) "(sin    (x)    +cos   (x)   )   *  5" output.ps -10.5:10.6:-5.7:5

test2: $(EXEC)
	valgrind --leak-check=full ./$(EXEC) "(x^ 2+ 3*x + 2)" output.ps -20:20:-10:10

test3: $(EXEC)
	valgrind --leak-check=full ./$(EXEC) "sqrt(x)" output.ps 0:100:0:10

test4: $(EXEC)
	valgrind --leak-check=full ./$(EXEC) "log(x)" output.ps 1:10:0:5

test5: $(EXEC)
	valgrind --leak-check=full ./$(EXEC) x^2+3*x+2 output.ps 

test6: $(EXEC)
	valgrind --leak-check=full ./$(EXEC) "a+(x^2+3*x+2)+c" output.ps

test7: $(EXEC)
	valgrind --leak-check=full ./$(EXEC) "sin(n)x^2+3*x+2" output.ps

test8: $(EXEC)
	valgrind --leak-check=full ./$(EXEC) "x - 1E-1 + 1E1 + .5E-02" output.ps

# Очистка скомпилированных файлов
clean:
	rm -f $(OBJ) $(EXEC)
