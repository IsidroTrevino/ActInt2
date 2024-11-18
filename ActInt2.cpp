#include <iostream>
#include <vector>
#include <string>
#include <climits>
#include <map>
#include <queue>
#include <fstream>
#include <algorithm>

#define INF INT_MAX

using namespace std;

struct edgeK
{
    pair<int,int> conection;
    int cost;
};

struct Graph{
    // E Arcos (Edges)
    // V Vertices (Vertex)
    int V, E, costMSTKruskal;
    map<pair<int,int>,int> edgeToWeight;
    map<pair<int,int>,int> edgeToIndex;
    vector<pair<int, pair<int,int>>> edges; // Utilizar en Kruskañ
    vector<pair<int, pair<int,int>>> selectedEdgesK; // Arcos seleccionados por Kruskal
    Graph(int V, int E){
        this->V = V;
        this->E = E;
        costMSTKruskal = 0;
    }
    // u = saldia del arco
    // v = llegada del arco
    // w = costo del arco
    void addEdge(int u, int v, int w){
        edges.push_back({w,{u,v}}); // first = costo, second = conexión
        edgeToWeight[{u,v}] = w;
        edgeToIndex[{u,v}] = edges.size() -1;
    }
    void load();
    void print();
    void kruskalMST();
    void primMST();
    void printEdgesK(map<int, string>, ofstream &checking2);
};

//disjoint sets (Union FInd)
struct DisjointSets{
    int *parent, *rank;
    int n;
    DisjointSets(int n){
        this->n = n;
        parent = new int[n+1];
        rank = new int[n+1];
        for (int i = 0; i <= n; i++){
            rank[i] = 0;
            parent[i] = i;
        }
        }
    //Para encontrar el parent de u
    int find(int u){
        if (u != parent[u]){
            parent[u] = find(parent[u]);
        }
        return parent[u];
        
    }

    void merge(int x, int y){
        x=find(x);
        y=find(y);
        if(rank[x]>rank[y]){
            parent[y] = x;
        }
        else{
            parent[x] = y;
        }
        if(rank[x] == rank[y]){
            rank[y]++;
        }
    }
        
    
};

void Graph::load(){
    int a,b,c;
    for (int i = 0; i < E; i++)
    {
        cin >> a>>b>>c;
        addEdge(a,b,c);
        addEdge(b,a,c);
    }
}

// O(E log E)
void Graph::kruskalMST(){
    sort(edges.begin(), edges.end()); //ordenar ascendentemente con respecto al costo
    DisjointSets ds(V);
    costMSTKruskal = 0;
    for(auto it:edges){
        int p1 = ds.find(it.second.first);
        int p2 = ds.find(it.second.second);
        if(p1 != p2){
            costMSTKruskal += it.first;
            selectedEdgesK.push_back({it.first,it.second});
            ds.merge(it.second.first,it.second.second);
        }
    }
}

void Graph::printEdgesK(map<int, string> indexToNombre, ofstream &checking2){
    checking2 << "-------------------" << endl;
    checking2 << "1 - Cableado óptimo de nueva conexión." << endl << endl;
    for(auto it:selectedEdgesK){
        if ( it.first != 0){
        checking2 << indexToNombre[it.second.first] << " - " << indexToNombre[it.second.second] << " " << it.first;
        checking2 << endl;
        }
    }
    checking2 << endl;
}

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

void leeDatos(vector<vector<int>> &matAdj, map<string, int> &nombreToIndex, int m, Graph & g) {
    string a, b;
    int c;
    for (int i = 0; i < m; i++) {
        cin >> a >> b >> c;
        int idxA = nombreToIndex[a];
        int idxB = nombreToIndex[b];
        matAdj[idxA][idxB] = matAdj[idxB][idxA] = c;
        g.addEdge(idxA,idxB,c);
    }
}

void floydWarshall(vector<Colonias> &colonias, vector<vector<int>> &next, vector<vector<int>> &dist, int n) {
    for (int k = 0; k < n; k++) {
        if (colonias[k].isNew) continue;
        for (int i = 0; i < n; i++) {
            if (colonias[i].isNew) continue;
            for (int j = 0; j < n; j++) {
                if (colonias[j].isNew) continue;
                if (dist[i][k] != INF && dist[k][j] != INF && dist[i][j] > dist[i][k] + dist[k][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j] = next[i][k];
                }
            }
        }
    }
}

void imprimirRutaCentrales(vector<int> &centralIndices, vector<vector<int>> &dist, vector<vector<int>> &next, vector<Colonias> &colonias, ofstream &checking2) {
    checking2 << "-------------------\n";
    checking2 << "3 - Caminos más cortos entre centrales.\n" << endl;
    for (int i = 0; i < centralIndices.size(); i++) {
        for (int j = i + 1; j < centralIndices.size(); j++) {
            int c1 = centralIndices[i];
            int c2 = centralIndices[j];
            if (dist[c1][c2] == INF) continue;

            vector<int> ruta;
            for (int at = c1; at != -1; at = next[at][c2]) {
                ruta.push_back(at);
                if (at == c2) break;
            }

            for (int k = 0; k < ruta.size(); k++) {
                checking2 << colonias[ruta[k]].nombre;
                if (k < ruta.size() - 1) checking2 << " - ";
            }
            checking2 << " (" << dist[c1][c2] << ")\n" << endl;
        }
    }
    checking2 << "-------------------\n";
}

void calculaCaminosCentrales(vector<vector<int>> &matAdj, vector<int> &centralIndices, vector<int> &nonCentralIndices, vector<Colonias> &colonias, ofstream &checking2) {
    int n = matAdj.size();
    vector<vector<int>> dist = matAdj;
    vector<vector<int>> next(n, vector<int>(n, -1));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (matAdj[i][j] != INF && i != j) {
                next[i][j] = j;
            }
        }
    }

    floydWarshall(colonias, next, dist, n);
    imprimirRutaCentrales(centralIndices, dist, next, colonias, checking2);
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

void imprimirTSP(int costoOptimo, vector<int> &rutaOptima, vector<Colonias> &Col, ofstream &checking2) {
    checking2 << "-------------------\n";
    checking2 << "2 - La ruta óptima.\n" << endl;
    for (int i = 0; i < rutaOptima.size(); i++) {
        checking2 << Col[rutaOptima[i]].nombre;
        if (i < rutaOptima.size() - 1) checking2 << " - ";
    }
    checking2 << endl;
    checking2 << "\nLa Ruta Óptima tiene un costo total de: " << costoOptimo << endl;
}

void TSP(vector<vector<int>> &matAdj, vector<int> &nonCentralIndices, vector<int> &centralIndices, vector<Colonias> &colonias, ofstream &checking2) {
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

        for (int i = 0; i < matAdj.size(); i++) {
            if (!nodoAct.visitados[i] && !colonias[i].isNew && matAdj[nodoAct.verticeActual][i] != INF) {
                node hijo;
                hijo.nivel = nodoAct.nivel + 1;
                hijo.verticeActual = i;
                hijo.visitados = nodoAct.visitados;
                hijo.visitados[i] = true;
                hijo.costoAcum = nodoAct.costoAcum + matAdj[nodoAct.verticeActual][i];
                hijo.ruta = nodoAct.ruta;
                hijo.ruta.push_back(i);

                bool todasVisitadas = true;
                for (int nc : nonCentralIndices) {
                    if (!hijo.visitados[nc]) {
                        todasVisitadas = false;
                        break;
                    }
                }

                if (todasVisitadas && hijo.nivel >= n) {
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
    imprimirTSP(costoOpt, rutaOptima, colonias, checking2);
}

int main() {
    int n, m, k, q;
    cin >> n >> m >> k >> q;

    vector<Colonias> Col;
    vector<int> centralIndices, nonCentralIndices;
    map<string, int> nombreToIndex;
    map<int, string> indexToNombre;
    ofstream checking2("checking2.txt");

    string nombre;
    int x, y;

    for (int i = 0; i < n; i++) {
        bool isCentral;
        cin >> nombre >> x >> y >> isCentral;
        Col.push_back(Colonias(nombre, isCentral, false, x, y));
        nombreToIndex[nombre] = i;
        indexToNombre[i] = nombre;
        if (isCentral) {
            centralIndices.push_back(i);
        } else {
            nonCentralIndices.push_back(i);
        }
    }

    vector<vector<int>> matAdj(n, vector<int>(n));
    iniciaMatriz(matAdj, n);
    Graph g(n,m);
    leeDatos(matAdj, nombreToIndex, m, g);
    string nombre2;

    for (int i = 0; i < k; i++)
    {
        cin >> nombre >> nombre2;
        int ind1 = nombreToIndex[nombre];
        int ind2 = nombreToIndex[nombre2];
        int ind = g.edgeToIndex[{ind1,ind2}];
        g.edges[i] = {0,{ind1,ind2}};//sobre escribimos edge para que su peso sea 0 y así no tome esa ruta para el costo
    }


    for (int i = 0; i < q; i++) {
        cin >> nombre >> x >> y;
        Col.push_back(Colonias(nombre, false, true, x, y));
    }

    g.kruskalMST();
    
    g.printEdgesK(indexToNombre, checking2);
    checking2 << "Costo Total: " << g.costMSTKruskal << endl << endl;

    TSP(matAdj, nonCentralIndices, centralIndices, Col, checking2);

    checking2 << endl;

    calculaCaminosCentrales(matAdj, centralIndices, nonCentralIndices, Col, checking2);

    checking2.close();

    return 0;
}

// Test cases:
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

/*
10 19 0 4
Condesa -193 -151 1
DelValle -142 -70 0
Polanco 144 -122 1
RomaNorte -91 -160 1
Pantitlán -108 42 0
Juárez -97 127 1
Tlatelolco -160 12 0
SantaFe 169 -91 1
Cosmopolita 160 -67 1
Merced 78 16 0
Condesa Polanco 27
Condesa Juárez 68
Condesa Cosmopolita 34
DelValle RomaNorte 80
DelValle Pantitlán 68
DelValle SantaFe 94
DelValle Cosmopolita 86
Polanco Juárez 92
Polanco Tlatelolco 58
Polanco Merced 54
RomaNorte Juárez 69
RomaNorte Cosmopolita 25
Pantitlán Juárez 4
Pantitlán SantaFe 67
Pantitlán Cosmopolita 25
Juárez SantaFe 78
Juárez Cosmopolita 29
Tlatelolco Cosmopolita 82
SantaFe Merced 66
FelipeAngeles -64 25
Tlazintla -85 -106
Penitenciaria -99 -83
Porvenir -95 -196
*/