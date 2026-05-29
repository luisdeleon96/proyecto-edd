#ifndef MATRIZ_DISPERSA_H
#define MATRIZ_DISPERSA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Nodo pixel ─────────────────────────────────────────── */
typedef struct NodoPixel {
    int  fila;
    int  columna;
    char color[12];          /* ej. #e74c3c */
    struct NodoPixel* derecha;
    struct NodoPixel* abajo;
} NodoPixel;

/* ── Cabecera de fila ────────────────────────────────────── */
typedef struct NodoCabeceraFila {
    int  fila;
    NodoPixel*             primero;
    struct NodoCabeceraFila* siguiente;
} NodoCabeceraFila;

/* ── Cabecera de columna ─────────────────────────────────── */
typedef struct NodoCabeceraColumna {
    int  columna;
    NodoPixel*                primero;
    struct NodoCabeceraColumna* siguiente;
} NodoCabeceraColumna;

/* ── Matriz dispersa ─────────────────────────────────────── */
typedef struct {
    NodoCabeceraFila*    filas;
    NodoCabeceraColumna* columnas;
} MatrizDispersa;

/* Funciones */
MatrizDispersa* crearMatriz(void);
void            destruirMatriz(MatrizDispersa* m);
void            insertarPixel(MatrizDispersa* m, int fila, int col, const char* color);
const char*     obtenerColor(MatrizDispersa* m, int fila, int col);
int             getMaxFila(MatrizDispersa* m);
int             getMaxColumna(MatrizDispersa* m);
void            generarDOTMatriz(MatrizDispersa* m, const char* nombre, char* out, int maxLen);

#endif
