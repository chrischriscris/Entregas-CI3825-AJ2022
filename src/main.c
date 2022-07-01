/**
 * Emtry-point del interpretador myutil. Soporta los comandos:
 * 
 * * wc:
 * * find:
 * * ifind:
 * * cfind:
 * * codif:
 * * repla:
 * * roll:
 *
 * Autor: Christopher Gómez.
 * Fecha: 29-06-2022.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

#include "wc.h"
#include "find.h"
#include "list.h"
#include "codif.h"
#include "repla.h"
#include "utils.h"
#include "roll.h"

int is_numeric(char *str);
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
        if (!command) {
            free(input);
            continue;
        }

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
            char *n_str = strtok(NULL, " ");
            int n;

            if (strtok(NULL, " ")) {
                fprintf(stderr, "Uso: roll [<n>]\n");
                free(input);
                continue;
            }

            if (n_str && !is_numeric(n_str)) {
                fprintf(stderr, "El comando roll requiere un número entero \
                    como argumento");
                free(input);
                continue;
            }

            n = n_str ? atoi(n_str) : 0;
            verify_exec(roll(root, n), "roll");
        }

        else
        {
            fprintf(stderr, "Comando no reconocido\n");
        }

        free(input);
    }
    return 0;
}

void verify_exec(int n, char *command) {
    if (n == -1)
        fprintf(stderr, "Ha habido un error ejecutando %s\n", command);
}

/**
 * Verifica si una string dada es numérica.
 * 
 * @param str: String a verificar.
 * @return 1 
 */
int is_numeric(char *str) {
    int i;
    for (i = 0; i < strlen(str); i++) {
        if (!isdigit(str[i]))
            return 0;
    }
    return 1;
}