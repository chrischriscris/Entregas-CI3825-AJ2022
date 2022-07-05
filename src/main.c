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
#include <regex.h>

#include "wc.h"
#include "find.h"
#include "list.h"
#include "codif.h"
#include "repla.h"
#include "utils.h"
#include "roll.h"

static const char *const re = "^[-+]?[0-9]+$"; /* Regex para números */

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
                fprintf(stderr, "Uso: find [<cadena>]\n");
                free(input);
                continue;
            }

            verify_exec(find(root, str), "find");
        }

        else if (!strcmp(command, "ifind"))
        {
            char *str = strtok(NULL, " ");

            if (strtok(NULL, " ")) {
                fprintf(stderr, "Uso: ifind [<cadena>]\n");
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
                fprintf(stderr, "Uso: cfind <cadena1> <cadena2>\n");
                free(input);
                continue;
            }

            verify_exec(cfind(root, str1, str2), "cfind");
        }

        else if (!strcmp(command, "repla"))
        {
            char *path = strtok(NULL, " ");

            if (!path || strtok(NULL, " ")) {
                fprintf(stderr, "Uso: repla <file>\n");
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

            if (n_str) {
                int isnum = is_numeric(n_str);
                if (isnum == -1) {
                    fprintf(stderr, "Hubo un error verificando el argumento\n");
                    free(input);
                    continue;
                } else if (isnum == 0) {
                    fprintf(stderr, "El comando roll requiere un número entero como argumento\n");
                    free(input);
                    continue;
                }
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
 * @return 1 si la string es numérica.
 *     0 en caso contrario.
 *     -1 si hubo algún error en la verificación.
 */
int is_numeric(char *str) {
    regex_t regex;
    int ret;

    if (regcomp(&regex, re, REG_EXTENDED)) return -1;

    ret = regexec(&regex, str, 0, NULL, 0);
    regfree(&regex);

    if (!ret) return 1;
    else if (ret == REG_NOMATCH) return 0;
    else return -1;
}