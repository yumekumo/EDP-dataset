#include <iostream>
#include <vector>
#include <random>
#define debug(x) cerr << #x << " : " << x << endl;

using namespace std;
using Graph = vector<vector<int>>;

int MAX_LEN = 1000;

int N, M, K;
vector<int> w;
Graph edges;
vector<int> v_group;
vector<int> w_sum;

void input() {
    cin >> N >> M >> K;
    w.resize(N+1);
    for(int i=1; i<=N; i++) {
        cin >> w.at(i);
    }
    edges.resize(N+1);
    for(int j=1; j<=M; j++) {
        int a,b;
        cin >> a >> b;
        edges.at(a).push_back(b);
        edges.at(b).push_back(a);
    }
    v_group.resize(N+1);
    w_sum.resize(K+1);
}

void output_ans() {
    for(int i=1; i<=N; i++) {
        cout << v_group[i];
        if(i!=N) {
            cout << " ";
        }
        else {
            cout << endl;
        }
    }
}

void dfs(vector<bool> &seen, const vector<Graph> &div_edges, int v, int k) {
    seen[v] = true;

    for(auto next_v : div_edges[k][v]) {
        if(seen[next_v]) continue;
        dfs(seen, div_edges, next_v, k);
    }
}

bool isconnected() {
    vector<Graph> div_edges(K+1, Graph(N+1));
    for(int i=1; i<=N; i++) {
        for(int l=0; l<edges[i].size(); l++) {
            if(v_group[i] == v_group[edges[i][l]]) {
                div_edges[v_group[i]][edges[i][l]].push_back(i);
                div_edges[v_group[i]][i].push_back(edges[i][l]);
            }
        }
    }

    vector<bool> seen(N+1, false);
    for(int k=1; k<=K; k++) {
        int start_v = -1;
        for(int i=1; i<=N; i++) {
            if(v_group[i] == k) {
                start_v = i;
                break;
            }
            if(i==N) return false;
        }
        dfs(seen, div_edges, start_v, k);
    }
    for(int i=1; i<=N; i++) {
        if(seen[i]) continue;
        else{
            cout << "i:" << i << endl;
            return false;
        }
    }
    return true;
}

void init_sol() {
    do{
        for(int i=1; i<=N; i++) {
            v_group[i] = 1;
        }
        for(int k=2; k<=K; k++) {
            while(true) {
                int label = rand()%N+1;
                if(v_group[label]==1) {
                    v_group[label] = k;
                    break;
                }
            }
        }
    }while(!isconnected());
}

int main() {
    input();
    init_sol();
    output_ans();

}