#ifndef CALCULOCOSTOS_H
#define CALCULOCOSTOS_H

#define MAX_NOMBRE 100

// Macro para exportación de DLL en Windows y símbolos en Linux/macOS
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    #define JNA_API __declspec(dllexport)
#else
    #define JNA_API __attribute__((visibility("default")))
#endif

// Estructura para representar un costo (alineada con JNA)
typedef struct Costo {
    char nombre[MAX_NOMBRE];
    int tipo;           // 0 = Directo, 1 = Indirecto
    double monto;
    struct Costo* siguiente;
} Costo;

// --- FUNCIONES EXPORTADAS PARA JNA ---

#ifdef __cplusplus
extern "C" {
#endif

// Crea un nodo individual de Costo. Retorna un puntero al nodo creado.
JNA_API Costo* crear_costo(const char* nombre, int tipo, double monto);

// Agrega un nuevo costo al final de la lista. Retorna la cabeza de la lista.
JNA_API Costo* agregar_costo(Costo* cabeza, const char* nombre, int tipo, double monto);

// Elimina un costo por su índice (1-basado). Retorna la nueva cabeza. 
// Asigna 1 a exito si se eliminó, 0 si no se encontró.
JNA_API Costo* eliminar_costo(Costo* cabeza, int indice, int* exito);

// Calcula los totales de costos directos e indirectos.
JNA_API void calcular_totales(const Costo* cabeza, double* total_directo, double* total_indirecto);

// Libera toda la memoria de la lista enlazada de costos.
JNA_API void liberar_lista(Costo* cabeza);

// Retorna la cantidad total de elementos en la lista.
JNA_API int obtener_cantidad(const Costo* cabeza);

// Obtiene un elemento específico por su índice (1-basado). Retorna NULL si no existe.
JNA_API const Costo* obtener_costo_por_indice(const Costo* cabeza, int indice);

#ifdef __cplusplus
}
#endif

#endif // CALCULOCOSTOS_H
