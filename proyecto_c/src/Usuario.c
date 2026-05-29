#include "../include/Usuario.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Lista imágenes usuario ─────────────────────────────── */
ListaImgUsr* crearListaImgUsr(void) {
    ListaImgUsr* l = (ListaImgUsr*)malloc(sizeof(ListaImgUsr));
    l->cabeza = NULL; l->cantidad = 0;
    return l;
}
void destruirListaImgUsr(ListaImgUsr* l) {
    NodoImgUsr* curr = l->cabeza;
    while (curr) { NodoImgUsr* tmp = curr->siguiente; free(curr); curr = tmp; }
    free(l);
}
void listaImgUsr_agregar(ListaImgUsr* l, NodoImagen* img) {
    NodoImgUsr* nuevo = (NodoImgUsr*)malloc(sizeof(NodoImgUsr));
    nuevo->imagen    = img;
    nuevo->siguiente = NULL;
    nuevo->anterior  = NULL;
    if (!l->cabeza) { l->cabeza = nuevo; }
    else {
        NodoImgUsr* tmp = l->cabeza;
        while (tmp->siguiente) tmp = tmp->siguiente;
        tmp->siguiente  = nuevo;
        nuevo->anterior = tmp;
    }
    l->cantidad++;
}
int listaImgUsr_eliminar(ListaImgUsr* l, int idImg) {
    NodoImgUsr* curr = l->cabeza;
    while (curr) {
        if (curr->imagen->id == idImg) {
            if (curr->anterior) curr->anterior->siguiente = curr->siguiente;
            else l->cabeza = curr->siguiente;
            if (curr->siguiente) curr->siguiente->anterior = curr->anterior;
            free(curr);
            l->cantidad--;
            return 1;
        }
        curr = curr->siguiente;
    }
    return 0;
}
NodoImagen* listaImgUsr_buscar(ListaImgUsr* l, int idImg) {
    NodoImgUsr* curr = l->cabeza;
    while (curr) { if (curr->imagen->id == idImg) return curr->imagen; curr = curr->siguiente; }
    return NULL;
}

/* ── ABB Usuarios ───────────────────────────────────────── */
static Usuario* _usrInsertar(Usuario* n, const char* nombre) {
    if (!n) {
        Usuario* nuevo = (Usuario*)malloc(sizeof(Usuario));
        strncpy(nuevo->nombre, nombre, 63); nuevo->nombre[63]='\0';
        nuevo->imagenes   = crearListaImgUsr();
        nuevo->izquierda  = NULL;
        nuevo->derecha    = NULL;
        return nuevo;
    }
    int cmp = strcmp(nombre, n->nombre);
    if      (cmp < 0) n->izquierda = _usrInsertar(n->izquierda, nombre);
    else if (cmp > 0) n->derecha   = _usrInsertar(n->derecha,   nombre);
    return n;
}

static Usuario* _usrBuscar(Usuario* n, const char* nombre) {
    if (!n) return NULL;
    int cmp = strcmp(nombre, n->nombre);
    if (cmp == 0) return n;
    if (cmp  < 0) return _usrBuscar(n->izquierda, nombre);
    return _usrBuscar(n->derecha, nombre);
}

static Usuario* _usrMinimo(Usuario* n) {
    while (n->izquierda) n = n->izquierda;
    return n;
}

static Usuario* _usrEliminar(Usuario* n, const char* nombre) {
    if (!n) return NULL;
    int cmp = strcmp(nombre, n->nombre);
    if      (cmp < 0) n->izquierda = _usrEliminar(n->izquierda, nombre);
    else if (cmp > 0) n->derecha   = _usrEliminar(n->derecha,   nombre);
    else {
        if (!n->izquierda) { Usuario* tmp = n->derecha; destruirListaImgUsr(n->imagenes); free(n); return tmp; }
        if (!n->derecha)   { Usuario* tmp = n->izquierda; destruirListaImgUsr(n->imagenes); free(n); return tmp; }
        Usuario* suc = _usrMinimo(n->derecha);
        strncpy(n->nombre, suc->nombre, 63);
        n->derecha = _usrEliminar(n->derecha, suc->nombre);
    }
    return n;
}

static void _usrDestruir(Usuario* n) {
    if (!n) return;
    _usrDestruir(n->izquierda);
    _usrDestruir(n->derecha);
    destruirListaImgUsr(n->imagenes);
    free(n);
}

static void _usrDotNodos(Usuario* n, char* out, int maxLen, int* pos) {
    if (!n || *pos >= maxLen-400) return;
    *pos += snprintf(out+*pos, maxLen-*pos,
        "  usr_%s [label=\"%s\" shape=box style=filled fillcolor=lightgreen];\n",
        n->nombre, n->nombre);
    if (n->izquierda) {
        *pos += snprintf(out+*pos, maxLen-*pos,
            "  usr_%s -> usr_%s [label=\"izq\"];\n", n->nombre, n->izquierda->nombre);
        _usrDotNodos(n->izquierda, out, maxLen, pos);
    }
    if (n->derecha) {
        *pos += snprintf(out+*pos, maxLen-*pos,
            "  usr_%s -> usr_%s [label=\"der\"];\n", n->nombre, n->derecha->nombre);
        _usrDotNodos(n->derecha, out, maxLen, pos);
    }
    /* lista de imágenes del usuario */
    NodoImgUsr* curr = n->imagenes->cabeza;
    int idx = 0;
    char prevId[128];
    snprintf(prevId, sizeof(prevId), "usr_%s", n->nombre);
    while (curr && *pos < maxLen-300) {
        char nid[128];
        snprintf(nid, sizeof(nid), "uimg_%s_%d", n->nombre, idx);
        *pos += snprintf(out+*pos, maxLen-*pos,
            "  %s [label=\"img_%d\" style=filled fillcolor=lightsalmon shape=ellipse];\n",
            nid, curr->imagen->id);
        *pos += snprintf(out+*pos, maxLen-*pos,
            "  %s -> %s [style=dotted color=red];\n", prevId, nid);
        strncpy(prevId, nid, sizeof(prevId)-1);
        curr = curr->siguiente;
        idx++;
    }
}

/* ── API pública ────────────────────────────────────────── */
ABBUsuarios* crearABBUsuarios(void) {
    ABBUsuarios* a = (ABBUsuarios*)malloc(sizeof(ABBUsuarios));
    a->raiz = NULL;
    return a;
}
void destruirABBUsuarios(ABBUsuarios* abb) { _usrDestruir(abb->raiz); free(abb); }

int abbUsr_insertar(ABBUsuarios* abb, const char* nombre) {
    if (_usrBuscar(abb->raiz, nombre)) return 0;
    abb->raiz = _usrInsertar(abb->raiz, nombre);
    return 1;
}
Usuario* abbUsr_buscar(ABBUsuarios* abb, const char* nombre) {
    return _usrBuscar(abb->raiz, nombre);
}
int abbUsr_eliminar(ABBUsuarios* abb, const char* nombre) {
    if (!_usrBuscar(abb->raiz, nombre)) return 0;
    abb->raiz = _usrEliminar(abb->raiz, nombre);
    return 1;
}
int abbUsr_modificar(ABBUsuarios* abb, const char* viejo, const char* nuevo) {
    Usuario* u = _usrBuscar(abb->raiz, viejo);
    if (!u) return 0;
    if (_usrBuscar(abb->raiz, nuevo)) return 0;
    /* Guardar lista de imagenes */
    ListaImgUsr* imgs = u->imagenes;
    u->imagenes = crearListaImgUsr(); /* vaciar para que no se destruya */
    abb->raiz = _usrEliminar(abb->raiz, viejo);
    abb->raiz = _usrInsertar(abb->raiz, nuevo);
    Usuario* n = _usrBuscar(abb->raiz, nuevo);
    destruirListaImgUsr(n->imagenes);
    n->imagenes = imgs;
    return 1;
}

void abbUsr_generarDOT(ABBUsuarios* abb, char* out, int maxLen) {
    int pos = 0;
    pos += snprintf(out+pos, maxLen-pos,
        "digraph ABBUsuarios {\n  node [fontsize=12];\n  label=\"Arbol de Usuarios\";\n");
    if (!abb->raiz)
        pos += snprintf(out+pos, maxLen-pos, "  vacio [label=\"(vacio)\"];\n");
    else
        _usrDotNodos(abb->raiz, out, maxLen, &pos);
    snprintf(out+pos, maxLen-pos, "}\n");
}
