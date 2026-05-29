#include "../include/Imagen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Lista de capas ─────────────────────────────────────── */
ListaCapasImagen* crearListaCapas(void) {
    ListaCapasImagen* l = (ListaCapasImagen*)malloc(sizeof(ListaCapasImagen));
    l->cabeza   = NULL;
    l->cantidad = 0;
    return l;
}

void destruirListaCapas(ListaCapasImagen* l) {
    NodoListaCapa* curr = l->cabeza;
    while (curr) { NodoListaCapa* tmp = curr->siguiente; free(curr); curr = tmp; }
    free(l);
}

void listaCapas_agregar(ListaCapasImagen* l, Capa* c) {
    NodoListaCapa* nuevo = (NodoListaCapa*)malloc(sizeof(NodoListaCapa));
    nuevo->capa      = c;
    nuevo->siguiente = NULL;
    if (!l->cabeza) { l->cabeza = nuevo; }
    else {
        NodoListaCapa* tmp = l->cabeza;
        while (tmp->siguiente) tmp = tmp->siguiente;
        tmp->siguiente = nuevo;
    }
    l->cantidad++;
}

/* ── Lista circular ─────────────────────────────────────── */
ListaCircular* crearListaCircular(void) {
    ListaCircular* l = (ListaCircular*)malloc(sizeof(ListaCircular));
    l->cabeza   = NULL;
    l->cantidad = 0;
    return l;
}

void destruirListaCircular(ListaCircular* l) {
    if (!l->cabeza) { free(l); return; }
    /* romper circularidad */
    l->cabeza->anterior->siguiente = NULL;
    NodoImagen* curr = l->cabeza;
    while (curr) {
        NodoImagen* tmp = curr->siguiente;
        destruirListaCapas(curr->capas);
        free(curr);
        curr = tmp;
    }
    free(l);
}

void listaCircular_insertar(ListaCircular* l, int id) {
    if (listaCircular_buscar(l, id)) {
        printf("  [Lista] Imagen %d ya existe.\n", id);
        return;
    }
    NodoImagen* nuevo = (NodoImagen*)malloc(sizeof(NodoImagen));
    nuevo->id        = id;
    nuevo->capas     = crearListaCapas();
    nuevo->siguiente = NULL;
    nuevo->anterior  = NULL;

    if (!l->cabeza) {
        l->cabeza        = nuevo;
        nuevo->siguiente = nuevo;
        nuevo->anterior  = nuevo;
        l->cantidad++;
        return;
    }
    /* insertar ordenado */
    NodoImagen* curr = l->cabeza;
    do {
        if (id < curr->id) break;
        curr = curr->siguiente;
    } while (curr != l->cabeza);

    NodoImagen* prev = curr->anterior;
    prev->siguiente  = nuevo;
    nuevo->anterior  = prev;
    nuevo->siguiente = curr;
    curr->anterior   = nuevo;
    if (id < l->cabeza->id) l->cabeza = nuevo;
    l->cantidad++;
}

NodoImagen* listaCircular_buscar(ListaCircular* l, int id) {
    if (!l->cabeza) return NULL;
    NodoImagen* curr = l->cabeza;
    do {
        if (curr->id == id) return curr;
        curr = curr->siguiente;
    } while (curr != l->cabeza);
    return NULL;
}

int listaCircular_eliminar(ListaCircular* l, int id) {
    if (!l->cabeza) return 0;
    NodoImagen* curr = l->cabeza;
    do {
        if (curr->id == id) {
            if (l->cantidad == 1) { l->cabeza = NULL; }
            else {
                curr->anterior->siguiente = curr->siguiente;
                curr->siguiente->anterior = curr->anterior;
                if (curr == l->cabeza) l->cabeza = curr->siguiente;
            }
            destruirListaCapas(curr->capas);
            free(curr);
            l->cantidad--;
            return 1;
        }
        curr = curr->siguiente;
    } while (curr != l->cabeza);
    return 0;
}

void listaCircular_imprimir(ListaCircular* l) {
    if (!l->cabeza) { printf("  (lista vacia)\n"); return; }
    NodoImagen* curr = l->cabeza;
    do {
        printf("  Imagen %d -> capas: ", curr->id);
        NodoListaCapa* lc = curr->capas->cabeza;
        while (lc) { printf("%d ", lc->capa->id); lc = lc->siguiente; }
        printf("\n");
        curr = curr->siguiente;
    } while (curr != l->cabeza);
}

void listaCircular_generarDOT(ListaCircular* l, char* out, int maxLen) {
    int pos = 0;
    pos += snprintf(out+pos, maxLen-pos,
        "digraph ListaImagenes {\n  rankdir=LR;\n"
        "  node [shape=record fontsize=11];\n"
        "  label=\"Lista Circular de Imagenes\";\n");
    if (!l->cabeza) {
        pos += snprintf(out+pos, maxLen-pos, "  vacio [label=\"(vacio)\"];\n");
        snprintf(out+pos, maxLen-pos, "}\n");
        return;
    }
    NodoImagen* curr = l->cabeza;
    do {
        pos += snprintf(out+pos, maxLen-pos,
            "  img_%d [label=\"imagen %d\" style=filled fillcolor=lightyellow];\n",
            curr->id, curr->id);
        curr = curr->siguiente;
    } while (curr != l->cabeza);
    curr = l->cabeza;
    do {
        pos += snprintf(out+pos, maxLen-pos,
            "  img_%d -> img_%d [label=\"sig\"];\n", curr->id, curr->siguiente->id);
        pos += snprintf(out+pos, maxLen-pos,
            "  img_%d -> img_%d [label=\"ant\" style=dashed];\n",
            curr->id, curr->anterior->id);
        curr = curr->siguiente;
    } while (curr != l->cabeza);
    snprintf(out+pos, maxLen-pos, "}\n");
}

void listaCircular_generarDOTConCapas(ListaCircular* l, char* out, int maxLen) {
    int pos = 0;
    pos += snprintf(out+pos, maxLen-pos,
        "digraph ListaConCapas {\n  rankdir=TB;\n"
        "  node [shape=box fontsize=11];\n"
        "  label=\"Lista de Imagenes con Capas\";\n");

    if (!l->cabeza) {
        pos += snprintf(out+pos, maxLen-pos, "  vacio [label=\"(vacio)\"];\n");
        snprintf(out+pos, maxLen-pos, "}\n");
        return;
    }

    NodoImagen* curr = l->cabeza;
    do {
        pos += snprintf(out+pos, maxLen-pos,
            "  img_%d [label=\"imagen_%d\" style=filled fillcolor=lightyellow];\n",
            curr->id, curr->id);
        curr = curr->siguiente;
    } while (curr != l->cabeza);

    /* conexiones circulares */
    curr = l->cabeza;
    do {
        pos += snprintf(out+pos, maxLen-pos,
            "  img_%d -> img_%d;\n", curr->id, curr->siguiente->id);
        pos += snprintf(out+pos, maxLen-pos,
            "  img_%d -> img_%d [style=dashed];\n", curr->id, curr->anterior->id);
        curr = curr->siguiente;
    } while (curr != l->cabeza);

    /* listas de capas */
    curr = l->cabeza;
    do {
        NodoListaCapa* lc = curr->capas->cabeza;
        int idx = 0;
        char prevId[64];
        snprintf(prevId, sizeof(prevId), "img_%d", curr->id);
        while (lc && pos < maxLen-300) {
            char nid[64];
            snprintf(nid, sizeof(nid), "lc_%d_%d", curr->id, idx);
            pos += snprintf(out+pos, maxLen-pos,
                "  %s [label=\"capa_%d\" style=filled fillcolor=lightcyan shape=ellipse];\n",
                nid, lc->capa->id);
            pos += snprintf(out+pos, maxLen-pos, "  %s -> %s;\n", prevId, nid);
            strncpy(prevId, nid, sizeof(prevId)-1);
            lc = lc->siguiente;
            idx++;
        }
        curr = curr->siguiente;
    } while (curr != l->cabeza);

    snprintf(out+pos, maxLen-pos, "}\n");
}
