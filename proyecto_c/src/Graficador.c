#include "../include/Graficador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Escribe DOT directo a FILE* sin buffer ─────────────── */

int compilarDOT(const char* dotFile, const char* pngFile) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "dot -Tpng \"%s\" -o \"%s\"", dotFile, pngFile);
    int ret = system(cmd);
    if (ret != 0) {
        printf("[ERROR] Graphviz fallo. Verifica que dot.exe este en el PATH.\n");
        return 0;
    }
    printf("  -> Imagen generada: %s\n", pngFile);
    return 1;
}

void abrirImagen(const char* pngFile) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "start \"\" \"%s\"", pngFile);
    system(cmd);
}

/* ── Imagen rasterizada: escribe DOT directo a archivo ───── */
static int escribirDOTImagen(Capa** capas, int n, const char* dotFile) {
    FILE* f = fopen(dotFile, "w");
    if (!f) { printf("[ERROR] No se pudo crear: %s\n", dotFile); return 0; }

    if (n == 0) {
        fprintf(f, "digraph img { vacio [label=\"Sin capas\"]; }\n");
        fclose(f); return 1;
    }

    /* Calcular dimensiones */
    int maxF = 0, maxC = 0;
    for (int i = 0; i < n; i++) {
        int mf = getMaxFila(capas[i]->matriz);
        int mc = getMaxColumna(capas[i]->matriz);
        if (mf > maxF) maxF = mf;
        if (mc > maxC) maxC = mc;
    }
    if (maxF <= 0) maxF = 1;
    if (maxC <= 0) maxC = 1;

    /* Canvas dinamico */
    int filas = maxF + 1;
    int cols  = maxC + 1;
    char** canvas = (char**)malloc(filas * sizeof(char*));
    for (int i = 0; i < filas; i++) {
        canvas[i] = (char*)malloc(cols * 12);
        for (int j = 0; j < cols; j++)
            strcpy(canvas[i] + j*12, "#FFFFFF");
    }

    /* Superponer capas */
    for (int i = 0; i < n; i++) {
        NodoCabeceraFila* cf = capas[i]->matriz->filas;
        while (cf) {
            NodoPixel* p = cf->primero;
            while (p) {
                if (p->fila < filas && p->columna < cols) {
                    char* dest = canvas[p->fila] + p->columna * 12;
                    strncpy(dest, p->color, 11);
                    dest[11] = '\0';
                    if (dest[0] != '#') {
                        char tmp[12];
                        snprintf(tmp, 12, "#%s", dest);
                        strncpy(dest, tmp, 11);
                        dest[11] = '\0';
                    }
                }
                p = p->derecha;
            }
            cf = cf->siguiente;
        }
    }

    /* Tamano de pixel dinamico */
    int dimMax = maxF > maxC ? maxF : maxC;
    int px;
    if      (dimMax <= 10)  px = 24;
    else if (dimMax <= 20)  px = 18;
    else if (dimMax <= 35)  px = 12;
    else if (dimMax <= 50)  px = 8;
    else if (dimMax <= 80)  px = 5;
    else if (dimMax <= 120) px = 4;
    else                    px = 3;

    /* Escribir DOT */
    fprintf(f,
        "digraph Imagen {\n"
        "  graph [dpi=96 bgcolor=white];\n"
        "  node [shape=plaintext margin=0];\n"
        "  imagen [label=<\n"
        "    <TABLE BORDER=\"1\" CELLBORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\">\n");

    for (int row = 1; row <= maxF; row++) {
        fprintf(f, "      <TR>\n");
        for (int col = 1; col <= maxC; col++) {
            char* color = canvas[row] + col * 12;
            fprintf(f,
                "        <TD WIDTH=\"%d\" HEIGHT=\"%d\" BGCOLOR=\"%s\" FIXEDSIZE=\"TRUE\"></TD>\n",
                px, px, color);
        }
        fprintf(f, "      </TR>\n");
    }

    fprintf(f, "    </TABLE>>];\n}\n");
    fclose(f);

    /* Liberar canvas */
    for (int i = 0; i < filas; i++) free(canvas[i]);
    free(canvas);
    return 1;
}

int graficar_desdeCapas(Capa** capas, int n, const char* nombreSalida) {
    char dotFile[256], pngFile[256];
    snprintf(dotFile, sizeof(dotFile), "%s.dot", nombreSalida);
    snprintf(pngFile, sizeof(pngFile), "%s.png", nombreSalida);
    if (!escribirDOTImagen(capas, n, dotFile)) return 0;
    int ok = compilarDOT(dotFile, pngFile);
    if (ok) abrirImagen(pngFile);
    return ok;
}

/* ── Matriz dispersa ─────────────────────────────────────── */
int graficar_matrizCapa(Capa* c, const char* nombreSalida) {
    if (!c) { printf("[ERROR] Capa nula.\n"); return 0; }
    char dotFile[256], pngFile[256];
    snprintf(dotFile, sizeof(dotFile), "%s.dot", nombreSalida);
    snprintf(pngFile, sizeof(pngFile), "%s.png", nombreSalida);

    /* Buffer generoso para la matriz */
    int bufSize = 1024 * 1024; /* 1 MB */
    char* buf = (char*)malloc(bufSize);
    if (!buf) return 0;
    char nombre[32]; snprintf(nombre, sizeof(nombre), "Capa%d", c->id);
    generarDOTMatriz(c->matriz, nombre, buf, bufSize);
    FILE* f = fopen(dotFile, "w");
    if (f) { fputs(buf, f); fclose(f); }
    free(buf);

    int ok = compilarDOT(dotFile, pngFile);
    if (ok) abrirImagen(pngFile);
    return ok;
}

/* ── ABB capas ───────────────────────────────────────────── */
int graficar_abbCapas(ABBCapas* abb, const char* nombreSalida) {
    char dotFile[256], pngFile[256];
    snprintf(dotFile, sizeof(dotFile), "%s.dot", nombreSalida);
    snprintf(pngFile, sizeof(pngFile), "%s.png", nombreSalida);

    int bufSize = 512 * 1024;
    char* buf = (char*)malloc(bufSize);
    if (!buf) return 0;
    abbCapas_generarDOT(abb, buf, bufSize);
    FILE* f = fopen(dotFile, "w");
    if (f) { fputs(buf, f); fclose(f); }
    free(buf);

    int ok = compilarDOT(dotFile, pngFile);
    if (ok) abrirImagen(pngFile);
    return ok;
}

/* ── Lista circular ──────────────────────────────────────── */
int graficar_listaImagenes(ListaCircular* l, const char* nombreSalida) {
    char dotFile[256], pngFile[256];
    snprintf(dotFile, sizeof(dotFile), "%s.dot", nombreSalida);
    snprintf(pngFile, sizeof(pngFile), "%s.png", nombreSalida);

    int bufSize = 512 * 1024;
    char* buf = (char*)malloc(bufSize);
    if (!buf) return 0;
    listaCircular_generarDOTConCapas(l, buf, bufSize);
    FILE* f = fopen(dotFile, "w");
    if (f) { fputs(buf, f); fclose(f); }
    free(buf);

    int ok = compilarDOT(dotFile, pngFile);
    if (ok) abrirImagen(pngFile);
    return ok;
}

/* ── Imagen + ABB con flechas ────────────────────────────── */
static void _dotABBNodos(Capa* n, FILE* f) {
    if (!n) return;
    fprintf(f,
        "    abb_%d [label=\"capa_%d\" style=filled fillcolor=lightblue];\n",
        n->id, n->id);
    if (n->izquierda) {
        fprintf(f, "    abb_%d -> abb_%d [label=\"izq\"];\n", n->id, n->izquierda->id);
        _dotABBNodos(n->izquierda, f);
    }
    if (n->derecha) {
        fprintf(f, "    abb_%d -> abb_%d [label=\"der\"];\n", n->id, n->derecha->id);
        _dotABBNodos(n->derecha, f);
    }
}

int graficar_imagenConArbol(NodoImagen* img, ABBCapas* abb, const char* nombreSalida) {
    if (!img) { printf("[ERROR] Imagen no encontrada.\n"); return 0; }
    char dotFile[256], pngFile[256];
    snprintf(dotFile, sizeof(dotFile), "%s.dot", nombreSalida);
    snprintf(pngFile, sizeof(pngFile), "%s.png", nombreSalida);

    FILE* f = fopen(dotFile, "w");
    if (!f) return 0;

    fprintf(f,
        "digraph ImagenConArbol {\n"
        "  rankdir=TB;\n"
        "  node [shape=box fontsize=11];\n"
        "  label=\"Imagen %d con Arbol de Capas\";\n"
        "  compound=true;\n", img->id);

    fprintf(f,
        "  img_%d [label=\"imagen_%d\" style=filled fillcolor=lightyellow];\n",
        img->id, img->id);

    /* Lista de capas */
    NodoListaCapa* lc = img->capas->cabeza;
    int idx = 0;
    char prevId[64];
    snprintf(prevId, sizeof(prevId), "img_%d", img->id);

    int idsCapa[512];
    char nids[512][64];
    int nCapa = 0;

    while (lc) {
        char nid[64];
        snprintf(nid, sizeof(nid), "lc_%d", idx);
        fprintf(f,
            "  %s [label=\"capa_%d\" style=filled fillcolor=salmon shape=ellipse];\n",
            nid, lc->capa->id);
        fprintf(f, "  %s -> %s [color=red];\n", prevId, nid);
        strncpy(prevId, nid, sizeof(prevId)-1);
        if (nCapa < 512) {
            idsCapa[nCapa] = lc->capa->id;
            strncpy(nids[nCapa], nid, 63);
            nCapa++;
        }
        lc = lc->siguiente;
        idx++;
    }

    /* Subgraph ABB */
    fprintf(f,
        "  subgraph cluster_abb {\n"
        "    label=\"ABB Capas\";\n"
        "    style=dashed;\n");
    _dotABBNodos(abb->raiz, f);
    fprintf(f, "  }\n");

    /* Flechas lista -> ABB */
    for (int i = 0; i < nCapa; i++)
        fprintf(f, "  %s -> abb_%d [color=red style=dashed constraint=false];\n",
            nids[i], idsCapa[i]);

    fprintf(f, "}\n");
    fclose(f);

    int ok = compilarDOT(dotFile, pngFile);
    if (ok) abrirImagen(pngFile);
    return ok;
}

/* ── ABB usuarios ────────────────────────────────────────── */
int graficar_abbUsuarios(ABBUsuarios* abb, const char* nombreSalida) {
    char dotFile[256], pngFile[256];
    snprintf(dotFile, sizeof(dotFile), "%s.dot", nombreSalida);
    snprintf(pngFile, sizeof(pngFile), "%s.png", nombreSalida);

    int bufSize = 512 * 1024;
    char* buf = (char*)malloc(bufSize);
    if (!buf) return 0;
    abbUsr_generarDOT(abb, buf, bufSize);
    FILE* f = fopen(dotFile, "w");
    if (f) { fputs(buf, f); fclose(f); }
    free(buf);

    int ok = compilarDOT(dotFile, pngFile);
    if (ok) abrirImagen(pngFile);
    return ok;
}
