#ifndef IMAGEN_H
#define IMAGEN_H

#include "Capa.h"

/* ── Lista de capas de una imagen ───────────────────────── */
typedef struct NodoListaCapa {
    Capa*                capa;      /* puntero al nodo en ABB */
    struct NodoListaCapa* siguiente;
} NodoListaCapa;

typedef struct {
    NodoListaCapa* cabeza;
    int            cantidad;
} ListaCapasImagen;

ListaCapasImagen* crearListaCapas(void);
void              destruirListaCapas(ListaCapasImagen* l);
void              listaCapas_agregar(ListaCapasImagen* l, Capa* c);

/* ── Nodo imagen (lista circular doble) ─────────────────── */
typedef struct NodoImagen {
    int               id;
    ListaCapasImagen* capas;
    struct NodoImagen* siguiente;
    struct NodoImagen* anterior;
} NodoImagen;

/* ── Lista circular doblemente enlazada de imágenes ─────── */
typedef struct {
    NodoImagen* cabeza;
    int         cantidad;
} ListaCircular;

ListaCircular* crearListaCircular(void);
void           destruirListaCircular(ListaCircular* l);
void           listaCircular_insertar(ListaCircular* l, int id);
NodoImagen*    listaCircular_buscar(ListaCircular* l, int id);
int            listaCircular_eliminar(ListaCircular* l, int id);
void           listaCircular_imprimir(ListaCircular* l);

/* DOT */
void listaCircular_generarDOT(ListaCircular* l, char* out, int maxLen);
void listaCircular_generarDOTConCapas(ListaCircular* l, char* out, int maxLen);

#endif
