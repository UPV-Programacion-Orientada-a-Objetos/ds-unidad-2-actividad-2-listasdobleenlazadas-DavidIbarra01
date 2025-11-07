/**
 * @file decodificador_prt7.cpp
 * @brief Sistema Decodificador de Protocolo PRT-7
 * @author Sistema de Ciberseguridad Industrial
 * @date 2025
 * * Este programa implementa un decodificador para el protocolo PRT-7,
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
/**
 * @brief Nodo para la lista circular del rotor de mapeo
 */
struct NodoRotor {
    char dato;
    NodoRotor* siguiente;
    NodoRotor* previo;
    
    NodoRotor(char c) : dato(c), siguiente(nullptr), previo(nullptr) {}
};
/**
 * @brief Nodo para la lista doblemente enlazada de caracteres decodificados
 */
struct NodoCarga {
    char dato;
    NodoCarga* siguiente;
    NodoCarga* previo;
    
    NodoCarga(char c) : dato(c), siguiente(nullptr), previo(nullptr) {}
};
class ListaDeCarga;
class RotorDeMapeo;
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
    virtual void procesar(ListaDeCarga*, RotorDeMapeo* rotor) = 0;
    
    /**
     * @brief Destructor virtual obligatorio para limpieza polimórfica
     */
    virtual ~TramaBase() {}
};
/**
 * @brief Implementación del rotor de mapeo circular
 * * Actúa como un "disco de cifrado" que contiene el alfabeto (A-Z)
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
    
    /**
     * @brief Destructor que libera toda la memoria del rotor
     */
    ~RotorDeMapeo() {
        if (cabeza == nullptr) return;
        
        NodoRotor* actual = cabeza;
        NodoRotor* siguiente;
        
        // Romper el círculo
        cabeza->previo->siguiente = nullptr;
        
        while (actual != nullptr) {
            siguiente = actual->siguiente;
            delete actual;
            actual = siguiente;
        }
    }
    
    /**
     * @brief Rota el rotor N posiciones (Versión Eficiente)
     * @param n Número de posiciones a rotar (positivo o negativo)
     */
    void rotar(int n) {
        if (cabeza == nullptr || tamanio == 0) return;
        
        // Normalizar n al rango del tamaño
        int pasos = n % tamanio;
        
        if (pasos > 0) {
            // Rotar hacia adelante
            for (int i = 0; i < pasos; i++) {
                cabeza = cabeza->siguiente;
            }
        } else if (pasos < 0) {
            // Rotar hacia atrás
            for (int i = 0; i < -pasos; i++) {
                cabeza = cabeza->previo;
            }
        }
        // Si pasos == 0, no hacer nada
    }
    
    /**
     * @brief Obtiene el mapeo de un carácter según la rotación actual
     * @param in Carácter de entrada
     * @return Carácter mapeado según la posición del rotor
     */
    char getMapeo(char in) {
        // Si no es letra mayúscula, retornar sin cambios
        if (in < 'A' || in > 'Z') {
            return in;
        }
        
        // 1. Calcular la posición alfabética absoluta (A=0, B=1, ...)
        int posicion = in - 'A';
        
        // 2. Iniciar en la cabeza actual del rotor
        NodoRotor* actual = cabeza;
        
        // 3. Avanzar 'posicion' pasos desde la cabeza
        for (int i = 0; i < posicion; i++) {
            actual = actual->siguiente;
        }
        
        // 4. Devolver el carácter en esa nueva posición
        return actual->dato;
    }
    
    /**
     * @brief Obtiene el carácter actual de la cabeza (posición 'A')
     */
    char getCabeza() const {
        return cabeza ? cabeza->dato : 'A';
    }
};
/**
 * @brief Lista doblemente enlazada para almacenar caracteres decodificados
 */
class ListaDeCarga {
private:
    NodoCarga* cabeza;
    NodoCarga* cola;
    int tamanio;
    
public:
    /**
     * @brief Constructor que inicializa una lista vacía
     */
    ListaDeCarga() : cabeza(nullptr), cola(nullptr), tamanio(0) {}
    
    /**
     * @brief Destructor que libera toda la memoria
     */
    ~ListaDeCarga() {
        NodoCarga* actual = cabeza;
        while (actual != nullptr) {
            NodoCarga* siguiente = actual->siguiente;
            delete actual;
            actual = siguiente;
        }
    }
    
    /**
     * @brief Inserta un carácter al final de la lista
     * @param dato Carácter a insertar
     */
    void insertarAlFinal(char dato) {
        NodoCarga* nuevo = new NodoCarga(dato);
        
        if (cabeza == nullptr) {
            cabeza = cola = nuevo;
        } else {
            cola->siguiente = nuevo;
            nuevo->previo = cola;
            cola = nuevo;
        }
        
        tamanio++;
    }
    
    /**
     * @brief Imprime el mensaje completo almacenado
     */
    void imprimirMensaje() const {
        NodoCarga* actual = cabeza;
        while (actual != nullptr) {
            std::cout << actual->dato;
            actual = actual->siguiente;
        }
    }
    
    /**
     * @brief Imprime el mensaje con formato de fragmentos
     */
    void imprimirConFormato() const {
        std::cout << "Mensaje: ";
        NodoCarga* actual = cabeza;
        while (actual != nullptr) {
            std::cout << "[" << actual->dato << "]";
            actual = actual->siguiente;
        }
    }
};
/**
 * @brief Trama de tipo LOAD - Contiene un carácter para decodificar
 */
class TramaLoad : public TramaBase {
private:
    char caracter;
    
public:
    TramaLoad(char c) : caracter(c) {}
    
    void procesar(ListaDeCarga* carga, RotorDeMapeo* rotor) override {
        char decodificado = rotor->getMapeo(caracter);
        carga->insertarAlFinal(decodificado);
        
        // Manejo especial para el espacio en la impresión
        char c_print = (caracter == ' ') ? ' ' : caracter;
        char d_print = (decodificado == ' ') ? ' ' : decodificado;
        
        std::cout << "Fragmento '" << c_print << "' decodificado como '" 
                  << d_print << "'. ";
        carga->imprimirConFormato();
        std::cout << std::endl;
    }
};

/**
 * @brief Trama de tipo MAP - Contiene instrucción de rotación
 */
class TramaMap : public TramaBase {
private:
    int rotacion;
    
public:
    TramaMap(int n) : rotacion(n) {}
    
    void procesar(ListaDeCarga* carga, RotorDeMapeo* rotor) override {
        // --- CORRECCIÓN DE COMPILACIÓN ---
        // Le decimos al compilador que es intencional no usar 'carga'
        (void)carga; 
        
        rotor->rotar(rotacion);
        
        std::cout << "ROTANDO ROTOR " << (rotacion >= 0 ? "+" : "") << rotacion 
                  << ". (Ahora 'A' se mapea a '" << rotor->getCabeza() << "')" 
                  << std::endl;
    }
};
/**
 * @brief Compara dos cadenas manualmente
 */
bool sonIguales(const char* str1, const char* str2) {
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) return false;
        i++;
    }
    return str1[i] == str2[i];
}

/**
 * @brief Convierte cadena a entero manualmente
 */
int aEntero(const char* str) {
    int resultado = 0;
    int signo = 1;
    int i = 0;
    
    if (str[0] == '-') {
        signo = -1;
        i = 1;
    } else if (str[0] == '+') {
        i = 1;
    }
    
    while (str[i] >= '0' && str[i] <= '9') {
        resultado = resultado * 10 + (str[i] - '0');
        i++;
    }
    
    return resultado * signo;
}

/**
 * @brief Parsea una línea del serial y crea la trama correspondiente
 */
TramaBase* parsearTrama(char* linea) {
    // Formato: "L,X" o "M,N"
    char tipo = linea[0];
    
    if (linea[1] != ',') return nullptr; // Trama mal formada

    if (tipo == 'L') {
        // Trama LOAD
        char caracter = linea[2];
        // Manejo especial para 'Space' del README
        if (linea[2] == 'S' && linea[3] == 'p' && linea[4] == 'a') {
             caracter = ' ';
        }
        return new TramaLoad(caracter);
    } else if (tipo == 'M') {
        // Trama MAP
        int rotacion = aEntero(&linea[2]);
        return new TramaMap(rotacion);
    }
    
    return nullptr;
}
#ifdef _WIN32
/**
 * @brief Abre puerto serial en Windows
 */
HANDLE abrirPuertoSerial(const char* puerto) {
    HANDLE hSerial = CreateFileA(
        puerto,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    
    if (hSerial == INVALID_HANDLE_VALUE) {
        return INVALID_HANDLE_VALUE;
    }
    
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }
    
    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    
    if (!SetCommState(hSerial, &dcbSerialParams)) {
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }
    
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    
    SetCommTimeouts(hSerial, &timeouts);
    
    return hSerial;
}

/**
 * @brief Lee una línea del puerto serial (Windows)
 */
bool leerLineaSerial(HANDLE hSerial, char* buffer, int maxLen) {
    DWORD bytesLeidos;
    char c;
    int pos = 0;
    
    while (pos < maxLen - 1) {
        if (!ReadFile(hSerial, &c, 1, &bytesLeidos, NULL)) {
            return false;
        }
        
        if (bytesLeidos > 0) {
            if (c == '\n') {
                buffer[pos] = '\0';
                return pos > 0; // Solo retorna true si la línea no está vacía
            } else if (c != '\r') {
                buffer[pos++] = c;
            }
        }
    }
    
    buffer[pos] = '\0';
    return pos > 0;
}

#else
/**
 * @brief Abre puerto serial en Linux/Mac
 */
int abrirPuertoSerial(const char* puerto) {
    int fd = open(puerto, O_RDONLY | O_NOCTTY);
    
    if (fd == -1) {
        return -1;
    }
    
    struct termios opciones;
    tcgetattr(fd, &opciones);
    
    cfsetispeed(&opciones, B9600);
    opciones.c_cflag |= (CLOCAL | CREAD);
    opciones.c_cflag &= ~PARENB;
    opciones.c_cflag &= ~CSTOPB;
    opciones.c_cflag &= ~CSIZE;
    opciones.c_cflag |= CS8;
    
    tcsetattr(fd, TCSANOW, &opciones);
    
    return fd;
}

/**
 * @brief Lee una línea del puerto serial (Linux/Mac)
 */
bool leerLineaSerial(int fd, char* buffer, int maxLen) {
    char c;
    int pos = 0;
    
    while (pos < maxLen - 1) {
        int n = read(fd, &c, 1);
        
        if (n > 0) {
            if (c == '\n') {
                buffer[pos] = '\0';
                return pos > 0; // Solo retorna true si la línea no está vacía
            } else if (c != '\r') {
                buffer[pos++] = c;
            }
        } else if (n == 0) {
            // Timeout o fin de datos
            usleep(10000); // Pequeña pausa
            if (pos > 0) {
                buffer[pos] = '\0';
                return true;
            }
            return false;
        } else {
            return false;
        }
    }
    
    buffer[pos] = '\0';
    return pos > 0;
}
#endif
/**
 * @brief Función principal del decodificador PRT-7
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   DECODIFICADOR PRT-7 v1.0" << std::endl;
    std::cout << "   Sistema de Ciberseguridad Industrial" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    // Inicializar estructuras
    ListaDeCarga miListaDeCarga;
    RotorDeMapeo miRotorDeMapeo;
    
    std::cout << "Iniciando Decodificador PRT-7. Conectando a puerto COM..." << std::endl;
    
    // Intentar abrir puerto serial
    #ifdef _WIN32
        const char* puertos[] = {"\\\\.\\COM3", "\\\\.\\COM4", "\\\\.\\COM5", "\\\\.\\COM6", "\\\\.\\COM7"};
        HANDLE hSerial = INVALID_HANDLE_VALUE;
        
        for (int i = 0; i < 5; i++) {
            hSerial = abrirPuertoSerial(puertos[i]);
            if (hSerial != INVALID_HANDLE_VALUE) {
                std::cout << "Conexion establecida en " << puertos[i] << std::endl;
                break;
            }
        }
        
        if (hSerial == INVALID_HANDLE_VALUE) {
            std::cerr << "ERROR: No se pudo conectar a ningun puerto COM." << std::endl;
            std::cerr << "Verifique que el Arduino este conectado." << std::endl;
            return 1;
        }
    #else
        const char* puertos[] = {"/dev/ttyUSB0", "/dev/ttyACM0", "/dev/ttyUSB1", "/dev/ttyACM1"};
        int fd = -1;
        
        for (int i = 0; i < 4; i++) {
            fd = abrirPuertoSerial(puertos[i]);
            if (fd != -1) {
                std::cout << "Conexion establecida en " << puertos[i] << std::endl;
                break;
            }
        }
        
        if (fd == -1) {
            std::cerr << "ERROR: No se pudo conectar a ningun puerto serial." << std::endl;
            std::cerr << "Verifique que el Arduino este conectado." << std::endl;
            return 1;
        }
    #endif
    
    std::cout << "Esperando tramas..." << std::endl << std::endl;
    
    // Bucle de procesamiento
    char buffer[256];
    int tramasRecibidas = 0;
    while (true) {
        #ifdef _WIN32
            bool hayDatos = leerLineaSerial(hSerial, buffer, 256);
        #else
            bool hayDatos = leerLineaSerial(fd, buffer, 256);
        #endif
        
        if (hayDatos) {
            
            // 1. Verificar si es la trama de FIN
            if (sonIguales(buffer, "FIN")) {
                std::cout << "Trama recibida: [FIN]. Deteniendo." << std::endl;
                break; // Salir del bucle while(true)
            }
            
            // 2. Verificar si es el saludo inicial (y saltarlo)
            if (sonIguales(buffer, "SISTEMA PRT-7 ACTIVO")) {
                std::cout << "Mensaje de control recibido: [SISTEMA PRT-7 ACTIVO]" << std::endl << std::endl;
                continue; // Saltar al siguiente ciclo de lectura
            }

            // Si no es FIN ni el saludo, procesar la trama
            std::cout << "Trama recibida: [" << buffer << "] -> Procesando... -> ";
            
            TramaBase* trama = parsearTrama(buffer);
            
            if (trama != nullptr) {
                trama->procesar(&miListaDeCarga, &miRotorDeMapeo);
                delete trama;
                tramasRecibidas++;
            } else {
                std::cout << "ERROR: Trama mal formada." << std::endl;
            }
            
            std::cout << std::endl;
        }
    }
    
    // Cerrar puerto
    #ifdef _WIN32
        CloseHandle(hSerial);
    #else
        close(fd);
    #endif
    
    // Mostrar resultado final
    std::cout << "---" << std::endl;
    std::cout << "Flujo de datos terminado." << std::endl;
    std::cout << "MENSAJE OCULTO ENSAMBLADO:" << std::endl;
    miListaDeCarga.imprimirMensaje();
    std::cout << std::endl;
    std::cout << "---" << std::endl;
    std::cout << "Liberando memoria... Sistema apagado." << std::endl;
    
    return 0;
}