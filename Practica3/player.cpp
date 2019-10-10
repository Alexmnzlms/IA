#include <iostream>
#include <cstdlib>
#include <vector>
#include <queue>
#include "player.h"
#include "environment.h"

using namespace std;

const double masinf=9999999999.0, menosinf=-9999999999.0;


// Constructor
Player::Player(int jug){
    jugador_=jug;
}

// Actualiza el estado del juego para el jugador
void Player::Perceive(const Environment & env){
    actual_=env;
}

double Puntuacion(int jugador, const Environment &estado){
    double suma=0;

    for (int i=0; i<7; i++)
      for (int j=0; j<7; j++){
         if (estado.See_Casilla(i,j)==jugador){
            if (j<3)
               suma += j;
            else
               suma += (6-j);
         }
      }

    return suma;
}


// Funcion de valoracion para testear Poda Alfabeta
double ValoracionTest(const Environment &estado, int jugador){
    int ganador = estado.RevisarTablero();

    if (ganador==jugador)
       return 99999999.0; // Gana el jugador que pide la valoracion
    else if (ganador!=0)
            return -99999999.0; // Pierde el jugador que pide la valoracion
    else if (estado.Get_Casillas_Libres()==0)
            return 0;  // Hay un empate global y se ha rellenado completamente el tablero
    else
          return Puntuacion(jugador,estado);
}

// ------------------- Los tres metodos anteriores no se pueden modificar

double PuntuacionAlrededor(const Environment &estado, int fil, int col){
  char casilla = estado.See_Casilla(fil,col);
  double total = 0;
  for(int i = fil-1; i <= fil; i++){
    for(int j = col-1; j <= col; j++){
      if(i != fil && j != col && i >= 0 && j >= 0 && j < 7 && i < 7){
        if(casilla == estado.See_Casilla(i,j) && casilla != '0'){
          total++;
        }
      }
    }
  }
  return total;
}

double Bomba(const Environment &estado, int fil, int col, char bomba){
  double total = 0;
  char mi_color, su_color;
  switch (bomba) {
    case '4':
      mi_color = '1';
      su_color = '2';
      break;
    case '5':
      mi_color = '2';
      su_color = '1';
      break;
  }
  for(int i = fil; i < 7; i++){
    for(int j = 0; j < 7; j++){
      if(i != fil){
        if(estado.See_Casilla(i,j) == su_color)
          total++;
        else if(estado.See_Casilla(i,j) == mi_color)
          total--;
      }
      else{
        if(estado.See_Casilla(i,j) == su_color)
          total--;
        else if(estado.See_Casilla(i,j) == mi_color)
          total++;
      }
    }
  }
  return total;
}

// Funcion heuristica (ESTA ES LA QUE TENEIS QUE MODIFICAR)
double Valoracion(const Environment &estado, int jugador){
  int ganador = estado.RevisarTablero();

  if (ganador==jugador)
     return 99999999.0; // Gana el jugador que pide la valoracion
  else if (ganador!=0)
     return -99999999.0; // Pierde el jugador que pide la valoracion
  else if (estado.Get_Casillas_Libres()==0)
     return 0;  // Hay un empate global y se ha rellenado completamente el tablero
  else{
    double total = 0;
    double actual = 0;
    char casilla;
    for(int i = 0; i < 7; i++){
      for(int j = 0; j < 7; j++){
        actual = PuntuacionAlrededor(estado,i,j);
        casilla = estado.See_Casilla(i,j);
        if(casilla == '4' || casilla == '5'){
          actual += Bomba(estado,i,j,casilla);
        }
        if(casilla == jugador){
          total -= actual;
        }
        else{
          total += actual;
        }
      }
    }
    return total;
  }
}

// Esta funcion no se puede usar en la version entregable
// Aparece aqui solo para ILUSTRAR el comportamiento del juego
// ESTO NO IMPLEMENTA NI MINIMAX, NI PODA ALFABETA
void JuegoAleatorio(bool aplicables[], int opciones[], int &j){
    j=0;
    for (int i=0; i<8; i++){
        if (aplicables[i]){
           opciones[j]=i;
           j++;
        }
    }
}

double Poda_AlfaBeta(Environment actual, int jugador, int profundidad, int profundidad_max, Environment::ActionType& accion, double a, double b){
  if(profundidad == profundidad_max || actual.JuegoTerminado()){
    return Valoracion(actual, jugador);
  }
  double a_antes;
  Environment v[8];
  int n_mov = actual.GenerateAllMoves(v);
  if(jugador == actual.JugadorActivo()){//Max
    for(int i = 0; i < n_mov; i++){
      a_antes = a;
      a = max(a, Poda_AlfaBeta(v[i], jugador, profundidad+1, profundidad_max, accion, a, b));
      if((a != a_antes) && (profundidad == 0)){
        accion =  static_cast< Environment::ActionType > (v[i].Last_Action(jugador));
      }
      if (b <= a)
       return b;
    }
    return a;
  }
  else{//Min
    for(int i = 0; i < n_mov; i++){
      b = min(b, Poda_AlfaBeta(v[i], jugador, profundidad+1, profundidad_max, accion, a, b));
      if (b <= a)
       return a;
    }
    return b;
  }
}

// Invoca el siguiente movimiento del jugador
Environment::ActionType Player::Think(){
    const int PROFUNDIDAD_MINIMAX = 6;  // Umbral maximo de profundidad para el metodo MiniMax
    const int PROFUNDIDAD_ALFABETA = 8; // Umbral maximo de profundidad para la poda Alfa_Beta

    Environment::ActionType accion; // acci�n que se va a devolver
    bool aplicables[8]; // Vector bool usado para obtener las acciones que son aplicables en el estado actual. La interpretacion es
                        // aplicables[0]==true si PUT1 es aplicable
                        // aplicables[1]==true si PUT2 es aplicable
                        // aplicables[2]==true si PUT3 es aplicable
                        // aplicables[3]==true si PUT4 es aplicable
                        // aplicables[4]==true si PUT5 es aplicable
                        // aplicables[5]==true si PUT6 es aplicable
                        // aplicables[6]==true si PUT7 es aplicable
                        // aplicables[7]==true si BOOM es aplicable



    double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
    double alpha = menosinf, beta = masinf; // Cotas de la poda AlfaBeta

    int n_act; //Acciones posibles en el estado actual


    n_act = actual_.possible_actions(aplicables); // Obtengo las acciones aplicables al estado actual en "aplicables"
    int opciones[10];

    // Muestra por la consola las acciones aplicable para el jugador activo
    //actual_.PintaTablero();
    cout << " Acciones aplicables ";
    (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
    for (int t=0; t<8; t++)
      if (aplicables[t])
         cout << " " << actual_.ActionStr( static_cast< Environment::ActionType > (t)  );
    cout << endl;

    /*
    //--------------------- COMENTAR Desde aqui
    cout << "\n\t";
    int n_opciones=0;
    JuegoAleatorio(aplicables, opciones, n_opciones);

    if (n_act==0){
      (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
      cout << " No puede realizar ninguna accion!!!\n";
      //accion = Environment::actIDLE;
    }
    else if (n_act==1){
           (jugador_==1) ? cout << "Verde: " : cout << "Azul: ";
            cout << " Solo se puede realizar la accion "
                 << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[0])  ) << endl;
            accion = static_cast< Environment::ActionType > (opciones[0]);

         }
         else { // Hay que elegir entre varias posibles acciones
            int aleatorio = rand()%n_opciones;
            cout << " -> " << actual_.ActionStr( static_cast< Environment::ActionType > (opciones[aleatorio])  ) << endl;
            accion = static_cast< Environment::ActionType > (opciones[aleatorio]);
         }

    //--------------------- COMENTAR Hasta aqui
    */

    //--------------------- AQUI EMPIEZA LA PARTE A REALIZAR POR EL ALUMNO ------------------------------------------------


    // Opcion: Poda AlfaBeta
    // NOTA: La parametrizacion es solo orientativa
    valor = Poda_AlfaBeta(actual_, jugador_, 0, PROFUNDIDAD_ALFABETA, accion, alpha, beta);
    cout << "Valor MiniMax: " << valor << "  Accion: " << actual_.ActionStr(accion) << endl;

    return accion;
}
