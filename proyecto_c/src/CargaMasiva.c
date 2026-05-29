#include "../include/CargaMasiva.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ── trim in-place ──────────────────────────────────────── */
static void trim(char* s) {
    /* izquierda */
    int start = 0;
    while (s[start] && isspace((unsigned char)s[start])) start++;
    if (start) memmove(s, s+start, strlen(s)-start+1);
    /* derecha */
    int len = (int)strlen(s);
    while (len > 0 && isspace((unsigned char)s[len-1])) s[--len] = '\0';
}

/* ── Cargar capas ────────────────────────────────────────── */
int cargarCapas(const char* archivo, ABBCapas* abb) {
    FILE* f = fopen(archivo, "r");
    if (!f) { printf("[ERROR] No se pudo abrir: %s\n", archivo); return 0; }
    printf("[CARGA] Cargando capas desde: %s\n", archivo);

    char linea[256];
    int  capaActual = -1;
    int  insertadas = 0;

    while (fgets(linea, sizeof(linea), f)) {
        /* quitar \n */
        linea[strcspn(linea, "\r\n")] = '\0';
        trim(linea);
        if (strlen(linea) == 0) continue;

        /* inicio de capa: contiene '{' pero no '}' */
        char* pLlave = strchr(linea, '{');
        char* pCierre = strchr(linea, '}');

        if (pLlave && !pCierre) {
            *pLlave = '\0';
            trim(linea);
            capaActual = atoi(linea);
            abbCapas_insertar(abb, capaActual);
            insertadas++;
            continue;
        }
        if (pCierre && !pLlave) { capaActual = -1; continue; }

        /* pixel: fila,columna,color; */
        if (capaActual >= 0 && strchr(linea, ',')) {
            /* quitar ; */
            char* pPV = strchr(linea, ';');
            if (pPV) *pPV = '\0';

            char* tok = strtok(linea, ",");
            if (!tok) continue;
            int fila = atoi(tok);
            tok = strtok(NULL, ",");
            if (!tok) continue;
            int col = atoi(tok);
            tok = strtok(NULL, ",");
            if (!tok) continue;
            trim(tok);

            Capa* c = abbCapas_buscar(abb, capaActual);
            if (c) insertarPixel(c->matriz, fila, col, tok);
        }
    }
    fclose(f);
    printf("  -> %d capas cargadas.\n", insertadas);
    return 1;
}

/* ── Cargar imágenes ─────────────────────────────────────── */
int cargarImagenes(const char* archivo, ListaCircular* lista, ABBCapas* abb) {
    FILE* f = fopen(archivo, "r");
    if (!f) { printf("[ERROR] No se pudo abrir: %s\n", archivo); return 0; }
    printf("[CARGA] Cargando imagenes desde: %s\n", archivo);

    char linea[512];
    int  insertadas = 0;

    while (fgets(linea, sizeof(linea), f)) {
        linea[strcspn(linea, "\r\n")] = '\0';
        trim(linea);
        if (strlen(linea) == 0) continue;

        char* pA = strchr(linea, '{');
        char* pC = strchr(linea, '}');
        if (!pA || !pC) continue;

        /* id */
        *pA = '\0';
        trim(linea);
        int idImg = atoi(linea);

        /* capas entre { } */
        char capasStr[400];
        int len = (int)(pC - pA - 1);
        if (len < 0) len = 0;
        if (len > 399) len = 399;
        strncpy(capasStr, pA+1, len);
        capasStr[len] = '\0';
        trim(capasStr);

        listaCircular_insertar(lista, idImg);
        insertadas++;

        NodoImagen* img = listaCircular_buscar(lista, idImg);
        if (!img || strlen(capasStr) == 0) continue;

        char* tok = strtok(capasStr, ",");
        while (tok) {
            trim(tok);
            if (strlen(tok) > 0) {
                int idCapa = atoi(tok);
                Capa* c = abbCapas_buscar(abb, idCapa);
                if (c) listaCapas_agregar(img->capas, c);
                else   printf("  [WARN] Capa %d no encontrada para imagen %d\n", idCapa, idImg);
            }
            tok = strtok(NULL, ",");
        }
    }
    fclose(f);
    printf("  -> %d imagenes cargadas.\n", insertadas);
    return 1;
}

/* ── Cargar usuarios ─────────────────────────────────────── */
int cargarUsuarios(const char* archivo, ABBUsuarios* abbUsr, ListaCircular* listaImg) {
    FILE* f = fopen(archivo, "r");
    if (!f) { printf("[ERROR] No se pudo abrir: %s\n", archivo); return 0; }
    printf("[CARGA] Cargando usuarios desde: %s\n", archivo);

    char linea[512];
    int  insertados = 0;

    while (fgets(linea, sizeof(linea), f)) {
        linea[strcspn(linea, "\r\n")] = '\0';
        trim(linea);
        if (strlen(linea) == 0) continue;

        /* quitar ; final */
        int len = (int)strlen(linea);
        if (len > 0 && linea[len-1] == ';') linea[--len] = '\0';

        char* pDos = strchr(linea, ':');
        if (!pDos) continue;
        *pDos = '\0';
        trim(linea);
        char nombre[64];
        strncpy(nombre, linea, 63); nombre[63]='\0';

        char* imgsStr = pDos + 1;
        trim(imgsStr);

        if (strlen(nombre) == 0) continue;
        abbUsr_insertar(abbUsr, nombre);
        insertados++;

        Usuario* u = abbUsr_buscar(abbUsr, nombre);
        if (!u || strlen(imgsStr) == 0) continue;

        char* tok = strtok(imgsStr, ",");
        while (tok) {
            trim(tok);
            if (strlen(tok) > 0) {
                int idImg = atoi(tok);
                NodoImagen* img = listaCircular_buscar(listaImg, idImg);
                if (img) listaImgUsr_agregar(u->imagenes, img);
                else printf("  [WARN] Imagen %d no encontrada para usuario %s\n", idImg, nombre);
            }
            tok = strtok(NULL, ",");
        }
    }
    fclose(f);
    printf("  -> %d usuarios cargados.\n", insertados);
    return 1;
}
