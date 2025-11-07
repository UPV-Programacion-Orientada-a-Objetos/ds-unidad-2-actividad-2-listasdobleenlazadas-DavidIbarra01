/**
 * @file decodificador_prt7.cpp
 * @brief Sistema Decodificador de Protocolo PRT-7
 * @author Sistema de Ciberseguridad Industrial
 * @date 2025
 * 
 * Este programa implementa un decodificador para el protocolo PRT-7,
 * que recibe tramas de un Arduino y ensambla mensajes ocultos mediante
 * un sistema de rotación de mapeo circular.
 */

#include <iostream>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <fcntl.h>
    #include <unistd.h>
    #include <termios.h>
    #include <cstring>
#endif

// ============================================================================
// NODO PARA LISTA CIRCULAR (ROTOR DE MAPEO)
// ============================================================================

/**
 * @brief Nodo para la lista circular del rotor de mapeo
 */
struct NodoRotor {
    char dato;
    NodoRotor* siguiente;
    NodoRotor* previo;
    
    NodoRotor(char c) : dato(c), siguiente(nullptr), previo(nullptr) {}
};

// ============================================================================
// NODO PARA LISTA DOBLEMENTE ENLAZADA (LISTA DE CARGA)
// ============================================================================

/**
 * @brief Nodo para la lista doblemente enlazada de caracteres decodificados
 */
struct NodoCarga {
    char dato;
    NodoCarga* siguiente;
    NodoCarga* previo;
    
    NodoCarga(char c) : dato(c), siguiente(nullptr), previo(nullptr) {}
};

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================

class ListaDeCarga;
class RotorDeMapeo;

// ============================================================================
// CLASE BASE ABSTRACTA: TramaBase
// ============================================================================

/**
 * @brief Clase base abstracta para todas las tramas del protocolo PRT-7
 */
class TramaBase {
public:
    /**
     * @brief Método virtual puro para procesar la trama
     * @param carga Puntero a la lista de carga donde se almacenan caracteres
     * @param rotor Puntero al rotor de mapeo circular
     */
    virtual void procesar(ListaDeCarga* carga, RotorDeMapeo* rotor) = 0;
    
    /**
     * @brief Destructor virtual obligatorio para limpieza polimórfica
     */
    virtual ~TramaBase() {}
};

// ============================================================================
// ROTOR DE MAPEO (LISTA CIRCULAR DOBLEMENTE ENLAZADA)
// ============================================================================

/**
 * @brief Implementación del rotor de mapeo circular
 * 
 * Actúa como un "disco de cifrado" que contiene el alfabeto (A-Z)
 * y puede rotar para cambiar el mapeo de caracteres.
 */
class RotorDeMapeo {
private:
    NodoRotor* cabeza;
    int tamanio;
    
public:
    /**
     * @brief Constructor que inicializa el rotor con A-Z
     */
    RotorDeMapeo() : cabeza(nullptr), tamanio(0) {
        // Crear lista circular con A-Z
        const char* alfabeto = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        NodoRotor* primero = nullptr;
        NodoRotor* anterior = nullptr;
        
        for (int i = 0; alfabeto[i] != '\0'; i++) {
            NodoRotor* nuevo = new NodoRotor(alfabeto[i]);
            
            if (primero == nullptr) {
                primero = nuevo;
                cabeza = nuevo;
            }
            
            if (anterior != nullptr) {
                anterior->siguiente = nuevo;
                nuevo->previo = anterior;
            }
            
            anterior = nuevo;
            tamanio++;
        }
        
        // Cerrar el círculo
        if (anterior != nullptr && primero != nullptr) {
            anterior->siguiente = primero;
            primero->previo = anterior;
        }
    }
    