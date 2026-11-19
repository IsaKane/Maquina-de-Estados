# Máquina de Estados em C – Pedido de E-commerce
Trabalho Prático – Análise de Algoritmos.

## Estrutura
- `include/maquina_estado.h` – API pública (estados, eventos, pedido)
- `src/maquina_estado.c` – implementação (tabela de transições)
- `tests/test_maquina.c` – testes automatizados de estados, transições e caminhos
- `tests/mini_test.h` – mini-framework de testes (sem dependências externas)

## Como executar (requer gcc e make)
    make test       # compila e executa os 55 testes
    make cov        # cobertura de linhas e ramos com gcov
    make sanitize   # testes com AddressSanitizer + UBSan
    make clean
