#ifndef USUARIO_H
#define USUARIO_H

#include "Imagen.h"

/* ── Lista de imágenes de un usuario ────────────────────── */
typedef struct NodoImgUsr {
    NodoImagen*       imagen;
    struct NodoImgUsr* siguiente;
    struct NodoImgUsr* anterior;
} NodoImgUsr;

typedef struct {
    NodoImgUsr* cabeza;
    int         cantidad;
} ListaImgUsr;

ListaImgUsr* crearListaImgUsr(void);
void         destruirListaImgUsr(ListaImgUsr* l);
void         listaImgUsr_agregar(ListaImgUsr* l, NodoImagen* img);
int          listaImgUsr_eliminar(ListaImgUsr* l, int idImg);
NodoImagen*  listaImgUsr_buscar(ListaImgUsr* l, int idImg);

/* ── Nodo usuario (nodo ABB) ────────────────────────────── */
typedef struct Usuario {
    char          nombre[64];
    ListaImgUsr*  imagenes;
    struct Usuario* izquierda;
    struct Usuario* derecha;
} Usuario;

/* ── ABB de usuarios ────────────────────────────────────── */
typedef struct {
    Usuario* raiz;
} ABBUsuarios;

ABBUsuarios* crearABBUsuarios(void);
void         destruirABBUsuarios(ABBUsuarios* abb);
int          abbUsr_insertar(ABBUsuarios* abb, const char* nombre);
Usuario*     abbUsr_buscar(ABBUsuarios* abb, const char* nombre);
int          abbUsr_eliminar(ABBUsuarios* abb, const char* nombre);
int          abbUsr_modificar(ABBUsuarios* abb, const char* viejo, const char* nuevo);

void abbUsr_generarDOT(ABBUsuarios* abb, char* out, int maxLen);

#endif
