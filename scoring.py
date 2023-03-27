import sys
import math
import networkx as nx

def error_print(msg):
    RED = '\033[31m'
    END = '\033[0m'
    print(RED+'Error: '+msg+END, file=sys.stderr)
    sys.exit(1)

prob_num = int(input('都道府県idを入力:'))

if prob_num < 0 or prob_num > 47:
    error_print('不正なidです')

DATA_PATH = './data/'+str(prob_num)+'/in.dat'

edge = []

with open(DATA_PATH) as f:
    cnt = 0
    for s_line in f:
        l = [int(x) for x in s_line.replace('\n', '').split()]
        if cnt==0:
            N,M,K = l[0],l[1],l[2]
        elif cnt==1:
            w = l
        else:
            edge.append(l)
        cnt += 1

print('部分グラフの頂点集合を入力')
l = list(map(int, input().split()))

if len(l) != N:
    error_print('頂点数が正しくありません')

v_set = []
w_sum = [0] * (K+1)

for i in range(K+1):
    v_set.append(set())

for i in range(len(l)):
    v_set[l[i]].add(i+1)
    w_sum[l[i]] += w[i]

for i in range(1,K+1):
    if len(v_set[i]) == 0:
        error_print(f'{i}番目の部分グラフの頂点集合が空です')
    elif len(v_set[i]) == 1:
        continue
    else:
        # 部分グラフを作成
        g_dash = nx.Graph()
        for e in edge:
            if e[0] in v_set[i] and e[1] in v_set[i]:
                g_dash.add_edge(e[0], e[1])
        # 連結かを判定
        if nx.number_of_nodes(g_dash) == 0 or nx.is_connected(g_dash) == False:
            error_print(f'{i}番目の部分グラフが非連結です')

# スコア計算
w_max = max(w_sum[1:])
w_min = min(w_sum[1:])

score = math.floor(w_max*10000/w_min)-10000

print('---------------')
print('W_max:', w_max)
print('W_min:', w_min)
print('score:', score)
print('---------------')
for i in range(1, K+1):
    print(f"{i}: {w_sum[i]}")
