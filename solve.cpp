#include <iostream>
#include <vector>
#include <random>
#include <set>
#include <algorithm>
#define debug(x) cerr << #x << " : " << x << endl;

using namespace std;
using Graph = vector<vector<int>>;

int MAX_LEN = 1000;

int N, M, K;
vector<int> w;
Graph edges;
vector<int> v_group;
vector<int> w_sum;
int current_score;

void input() {
    cin >> N >> M >> K;
    w.resize(N+1);
    for(int i=1; i<=N; i++) {
        cin >> w[i];
    }
    edges.resize(N+1);
    for(int j=1; j<=M; j++) {
        int a,b;
        cin >> a >> b;
        edges[a].push_back(b);
        edges[b].push_back(a);
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
void output_score() {
    cout << "score: " << current_score << endl;
}

int calc_score(vector<int> weight) {
    int w_max = *max_element(weight.begin()+1, weight.end());
    int w_min = *min_element(weight.begin()+1, weight.end());
    int score = (w_max * 10000 / w_min) - 10000;
    return score;
}

void dfs(vector<bool> &seen, const vector<Graph> &div_edges, int v, int k) {
    seen[v] = true;

    for(auto next_v : div_edges[k][v]) {
        if(seen[next_v]) continue;
        dfs(seen, div_edges, next_v, k);
    }
}

// 各部分グラフが全て連結か判定
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
            return false;
        }
    }
    return true;
}

// 初期解生成
void init_sol() {
    do{
        // 全てのノードのグループを1にセット
        for(int i=1; i<=N; i++) {
            v_group[i] = 1;
        }
        // 各グループに一つだけノードを割り当てる
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
    // 頂点重みの和を計算
    for(int k=1; k<=K; k++) {
        w_sum[k] = 0;
    }
    for(int i=1; i<=N; i++) {
        w_sum[v_group[i]] += w[i];
    }
}

template<typename S>
auto select_random(const S &s, size_t n) {
    auto it = std::begin(s);
    // 'advance' the iterator n times
    std::advance(it,n);
    return it;
}

void single_change() {
    int change_v = rand()%N+1; // 変更するノード番号
    int before_group = v_group[change_v]; // 変更前のグループ
    set<int> group_candidate; // 変更後のグループ候補(隣接しているグループが入る)
    for(int l=0; l<edges[change_v].size(); l++) {
        int tmp_group = v_group[edges[change_v][l]];
        if(tmp_group != before_group) {
            group_candidate.insert(tmp_group);
        }
    }
    if(group_candidate.size() == 0) return;
    auto r = rand() % group_candidate.size(); // not _really_ random
    // ランダムで変更後のグループを選ぶ
    auto after_group = *select_random(group_candidate, r);

    // 連結しているかの判定
    v_group[change_v] = after_group; // グループの変更
    if(!isconnected()) { // 連結でないならば元に戻す
        v_group[change_v] = before_group;
        return;
    }
    // 部分グラフの重みの再計算
    vector<int> change_weight = w_sum;
    change_weight[before_group] -= w[change_v];
    change_weight[after_group] += w[change_v];
    int after_score = calc_score(change_weight);
    if(after_score > current_score) { // スコアが悪化した場合
        v_group[change_v] = before_group;
        return;
    } else { // 現状維持または改善した場合は，変更を保存する
        if(after_score < current_score) {
            debug(after_score);
        }
        w_sum = change_weight;
        current_score = after_score;
    }
}

int main() {
    input();
    init_sol();
    current_score = calc_score(w_sum);
    for(int i=0; i<10000000; i++) {
        single_change();
    }
    output_ans();
    output_score();

}