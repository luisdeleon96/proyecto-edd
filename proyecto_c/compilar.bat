@echo off
echo ============================================
echo  Compilando Generador de Imagenes por Capas
echo  Lenguaje: C (C99)
echo ============================================

gcc -std=c99 -Wall -O2 -o generador.exe ^
    main.c ^
    src/MatrizDispersa.c ^
    src/Capa.c ^
    src/Imagen.c ^
    src/Usuario.c ^
    src/CargaMasiva.c ^
    src/Graficador.c

if %ERRORLEVEL% == 0 (
    echo.
    echo Compilacion exitosa: generador.exe
    echo Para ejecutar escribe: generador.exe
) else (
    echo.
    echo ERROR en la compilacion.
)
pause
