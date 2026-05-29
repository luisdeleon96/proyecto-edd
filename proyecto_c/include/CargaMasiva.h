#ifndef CARGA_MASIVA_H
#define CARGA_MASIVA_H

#include "Usuario.h"

int cargarCapas(const char* archivo, ABBCapas* abb);
int cargarImagenes(const char* archivo, ListaCircular* lista, ABBCapas* abb);
int cargarUsuarios(const char* archivo, ABBUsuarios* abbUsr, ListaCircular* listaImg);

#endif
