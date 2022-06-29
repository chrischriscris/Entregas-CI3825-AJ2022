#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "wc.h"
#include "find.h"
#include "list.h"
#include "codif.h"
#include "repla.h"

char *readline(FILE* fp, int size);
void verify_exec(int n, char *command);

int main(int argc, char **argv) {
    char *root = argv[1];
    char *input;
    DIR *dir;


    if (argc != 2) {
        fprintf(stderr, "Uso: %s <directorio_raiz>\n", argv[0]);
        return 1;
    }
    
    /* Abre el directorio */
    dir = opendir(root);
    if (!dir) {
        fprintf(stderr, "Error: No se puede abrir el directorio raíz\n");
        closedir(dir);
        return 1;
    }
    closedir(dir);


    /* Inicialización del interpretador */
    while (1) {
        char *command;

        printf("myutil> ");
        input = readline(stdin, 64);
        if (!input)
            fprintf(stderr, "Error: hubo un error leyendo la línea.\n");

        command = strtok(input, " ");

        if (!strcmp(command, "wc"))
        {
            int c, l;

            if (strtok(NULL, " ")) {
                fprintf(stderr, "Uso: wc\n");
                free(input);
                continue;
            }

            verify_exec(wc(root, &c, &l), "wc");
        }

        else if (!strcmp(command, "codif"))
        {
            if (strtok(NULL, " ")) {
                fprintf(stderr, "Uso: wc\n");
                free(input);
                continue;
            }

            verify_exec(codif(root), "codif");
        }

        else if (!strcmp(command, "find"))
        {
            char *str = strtok(NULL, " ");

            if (strtok(NULL, " ")) {
                fprintf(stderr, "Uso: find [<cadena>]");
                free(input);
                continue;
            }

            verify_exec(find(root, str), "find");
        }

        else if (!strcmp(command, "ifind"))
        {
            char *str = strtok(NULL, " ");

            if (strtok(NULL, " ")) {
                fprintf(stderr, "Uso: ifind [<cadena>]");
                free(input);
                continue;
            }

            verify_exec(ifind(root, str), "ifind");
        }

        else if (!strcmp(command, "cfind"))
        {
            char *str1 = strtok(NULL, " ");
            char *str2 = strtok(NULL, " ");

            if (!str1 || !str2 || !strtok(NULL, " ")) {
                fprintf(stderr, "Uso: cfind <cadena1> <cadena2>");
                free(input);
                continue;
            }

            verify_exec(cfind(root, str1, str2), "cfind");
        }

        else if (!strcmp(command, "repla"))
        {
            char *path = strtok(NULL, " ");

            if (!path || strtok(NULL, " ")) {
                fprintf(stderr, "Uso: repla <file>");
                free(input);
                continue;
            }

            verify_exec(repla(root, path), "repla");
        }

        else if (!strcmp(command, "roll"))
        {
            char *n_read = strtok(NULL, " ");
            int n = 0;

            atoi(n_read);
        }

        else
        {
            fprintf(stderr, "Comando no reconocido\n");
        }

        free(input);
    }
    return 0;
}

/**
 * Lee una string de un apuntador a FILE y asigna memoria dinámicamente
 * para leer todo su contenido. Comienza con tamaño size y aumenta en 16
 * en 16 la longitud.
 * 
 * @param fp: Apuntador a FILE.
 * @param size: Tamaño inicial esperado de la string.
 * @return Apuntador a string, debe ser liberada por el usuario.
 */
char *readline(FILE* fp, int size) {
    char *str;
    char c;
    int len = 0;

    /* Se asigna size cantidad de memoria */
    str = malloc(sizeof(char) * size);
    if (!str) return NULL;

    while ((c=fgetc(fp)) != EOF && c != '\n'){
        str[len++] = c;

        if (len == size + 1) {
            size += 16;
            str = realloc(str, sizeof(char) * size);
            if (!str) {
                free(str);
                return NULL;
            }
        }
    }

    str[len++] = '\0';
    str = realloc(str, sizeof(char) * len);

    if (!str) {
        free(str);
        return NULL;
    }
    return str;
}

void verify_exec(int n, char *command) {
    if (n == -1)
        fprintf(stderr, "Ha habido un error ejecutando %s\n", command);
}

/*
Ejemplo de uso de las funciones

root = argv[1];
codif(root); ------------------------------

root = argv[1];
path = argv[2];
find(root, path); --------------------------

root = argv[1];
path = argv[2];
ifind(root, path); -------------------------

root = argv[1];
str1 = argv[2];
str2 = argv[3];
cfind(root, str1, str2); -------------------

root = argv[1];
path = argv[2];
repla(root, path); -------------------------

root = argv[1];
wc(root, &c, &l); --------------------------
*/