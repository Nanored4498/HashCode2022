#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <numeric>
#include <algorithm>
#include <random>

using namespace std;
const int NS = 36;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	int C, P;
	cin >> C >> P;
	vector<string> Cnames(C), Pnames(P);
	unordered_map<string, int> s2i;
	vector<array<int, NS>> skill(C);
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
	cerr << C << ' ' << P << ' ' << s2i.size() << endl;

	const auto fun = [&](int seed) {
		vector<array<int, NS>> skill0 = skill;
		vector<int> ts0 = ts;
		
		int score = 0;
		vector<pair<int, vector<int>>> sol;

		vector<int> av(C, 0);
		vector<int> order(P);
		vector<bool> chosen(C, false);
		iota(order.begin(), order.end(), 0);
		mt19937 mt(seed);
		const auto ckey = [&](int c, int m, int s, int l) { return make_tuple(max(av[c], m), skill[c][s], ts[c]); };
		while(true) {
			bool bad = true;
			vector<int> fail;
			shuffle(order.begin(), order.end(), mt);
			for(int p : order) {
				int mav=0;
				vector<int> cs(req[p].size(), 0), cs2, ro=cs;
				iota(ro.begin(), ro.end(), 0);
				array<int, NS> ms;
				for(int test = 0; test < 100; ++test) {
					shuffle(ro.begin(), ro.end(), mt);
					ms.fill(0);
					mav = 0;
					bool good = true;
					for(int o : ro) {
						auto [s, l] = req[p][o];
						int l0 = l;
						if(ms[s] >= l) --l;
						int best = -1;
						for(int c = 0; c < C; ++c) if(!chosen[c] && skill[c][s] >= l)
							if(best == -1 || ckey(c, mav, s, l0) < ckey(best, mav, s, l0))
								best = c;
						if(best == -1) {
							good = false;
							break;
						}
						cs[o] = best;
						for(int i = 0; i < s2i.size(); ++i) ms[i] = max(ms[i], skill[best][i]);
						chosen[best] = true;
						mav = max(mav, av[best]);
					}
					for(int c : cs) chosen[c] = false;
					int end = mav+D[p];
					if(!good || end >= B[p]+S[p]) continue;
					cs2 = cs;
					break;
				}
				int end = mav+D[p];
				if(cs2.empty() || end >= B[p]+S[p]) {
					fail.push_back(p);
					continue;
				}
				for(int i = 1; i < cs.size(); ++i) if(skill[cs[i]][req[p][i].first] > req[p][i].second)
					for(int j = 0; j < i; ++j) if(skill[cs[j]][req[p][j].first] > req[p][j].second)
						if(skill[cs[i]][req[p][j].first] >= req[p][j].second-1 && skill[cs[j]][req[p][i].first] >= req[p][i].second-1)
							swap(cs[i], cs[j]);
				cs = cs2;
				score += S[p] - max(0, end-B[p]);
				for(int i = 0; i < cs.size(); ++i) {
					av[cs[i]] = end;
					auto [s, l] = req[p][i];
					if(skill[cs[i]][s] <= l) {
						++ skill[cs[i]][s];
						++ ts[cs[i]];
					}
				}
				sol.emplace_back(p, cs);
				bad = false;
			}
			if(bad) break;
			order = fail;
		}

		skill = move(skill0);
		ts = move(ts0);
		return make_pair(score, sol);
	};
	
	int score = -1;
	vector<pair<int, vector<int>>> sol;
	int T = 10000;
	while(T--) {
		auto [sc, so] = fun(T);
		if(sc > score) {
			score = sc;
			sol = move(so);
			cerr << score << ' ' << T << endl;
		}
	}
	cout << sol.size() << endl;
	for(const auto &[p, cs] : sol) {
		cout << Pnames[p] << '\n';
		for(int c : cs) cout << Cnames[c] << ' ';
		cout << '\n';
	}

	return 0;
}