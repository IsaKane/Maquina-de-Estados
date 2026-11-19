#include "maquina_pedido.h"

#define X (-1) /* transição inválida */

/* Tabela de transições: TRANSICOES[estado_atual][evento] = próximo estado */
static const int TRANSICOES[N_ESTADOS][N_EVENTOS] = {
    /*               PAGAR  SEPARAR       ENVIAR   ENTREGAR  CANCELAR  */
    /* CRIADO       */ {PAGO, X,            X,       X,        CANCELADO},
    /* PAGO         */ {X,    EM_SEPARACAO, X,       X,        CANCELADO},
    /* EM_SEPARACAO */ {X,    X,            ENVIADO, X,        CANCELADO},
    /* ENVIADO      */ {X,    X,            X,       ENTREGUE, X},
    /* ENTREGUE     */ {X,    X,            X,       X,        X},
    /* CANCELADO    */ {X,    X,            X,       X,        X},
};

static const char *NOMES_ESTADO[N_ESTADOS] = {
    "CRIADO", "PAGO", "EM_SEPARACAO", "ENVIADO", "ENTREGUE", "CANCELADO"};

static const char *NOMES_EVENTO[N_EVENTOS] = {
    "pagar", "separar", "enviar", "entregar", "cancelar"};

void pedido_init(Pedido *p) {
    p->estado = CRIADO;
    p->historico[0] = CRIADO;
    p->tam_historico = 1;
    p->estorno_pendente = 0;
}

int transicao_destino(Estado origem, Evento ev) {
    if ((int)origem < 0 || origem >= N_ESTADOS || (int)ev < 0 || ev >= N_EVENTOS) {
        return X;
    }
    return TRANSICOES[origem][ev];
}

Resultado pedido_disparar(Pedido *p, Evento ev) {
    if (p == 0) {
        return ERRO_ARGUMENTO_INVALIDO;
    }
    int proximo = transicao_destino(p->estado, ev);
    if (proximo == X) {
        return ERRO_TRANSICAO_INVALIDA;
    }
    /* Cancelar após o pagamento exige estorno */
    if (ev == EV_CANCELAR && (p->estado == PAGO || p->estado == EM_SEPARACAO)) {
        p->estorno_pendente = 1;
    }
    p->estado = (Estado)proximo;
    p->historico[p->tam_historico++] = p->estado;
    return OK;
}

int estado_final(Estado e) {
    return e == ENTREGUE || e == CANCELADO;
}

const char *nome_estado(Estado e) {
    return ((int)e >= 0 && e < N_ESTADOS) ? NOMES_ESTADO[e] : "?";
}

const char *nome_evento(Evento ev) {
    return ((int)ev >= 0 && ev < N_EVENTOS) ? NOMES_EVENTO[ev] : "?";
}
