#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/CargaMasiva.h"
#include "include/Graficador.h"

/* ── Datos globales ──────────────────────────────────────── */
ABBCapas*      abbCapas;
ListaCircular* listaImagenes;
ABBUsuarios*   abbUsuarios;

/* ── Utilidades ──────────────────────────────────────────── */
static void limpiarBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static int leerEntero(const char* msg) {
    int v;
    printf("%s", msg);
    while (scanf("%d", &v) != 1) {
        printf("  Valor invalido. Ingrese un numero: ");
        limpiarBuffer();
    }
    limpiarBuffer();
    return v;
}

static void leerCadena(const char* msg, char* dest, int maxLen) {
    printf("%s", msg);
    fgets(dest, maxLen, stdin);
    dest[strcspn(dest, "\r\n")] = '\0';
}

static void pausar(void) {
    printf("\n  Presione ENTER para continuar...");
    getchar();
}

static void cabecera(const char* titulo) {
    printf("\n+----------------------------------------------+\n");
    printf("|  %-44s|\n", titulo);
    printf("+----------------------------------------------+\n");
}

/* ── Carga masiva ────────────────────────────────────────── */
static void menuCargaMasiva(void) {
    cabecera("CARGA MASIVA");
    char archCap[256], archImg[256], archUsr[256];
    leerCadena("  Archivo de capas    (.cap): ", archCap, sizeof(archCap));
    leerCadena("  Archivo de imagenes (.im) : ", archImg, sizeof(archImg));
    leerCadena("  Archivo de usuarios (.usr): ", archUsr, sizeof(archUsr));

    cargarCapas(archCap, abbCapas);
    cargarImagenes(archImg, listaImagenes, abbCapas);
    cargarUsuarios(archUsr, abbUsuarios, listaImagenes);
    pausar();
}

/* ── Generacion de imagenes ──────────────────────────────── */
static void generarPorRecorrido(void) {
    cabecera("GENERAR: RECORRIDO LIMITADO");
    int total = abbCapas_total(abbCapas);
    if (total == 0) { printf("  No hay capas cargadas.\n"); pausar(); return; }

    int n = leerEntero("  Numero de capas a utilizar: ");
    printf("  Tipo de recorrido:\n");
    printf("    1. Inorden\n    2. Preorden\n    3. Postorden\n");
    int tipo = leerEntero("  Opcion: ");

    Capa** arreglo = (Capa**)malloc(total * sizeof(Capa*));
    int totalRec = 0;

    switch (tipo) {
        case 1: abbCapas_inorden(abbCapas,   arreglo, &totalRec); break;
        case 2: abbCapas_preorden(abbCapas,  arreglo, &totalRec); break;
        case 3: abbCapas_postorden(abbCapas, arreglo, &totalRec); break;
        default: printf("  Opcion invalida.\n"); free(arreglo); pausar(); return;
    }

    if (n > totalRec) n = totalRec;
    printf("  Capas seleccionadas: ");
    for (int i = 0; i < n; i++) printf("%d ", arreglo[i]->id);
    printf("\n");

    graficar_desdeCapas(arreglo, n, "salida_recorrido");
    free(arreglo);
    pausar();
}

static void generarPorLista(void) {
    cabecera("GENERAR: POR LISTA DE IMAGENES");
    int id = leerEntero("  ID de imagen: ");
    NodoImagen* img = listaCircular_buscar(listaImagenes, id);
    if (!img) { printf("  Imagen no encontrada.\n"); pausar(); return; }

    int n = img->capas->cantidad;
    if (n == 0) {
        printf("  La imagen no tiene capas, se genera 1 pixel negro.\n");
        /* capa temporal */
        Capa* tmp = (Capa*)malloc(sizeof(Capa));
        tmp->id = -1; tmp->matriz = crearMatriz();
        tmp->izquierda = tmp->derecha = NULL;
        insertarPixel(tmp->matriz, 1, 1, "#000000");
        Capa* arr[1] = { tmp };
        char nombre[64]; snprintf(nombre, sizeof(nombre), "salida_img_%d", id);
        graficar_desdeCapas(arr, 1, nombre);
        destruirMatriz(tmp->matriz); free(tmp);
        pausar(); return;
    }

    Capa** capas = (Capa**)malloc(n * sizeof(Capa*));
    NodoListaCapa* lc = img->capas->cabeza;
    for (int i = 0; i < n && lc; i++, lc = lc->siguiente)
        capas[i] = lc->capa;

    char nombre[64]; snprintf(nombre, sizeof(nombre), "salida_img_%d", id);
    graficar_desdeCapas(capas, n, nombre);
    free(capas);
    pausar();
}

static void generarPorCapa(void) {
    cabecera("GENERAR: POR CAPA");
    int id = leerEntero("  ID de capa: ");
    Capa* c = abbCapas_buscar(abbCapas, id);
    if (!c) { printf("  Capa no encontrada.\n"); pausar(); return; }
    Capa* arr[1] = { c };
    char nombre[64]; snprintf(nombre, sizeof(nombre), "salida_capa_%d", id);
    graficar_desdeCapas(arr, 1, nombre);
    pausar();
}

static void generarPorUsuario(void) {
    cabecera("GENERAR: POR USUARIO");
    char nombre[64];
    leerCadena("  Nombre de usuario: ", nombre, sizeof(nombre));
    Usuario* u = abbUsr_buscar(abbUsuarios, nombre);
    if (!u) { printf("  Usuario no encontrado.\n"); pausar(); return; }
    if (u->imagenes->cantidad == 0) {
        printf("  El usuario no tiene imagenes.\n"); pausar(); return;
    }
    printf("  Imagenes del usuario:\n");
    NodoImgUsr* curr = u->imagenes->cabeza;
    while (curr) { printf("    - Imagen %d\n", curr->imagen->id); curr = curr->siguiente; }

    int idImg = leerEntero("  ID de imagen a graficar: ");
    NodoImagen* img = listaImgUsr_buscar(u->imagenes, idImg);
    if (!img) { printf("  El usuario no tiene esa imagen.\n"); pausar(); return; }

    int n = img->capas->cantidad;
    char salida[64]; snprintf(salida, sizeof(salida), "salida_usr_%s_%d", nombre, idImg);

    if (n == 0) {
        Capa* tmp = (Capa*)malloc(sizeof(Capa));
        tmp->id = -1; tmp->matriz = crearMatriz();
        tmp->izquierda = tmp->derecha = NULL;
        insertarPixel(tmp->matriz, 1, 1, "#000000");
        Capa* arr[1] = { tmp };
        graficar_desdeCapas(arr, 1, salida);
        destruirMatriz(tmp->matriz); free(tmp);
    } else {
        Capa** capas = (Capa**)malloc(n * sizeof(Capa*));
        NodoListaCapa* lc = img->capas->cabeza;
        for (int i = 0; i < n && lc; i++, lc = lc->siguiente) capas[i] = lc->capa;
        graficar_desdeCapas(capas, n, salida);
        free(capas);
    }
    pausar();
}

static void menuGeneracion(void) {
    int op;
    do {
        cabecera("GENERACION DE IMAGENES");
        printf("  1. Por recorrido limitado\n");
        printf("  2. Por lista de imagenes\n");
        printf("  3. Por capa\n");
        printf("  4. Por usuario\n");
        printf("  0. Volver\n");
        op = leerEntero("  Opcion: ");
        switch (op) {
            case 1: generarPorRecorrido(); break;
            case 2: generarPorLista();     break;
            case 3: generarPorCapa();      break;
            case 4: generarPorUsuario();   break;
        }
    } while (op != 0);
}

/* ── CRUD Usuarios ───────────────────────────────────────── */
static void menuCRUDUsuarios(void) {
    int op;
    do {
        cabecera("CRUD USUARIOS");
        printf("  1. Agregar usuario\n");
        printf("  2. Eliminar usuario\n");
        printf("  3. Modificar usuario\n");
        printf("  4. Buscar usuario\n");
        printf("  0. Volver\n");
        op = leerEntero("  Opcion: ");

        char nombre[64];
        if (op == 1) {
            leerCadena("  Nombre del nuevo usuario: ", nombre, sizeof(nombre));
            if (abbUsr_insertar(abbUsuarios, nombre))
                printf("  Usuario '%s' agregado.\n", nombre);
            else
                printf("  El usuario ya existe.\n");
            pausar();

        } else if (op == 2) {
            leerCadena("  Nombre a eliminar: ", nombre, sizeof(nombre));
            if (abbUsr_eliminar(abbUsuarios, nombre))
                printf("  Usuario eliminado.\n");
            else
                printf("  Usuario no encontrado.\n");
            pausar();

        } else if (op == 3) {
            char nuevo[64];
            leerCadena("  Nombre actual  : ", nombre, sizeof(nombre));
            leerCadena("  Nuevo nombre   : ", nuevo, sizeof(nuevo));
            if (abbUsr_modificar(abbUsuarios, nombre, nuevo))
                printf("  Usuario modificado.\n");
            else
                printf("  No se pudo modificar.\n");
            pausar();

        } else if (op == 4) {
            leerCadena("  Nombre a buscar: ", nombre, sizeof(nombre));
            Usuario* u = abbUsr_buscar(abbUsuarios, nombre);
            if (u) {
                printf("  Usuario encontrado: %s\n", u->nombre);
                printf("  Imagenes (%d):\n", u->imagenes->cantidad);
                NodoImgUsr* curr = u->imagenes->cabeza;
                while (curr) {
                    printf("    - Imagen %d\n", curr->imagen->id);
                    curr = curr->siguiente;
                }
            } else {
                printf("  Usuario no encontrado.\n");
            }
            pausar();
        }
    } while (op != 0);
}

/* ── CRUD Imagenes ───────────────────────────────────────── */
static void menuCRUDImagenes(void) {
    int op;
    do {
        cabecera("CRUD IMAGENES");
        printf("  1. Agregar imagen a usuario\n");
        printf("  2. Eliminar imagen de usuario\n");
        printf("  0. Volver\n");
        op = leerEntero("  Opcion: ");

        char nombre[64];
        if (op == 1) {
            leerCadena("  Nombre de usuario: ", nombre, sizeof(nombre));
            Usuario* u = abbUsr_buscar(abbUsuarios, nombre);
            if (!u) { printf("  Usuario no encontrado.\n"); pausar(); continue; }

            int idImg = leerEntero("  ID de la nueva imagen: ");
            if (listaCircular_buscar(listaImagenes, idImg)) {
                printf("  El ID de imagen ya existe.\n"); pausar(); continue;
            }

            listaCircular_insertar(listaImagenes, idImg);
            NodoImagen* img = listaCircular_buscar(listaImagenes, idImg);

            printf("  Agregar capas (0 para terminar):\n");
            while (1) {
                int idCapa = leerEntero("    ID capa (0 = terminar): ");
                if (idCapa == 0) break;
                Capa* c = abbCapas_buscar(abbCapas, idCapa);
                if (c) listaCapas_agregar(img->capas, c);
                else   printf("    Capa no encontrada.\n");
            }
            listaImgUsr_agregar(u->imagenes, img);
            printf("  Imagen %d agregada al usuario %s.\n", idImg, nombre);
            pausar();

        } else if (op == 2) {
            leerCadena("  Nombre de usuario: ", nombre, sizeof(nombre));
            Usuario* u = abbUsr_buscar(abbUsuarios, nombre);
            if (!u) { printf("  Usuario no encontrado.\n"); pausar(); continue; }

            int idImg = leerEntero("  ID de imagen a eliminar: ");
            int ok1 = listaImgUsr_eliminar(u->imagenes, idImg);
            int ok2 = listaCircular_eliminar(listaImagenes, idImg);
            if (ok1 || ok2) printf("  Imagen %d eliminada.\n", idImg);
            else            printf("  Imagen no encontrada.\n");
            pausar();
        }
    } while (op != 0);
}

/* ── Estado de la memoria ────────────────────────────────── */
static void menuEstadoMemoria(void) {
    int op;
    do {
        cabecera("ESTADO DE LA MEMORIA");
        printf("  1. Ver lista circular de imagenes\n");
        printf("  2. Ver arbol de capas (ABB)\n");
        printf("  3. Ver matriz de una capa\n");
        printf("  4. Ver imagen con arbol de capas\n");
        printf("  5. Ver arbol de usuarios\n");
        printf("  0. Volver\n");
        op = leerEntero("  Opcion: ");

        if (op == 1) {
            graficar_listaImagenes(listaImagenes, "mem_lista_imagenes");
            pausar();
        } else if (op == 2) {
            graficar_abbCapas(abbCapas, "mem_abb_capas");
            pausar();
        } else if (op == 3) {
            int id = leerEntero("  ID de capa: ");
            Capa* c = abbCapas_buscar(abbCapas, id);
            if (c) {
                char nombre[64]; snprintf(nombre, sizeof(nombre), "mem_capa_%d", id);
                graficar_matrizCapa(c, nombre);
            } else printf("  Capa no encontrada.\n");
            pausar();
        } else if (op == 4) {
            int id = leerEntero("  ID de imagen: ");
            NodoImagen* img = listaCircular_buscar(listaImagenes, id);
            if (img) {
                char nombre[64]; snprintf(nombre, sizeof(nombre), "mem_img_arbol_%d", id);
                graficar_imagenConArbol(img, abbCapas, nombre);
            } else printf("  Imagen no encontrada.\n");
            pausar();
        } else if (op == 5) {
            graficar_abbUsuarios(abbUsuarios, "mem_abb_usuarios");
            pausar();
        }
    } while (op != 0);
}

/* ── Main ────────────────────────────────────────────────── */
int main(void) {
    abbCapas      = crearABBCapas();
    listaImagenes = crearListaCircular();
    abbUsuarios   = crearABBUsuarios();

    printf("\n");
    printf("  +=======================================+\n");
    printf("  |   GENERADOR DE IMAGENES POR CAPAS     |\n");
    printf("  |   Universidad Rafael Landivar          |\n");
    printf("  |   Estructura de Datos I  2026          |\n");
    printf("  +=======================================+\n");

    int op;
    do {
        cabecera("MENU PRINCIPAL");
        printf("  1. Carga masiva de datos\n");
        printf("  2. Generacion de imagenes\n");
        printf("  3. CRUD Usuarios\n");
        printf("  4. CRUD Imagenes\n");
        printf("  5. Estado de la memoria (graficas)\n");
        printf("  0. Salir\n");
        op = leerEntero("  Opcion: ");

        switch (op) {
            case 1: menuCargaMasiva();   break;
            case 2: menuGeneracion();    break;
            case 3: menuCRUDUsuarios();  break;
            case 4: menuCRUDImagenes();  break;
            case 5: menuEstadoMemoria(); break;
            case 0: printf("\n  Hasta luego.\n\n"); break;
            default: printf("  Opcion invalida.\n"); break;
        }
    } while (op != 0);

    /* liberar memoria */
    destruirABBCapas(abbCapas);
    destruirListaCircular(listaImagenes);
    destruirABBUsuarios(abbUsuarios);
    return 0;
}
