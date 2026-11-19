#ifndef MINI_TEST_H
#define MINI_TEST_H
#include <stdio.h>
#include <stdarg.h>

/* Mini-framework de testes: cada chamada de caso() é um caso reportado. */
static int g_total = 0, g_falhas = 0;

static void caso(int condicao, const char *fmt, ...) {
    char nome[160];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(nome, sizeof nome, fmt, ap);
    va_end(ap);
    g_total++;
    if (!condicao) g_falhas++;
    printf("[%s] %s\n", condicao ? "PASS" : "FAIL", nome);
}

static int resumo(void) {
    printf("\n%d testes, %d aprovados, %d falhas\n", g_total, g_total - g_falhas, g_falhas);
    return g_falhas == 0 ? 0 : 1;
}
#endif
