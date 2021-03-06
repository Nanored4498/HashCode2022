#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <numeric>
#include <algorithm>
#include <random>

using namespace std;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	int C, P;
	cin >> C >> P;
	vector<string> Cnames(C), Pnames(P);
	unordered_map<string, int> s2i;
	vector<array<int, 800>> skill(C);
	vector<int> ts(C, 0);
	vector<vector<pair<int, int>>> req(P);
	vector<int> D(P), S(P), B(P);

	for(int c = 0; c < C; ++c) {
		int n;
		cin >> Cnames[c] >> n;
		skill[c].fill(0);
		while(n--) {
			string s; int l;
			cin >> s >> l;
			if(!s2i.count(s)) s2i[s] = s2i.size();
			skill[c][s2i[s]] = l;
			ts[c] += l;
		}
	}
	for(int p = 0; p < P; ++p) {
		int r;
		cin >> Pnames[p] >> D[p] >> S[p] >> B[p] >> r;
		while(r--) {
			string s; int l;
			cin >> s >> l;
			if(!s2i.count(s)) s2i[s] = s2i.size();
			req[p].emplace_back(s2i[s], l);
		}
	}

	int score = 0;
	vector<pair<int, vector<int>>> sol;

	vector<int> av(C, 0);
	vector<int> order(P);
	vector<bool> chosen(C, false);
	iota(order.begin(), order.end(), 0);
	mt19937 mt;
	const auto ckey = [&](int c, int m, int s) { return make_tuple(max(av[c], m), skill[c][s], ts[c]); };
	bool need_lvl = false;
	while(true) {
		bool bad = true;
		vector<int> fail;
		if(need_lvl) sort(order.begin(), order.end(), [&](int a, int b) { return S[a]+B[a]-D[a] > S[b]+B[b]-D[b]; });
		else sort(order.begin(), order.end(), [&](int a, int b) { return B[a]-D[a] < B[b]-D[b]; });
		for(int p : order) {
			int mav = 0;
			vector<int> cs;
			array<int, 800> ms; ms.fill(0);
			for(auto [s, l] : req[p]) {
				if(ms[s] >= l) --l;
				int best = -1;
				for(int c = 0; c < C; ++c) if(!chosen[c] && skill[c][s] >= l)
					if(best == -1 || ckey(c, mav, s) < ckey(best, mav, s))
						best = c;
				if(best == -1) break;
				cs.push_back(best);
				for(int i = 0; i < s2i.size(); ++i) ms[i] = max(ms[i], skill[best][i]);
				chosen[best] = true;
				mav = max(mav, av[best]);
			}
			for(int c : cs) chosen[c] = false;
			int end = mav+D[p];
			if(cs.size() < req[p].size() || (!need_lvl && end >= B[p]+S[p])) {
				fail.push_back(p);
				continue;
			}
			for(int i = 1; i < cs.size(); ++i) if(skill[cs[i]][req[p][i].first] > req[p][i].second)
				for(int j = 0; j < i; ++j) if(skill[cs[j]][req[p][j].first] > req[p][j].second)
					if(skill[cs[i]][req[p][j].first] >= req[p][j].second-1 && skill[cs[j]][req[p][i].first] >= req[p][i].second-1)
						swap(cs[i], cs[j]);
			bool impLvl = false;
			for(int i = 0; i < cs.size(); ++i) {
				av[cs[i]] = end;
				auto [s, l] = req[p][i];
				if(skill[cs[i]][s] <= l) {
					++ skill[cs[i]][s];
					++ ts[cs[i]];
					impLvl = true;
				}
			}
			if(need_lvl && !impLvl && end >= B[p]+S[p]) {
				fail.push_back(p);
				continue;
			}
			score += max(0, S[p] - max(0, end-B[p]));
			sol.emplace_back(p, cs);
			bad = false;
		}
		if(bad) {
			if(need_lvl) break;
			need_lvl = true;
		}
		order = fail;
	}

	cerr << score << endl;
	cout << sol.size() << endl;
	for(const auto &[p, cs] : sol) {
		cout << Pnames[p] << '\n';
		for(int c : cs) cout << Cnames[c] << ' ';
		cout << '\n';
	}

	return 0;
}