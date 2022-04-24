// Plantilla de un Menú de Opciones

#include <iostream>
#include <cstdlib>

using namespace std;

int main()
{
    int opcion;
    bool repetir = true;
    
    do {
        system("cls");
        
        // Texto del menú que se verá cada vez
        cout << "\n\nMenu de Opciones" << endl;
        cout << "1. Chat con todods lo usuarios" << endl;
        cout << "2. Enviar y recibir mensajes directos, privados, aparte del chat general" << endl;
        cout << "3. Cambiar de status" << endl;
        cout << "4. Listar los usuarios conectados al sistema de chat" << endl;
        cout << "5. Desplegar información de un usuario en particular" << endl;
        cout << "6. Ayuda" << endl;
        cout << "7. Salir" << endl;
        
        cout << "\nIngrese una opcion: ";
        cin >> opcion;
        
        switch (opcion) {
            case 1:
                // Lista de instrucciones de la opción 1                
                
                system("pause>nul"); // Pausa
                break;
                
            case 2:
                // Lista de instrucciones de la opción 2                
                
                system("pause>nul"); // Pausa
                break;
                
            case 3:
                // Lista de instrucciones de la opción 3                
                
                system("pause>nul"); // Pausa            
                break;
                
            case 4:
                // Lista de instrucciones de la opción 4                
                
                system("pause>nul"); // Pausa                
                break;
            case 5:
                // Lista de instrucciones de la opción 5              
                
                system("pause>nul"); // Pausa                
                break;
            case 6:
                // Lista de instrucciones de la opción 6                
                
                system("pause>nul"); // Pausa                
                break;
            case 7:
                // Lista de instrucciones de la opción 7                
                
                system("pause>nul"); // Pausa                
                break;

            case 0:
            	repetir = false;
            	break;
        }        
    } while (repetir);
	 
    return 0;
}