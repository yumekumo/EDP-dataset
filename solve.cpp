#include <iostream>
#include <vector>
#include <random>
#include <set>
#include <algorithm>
#include <time.h>
#define debug(x) cerr << #x << " : " << x << endl;

using namespace std;
using Graph = vector<vector<int>>;

int TIME_ANN = 5;
int TIME_LOC = 2;

// 入力データ
int N, M, K;
vector<int> w, a, b;
Graph edges;
// 変数
vector<int> v_group;
vector<int> v_group_cnt;
vector<int> w_sum;
int64_t current_score;
// 焼きなましで使う変数
double start_temp = 10000000;
double end_temp = 10;

void input() {
    cin >> N >> M >> K;
    w.resize(N+1);
    for(int i=1; i<=N; i++) {
        cin >> w[i];
    }
    a.resize(M+1);
    b.resize(M+1);
    edges.resize(N+1);
    for(int j=1; j<=M; j++) {
        cin >> a[j] >> b[j];
        edges[a[j]].push_back(b[j]);
        edges[b[j]].push_back(a[j]);
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

int64_t calc_score(const vector<int> &weight) {
    int w_max = *max_element(weight.begin()+1, weight.end());
    int w_min = *min_element(weight.begin()+1, weight.end());
    int64_t score = ((int64_t)w_max * 10000 / w_min) - 10000;
    return score;
}

void count_group() {
    v_group_cnt.resize(K+1);
    for(int i=1; i<=N; i++) {
        v_group_cnt[v_group[i]]++;
    }
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
    for(int j=1; j<=M; j++) {
        int group_id = v_group[a[j]];
        if(group_id == v_group[b[j]]) {
            div_edges[group_id][a[j]].push_back(b[j]);
            div_edges[group_id][b[j]].push_back(a[j]);
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
        }
        if(start_v == -1) return false;
        dfs(seen, div_edges, start_v, k);
        for(int i=1; i<=N; i++) {
            if(v_group[i]==k && !seen[i]) {
                return false;
            }
        }
    }
    //output_ans();
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

void init_sol_bfs() {
    //vector<int>
}

void annealing_chain(const clock_t &start_time, const clock_t &now_time) {
    int change_v = rand()%N+1; // 変更するノード番号
    auto before_v_group = v_group;
    int before_group = v_group[change_v]; // 変更前のグループ
    if(v_group_cnt[before_group]==1) return;
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

    set<int> change_v2_candidate;
    for(int i=1; i<=N; i++) {
        if(v_group[i]==before_group){
            for(int l=0; l<edges[i].size(); l++) {
                if(v_group[edges[i][l]]!=before_group && v_group_cnt[v_group[edges[i][l]]]!=1) {
                    change_v2_candidate.insert(edges[i][l]);
                }
            }
        }
    }
    if(change_v2_candidate.size()==0) return;
    r = rand() % change_v2_candidate.size(); // not _really_ random
    auto v2 = *select_random(change_v2_candidate, r);
    int before_group_v2 = v_group[v2];

    // 連結しているかの判定
    v_group[change_v] = after_group; // グループの変更
    v_group[v2] = before_group;
    if(!isconnected()) { // 連結でないならば元に戻す
        v_group = before_v_group;
        return;
    }
    // 部分グラフの重みの再計算
    vector<int> change_weight = w_sum;
    change_weight[before_group] -= w[change_v];
    change_weight[after_group] += w[change_v];
    change_weight[before_group_v2] -= w[v2];
    change_weight[before_group] += w[v2];
    int64_t new_score = calc_score(change_weight);
    
    // 温度関数
    double temp = start_temp + (end_temp - start_temp) * (double)(now_time-start_time)/CLOCKS_PER_SEC / TIME_ANN;
    // 遷移確率関数
    double prob = exp((current_score-new_score)/temp);

    if(prob < (rand()%SHRT_MAX)/(double)SHRT_MAX) {
        v_group = before_v_group;
        return;
    } else {
        //debug(new_score);
        w_sum = change_weight;
        v_group_cnt[after_group]++;
        v_group_cnt[before_group_v2]--;
        current_score = new_score;
    }
}

void annealing(const clock_t &start_time, const clock_t &now_time) {
    int change_v;
    if(rand()%4!=0) {
        change_v = distance(w_sum.begin(), max_element(w_sum.begin(), w_sum.end()));
    } else {
        change_v = rand()%N+1; // 変更するノード番号
    }
    int before_group = v_group[change_v]; // 変更前のグループ
    if(v_group_cnt[before_group]==1) return;
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
    int64_t new_score = calc_score(change_weight);
    
    // 温度関数
    double temp = start_temp + (end_temp - start_temp) * (double)(now_time-start_time)/CLOCKS_PER_SEC / TIME_ANN;
    // 遷移確率関数
    double prob = exp((current_score-new_score)/temp);

    if(prob < (rand()%SHRT_MAX)/(double)SHRT_MAX) {
        v_group[change_v] = before_group;
        return;
    } else {
        w_sum = change_weight;
        v_group_cnt[after_group]++;
        v_group_cnt[before_group]--;
        current_score = new_score;
    }
}

void local_search() {
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
    int64_t after_score = calc_score(change_weight);
    if(after_score > current_score) { // スコアが悪化した場合
        v_group[change_v] = before_group;
        return;
    } else { // 現状維持または改善した場合は，変更を保存する
        w_sum = change_weight;
        current_score = after_score;
    }
}

void local_search2() {
    int change_v = rand()%N+1; // 変更するノード番号
    auto before_v_group = v_group;
    int before_group = v_group[change_v]; // 変更前のグループ
    if(v_group_cnt[before_group]==1) return;
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

    set<int> change_v2_candidate;
    for(int i=1; i<=N; i++) {
        if(v_group[i]==before_group){
            for(int l=0; l<edges[i].size(); l++) {
                if(v_group[edges[i][l]]!=before_group) {
                    change_v2_candidate.insert(edges[i][l]);
                }
            }
        }
    }
    if(change_v2_candidate.size()==0) return;
    r = rand() % change_v2_candidate.size(); // not _really_ random
    auto v2 = *select_random(change_v2_candidate, r);
    int before_group_v2 = v_group[v2];

    // 連結しているかの判定
    v_group[change_v] = after_group; // グループの変更
    v_group[v2] = before_group;
    if(!isconnected()) { // 連結でないならば元に戻す
        v_group = before_v_group;
        return;
    }
    // 部分グラフの重みの再計算
    vector<int> change_weight = w_sum;
    change_weight[before_group] -= w[change_v];
    change_weight[after_group] += w[change_v];
    change_weight[before_group_v2] -= w[v2];
    change_weight[before_group] += w[v2];
    int64_t new_score = calc_score(change_weight);

    if(new_score > current_score) {
        v_group = before_v_group;
        return;
    } else {
        w_sum = change_weight;
        v_group_cnt[after_group]++;
        v_group_cnt[before_group_v2]--;
        current_score = new_score;
    }

}

void output_v_group_cnt() {
    for(int i=1; i<=K; i++) {
        cout << i << " " << v_group_cnt[i] <<endl;
    }
}


int main() {
    input();
    init_sol();
    current_score = calc_score(w_sum);
    count_group();
    clock_t start_time = clock();
    clock_t now_time = clock();
    while((double)(now_time - start_time) / CLOCKS_PER_SEC < TIME_ANN) {
        if((double)(now_time - start_time) / CLOCKS_PER_SEC > 0.1 && rand()%3==0) {
            annealing_chain(start_time, now_time);
        } else {
            annealing(start_time, now_time);
        }
        now_time = clock();
    }
    while((double)(now_time - start_time) / CLOCKS_PER_SEC < TIME_ANN + TIME_LOC) {
        if(rand()%2==0) {
            local_search2();
        } else {
            local_search();
        }
        now_time = clock();
    }
    output_ans();
    output_score();
    output_v_group_cnt();
}