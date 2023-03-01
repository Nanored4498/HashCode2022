#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>

using namespace std;
typedef vector<int> vi;
typedef pair<int, int> pii;

struct Target {
	int i, d, g;
	vi sc;
};

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	int C, T, S;
	cin >> C >> T >> S;
	unordered_map<string, int> s2i;
	vector<string> i2s(C);
	vi c(C), r(C);
	vector<vi> in(C), out(C);
	for(int i = 0; i < C; ++i) {
		int n;
		cin >> i2s[i] >> c[i] >> r[i] >> n;
		s2i[i2s[i]] = i;
		in[i].resize(n);
		for(int &j : in[i]) {
			string d; cin >> d;
			out[j = s2i[d]].push_back(i);
		}
	}
	vector<Target> ts(T);
	for(Target &t : ts) {
		string s;
		cin >> s >> t.d >> t.g;
		t.i = s2i[s];
	}

	vi servers0(S, 0);
	vector<vi> av0(S, vi(C, 1e9));
	vector<pii> ans;
	int score = 0;

	vi alli;
	int ssc = 0;
	bool partition_success = false;
	for(Target &t : ts) {
		vi count(C, 0), count2(C, 0), time(C, 0);
		vi st = {t.i};
		while(!st.empty()) {
			int i = st.back(); st.pop_back();
			for(int j : in[i]) {
				if(!count[j]) st.push_back(j);
				++ count[j];
			}
		}
		st.push_back(t.i);
		while(!st.empty()) {
			int i = st.back(); st.pop_back();
			time[i] += c[i];
			if(count[i] > 1) time[i] += min(r[i], (count[i]-1)*c[i]);
			for(int j : in[i]) {
				time[j] = max(time[j], time[i]);
				++ count2[j];
				if(count2[j] == count[j]) st.push_back(j);
			}
			t.sc.push_back(i);
		}
		sort(t.sc.begin(), t.sc.end(), [&](int i, int j) { return time[i] > time[j]; });
		int si = 0, sc = 0;
		unordered_map<int, int> mm;
		for(int i : t.sc) {
			sc += c[i];
			++mm[in[i].size()];
		}
		for(auto [i, x] : mm) cerr << "(" << i << ":" << x << ") "; cerr << endl;
		if(t.sc.size()==101 && !partition_success) {
			const int m = t.d-c[t.i]-1;
			int scs = -c[t.i];
			for(int i : t.sc) scs += c[i];
			cerr << t.sc.size() << ' ' << sc << ' ' << t.d << ' ' << t.g << ' ' << m << ' ' << S*m-scs << endl;
			for(int i : t.sc) cerr << c[i] << ' '; cerr << endl;
			vector<bool> used(100, false);
			vector<vi> cs;
			for(int i = 0; i < 100; ++i) if(!used[i]) {
				cs.emplace_back(1, t.sc[i]);
				used[i] = true;
				vector<int> dp(m-c[t.sc[i]]+1, -1);
				dp[0] = i;
				for(int j = 0; j < 100; ++j) if(!used[j]) {
					for(int x = m-c[t.sc[i]]; x >= c[t.sc[j]]; --x)
						if(dp[x] == -1 && dp[x - c[t.sc[j]]] != -1)
							dp[x] = j;
				}
				int x = m-c[t.sc[i]];
				while(dp[x] == -1) --x;
				while(x != 0) {
					used[dp[x]] = true;
					cs.back().push_back(t.sc[dp[x]]);
					x -= c[t.sc[dp[x]]];
				}
			}
			cerr << "HHHHHHHH: " << cs.size() << endl;
			if(cs.size() == S) {
				partition_success = true;
				for(int s = 0; s < S; ++s) {
					for(int i : cs[s]) servers0[s] += c[i];
					cerr << cs[s].size() << ' ' << servers0[s] << endl;
					for(int i : cs[s]) ans.emplace_back(i, s);
				}
				ans.emplace_back(t.i, 0);
				servers0[0] += c[t.i];
				score += t.g;
			}
		}
		alli.insert(alli.end(), t.sc.begin(), t.sc.end());
	}

	for(int i = 0; i < ts.size();)
		if(ts[i].sc.size() == 101) {
			if(i+1 < ts.size()) ts[i] = move(ts.back());
			ts.pop_back();
		} else ++i;

	while(!ts.empty()) {
		auto bestT = ts.end();
		double bestScore = -1;
		int bestGain = -1;
		vector<vi> bestAv;
		decltype(servers0) bestServers;
		vector<pii> bestAdd;
		for(auto itT = ts.begin(); itT != ts.end(); ++itT) {
			const Target &t = *itT;
			vector<vi> av = av0;
			auto servers = servers0;
			vector<pii> add;
			function<pair<int, vector<pii>>(int, int, int)> soon;
			soon = [&](int s, int i, int t0) {
				if(in[i].empty()) return make_pair(t0, vector<pii>());
				sort(in[i].begin(), in[i].end(), [&](int i, int j) { return av[s][i] > av[s][j]; });
				if(av[s][in[i][0]] <= t0) return make_pair(t0, vector<pii>());
				vector<pii> a;
				for(int j : in[i]) {
					auto [t1, a2] = soon(s, j, t0);
					t1 += c[j];
					if(t1 > av[s][j]) {
						t0 = max(t0, av[s][j]);
						break;
					}
					t0 = t1;
					a.insert(a.end(), a2.begin(), a2.end());
					a.emplace_back(j, s);
				}
				return make_pair(t0, a);
			};
			for(int i : t.sc) {
				int best_av = 1e9;
				for(auto &a : av) best_av = min(best_av, a[i]);
				int s = 0, t1 = 1e9;
				vector<pii> aa;
				for(int s2 = 0; s2 < S; ++s2) {
					auto [t0, aaa] = soon(s2, i, servers[s2]);
					if(t0 < t1) {
						t1 = t0;
						s = s2;
						aa = move(aaa);
					}
				}
				const int t2 = t1 + c[i];
				if(best_av <= t2) continue;
				av[s][i] = t2;
				const int t3 = t2 + r[i];
				for(auto &a : av) a[i] = min(a[i], t3);
				servers[s] = t2;
				add.insert(add.end(), aa.begin(), aa.end());
				add.emplace_back(i, s);
			}
			int lastT = 1e9;
			for(auto &a : av) lastT = min(lastT, a[t.i]);
			if(lastT > t.d) continue;
			double dt = 1e-3;
			for(int tt : servers)  dt += tt;
			for(int tt : servers0) dt -= tt;
			int gain = t.g + t.d - lastT;
			double sc = 1e6 * gain / dt + 1e5 - dt;
			if(sc > bestScore) {
				bestT = itT;
				bestScore = sc;
				bestGain = gain;
				bestAv = move(av);
				bestServers = move(servers);
				bestAdd = move(add);
			}
		}
		if(bestT == ts.end()) break;
		cerr << i2s[bestT->i] << ' ' << score << ' ' << bestT->g+bestT->d-bestGain << '/' << bestT->d << " + " << bestT->g << endl;
		if(bestT+1 != ts.end()) *bestT = move(ts.back());
		ts.pop_back();
		score += bestGain;
		av0 = move(bestAv);
		servers0 = move(bestServers);
		ans.insert(ans.end(), bestAdd.begin(), bestAdd.end());
	}

	cerr << "Remaining: " << ts.size() << endl;
	for(auto &t : ts) cerr << t.sc.size() << ' ' << t.d << ' ' << t.g << endl;
	cerr << "Score: " << score << endl;
	cout << ans.size() << '\n';
	for(auto [i, s] : ans) cout << i2s[i] << ' ' << s << '\n';

	return 0;
}