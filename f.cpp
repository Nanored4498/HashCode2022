#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <numeric>
#include <algorithm>
#include <random>

using namespace std;
const int NS = 500;

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
	array<vector<int>, NS> s2c;
	vector<vector<int>> c2s(C);

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
			s2c[s2i[s]].push_back(c);
			c2s[c].push_back(s2i[s]);
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

	int score = 0;
	vector<pair<int, vector<int>>> sol;

	vector<int> ps(P);
	vector<bool> chosen(C, false);
	vector<int> av(C, 0);
	iota(ps.begin(), ps.end(), 0);
	mt19937 mt(42);
	const auto ckey = [&](int c, int m, int s, int l) { return make_tuple(max(av[c], m), -min(av[c], m), skill[c][s], ts[c]); };
	while(true) {
		int bestP = -1, bestEnd = -1;
		double bestScore = -1.;
		vector<int> bestCS;
		for(int p : ps) {
			double bestScoreP = -10.;
			int bestEndP = -1;
			const int MAV = S[p]+B[p]-D[p];
			vector<int> cs(req[p].size(), 0), cs2;
			vector<int> ro(req[p].size());
			array<int, NS> ms;
			iota(ro.begin(), ro.end(), 0);
			for(int test = 0; test < 3; ++test) {
				shuffle(ro.begin(), ro.end(), mt);
				ms.fill(0);
				int sav = 0, mav = 0, nr = req[p].size();
				for(int o : ro) {
					auto [s, l] = req[p][o];
					const int l0 = l;
					if(ms[s] >= l) --l;
					int best = -1;
					if(l) for(int c : s2c[s]) if(!chosen[c] && skill[c][s] >= l && av[c] < MAV)
						if(best == -1 || ckey(c, mav, s, l0) < ckey(best, mav, s, l0))
							best = c;
					if(!l) for(int c = 0; c < C; ++c) if(!chosen[c] && av[c] < MAV)
						if(best == -1 || ckey(c, mav, s, l0) < ckey(best, mav, s, l0))
							best = c;
					if(best == -1) break;
					--nr;
					cs[o] = best;
					for(int s : c2s[best]) ms[s] = max(ms[s], skill[best][s]);
					chosen[best] = true;
					sav += av[best];
					mav = max(mav, av[best]);
				}
				for(int c : cs) chosen[c] = false;
				if(nr) continue;
				int end = mav+D[p];
				double time = cs.size()*end - sav;
				double score = double(S[p] - max(0, end-B[p])) / time;
				if(score > bestScoreP) {
					cs2 = cs;
					bestScoreP = score;
					bestEndP = end;
				}
			}
			if(bestScoreP > bestScore) {
				bestScore = bestScoreP;
				bestEnd = bestEndP;
				bestCS = move(cs2);
				bestP = p;
			}
		}
		if(bestP == -1) break;
		score += max(0, S[bestP] - max(0, bestEnd-B[bestP]));
		cerr << score << endl;
		for(int i = 1; i < bestCS.size(); ++i) if(skill[bestCS[i]][req[bestP][i].first] > req[bestP][i].second)
			for(int j = 0; j < i; ++j) if(skill[bestCS[j]][req[bestP][j].first] > req[bestP][j].second)
				if(skill[bestCS[i]][req[bestP][j].first] >= req[bestP][j].second-1 && skill[bestCS[j]][req[bestP][i].first] >= req[bestP][i].second-1)
					swap(bestCS[i], bestCS[j]);
		for(int i = 0; i < bestCS.size(); ++i) {
			av[bestCS[i]] = bestEnd;
			auto [s, l] = req[bestP][i];
			if(skill[bestCS[i]][s] <= l) {
				if(!skill[bestCS[i]][s]) {
					s2c[s].push_back(bestCS[i]);
					c2s[bestCS[i]].push_back(s);
				}
				++ skill[bestCS[i]][s];
				++ ts[bestCS[i]];
			}
		}
		sol.emplace_back(bestP, bestCS);
		int i = 0;
		while(ps[i] != bestP) ++ i;
		ps[i] = ps.back();
		ps.pop_back();
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