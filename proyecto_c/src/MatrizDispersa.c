#include "../include/MatrizDispersa.h"

MatrizDispersa* crearMatriz(void) {
    MatrizDispersa* m = (MatrizDispersa*)malloc(sizeof(MatrizDispersa));
    m->filas    = NULL;
    m->columnas = NULL;
    return m;
}

/* ── Cabecera fila: obtener o crear ─────────────────────── */
static NodoCabeceraFila* obtenerOCrearFila(MatrizDispersa* m, int f) {
    NodoCabeceraFila* prev = NULL;
    NodoCabeceraFila* curr = m->filas;
    while (curr && curr->fila < f) { prev = curr; curr = curr->siguiente; }
    if (curr && curr->fila == f) return curr;
    NodoCabeceraFila* nuevo = (NodoCabeceraFila*)malloc(sizeof(NodoCabeceraFila));
    nuevo->fila     = f;
    nuevo->primero  = NULL;
    nuevo->siguiente = curr;
    if (prev) prev->siguiente = nuevo; else m->filas = nuevo;
    return nuevo;
}

/* ── Cabecera columna: obtener o crear ──────────────────── */
static NodoCabeceraColumna* obtenerOCrearColumna(MatrizDispersa* m, int c) {
    NodoCabeceraColumna* prev = NULL;
    NodoCabeceraColumna* curr = m->columnas;
    while (curr && curr->columna < c) { prev = curr; curr = curr->siguiente; }
    if (curr && curr->columna == c) return curr;
    NodoCabeceraColumna* nuevo = (NodoCabeceraColumna*)malloc(sizeof(NodoCabeceraColumna));
    nuevo->columna  = c;
    nuevo->primero  = NULL;
    nuevo->siguiente = curr;
    if (prev) prev->siguiente = nuevo; else m->columnas = nuevo;
    return nuevo;
}

void insertarPixel(MatrizDispersa* m, int fila, int col, const char* color) {
    NodoCabeceraFila*    cf = obtenerOCrearFila(m, fila);
    NodoCabeceraColumna* cc = obtenerOCrearColumna(m, col);

    /* Si ya existe, actualizar color */
    NodoPixel* p = cf->primero;
    while (p) {
        if (p->columna == col) { strncpy(p->color, color, 11); p->color[11]='\0'; return; }
        p = p->derecha;
    }

    NodoPixel* nuevo = (NodoPixel*)malloc(sizeof(NodoPixel));
    nuevo->fila    = fila;
    nuevo->columna = col;
    strncpy(nuevo->color, color, 11); nuevo->color[11] = '\0';
    nuevo->derecha = NULL;
    nuevo->abajo   = NULL;

    /* Insertar en lista de fila (orden por columna) */
    NodoPixel* prevP = NULL;
    NodoPixel* currP = cf->primero;
    while (currP && currP->columna < col) { prevP = currP; currP = currP->derecha; }
    nuevo->derecha = currP;
    if (prevP) prevP->derecha = nuevo; else cf->primero = nuevo;

    /* Insertar en lista de columna (orden por fila) */
    prevP = NULL;
    currP = cc->primero;
    while (currP && currP->fila < fila) { prevP = currP; currP = currP->abajo; }
    nuevo->abajo = currP;
    if (prevP) prevP->abajo = nuevo; else cc->primero = nuevo;
}

const char* obtenerColor(MatrizDispersa* m, int fila, int col) {
    NodoCabeceraFila* cf = m->filas;
    while (cf && cf->fila < fila) cf = cf->siguiente;
    if (!cf || cf->fila != fila) return "";
    NodoPixel* p = cf->primero;
    while (p && p->columna < col) p = p->derecha;
    if (!p || p->columna != col) return "";
    return p->color;
}

int getMaxFila(MatrizDispersa* m) {
    int max = 0;
    NodoCabeceraFila* cf = m->filas;
    while (cf) { if (cf->fila > max) max = cf->fila; cf = cf->siguiente; }
    return max;
}

int getMaxColumna(MatrizDispersa* m) {
    int max = 0;
    NodoCabeceraColumna* cc = m->columnas;
    while (cc) { if (cc->columna > max) max = cc->columna; cc = cc->siguiente; }
    return max;
}

void destruirMatriz(MatrizDispersa* m) {
    NodoCabeceraFila* cf = m->filas;
    while (cf) {
        NodoPixel* p = cf->primero;
        while (p) { NodoPixel* tmp = p->derecha; free(p); p = tmp; }
        NodoCabeceraFila* tmp = cf->siguiente; free(cf); cf = tmp;
    }
    NodoCabeceraColumna* cc = m->columnas;
    while (cc) { NodoCabeceraColumna* tmp = cc->siguiente; free(cc); cc = tmp; }
    free(m);
}

void generarDOTMatriz(MatrizDispersa* m, const char* nombre, char* out, int maxLen) {
    int pos = 0;
    pos += snprintf(out+pos, maxLen-pos,
        "digraph %s {\n  rankdir=LR;\n  node [shape=record fontsize=10];\n"
        "  label=\"Matriz Capa %s\";\n", nombre, nombre);

    NodoCabeceraFila* cf = m->filas;
    while (cf && pos < maxLen-200) {
        pos += snprintf(out+pos, maxLen-pos,
            "  fila_%d [label=\"%d\" shape=box style=filled fillcolor=lightyellow];\n",
            cf->fila, cf->fila);
        NodoPixel* p = cf->primero;
        while (p && pos < maxLen-200) {
            char col[16];
            strncpy(col, p->color, 15); col[15]='\0';
            /* quitar # para fillcolor */
            char colHex[16];
            if (col[0]=='#') strncpy(colHex, col+1, 14);
            else             strncpy(colHex, col, 14);
            colHex[14]='\0';
            pos += snprintf(out+pos, maxLen-pos,
                "  p_%d_%d [label=\"%s\\n(%d,%d)\" style=filled fillcolor=\"#%s\"];\n",
                p->fila, p->columna, p->color, p->fila, p->columna, colHex);
            p = p->derecha;
        }
        cf = cf->siguiente;
    }
    /* conexiones fila */
    cf = m->filas;
    while (cf && pos < maxLen-200) {
        pos += snprintf(out+pos, maxLen-pos, "  fila_%d", cf->fila);
        NodoPixel* p = cf->primero;
        while (p && pos < maxLen-200) {
            pos += snprintf(out+pos, maxLen-pos, " -> p_%d_%d", p->fila, p->columna);
            p = p->derecha;
        }
        pos += snprintf(out+pos, maxLen-pos, ";\n");
        cf = cf->siguiente;
    }
    /* conexiones columna (abajo) */
    NodoCabeceraColumna* cc = m->columnas;
    while (cc && pos < maxLen-200) {
        NodoPixel* p = cc->primero;
        while (p && p->abajo && pos < maxLen-200) {
            pos += snprintf(out+pos, maxLen-pos,
                "  p_%d_%d -> p_%d_%d [style=dashed color=blue];\n",
                p->fila, p->columna, p->abajo->fila, p->abajo->columna);
            p = p->abajo;
        }
        cc = cc->siguiente;
    }
    snprintf(out+pos, maxLen-pos, "}\n");
}
