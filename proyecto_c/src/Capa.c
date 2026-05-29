#include "../include/Capa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── helpers recursivos ─────────────────────────────────── */
static Capa* _insertar(Capa* n, int id) {
    if (!n) {
        Capa* nuevo = (Capa*)malloc(sizeof(Capa));
        nuevo->id         = id;
        nuevo->matriz     = crearMatriz();
        nuevo->izquierda  = NULL;
        nuevo->derecha    = NULL;
        return nuevo;
    }
    if      (id < n->id) n->izquierda = _insertar(n->izquierda, id);
    else if (id > n->id) n->derecha   = _insertar(n->derecha,   id);
    return n;
}

static Capa* _buscar(Capa* n, int id) {
    if (!n) return NULL;
    if (id == n->id) return n;
    if (id  < n->id) return _buscar(n->izquierda, id);
    return _buscar(n->derecha, id);
}

static void _destruir(Capa* n) {
    if (!n) return;
    _destruir(n->izquierda);
    _destruir(n->derecha);
    destruirMatriz(n->matriz);
    free(n);
}

static int _contar(Capa* n) {
    if (!n) return 0;
    return 1 + _contar(n->izquierda) + _contar(n->derecha);
}

static void _inorden(Capa* n, Capa** arr, int* idx) {
    if (!n) return;
    _inorden(n->izquierda, arr, idx);
    arr[(*idx)++] = n;
    _inorden(n->derecha, arr, idx);
}

static void _preorden(Capa* n, Capa** arr, int* idx) {
    if (!n) return;
    arr[(*idx)++] = n;
    _preorden(n->izquierda, arr, idx);
    _preorden(n->derecha, arr, idx);
}

static void _postorden(Capa* n, Capa** arr, int* idx) {
    if (!n) return;
    _postorden(n->izquierda, arr, idx);
    _postorden(n->derecha, arr, idx);
    arr[(*idx)++] = n;
}

static void _dotNodos(Capa* n, char* out, int maxLen, int* pos) {
    if (!n || *pos >= maxLen-300) return;
    *pos += snprintf(out+*pos, maxLen-*pos,
        "  capa_%d [label=\"capa_%d\" shape=box style=filled fillcolor=lightblue];\n",
        n->id, n->id);
    if (n->izquierda) {
        *pos += snprintf(out+*pos, maxLen-*pos,
            "  capa_%d -> capa_%d [label=\"izq\"];\n", n->id, n->izquierda->id);
        _dotNodos(n->izquierda, out, maxLen, pos);
    }
    if (n->derecha) {
        *pos += snprintf(out+*pos, maxLen-*pos,
            "  capa_%d -> capa_%d [label=\"der\"];\n", n->id, n->derecha->id);
        _dotNodos(n->derecha, out, maxLen, pos);
    }
}

/* ── API pública ────────────────────────────────────────── */
ABBCapas* crearABBCapas(void) {
    ABBCapas* a = (ABBCapas*)malloc(sizeof(ABBCapas));
    a->raiz = NULL;
    return a;
}

void destruirABBCapas(ABBCapas* abb) {
    _destruir(abb->raiz);
    free(abb);
}

void abbCapas_insertar(ABBCapas* abb, int id) {
    if (_buscar(abb->raiz, id)) {
        printf("  [ABB] Capa %d ya existe.\n", id);
        return;
    }
    abb->raiz = _insertar(abb->raiz, id);
}

Capa* abbCapas_buscar(ABBCapas* abb, int id) {
    return _buscar(abb->raiz, id);
}

int abbCapas_total(ABBCapas* abb) { return _contar(abb->raiz); }

void abbCapas_inorden(ABBCapas* abb, Capa** arr, int* total) {
    *total = 0; _inorden(abb->raiz, arr, total);
}
void abbCapas_preorden(ABBCapas* abb, Capa** arr, int* total) {
    *total = 0; _preorden(abb->raiz, arr, total);
}
void abbCapas_postorden(ABBCapas* abb, Capa** arr, int* total) {
    *total = 0; _postorden(abb->raiz, arr, total);
}

void abbCapas_generarDOT(ABBCapas* abb, char* out, int maxLen) {
    int pos = 0;
    pos += snprintf(out+pos, maxLen-pos,
        "digraph ABBCapas {\n  node [shape=box fontsize=12];\n  label=\"Arbol de Capas\";\n");
    if (!abb->raiz)
        pos += snprintf(out+pos, maxLen-pos, "  vacio [label=\"(vacio)\"];\n");
    else
        _dotNodos(abb->raiz, out, maxLen, &pos);
    snprintf(out+pos, maxLen-pos, "}\n");
}
