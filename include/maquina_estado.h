#ifndef MAQUINA_ESTADO_H
#define MAQUINA_ESTADO_H

/* Máquina de estados de um Pedido de e-commerce. */

typedef enum {
    CRIADO = 0,
    PAGO,
    EM_SEPARACAO,
    ENVIADO,
    ENTREGUE,
    CANCELADO,
    N_ESTADOS
} Estado;

typedef enum {
    EV_PAGAR = 0,
    EV_SEPARAR,
    EV_ENVIAR,
    EV_ENTREGAR,
    EV_CANCELAR,
    N_EVENTOS
} Evento;

typedef enum {
    OK = 0,
    ERRO_TRANSICAO_INVALIDA = -1,
    ERRO_ARGUMENTO_INVALIDO = -2
} Resultado;

/* O grafo não tem ciclos: o histórico nunca excede N_ESTADOS posições. */
#define MAX_HISTORICO N_ESTADOS

typedef struct {
    Estado estado;
    Estado historico[MAX_HISTORICO];
    int tam_historico;
    int estorno_pendente;
} Pedido;

/* Cria um pedido no estado inicial CRIADO. */
void pedido_init(Pedido *p);

/* Dispara um evento. Em caso de erro, o pedido permanece inalterado. */
Resultado pedido_disparar(Pedido *p, Evento ev);

/* Retorna o estado destino de origem, ev ou -1 se a transição é inválida. */
int transicao_destino(Estado origem, Evento ev);

/* 1 se o estado é final, ENTREGUE ou CANCELADO, 0 caso contrário. */
int estado_final(Estado e);

const char *nome_estado(Estado e);
const char *nome_evento(Evento ev);

#endif
