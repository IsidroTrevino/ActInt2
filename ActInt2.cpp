#include <iostream>
#include <vector>
#include <string>
#include <climits>
#include <map>
#include <queue>

#define INF INT_MAX

using namespace std;

struct Colonias{
    string nombre;
	float x, y;
    bool isCentral, isNew;
	Colonias(){
		x = y = 0;
        nombre = "";
        isCentral = false;
        isNew = false;
	}
    
	Colonias(string nombre, bool isCentral, bool isNew, float x, float y){
		this->x = x;
		this->y = y;
        this->nombre = nombre;
        this->isCentral = isCentral;
        this->isNew = isNew;
	}
};

struct node {
    int nivel;
    int costoAcum;
    int costoPos;
    int verticeActual;
    vector<bool> visitados;
    vector<int> ruta;
    bool operator<(const node &otro) const {
        return costoPos > otro.costoPos;
    }
};

void iniciaMatriz(vector<vector<int>> &matAdj, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matAdj[i][j] = (i == j) ? 0 : INF;
        }
    }
}

void leeDatos(vector<vector<int>> &matAdj, map<string, int> &nombreToIndex, int m) {
    string a, b;
    int c;
    for (int i = 0; i < m; i++) {
        cin >> a >> b >> c;
        int idxA = nombreToIndex[a];
        int idxB = nombreToIndex[b];
        matAdj[idxA][idxB] = matAdj[idxB][idxA] = c;
    }
}

void floydWarshall(vector<vector<int>> &matAdj, int n) {
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (matAdj[i][k] != INF && matAdj[k][j] != INF) {
                    matAdj[i][j] = min(matAdj[i][j], matAdj[i][k] + matAdj[k][j]);
                }
            }
        }
    }
}

void calculaCostoPosible(node &nodoAct, vector<vector<int>> &matAdj, int n) {
    nodoAct.costoPos = nodoAct.costoAcum;
    for (int i = 0; i < n; i++) {
        if (!nodoAct.visitados[i]) {
            int min1 = INF, min2 = INF;
            for (int j = 0; j < n; j++) {
                if (i != j) {
                    if (matAdj[i][j] < min1) {
                        min2 = min1;
                        min1 = matAdj[i][j];
                    } else if (matAdj[i][j] < min2) {
                        min2 = matAdj[i][j];
                    }
                }
            }
            nodoAct.costoPos += (min1 + min2) / 2;
        }
    }
}

pair<int, vector<int>> tsp(vector<vector<int>> &matAdj, const vector<int> &nonCentralIndices) {
    int n = nonCentralIndices.size();
    int costoOpt = INF;
    vector<int> rutaOptima;

    node raiz;
    raiz.nivel = 0;
    raiz.costoAcum = 0;
    raiz.verticeActual = nonCentralIndices[0];
    raiz.visitados = vector<bool>(matAdj.size(), false);
    raiz.visitados[raiz.verticeActual] = true;
    raiz.ruta.push_back(raiz.verticeActual);

    calculaCostoPosible(raiz, matAdj, matAdj.size());

    priority_queue<node> pq;
    pq.push(raiz);

    while (!pq.empty()) {
        node nodoAct = pq.top();
        pq.pop();

        if (nodoAct.costoPos >= costoOpt) continue;

        for (int i : nonCentralIndices) {
            if (!nodoAct.visitados[i] && matAdj[nodoAct.verticeActual][i] != INF) {
                node hijo;
                hijo.nivel = nodoAct.nivel + 1;
                hijo.verticeActual = i;
                hijo.visitados = nodoAct.visitados;
                hijo.visitados[i] = true;
                hijo.costoAcum = nodoAct.costoAcum + matAdj[nodoAct.verticeActual][i];
                hijo.ruta = nodoAct.ruta;
                hijo.ruta.push_back(i);

                if (hijo.nivel == n - 1) {
                    int costoReal = hijo.costoAcum + matAdj[i][nonCentralIndices[0]];
                    if (matAdj[i][nonCentralIndices[0]] != INF && costoReal < costoOpt) {
                        costoOpt = costoReal;
                        rutaOptima = hijo.ruta;
                        rutaOptima.push_back(nonCentralIndices[0]);
                    }
                } else {
                    calculaCostoPosible(hijo, matAdj, matAdj.size());
                    if (hijo.costoPos < costoOpt) {
                        pq.push(hijo);
                    }
                }
            }
        }
    }
    return {costoOpt, rutaOptima};
}

int main() {
    int n, m, k, q;
    cin >> n >> m >> k >> q;

    vector<Colonias> Col;
    vector<int> centralIndices, nonCentralIndices;
    map<string, int> nombreToIndex;

    string nombre;
    int x, y;

    for (int i = 0; i < n; i++) {
        bool isCentral;
        cin >> nombre >> x >> y >> isCentral;
        Col.push_back(Colonias(nombre, isCentral, false, x, y));
        nombreToIndex[nombre] = i;
        if (isCentral) {
            centralIndices.push_back(i);
        } else {
            nonCentralIndices.push_back(i);
        }
    }

    vector<vector<int>> matAdj(n, vector<int>(n));
    iniciaMatriz(matAdj, n);
    leeDatos(matAdj, nombreToIndex, m);

    for (int i = 0; i < k; i++) {
        string a, b;
        cin >> a >> b;
        int idxA = nombreToIndex[a];
        int idxB = nombreToIndex[b];
        matAdj[idxA][idxB] = matAdj[idxB][idxA] = 0;
    }

    for (int i = 0; i < q; i++) {
        cin >> nombre >> x >> y;
        Col.push_back(Colonias(nombre, false, true, x, y));
    }

    auto [costoOptimo, rutaOptima] = tsp(matAdj, nonCentralIndices);

    cout << "-------------------\n";
    cout << "2 - La ruta óptima.\n";
    for (size_t i = 0; i < rutaOptima.size(); i++) {
        cout << Col[rutaOptima[i]].nombre;
        if (i < rutaOptima.size() - 1) cout << " - ";
    }
    cout << "\nLa Ruta Óptima tiene un costo total de: " << costoOptimo << endl;

    floydWarshall(matAdj, n);

    cout << "-------------------\n";
    cout << "3 – Caminos más cortos entre centrales.\n";

    for (size_t i = 0; i < centralIndices.size(); i++) {
        for (size_t j = i + 1; j < centralIndices.size(); j++) {
            int c1 = centralIndices[i];
            int c2 = centralIndices[j];
            if (matAdj[c1][c2] != INF) {
                cout << Col[c1].nombre << " - " << Col[c2].nombre << " (" << matAdj[c1][c2] << ")\n";
            }
        }
    }
    cout << "-------------------\n";
    return 0;
}

/*
5 8 1 2
LindaVista 200 120 1
Purisima 150 75 0
Tecnologico -50 20 1
Roma -75 50 0
AltaVista -50 40 0
LindaVista Purisima 48
LindaVista Roma 17
Purisima Tecnologico 40
Purisima Roma 50
Purisima AltaVista 80
Tecnologico Roma 55
Tecnologico AltaVista 15
Roma AltaVista 18
Purisima Tecnologico
Independencia 180 -15
Roble 45 68
*/
