#include "Calculocostos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// --- IMPLEMENTACIÓN DE LAS FUNCIONES DE LA LIBRERÍA (JNA COMPATIBLE) ---

// Crea un nodo individual de Costo. Retorna un puntero al nodo creado.
JNA_API Costo* crear_costo(const char* nombre, int tipo, double monto) {
    Costo* nuevo = (Costo*)malloc(sizeof(Costo));
    if (nuevo == NULL) {
        return NULL;
    }
    strncpy(nuevo->nombre, nombre, MAX_NOMBRE - 1);
    nuevo->nombre[MAX_NOMBRE - 1] = '\0';
    nuevo->tipo = tipo;
    nuevo->monto = monto;
    nuevo->siguiente = NULL;
    return nuevo;
}

// Agrega un nuevo costo al final de la lista. Retorna la cabeza de la lista.
JNA_API Costo* agregar_costo(Costo* cabeza, const char* nombre, int tipo, double monto) {
    Costo* nuevo = crear_costo(nombre, tipo, monto);
    if (nuevo == NULL) {
        return cabeza; // Si no se pudo crear, retorna la cabeza actual sin cambios
    }

    if (cabeza == NULL) {
        return nuevo;
    }

    Costo* actual = cabeza;
    while (actual->siguiente != NULL) {
        actual = actual->siguiente;
    }
    actual->siguiente = nuevo;
    return cabeza;
}

// Elimina un costo por su índice (1-basado). Retorna la nueva cabeza. 
// Asigna 1 a exito si se eliminó, 0 si no se encontró.
JNA_API Costo* eliminar_costo(Costo* cabeza, int indice, int* exito) {
    if (exito) *exito = 0;
    if (cabeza == NULL || indice < 1) {
        return cabeza;
    }

    Costo* actual = cabeza;
    Costo* anterior = NULL;
    int pos = 1;

    while (actual != NULL && pos < indice) {
        anterior = actual;
        actual = actual->siguiente;
        pos++;
    }

    if (actual == NULL) {
        return cabeza; // Índice fuera de rango
    }

    // Desconectar el nodo
    Costo* siguiente_nodo = actual->siguiente;
    if (anterior == NULL) {
        // Eliminar el primer nodo (cabeza)
        free(actual);
        if (exito) *exito = 1;
        return siguiente_nodo;
    } else {
        anterior->siguiente = siguiente_nodo;
        free(actual);
        if (exito) *exito = 1;
        return cabeza;
    }
}

// Calcula los totales de costos directos e indirectos.
JNA_API void calcular_totales(const Costo* cabeza, double* total_directo, double* total_indirecto) {
    double dir = 0.0;
    double ind = 0.0;
    const Costo* actual = cabeza;
    while (actual != NULL) {
        if (actual->tipo == 0) {
            dir += actual->monto;
        } else {
            ind += actual->monto;
        }
        actual = actual->siguiente;
    }
    if (total_directo) *total_directo = dir;
    if (total_indirecto) *total_indirecto = ind;
}

// Libera toda la memoria de la lista enlazada de costos.
JNA_API void liberar_lista(Costo* cabeza) {
    Costo* actual = cabeza;
    while (actual != NULL) {
        Costo* temp = actual;
        actual = actual->siguiente;
        free(temp);
    }
}

// Retorna la cantidad total de elementos en la lista.
JNA_API int obtener_cantidad(const Costo* cabeza) {
    int contador = 0;
    const Costo* actual = cabeza;
    while (actual != NULL) {
        contador++;
        actual = actual->siguiente;
    }
    return contador;
}

// Obtiene un elemento específico por su índice (1-basado). Retorna NULL si no existe.
JNA_API const Costo* obtener_costo_por_indice(const Costo* cabeza, int indice) {
    if (indice < 1) return NULL;
    const Costo* actual = cabeza;
    int pos = 1;
    while (actual != NULL && pos < indice) {
        actual = actual->siguiente;
        pos++;
    }
    return actual;
}


// --- CÓDIGO INTERACTIVO PARA CLI (SE EXCLUYE SI SE COMPILA COMO DLL) ---
#ifndef BUILD_DLL

void limpiar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void mostrar_menu() {
    printf("================================\n");
    printf("  GESTIÓN DE COSTOS DEL PROYECTO (CLI)\n");
    printf("================================\n");
    printf("1. Agregar costo\n");
    printf("2. Eliminar costo (por índice)\n");
    printf("3. Listar todos los costos\n");
    printf("4. Calcular totales (directos/indirectos)\n");
    printf("5. Salir\n");
    printf("================================\n");
}

void listar_costos(Costo* cabeza) {
    if (cabeza == NULL) {
        printf("No hay costos registrados.\n");
        return;
    }

    printf("\n--- LISTA DE COSTOS ---\n");
    printf(" # | Tipo      | Nombre                           | Monto\n");
    printf("-----------------------------------------------------------\n");
    Costo* actual = cabeza;
    int contador = 1;
    while (actual != NULL) {
        printf("%2d | %-9s | %-30s | %10.2f\n",
               contador,
               (actual->tipo == 0) ? "Directo" : "Indirecto",
               actual->nombre,
               actual->monto);
        actual = actual->siguiente;
        contador++;
    }
    printf("-----------------------------------------------------------\n");
}

void cli_agregar_costo(Costo** cabeza) {
    char nombre[MAX_NOMBRE];
    char tipo_opcion;
    int tipo;
    double monto;

    printf("\n--- AGREGAR COSTO ---\n");
    printf("Nombre del costo: ");
    fgets(nombre, MAX_NOMBRE, stdin);
    nombre[strcspn(nombre, "\n")] = '\0';

    do {
        printf("Tipo (D = Directo, I = Indirecto): ");
        scanf("%c", &tipo_opcion);
        limpiar_buffer();
        tipo_opcion = toupper(tipo_opcion);
        if (tipo_opcion != 'D' && tipo_opcion != 'I') {
            printf("Opción inválida. Ingrese D o I.\n");
        }
    } while (tipo_opcion != 'D' && tipo_opcion != 'I');
    tipo = (tipo_opcion == 'D') ? 0 : 1;

    printf("Monto (Ej: 1250.50): ");
    while (scanf("%lf", &monto) != 1 || monto < 0) {
        printf("Entrada inválida. Ingrese un número positivo: ");
        limpiar_buffer();
    }
    limpiar_buffer();

    Costo* nuevo_head = agregar_costo(*cabeza, nombre, tipo, monto);
    if (nuevo_head != NULL) {
        *cabeza = nuevo_head;
        printf("Costo '%s' agregado correctamente.\n", nombre);
    } else {
        printf("Error de memoria. No se pudo agregar el costo.\n");
    }
}

void cli_eliminar_costo(Costo** cabeza) {
    if (*cabeza == NULL) {
        printf("No hay costos registrados para eliminar.\n");
        return;
    }

    listar_costos(*cabeza);

    int indice;
    printf("Ingrese el número del costo a eliminar: ");
    if (scanf("%d", &indice) != 1 || indice < 1) {
        printf("Índice inválido.\n");
        limpiar_buffer();
        return;
    }
    limpiar_buffer();

    // Obtener detalles del costo antes de eliminarlo para informar al usuario
    const Costo* elemento = obtener_costo_por_indice(*cabeza, indice);
    if (elemento == NULL) {
        printf("No existe un costo con el índice %d.\n", indice);
        return;
    }

    char nombre_temp[MAX_NOMBRE];
    double monto_temp = elemento->monto;
    strncpy(nombre_temp, elemento->nombre, MAX_NOMBRE - 1);
    nombre_temp[MAX_NOMBRE - 1] = '\0';

    int exito = 0;
    *cabeza = eliminar_costo(*cabeza, indice, &exito);

    if (exito) {
        printf("Costo '%s' (%.2f) eliminado.\n", nombre_temp, monto_temp);
    } else {
        printf("Error al intentar eliminar el costo.\n");
    }
}

int main() {
    Costo* lista = NULL;
    int opcion;

    do {
        mostrar_menu();
        printf("Seleccione una opción: ");
        if (scanf("%d", &opcion) != 1) {
            printf("Entrada inválida. Por favor ingrese un número.\n");
            limpiar_buffer();
            opcion = 0;
            continue;
        }
        limpiar_buffer();

        switch (opcion) {
            case 1:
                cli_agregar_costo(&lista);
                break;
            case 2:
                cli_eliminar_costo(&lista);
                break;
            case 3:
                listar_costos(lista);
                break;
            case 4: {
                double dir = 0.0, ind = 0.0;
                calcular_totales(lista, &dir, &ind);
                printf("\n--- RESUMEN DE COSTOS ---\n");
                printf("Total Costos Directos:   %.2f\n", dir);
                printf("Total Costos Indirectos: %.2f\n", ind);
                printf("Total del Proyecto:      %.2f\n", dir + ind);
                printf("--------------------------\n");
                break;
            }
            case 5:
                printf("Saliendo del programa. ¡Hasta luego!\n");
                break;
            default:
                printf("Opción no válida. Intente de nuevo.\n");
        }
        printf("\n");
    } while (opcion != 5);

    liberar_lista(lista);
    return 0;
}

#endif // BUILD_DLL
