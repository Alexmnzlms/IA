#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>

struct estado {
  int fila;
  int columna;
  int orientacion;
};

struct nodoPeso{
	estado st;
	list<Action> secuencia;
	int peso;
};

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
      // Inicializar Variables de Estado
      fil = col = 99;
      brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
      destino.fila = -1;
      destino.columna = -1;
      destino.orientacion = -1;
      objetivos = 0;
      hayPlan = false;
      seDondeEstoy = false;
      estadoActualIniciado = false;
      kvista = false;
      tam = 200;
      desconocidos = new char*[tam];
      for(int i = 0; i < tam; i++){
        desconocidos[i] = new char[tam];
        for(int j = 0; j < tam; j++){
          desconocidos[i][j] = '?';
        }
      }
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      // Inicializar Variables de Estado
      fil = col = 99;
      brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
      destino.fila = -1;
      destino.columna = -1;
      destino.orientacion = -1;
      objetivos = 0;
      hayPlan = false;
      seDondeEstoy = false;
      estadoActualIniciado = false;
      kvista = false;
      tam = 200;
      desconocidos = new char*[tam];
      for(int i = 0; i < tam; i++){
        desconocidos[i] = new char[tam];
        for(int j = 0; j < tam; j++){
          desconocidos[i][j] = '?';
        }
      }
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){
      for(int i = 0; i < tam; i++){
        delete desconocidos[i];
      }
      delete desconocidos;
    }

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void VisualizaPlan(const estado &st, const list<Action> &plan);
    ComportamientoJugador * clone(){return new ComportamientoJugador(*this);}

  private:
    // Declarar Variables de Estado
    int fil, col, brujula;
    estado actual, destino;
    list<Action> plan;

    // Nuevas Variables de Estado
    Action ultimaAccion;
    bool hayPlan;
    bool seDondeEstoy;
    bool estadoActualIniciado;
    char** desconocidos;
    int tam;
    bool kvista;
    int objetivos;

    // Métodos privados de la clase
    bool pathFinding(int level, const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_CU(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Desconocido(const estado &origen, list<Action> &plan);
    bool pathFinding_K(const estado &origen, list<Action> &plan);
    void PintaPlan(list<Action> plan);
    bool HayObstaculoDelante(estado &st);

    //Nuevos metodos privados
    void asignarPeso(nodoPeso &nodo);
    void asignarPesoForward(nodoPeso &nodo); //De momento solo se usa este
    void updateEstadoActual(estado &st, Action accion);
    void updateMapaDesconocido(estado &st, vector<unsigned char> vector);
    bool obstaculoNoContemplado(estado &st, vector<unsigned char> vector);
    void updateMapa(estado &st, vector<unsigned char> vector);
    bool estadoForwardnoValido(estado &st);

};

#endif
