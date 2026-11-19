#include <string.h>
#include "maquina_pedido.h"
#include "mini_test.h"

/* ---------- Auxiliares ---------- */
/* Eventos do caminho mais curto até cada estado */
static const Evento ROTA[N_ESTADOS][4] = {
    /* CRIADO       */ {0},
    /* PAGO         */ {EV_PAGAR},
    /* EM_SEPARACAO */ {EV_PAGAR, EV_SEPARAR},
    /* ENVIADO      */ {EV_PAGAR, EV_SEPARAR, EV_ENVIAR},
    /* ENTREGUE     */ {EV_PAGAR, EV_SEPARAR, EV_ENVIAR, EV_ENTREGAR},
    /* CANCELADO    */ {EV_CANCELAR},
};
static const int TAM_ROTA[N_ESTADOS] = {0, 1, 2, 3, 4, 1};

/* Leva um pedido ao estado desejado; retorna 1 se conseguiu. */
static int pedido_em(Pedido *p, Estado alvo) {
    pedido_init(p);
    for (int i = 0; i < TAM_ROTA[alvo]; i++) {
        if (pedido_disparar(p, ROTA[alvo][i]) != OK) return 0;
    }
    return p->estado == alvo;
}

/* ---------- Testes de ESTADOS (13 casos) ---------- */
static void testes_de_estados(void) {
    Pedido p;
    pedido_init(&p);
    caso(p.estado == CRIADO && p.tam_historico == 1 && p.historico[0] == CRIADO
             && !estado_final(p.estado) && !p.estorno_pendente,
         "estado_inicial_e_criado");

    for (int e = 0; e < N_ESTADOS; e++)
        caso(pedido_em(&p, (Estado)e), "estado_alcancavel[%s]", nome_estado((Estado)e));

    Estado finais[2] = {ENTREGUE, CANCELADO};
    for (int i = 0; i < 2; i++) {
        int ok = pedido_em(&p, finais[i]) && estado_final(p.estado);
        for (int ev = 0; ev < N_EVENTOS; ev++)
            ok = ok && pedido_disparar(&p, (Evento)ev) == ERRO_TRANSICAO_INVALIDA;
        ok = ok && p.estado == finais[i];
        caso(ok, "estado_final_nao_aceita_eventos[%s]", nome_estado(finais[i]));
    }

    Estado inter[4] = {CRIADO, PAGO, EM_SEPARACAO, ENVIADO};
    for (int i = 0; i < 4; i++)
        caso(pedido_em(&p, inter[i]) && !estado_final(p.estado),
             "estado_intermediario_nao_e_final[%s]", nome_estado(inter[i]));
}

/* ---------- Testes de TRANSIÇÕES (36 casos) ---------- */
static int eh_valida(int s, int e) { return transicao_destino((Estado)s, (Evento)e) != -1; }

static void testes_de_transicoes(void) {
    Pedido p;
    int validas = 0, invalidas = 0;

    /* 7 transições válidas */
    for (int s = 0; s < N_ESTADOS; s++)
        for (int e = 0; e < N_EVENTOS; e++) {
            if (!eh_valida(s, e)) continue;
            validas++;
            int destino = transicao_destino((Estado)s, (Evento)e);
            int ok = pedido_em(&p, (Estado)s)
                  && pedido_disparar(&p, (Evento)e) == OK
                  && (int)p.estado == destino
                  && p.historico[p.tam_historico - 1] == (Estado)destino;
            caso(ok, "transicao_valida[%s --%s--> %s]", nome_estado((Estado)s),
                 nome_evento((Evento)e), nome_estado((Estado)destino));
        }

    for (int s = 0; s < N_ESTADOS; s++)
        for (int e = 0; e < N_EVENTOS; e++)
            if (!eh_valida(s, e)) invalidas++;
    caso(validas == 7 && invalidas == 23, "quantidade_de_transicoes[7 validas, 23 invalidas]");

    /* 23 transições inválidas: erro e estado/histórico preservados */
    for (int s = 0; s < N_ESTADOS; s++)
        for (int e = 0; e < N_EVENTOS; e++) {
            if (eh_valida(s, e)) continue;
            int ok = pedido_em(&p, (Estado)s);
            int tam = p.tam_historico, estorno = p.estorno_pendente;
            ok = ok && pedido_disparar(&p, (Evento)e) == ERRO_TRANSICAO_INVALIDA
                    && p.estado == (Estado)s && p.tam_historico == tam
                    && p.estorno_pendente == estorno;
            caso(ok, "transicao_invalida[%s, %s]", nome_estado((Estado)s), nome_evento((Evento)e));
        }

    /* Estorno apenas após o pagamento */
    struct { Estado origem; int estorno; } ce[3] = {
        {CRIADO, 0}, {PAGO, 1}, {EM_SEPARACAO, 1}};
    for (int i = 0; i < 3; i++) {
        int ok = pedido_em(&p, ce[i].origem)
              && pedido_disparar(&p, EV_CANCELAR) == OK
              && p.estorno_pendente == ce[i].estorno;
        caso(ok, "cancelamento_estorno[%s -> estorno=%d]", nome_estado(ce[i].origem), ce[i].estorno);
    }

    /* Robustez: argumentos inválidos */
    caso(pedido_disparar(0, EV_PAGAR) == ERRO_ARGUMENTO_INVALIDO, "robustez_pedido_nulo");
    caso(transicao_destino((Estado)-1, EV_PAGAR) == -1
             && transicao_destino(N_ESTADOS, EV_PAGAR) == -1
             && transicao_destino(CRIADO, (Evento)-1) == -1
             && transicao_destino(CRIADO, N_EVENTOS) == -1
             && strcmp(nome_estado(N_ESTADOS), "?") == 0
             && strcmp(nome_estado((Estado)-1), "?") == 0
             && strcmp(nome_evento(N_EVENTOS), "?") == 0
             && strcmp(nome_evento((Evento)-1), "?") == 0,
         "robustez_enums_fora_do_intervalo");
}

/* ---------- Testes de CAMINHOS independentes (6 casos, V(G) = 4) ---------- */
typedef struct {
    const char *nome;
    int n;
    Evento eventos[4];
    Estado esperado[5];
    int estorno;
} Caminho;

static const Caminho CAMINHOS[4] = {
    {"C1_cancelar_sem_pagar", 1, {EV_CANCELAR}, {CRIADO, CANCELADO}, 0},
    {"C2_cancelar_apos_pagar", 2, {EV_PAGAR, EV_CANCELAR}, {CRIADO, PAGO, CANCELADO}, 1},
    {"C3_cancelar_em_separacao", 3, {EV_PAGAR, EV_SEPARAR, EV_CANCELAR},
     {CRIADO, PAGO, EM_SEPARACAO, CANCELADO}, 1},
    {"C4_fluxo_feliz", 4, {EV_PAGAR, EV_SEPARAR, EV_ENVIAR, EV_ENTREGAR},
     {CRIADO, PAGO, EM_SEPARACAO, ENVIADO, ENTREGUE}, 0},
};

static void testes_de_caminhos(void) {
    int usadas[N_ESTADOS][N_EVENTOS] = {{0}};

    for (int c = 0; c < 4; c++) {
        Pedido p;
        pedido_init(&p);
        int ok = 1;
        for (int i = 0; i < CAMINHOS[c].n; i++) {
            usadas[p.estado][CAMINHOS[c].eventos[i]] = 1;
            ok = ok && pedido_disparar(&p, CAMINHOS[c].eventos[i]) == OK;
        }
        ok = ok && p.tam_historico == CAMINHOS[c].n + 1;
        for (int i = 0; ok && i < p.tam_historico; i++)
            ok = p.historico[i] == CAMINHOS[c].esperado[i];
        ok = ok && estado_final(p.estado) && p.estorno_pendente == CAMINHOS[c].estorno;
        caso(ok, "caminho_independente[%s]", CAMINHOS[c].nome);
    }

    /* A união dos caminhos usa exatamente as transições válidas */
    int cobre = 1;
    for (int s = 0; s < N_ESTADOS; s++)
        for (int e = 0; e < N_EVENTOS; e++)
            if (usadas[s][e] != eh_valida(s, e)) cobre = 0;
    caso(cobre, "caminhos_cobrem_todas_as_transicoes");

    /* V(G) = E - N + 2, com nó de saída virtual */
    int n = N_ESTADOS + 1, e = 7 + 2;
    caso(e - n + 2 == 4 && 4 == (int)(sizeof CAMINHOS / sizeof CAMINHOS[0]),
         "numero_de_caminhos_igual_complexidade_ciclomatica[V(G)=%d]", e - n + 2);
}

int main(void) {
    testes_de_estados();
    testes_de_transicoes();
    testes_de_caminhos();
    return resumo();
}
