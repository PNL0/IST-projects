#include <iostream>
#include <queue>
using namespace std;

int vertices = 0;

void dfs(vector<vector<int>> &grafo,int vertice, vector<int> &visitado, 
    vector<int>& ordem_saida){

    vector<int> q;
    q.push_back(vertice);
    int finish = 0;

    visitado[vertice] = 1;
    while (!q.empty()){
        int primeiro = q.back();
        finish = 1;
        visitado[primeiro] = 1;
        for (auto x: grafo[primeiro]){
            if(visitado[x] == 0){
                visitado[x] = 1;
                q.push_back(x);
                finish = 0;
                break;
            }
        }
        if (finish == 1){
            q.pop_back();
            visitado[primeiro] = 2;
            ordem_saida.push_back(primeiro);
        }
    }
}

int dfs_transposta(vector<vector<int>> &adj,vector<int> &scc, vector<int> &visitado, 
    vector<int>& ordem_saida, vector<int>& distance, int caminho){
    
    int index = ordem_saida.back();
    vector<int> q;
    q.push_back(index);
    int finish = 0;
    int max_proc = -1;
    scc[index] = index;
    while (!q.empty()){
        int primeiro = q.back();
        finish = 1;
        for (auto x: adj[primeiro]){
            if (scc[primeiro] != scc[x] && max_proc < distance[x]){
                max_proc = distance[x];
            }
            if(visitado[x] == 2){
                scc[x] = index;
                visitado[primeiro] = 3;
                distance[x] = distance[primeiro];
                finish = 0;
                q.push_back(x);
                break;
            }
        }
        if (finish == 1){
            q.pop_back();
            ordem_saida.pop_back();
            distance[primeiro] = max_proc + 1;
            if (distance[primeiro] > caminho)
                caminho = distance[primeiro];
            visitado[primeiro] = 4;
        }
    }
    return caminho;
}

int findSCC(vector<vector<int>> &grafo,vector<vector<int>> &adj,vector<int> &scc){

    vector<int> visitado(vertices + 1,0);
    vector<int> distance(vertices + 1,-1);
    vector<int> ordem_saida;
    for (int i = 0; i <= vertices; i++){
        if (visitado[i] == 0)
            dfs(grafo,i,visitado,ordem_saida);
    }
    int caminho_max = 0;
    while (!ordem_saida.empty()){
        caminho_max = dfs_transposta(adj,scc,visitado,ordem_saida,distance,caminho_max);
    }    
    return caminho_max;
}

int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    int arestas, p1, p2;;
    cin >> vertices >> arestas;

    vector<vector<int>> grafo(vertices+1);
    vector<vector<int>> adj(vertices+1);
    vector<int> scc(vertices+1,-1);
    int distancia_max;

    for (int i = 0; i < arestas; i++) {
        cin >> p1 >> p2;
        grafo[p1].push_back(p2);
        adj[p2].push_back(p1);
    }
    
    distancia_max = findSCC(grafo,adj,scc);

    printf("%d\n",distancia_max);
    return 0;
}

