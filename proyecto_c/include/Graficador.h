#ifndef GRAFICADOR_H
#define GRAFICADOR_H

#include "Usuario.h"

/* Tamaño del buffer DOT */
#define DOT_BUF 131072   /* 128 KB */

/* Genera imagen PNG superponiendo capas */
int graficar_desdeCapas(Capa** capas, int n, const char* nombreSalida);

/* Grafica matriz dispersa de una capa */
int graficar_matrizCapa(Capa* c, const char* nombreSalida);

/* Grafica ABB de capas */
int graficar_abbCapas(ABBCapas* abb, const char* nombreSalida);

/* Grafica lista circular con capas */
int graficar_listaImagenes(ListaCircular* l, const char* nombreSalida);

/* Grafica imagen + ABB de capas con flechas (ilustracion 7) */
int graficar_imagenConArbol(NodoImagen* img, ABBCapas* abb, const char* nombreSalida);

/* Grafica ABB de usuarios */
int graficar_abbUsuarios(ABBUsuarios* abb, const char* nombreSalida);

/* Compila .dot -> .png con Graphviz y abre la imagen */
int  compilarDOT(const char* dotFile, const char* pngFile);
void abrirImagen(const char* pngFile);

#endif
