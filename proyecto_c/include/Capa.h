#ifndef CAPA_H
#define CAPA_H

#include "MatrizDispersa.h"

/* ── Nodo capa (nodo del ABB) ───────────────────────────── */
typedef struct Capa {
    int              id;
    MatrizDispersa*  matriz;
    struct Capa*     izquierda;
    struct Capa*     derecha;
} Capa;

/* ── ABB de capas ───────────────────────────────────────── */
typedef struct {
    Capa* raiz;
} ABBCapas;

/* Funciones ABB */
ABBCapas* crearABBCapas(void);
void      destruirABBCapas(ABBCapas* abb);
void      abbCapas_insertar(ABBCapas* abb, int id);
Capa*     abbCapas_buscar(ABBCapas* abb, int id);
int       abbCapas_total(ABBCapas* abb);

/* Recorridos: llenan arreglo[] con punteros a Capa */
void abbCapas_inorden(ABBCapas* abb,    Capa** arr, int* total);
void abbCapas_preorden(ABBCapas* abb,   Capa** arr, int* total);
void abbCapas_postorden(ABBCapas* abb,  Capa** arr, int* total);

/* DOT */
void abbCapas_generarDOT(ABBCapas* abb, char* out, int maxLen);

#endif
