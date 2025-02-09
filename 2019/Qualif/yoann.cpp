#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>

using namespace std;

using ll = long long;
using vi = vector<int>;

struct UF {
	vector<int> r;
	UF(int n): r(n, -1) {}
	int find(int i) {
		if(r[i] < 0) return i;
		return r[i] = find(r[i]);
	}
	void merge(int i, int j) {
		i = find(i); j = find(j);
		if(i == j) return;
		if(r[i] > r[j]) swap(i, j);
		r[i] += r[j];
		r[j] = i;
	}
	bool same(int i, int j) { return find(i) == find(j); }
};

void add(int a, int b, UF &U, vector<vi> &res) {
	U.merge(a, b);
	res[a].push_back(b);
	res[b].push_back(a);
}

int main() {
	int N;
	cin >> N;
	constexpr int maxW = 15;
	constexpr int minW = 2;
	vector<pair<int, int>> E[maxW+1];
	vector<vi> tags(N);
	unordered_map<string, int> t2i;
	vector<vi> t2p;
	vi com(N, 0);
	for(int i = 0; i < N; i++) {
		char c;
		int n;
		cin >> c >> n;
		tags[i].resize(n);
		for(int &t : tags[i]) {
			string s;
			cin >> s;
			t = t2i.emplace(s, t2i.size()).first->second;
		}
		t2p.resize(t2i.size());
		for(const int t : tags[i]) {
			for(const int j : t2p[t]) ++ com[j];
			t2p[t].push_back(i);
		}
		for(int j = 0; j < i; ++j) if(com[j]) {
			const int w = min(com[j], min((int) tags[i].size(), (int) tags[j].size()) - com[j]);
			com[j] = 0;
			if(w > maxW) return 1;
			if(w < minW) continue;
			E[w].emplace_back(i, j);
		}
	}
	ll nE = 0;
	for(const auto &es : E) nE += es.size();
	cerr << "Graph built " << nE << ' ' << t2i.size() << "\n";
	int score = 0;
	UF U(N);
	vector<vi> res(N);
	for(int w = maxW; w >= minW; --w) {
		for(const auto &[a, b] : E[w]) {
			if(res[a].size() < 2 && res[b].size() < 2 && !U.same(a, b)) {
				add(a, b, U, res);
				score += w;
			}
		}
		E[w].clear();
	}

	int N2 = 0;
	vi ni(N, -1), oi;
	for(vi &v : t2p) v.clear();
	for(int i = 0; i < N; ++i) if(res[i].size() != 2) {
		ni[i] = N2++;
		oi.push_back(i);
	}
	cerr << N2 << endl;
	for(int i = 0; i < N2; ++i) {
		for(const int t : tags[oi[i]]) {
			for(const int j : t2p[t]) ++ com[j];
			t2p[t].push_back(i);
		}
		for(int j = 0; j < i; ++j) if(com[j]) {
			const int w = min(com[j], min((int) tags[oi[i]].size(), (int) tags[oi[j]].size()) - com[j]);
			com[j] = 0;
			if(w != 1) continue;;
			E[1].emplace_back(oi[i], oi[j]);
		}
	}
	for(const auto &[a, b] : E[1]) {
		if(res[a].size() < 2 && res[b].size() < 2 && !U.same(a, b)) {
			add(a, b, U, res);
			++ score;
		}
	}
	cerr << "Graph built " << E[1].size() << endl;

	vi deg1;
	int z = 0;
	for(int i = 0; i < N; ++i) {
		if(res[i].size() == 1) deg1.push_back(i);
		else if(res[i].empty()) ++z;
	}
	while(deg1.size() > 2) {
		const int k = deg1.back();
		deg1.pop_back();
		const int i = U.same(deg1[0], k) ? 1 : 0;
		add(deg1[i], k, U, res);
		deg1[i] = deg1.back();
		deg1.pop_back();
	}
	cerr << score << ' ' << N-z << endl;
	int j = deg1[0];
	cout << N-z << "\n" << j << "\n";
	int i = res[j][0];
	while(res[i].size() == 2) {
		cout << i << "\n";
		if(res[i][0] == j) {
			j = i;
			i = res[j][1];
		} else {
			j = i;
			i = res[j][0];
		}
	}
	cout << i << "\n";
	return 0;
}