#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>


// Este es el método principal que debe contener los 4 Comportamientos_Jugador
// que se piden en la práctica. Tiene como entrada la información de los
// sensores y devuelve la acción a realizar.
Action ComportamientoJugador::think(Sensores sensores) {
	Action accion = actIDLE;
	// Estoy en el nivel 1
	if (sensores.nivel != 4){
		if (sensores.mensajeF != -1){
			fil = sensores.mensajeF;
			col = sensores.mensajeC;
			brujula = 0; // 0 corresponde con Norte

			actual.fila = fil;
			actual.columna = col;
			actual.orientacion = brujula;

			destino.fila = sensores.destinoF;
			destino.columna = sensores.destinoC;
		}
		if(!hayPlan){
			hayPlan = pathFinding (sensores.nivel, actual, destino, plan);
		}

		Action sigaction;
		if(hayPlan && plan.size() > 0){
			sigaction=plan.front();
			plan.erase(plan.begin());
		}
		ultimaAccion = sigaction;
		return sigaction;

	}
	else {
		//Estoy nivel 2
		if( sensores.colision or sensores.reset or sensores.tiempo > 300){
			cout << "FIN DEL JUEGO" << endl;
			cout << "Objetivos: " << objetivos << endl;
			cout << "Tiempo: " << sensores.tiempo << endl;
			cout << "Colision: " << sensores.colision << endl;
			cout << "Vida: " << sensores.reset << endl;
			exit(0);
		}
		if(!estadoActualIniciado){
			actual.fila = 100;
			actual.columna = 100;
			actual.orientacion = brujula;
			estadoActualIniciado = true;
		}
		if(sensores.superficie[2] == 'a'){
			updateEstadoActual(actual, actTURN_R);
			if(kvista){
				kvista = false;
			}
			hayPlan = false;
			return actTURN_R;
		}
		if(!seDondeEstoy){
			if(obstaculoNoContemplado(actual, sensores.terreno)){
				updateEstadoActual(actual, actTURN_L);
				hayPlan = false;
				return actTURN_L;
			}
			if(!hayPlan){
					if(kvista){
						hayPlan = pathFinding_K(actual, plan);
					}
					else{
						hayPlan = pathFinding_Desconocido(actual, plan);
					}
			}
			Action sigaction;
			if(hayPlan && plan.size() > 0){
				sigaction=plan.front();
				plan.erase(plan.begin());
			}
			else{
				sigaction = actIDLE;
			}
			ultimaAccion = sigaction;
			updateMapaDesconocido(actual, sensores.terreno);
			updateEstadoActual(actual, sigaction);
			if(!kvista){
				for(int i = 0; i < 16; i++){
					if(sensores.terreno[i] == 'K'){
						cout << "HE VISTO K" << endl;
						kvista = true;
						updateEstadoActual(actual, actIDLE);
						return actIDLE;
					}
				}
			}
			else{
				if(sensores.mensajeF != -1){
					cout << "ESTOY EN K" << endl;
					seDondeEstoy = true;
					hayPlan = false;
					actual.fila = sensores.mensajeF;
					actual.columna = sensores.mensajeC;
					destino.fila = sensores.destinoF;
					destino.columna = sensores.destinoC;
					return actIDLE;
				}
			}
			if(plan.size() == 0){
				hayPlan = false;
			}
			return sigaction;
		}
		else{
			if(obstaculoNoContemplado(actual, sensores.terreno) && hayPlan){
				updateEstadoActual(actual, actIDLE);
				hayPlan = false;
				return actIDLE;
			}
			if(!hayPlan){
				cout << "Actual: (" << actual.fila << "," << actual.columna << ")" << endl;
				cout << "Destino: (" << destino.fila << "," << destino.columna << ")" << endl;
				hayPlan = pathFinding (sensores.nivel, actual, destino, plan);
			}
			Action sigaction;
			if(hayPlan && plan.size() > 0){
				sigaction=plan.front();
				plan.erase(plan.begin());
			}
			ultimaAccion = sigaction;
			updateMapa(actual, sensores.terreno);
			updateEstadoActual(actual, sigaction);

			if(plan.size() == 0){
				if(actual.fila == destino.fila and actual.columna == destino.columna){
					objetivos++;
					cout << "Objetivos alcanzados: " << objetivos << endl;
				}
				destino.fila = sensores.destinoF;
				destino.columna = sensores.destinoC;
				hayPlan = false;
			}
			return sigaction;
		}
  return accion;
	}
}

bool ComportamientoJugador::obstaculoNoContemplado(estado &st, vector<unsigned char> vector){
	if(vector[2] == 'P' || vector[2] == 'M'){
		return true;
	}
	else{
		return false;
	}
}

void ComportamientoJugador::updateMapaDesconocido(estado &st, vector<unsigned char> vector){
	desconocidos[st.fila][st.columna] = vector[0];
	if(st.fila > 7 and st.fila < (200-7) and st.columna > 7 and st.fila < (200-7)){
		switch (st.orientacion) {
			case 0:
				desconocidos[st.fila-1][st.columna-1] = vector[1];
				desconocidos[st.fila-1][st.columna] = vector[2];
				desconocidos[st.fila-1][st.columna+1] = vector[3];

				desconocidos[st.fila-2][st.columna-2] = vector[4];
				desconocidos[st.fila-2][st.columna-1] = vector[5];
				desconocidos[st.fila-2][st.columna] = vector[6];
				desconocidos[st.fila-2][st.columna+1] = vector[7];
				desconocidos[st.fila-2][st.columna+2] = vector[8];

				desconocidos[st.fila-3][st.columna-3] = vector[9];
				desconocidos[st.fila-3][st.columna-2] = vector[10];
				desconocidos[st.fila-3][st.columna-1] = vector[11];
				desconocidos[st.fila-3][st.columna] = vector[12];
				desconocidos[st.fila-3][st.columna+1] = vector[13];
				desconocidos[st.fila-3][st.columna+2] = vector[14];
				desconocidos[st.fila-3][st.columna+3] = vector[15];
				break;
			case 1:
				desconocidos[st.fila-1][st.columna+1] = vector[1];
				desconocidos[st.fila][st.columna+1] = vector[2];
				desconocidos[st.fila+1][st.columna+1] = vector[3];

				desconocidos[st.fila-2][st.columna+2] = vector[4];
				desconocidos[st.fila-1][st.columna+2] = vector[5];
				desconocidos[st.fila][st.columna+2] = vector[6];
				desconocidos[st.fila+1][st.columna+2] = vector[7];
				desconocidos[st.fila+2][st.columna+2] = vector[8];

				desconocidos[st.fila-3][st.columna+3] = vector[9];
				desconocidos[st.fila-2][st.columna+3] = vector[10];
				desconocidos[st.fila-1][st.columna+3] = vector[11];
				desconocidos[st.fila][st.columna+3] = vector[12];
				desconocidos[st.fila+1][st.columna+3] = vector[13];
				desconocidos[st.fila+2][st.columna+3] = vector[14];
				desconocidos[st.fila+3][st.columna+3] = vector[15];
				break;
			case 2:
				desconocidos[st.fila+1][st.columna+1] = vector[1];
				desconocidos[st.fila+1][st.columna] = vector[2];
				desconocidos[st.fila+1][st.columna-1] = vector[3];

				desconocidos[st.fila+2][st.columna+2] = vector[4];
				desconocidos[st.fila+2][st.columna+1] = vector[5];
				desconocidos[st.fila+2][st.columna] = vector[6];
				desconocidos[st.fila+2][st.columna-1] = vector[7];
				desconocidos[st.fila+2][st.columna-2] = vector[8];

				desconocidos[st.fila+3][st.columna+3] = vector[9];
				desconocidos[st.fila+3][st.columna+2] = vector[10];
				desconocidos[st.fila+3][st.columna+1] = vector[11];
				desconocidos[st.fila+3][st.columna] = vector[12];
				desconocidos[st.fila+3][st.columna-1] = vector[13];
				desconocidos[st.fila+3][st.columna-2] = vector[14];
				desconocidos[st.fila+3][st.columna-3] = vector[15];
				break;
			case 3:
				desconocidos[st.fila+1][st.columna-1] = vector[1];
				desconocidos[st.fila][st.columna-1] = vector[2];
				desconocidos[st.fila-1][st.columna-1] = vector[3];

				desconocidos[st.fila+2][st.columna-2] = vector[4];
				desconocidos[st.fila+1][st.columna-2] = vector[5];
				desconocidos[st.fila][st.columna-2] = vector[6];
				desconocidos[st.fila-1][st.columna-2] = vector[7];
				desconocidos[st.fila-2][st.columna-2] = vector[8];

				desconocidos[st.fila+3][st.columna-3] = vector[9];
				desconocidos[st.fila+2][st.columna-3] = vector[10];
				desconocidos[st.fila+1][st.columna-3] = vector[11];
				desconocidos[st.fila][st.columna-3] = vector[12];
				desconocidos[st.fila-1][st.columna-3] = vector[13];
				desconocidos[st.fila-2][st.columna-3] = vector[14];
				desconocidos[st.fila-3][st.columna-3] = vector[15];
				break;
		}
	}
}

void ComportamientoJugador::updateMapa(estado &st, vector<unsigned char> vector){
	mapaResultado[st.fila][st.columna] = vector[0];
	if(st.fila > 0 and st.fila < (mapaResultado.size()) and st.columna > 0 and st.fila < (mapaResultado.size())){
		switch (st.orientacion) {
			case 0:
				mapaResultado[st.fila-1][st.columna-1] = vector[1];
				mapaResultado[st.fila-1][st.columna] = vector[2];
				mapaResultado[st.fila-1][st.columna+1] = vector[3];

				mapaResultado[st.fila-2][st.columna-2] = vector[4];
				mapaResultado[st.fila-2][st.columna-1] = vector[5];
				mapaResultado[st.fila-2][st.columna] = vector[6];
				mapaResultado[st.fila-2][st.columna+1] = vector[7];
				mapaResultado[st.fila-2][st.columna+2] = vector[8];

				mapaResultado[st.fila-3][st.columna-3] = vector[9];
				mapaResultado[st.fila-3][st.columna-2] = vector[10];
				mapaResultado[st.fila-3][st.columna-1] = vector[11];
				mapaResultado[st.fila-3][st.columna] = vector[12];
				mapaResultado[st.fila-3][st.columna+1] = vector[13];
				mapaResultado[st.fila-3][st.columna+2] = vector[14];
				mapaResultado[st.fila-3][st.columna+3] = vector[15];
				break;
			case 1:
				mapaResultado[st.fila-1][st.columna+1] = vector[1];
				mapaResultado[st.fila][st.columna+1] = vector[2];
				mapaResultado[st.fila+1][st.columna+1] = vector[3];

				mapaResultado[st.fila-2][st.columna+2] = vector[4];
				mapaResultado[st.fila-1][st.columna+2] = vector[5];
				mapaResultado[st.fila][st.columna+2] = vector[6];
				mapaResultado[st.fila+1][st.columna+2] = vector[7];
				mapaResultado[st.fila+2][st.columna+2] = vector[8];

				mapaResultado[st.fila-3][st.columna+3] = vector[9];
				mapaResultado[st.fila-2][st.columna+3] = vector[10];
				mapaResultado[st.fila-1][st.columna+3] = vector[11];
				mapaResultado[st.fila][st.columna+3] = vector[12];
				mapaResultado[st.fila+1][st.columna+3] = vector[13];
				mapaResultado[st.fila+2][st.columna+3] = vector[14];
				mapaResultado[st.fila+3][st.columna+3] = vector[15];
				break;
			case 2:
				mapaResultado[st.fila+1][st.columna+1] = vector[1];
				mapaResultado[st.fila+1][st.columna] = vector[2];
				mapaResultado[st.fila+1][st.columna-1] = vector[3];

				mapaResultado[st.fila+2][st.columna+2] = vector[4];
				mapaResultado[st.fila+2][st.columna+1] = vector[5];
				mapaResultado[st.fila+2][st.columna] = vector[6];
				mapaResultado[st.fila+2][st.columna-1] = vector[7];
				mapaResultado[st.fila+2][st.columna-2] = vector[8];

				mapaResultado[st.fila+3][st.columna+3] = vector[9];
				mapaResultado[st.fila+3][st.columna+2] = vector[10];
				mapaResultado[st.fila+3][st.columna+1] = vector[11];
				mapaResultado[st.fila+3][st.columna] = vector[12];
				mapaResultado[st.fila+3][st.columna-1] = vector[13];
				mapaResultado[st.fila+3][st.columna-2] = vector[14];
				mapaResultado[st.fila+3][st.columna-3] = vector[15];
				break;
			case 3:
				mapaResultado[st.fila+1][st.columna-1] = vector[1];
				mapaResultado[st.fila][st.columna-1] = vector[2];
				mapaResultado[st.fila-1][st.columna-1] = vector[3];

				mapaResultado[st.fila+2][st.columna-2] = vector[4];
				mapaResultado[st.fila+1][st.columna-2] = vector[5];
				mapaResultado[st.fila][st.columna-2] = vector[6];
				mapaResultado[st.fila-1][st.columna-2] = vector[7];
				mapaResultado[st.fila-2][st.columna-2] = vector[8];

				mapaResultado[st.fila+3][st.columna-3] = vector[9];
				mapaResultado[st.fila+2][st.columna-3] = vector[10];
				mapaResultado[st.fila+1][st.columna-3] = vector[11];
				mapaResultado[st.fila][st.columna-3] = vector[12];
				mapaResultado[st.fila-1][st.columna-3] = vector[13];
				mapaResultado[st.fila-2][st.columna-3] = vector[14];
				mapaResultado[st.fila-3][st.columna-3] = vector[15];
				break;
		}
	}
}

void ComportamientoJugador::updateEstadoActual(estado &st, Action accion){
		switch (accion) {
			case actFORWARD:
				switch (st.orientacion) {
					case 0: st.fila--; break;
					case 1: st.columna++; break;
					case 2: st.fila++; break;
					case 3: st.columna--; break;
				}
				break;
			case actTURN_L:
				st.orientacion = (st.orientacion+3)%4;
				break;
			case actTURN_R:
				st.orientacion = (st.orientacion+1)%4;
				break;
			case actIDLE:
				break;
		}
}
// Llama al algoritmo de busqueda que se usará en cada comportamiento del agente
// Level representa el comportamiento en el que fue iniciado el agente.
bool ComportamientoJugador::pathFinding (int level, const estado &origen, const estado &destino, list<Action> &plan){
	switch (level){
		case 1: cout << "Busqueda en profundad\n";
			      return pathFinding_Profundidad(origen,destino,plan);
						break;
		case 2: cout << "Busqueda en Anchura\n";
			      return pathFinding_Anchura(origen,destino,plan);
						break;
		case 3: cout << "Busqueda Costo Uniforme\n";
						return pathFinding_CU(origen,destino,plan);
						break;
		case 4: cout << "Busqueda para el reto\n";
						return pathFinding_CU(origen,destino,plan);
						break;
	}
	cout << "Comportamiento sin implementar\n";
	return false;
}

//---------------------- Implementación de la busqueda en profundidad ---------------------------
// Dado el código en carácter de una casilla del mapa dice si se puede
// pasar por ella sin riegos de morir o chocar.
bool EsObstaculo(unsigned char casilla){
	if (casilla=='P' or casilla=='M' or casilla =='D')
		return true;
	else
	  return false;
}
// Comprueba si la casilla que hay delante es un obstaculo. Si es un
// obstaculo devuelve true. Si no es un obstaculo, devuelve false y
// modifica st con la posición de la casilla del avance.
bool ComportamientoJugador::HayObstaculoDelante(estado &st){
	int fil=st.fila, col=st.columna;

  // calculo cual es la casilla de delante del agente
	switch (st.orientacion) {
		case 0: fil--; break;
		case 1: col++; break;
		case 2: fil++; break;
		case 3: col--; break;
	}

	// Compruebo que no me salgo fuera del rango del mapa
	if (fil<0 or fil>=mapaResultado.size()) return true;
	if (col<0 or col>=mapaResultado[0].size()) return true;

	// Miro si en esa casilla hay un obstaculo infranqueable
	if (!EsObstaculo(mapaResultado[fil][col])){
		// No hay obstaculo, actualizo el parámetro st poniendo la casilla de delante.
    st.fila = fil;
		st.columna = col;
		return false;
	}
	else{
	  return true;
	}
}

bool ComportamientoJugador::estadoForwardnoValido(estado &st){
	int fil=st.fila, col=st.columna;

  // calculo cual es la casilla de delante del agente
	switch (st.orientacion) {
		case 0: fil--; break;
		case 1: col++; break;
		case 2: fil++; break;
		case 3: col--; break;
	}

	// Compruebo que no me salgo fuera del rango del mapa
	if (fil<0 or fil>=200) return true;
	if (col<0 or col>200) return true;

	// Miro si en esa casilla hay un obstaculo infranqueable
	if (desconocidos[fil][col] != 'P' and desconocidos[fil][col] != 'M' and desconocidos[fil][col] != 'D'){
		// No hay obstaculo, actualizo el parámetro st poniendo la casilla de delante.
    st.fila = fil;
		st.columna = col;
		return false;
	}
	else{
	  return true;
	}
}

struct nodo{
	estado st;
	list<Action> secuencia;
};

struct ComparaEstados{
	bool operator()(const estado &a, const estado &n) const{
		if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
	      (a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion))
			return true;
		else
			return false;
	}
};

struct ComparaNodoPeso{
	bool operator()(const nodoPeso &a, const nodoPeso &n) const{
		if (a.peso <= n.peso)
			return true;
		else
			return false;
	}
};

void ComportamientoJugador::asignarPeso(nodoPeso &nodo){
	int fil = nodo.st.fila;
	int col = nodo.st.columna;
	char mapa = mapaResultado[fil][col];
	switch (mapa){
		case 'S': nodo.peso = 1; break;
		case 'T': nodo.peso = 2; break;
		case 'B': nodo.peso = 5; break;
		case 'A': nodo.peso = 10; break;
		case '?': nodo.peso = 15; break;
	}
}

void ComportamientoJugador::asignarPesoForward(nodoPeso &nodo){
	int fil = nodo.st.fila;
	int col = nodo.st.columna;
	if (fil<0 or fil>=mapaResultado.size() or col<0 or col>=mapaResultado[0].size()){
		switch (nodo.st.orientacion) {
			case 0: fil--; break;
			case 1: col++; break;
			case 2: fil++; break;
			case 3: col--; break;
		}
	}
	char mapa = mapaResultado[fil][col];
	switch (mapa){
		case 'S': nodo.peso = 1; break;
		case 'T': nodo.peso = 2; break;
		case 'B': nodo.peso = 5; break;
		case 'A': nodo.peso = 10; break;
		case '?': nodo.peso = 1; break;
	}
}
// Implementación de la búsqueda en profundidad.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	stack<nodo> pila;											// Lista de Abiertos

  nodo current;
	current.st = origen;
	current.secuencia.empty();

	pila.push(current);

  while (!pila.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){
		pila.pop();
		generados.insert(current.st);

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			pila.push(hijoTurnR);
		}

		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			pila.push(hijoTurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				pila.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la pila
		if (!pila.empty()){
			current = pila.top();
		}
	}

  cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}
	return false;
}
// Implementación de la búsqueda en anchura.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	queue<nodo> cola;											// Lista de Abiertos

  nodo current;
	current.st = origen;
	current.secuencia.empty();

	cola.push(current);

  while (!cola.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){
		cola.pop();
		generados.insert(current.st);

		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			cola.push(hijoTurnR);
		}
		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			cola.push(hijoTurnL);
		}
		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				cola.push(hijoForward);
			}
		}
		// Tomo el siguiente valor de la pila
		if (!cola.empty()){
			current = cola.front();
		}
	}

  cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}
	return false;
}
// Implementación de la búsqueda en costo uniforme.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_CU(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	cout << "Calculando plan CU\n";
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	set<nodoPeso, ComparaNodoPeso> orden;
	queue<nodoPeso> cola;									// Lista de Abiertos

  nodoPeso current;
	current.st = origen;
	current.secuencia.empty();
	current.peso = 0;

	cola.push(current);

  while (!cola.empty() and (current.st.fila!=destino.fila or current.st.columna != destino.columna)){
		cola.pop();
		generados.insert(current.st);

		// Generar descendiente de girar a la derecha
		nodoPeso hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		asignarPesoForward(hijoTurnR);
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			orden.insert(hijoTurnR);
		}
		// Generar descendiente de girar a la izquierda
		nodoPeso hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		asignarPesoForward(hijoTurnL);
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			orden.insert(hijoTurnL);
		}
		// Generar descendiente de avanzar
		nodoPeso hijoForward = current;
		asignarPesoForward(hijoForward);
		if (!HayObstaculoDelante(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				orden.insert(hijoForward);
			}
		}

		set<nodoPeso, ComparaNodoPeso>::iterator it;
		for(it = orden.begin(); it != orden.end(); ++it){
			cola.push(*it);
		}
		orden.clear();

		// Tomo el siguiente valor de la pila
		if (!cola.empty()){
			current = cola.front();
		}
	}

  cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}
	return false;
}

bool ComportamientoJugador::pathFinding_Desconocido(const estado &origen, list<Action> &plan) {
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	stack<nodo> pila;											// Lista de Abiertos

  nodo current;
	current.st = origen;
	current.secuencia.empty();

	pila.push(current);

  while (!pila.empty() and desconocidos[current.st.fila][current.st.columna] != '?'){
		pila.pop();
		generados.insert(current.st);


		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			pila.push(hijoTurnR);
		}
		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			pila.push(hijoTurnL);
		}
		// Generar descendiente de avanzar
		nodo hijoForward = current;
		switch (hijoForward.st.orientacion) {
			case 0: hijoForward.st.fila--; break;
			case 1: hijoForward.st.columna++; break;
			case 2: hijoForward.st.fila++; break;
			case 3: hijoForward.st.columna--; break;
		}
		if(!estadoForwardnoValido(hijoForward.st)){
			if (generados.find(hijoForward.st) == generados.end()){
				hijoForward.secuencia.push_back(actFORWARD);
				pila.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la cola
		if (!pila.empty()){
			current = pila.top();
		}
	}

  cout << "Terminada la busqueda\n";
	if (desconocidos[current.st.fila][current.st.columna] == '?'){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}
	return false;
}

bool ComportamientoJugador::pathFinding_K(const estado &origen, list<Action> &plan) {
	cout << "Calculando camino hacia K" << endl;
	plan.clear();
	set<estado,ComparaEstados> generados; // Lista de Cerrados
	queue<nodo> cola;											// Lista de Abiertos

  nodo current;
	current.st = origen;
	current.secuencia.empty();

	cola.push(current);

  while (!cola.empty() and desconocidos[current.st.fila][current.st.columna] != 'K'){
		cola.pop();
		generados.insert(current.st);


		// Generar descendiente de girar a la derecha
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion+1)%4;
		if (generados.find(hijoTurnR.st) == generados.end()){
			hijoTurnR.secuencia.push_back(actTURN_R);
			cola.push(hijoTurnR);
		}
		// Generar descendiente de girar a la izquierda
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion+3)%4;
		if (generados.find(hijoTurnL.st) == generados.end()){
			hijoTurnL.secuencia.push_back(actTURN_L);
			cola.push(hijoTurnL);
		}
		// Generar descendiente de avanzar
		nodo hijoForward = current;
		switch (hijoForward.st.orientacion) {
			case 0: hijoForward.st.fila--; break;
			case 1: hijoForward.st.columna++; break;
			case 2: hijoForward.st.fila++; break;
			case 3: hijoForward.st.columna--; break;
		}

		if (generados.find(hijoForward.st) == generados.end()){
			hijoForward.secuencia.push_back(actFORWARD);
			cola.push(hijoForward);
		}


		// Tomo el siguiente valor de la cola
		if (!cola.empty()){
			current = cola.front();
		}
	}

  cout << "Terminada la busqueda\n";
	if (desconocidos[current.st.fila][current.st.columna] == 'K'){
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		return true;
	}
	else {
		cout << "No encontrado plan\n";
	}
	return false;
}
// Sacar por la términal la secuencia del plan obtenido
void ComportamientoJugador::PintaPlan(list<Action> plan) {
	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			cout << "A ";
		}
		else if (*it == actTURN_R){
			cout << "D ";
		}
		else if (*it == actTURN_L){
			cout << "I ";
		}
		else {
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}

void AnularMatriz(vector<vector<unsigned char> > &m){
	for (int i=0; i<m[0].size(); i++){
		for (int j=0; j<m.size(); j++){
			m[i][j]=0;
		}
	}
}
// Pinta sobre el mapa del juego el plan obtenido
void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan){
  AnularMatriz(mapaConPlan);
	estado cst = st;

	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			switch (cst.orientacion) {
				case 0: cst.fila--; break;
				case 1: cst.columna++; break;
				case 2: cst.fila++; break;
				case 3: cst.columna--; break;
			}
			mapaConPlan[cst.fila][cst.columna]=1;
		}
		else if (*it == actTURN_R){
			cst.orientacion = (cst.orientacion+1)%4;
		}
		else {
			cst.orientacion = (cst.orientacion+3)%4;
		}
		it++;
	}
}



int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}
