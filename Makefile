CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -Werror -Iinclude
SRC     = src/maquina_pedido.c
TEST    = tests/test_maquina.c

.PHONY: test cov sanitize clean

test:
	$(CC) $(CFLAGS) $(SRC) $(TEST) -o test_maquina
	./test_maquina

# Cobertura de linhas e ramos com gcov
cov: clean
	$(CC) $(CFLAGS) --coverage -O0 $(SRC) $(TEST) -o test_maquina
	./test_maquina > /dev/null
	gcov -b -c -o . test_maquina-maquina_pedido.gcda 2>/dev/null || gcov -b -c maquina_pedido.c

# Testes com AddressSanitizer e UndefinedBehaviorSanitizer
sanitize:
	$(CC) $(CFLAGS) -g -fsanitize=address,undefined $(SRC) $(TEST) -o test_maquina_san
	./test_maquina_san > /dev/null && echo "sanitizers: OK"

clean:
	rm -f test_maquina test_maquina_san *.gcda *.gcno *.gcov
