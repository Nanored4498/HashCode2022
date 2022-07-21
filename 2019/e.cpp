#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;
typedef vector<int> vi;

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
			j = s2i[d];
		}
	}
	vector<Target> ts(T);
	for(Target &t : ts) {
		string s;
		cin >> s >> t.d >> t.g;
		t.i = s2i[s];
	}
	cerr << "C: " << C << "  T: " << T << "  S: " << S << endl;

	auto t = ts.back();
	auto &v = in[t.i];
	int sc = 0; for(int i : v) sc += c[i];
	cerr << t.g << ' ' << v.size() << ' ' << sc << ' ' << c[t.i] << endl;

	vector<vi> as;
	const int m = t.d-c[t.i]-1;
	for(int s = 1; s < S; ++s) {
		vi dp(m+1, 0), sol(m+1, -1);
		for(int i = 0; i < v.size(); ++i) {
			int ci = c[v[i]];
			for(int j = m; j >= ci; --j) if(dp[j] < ci+dp[j-ci]) {
				dp[j] = ci+dp[j-ci];
				sol[j] = i;
			}
		}
		cerr << dp[m] << ' ' << m << endl;
		vector<bool> part(v.size(), false);
		vi tmp, a;
		int ss = m;
		while(sol[ss] >= 0) {
			part[sol[ss]] = true;
			ss -= c[v[sol[ss]]];
		}
		for(int i = 0; i < v.size(); ++i)
			if(part[i]) a.push_back(v[i]);
			else tmp.push_back(v[i]);
		v = move(tmp);
		as.emplace_back(move(a));
	}

	sc = 0; for(int i : v) sc += c[i];
	cerr << sc << ' ' << sc+1+c[t.i] << ' ' << t.d << endl;
	as.emplace_back(move(v));
	as.back().push_back(t.i);
	int E = 0;
	for(const auto &a : as) E += a.size();

	cout << E << endl;
	for(int s = 0; s < S; ++s)
		for(int i : as[s])
			cout << i2s[i] << ' ' << s << '\n';
	cerr << "SCORE: " << t.g << endl;

	return 0;
}