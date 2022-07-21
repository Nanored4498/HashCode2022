#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>

using namespace std;
typedef vector<int> vi;
typedef pair<int, int> pii;

struct Target {
	int i, d, g, f, ff, w, sc;
	vi av;
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
		t.f = in[in[t.i][0]][0];
		t.ff = in[t.f][0];
		t.w = c[t.ff]+c[t.f]+r[t.f];
	}
	cerr << endl;
	cerr << "C: " << C << "  T: " << T << "  S: " << S << endl;

	int score = 0;
	vector<pii> ans;
	vi server(S, 0);
	const int N = 3, M = 14, P = 12, Q = T-N-1, R = 7;
	const auto add = [&](Target &t, int sv, int i) {
		ans.emplace_back(i, sv);
		server[sv] += c[i];
		for(int s = 0; s < S; ++s) if(s != sv)
			t.av[s] = max(t.av[s], server[sv]+r[i]);
	};

	for(Target &t : ts) {
		t.av.assign(S, 0);
		sort(in[t.i].begin(), in[t.i].end(), [&](int i, int j) { return c[i] > c[j]; });
		vi tmp = in[t.i];
		for(int i = 0; i < R; i += 2) sort(in[t.i].begin()+i*M, in[t.i].begin()+(i*M+M), [&](int i, int j) { return c[i] < c[j]; });
		for(int i = 0; i < R; ++i) for(int j = 0; j < M; ++j) tmp[i+j*R] = in[t.i][j+i*M];
		reverse(tmp.begin(), tmp.end());
		in[t.i] = move(tmp);
	}
	
	for(int i = 0; i < N; ++i) {
		for(int s = 0; s < M; ++s) {
			const int sv = i*M+s;
			add(ts[i], sv, ts[i].ff);
			add(ts[i], sv, ts[i].f);
			for(int j = 0; j < R; ++j) {
				add(ts[i], sv, in[ts[i].i].back());
				in[ts[i].i].pop_back();
			}
			cerr << server[sv] << '/' << server[sv]-ts[i].w << ' ';
		}
	}
	
	for(int s = 0; s < P; ++s) {
		const int sv = N*M+s;
		add(ts[N], sv, ts[N].ff);
		add(ts[N], sv, ts[N].f);
		for(int j = 0; j < R; ++j) {
			add(ts[N], sv, in[ts[N].i].back());
			in[ts[N].i].pop_back();
		}
		cerr << server[sv] << '/' << server[sv]-ts[N].w << ' ';
	}
	
	for(int i = N+1; i < T; ++i) {
		const int sv = N*M+P+i-N-1;
		add(ts[i], sv, ts[i].ff);
		add(ts[i], sv, ts[i].f);
		for(int j = 0; j < R; ++j) {
			add(ts[i], sv, in[ts[i].i].back());
			in[ts[i].i].pop_back();
		}
		cerr << server[sv] << '/' << server[sv]-ts[i].w << ' ';
	}
	cerr << endl;
	
	for(Target &t : ts) {
		t.sc = c[t.i];
		for(int i : in[t.i]) t.sc += c[i];
	}
	sort(ts.begin(), ts.end(), [&](const Target &a, const Target &b) { return a.sc < b.sc; });

	for(Target &t : ts) {
		if(!score) cerr << in[t.i].size() << ' ' << t.w << endl;
		sort(in[t.i].begin(), in[t.i].end(), [&](int i, int j) { return c[i]+r[i] > c[j]+r[j]; });
		for(int i : in[t.i]) {
			int si = 0, best = 1e9;
			for(int s = 0; s < S; ++s) {
				int ti = server[s] < t.w ? t.w-server[s] : server[s];
				if(ti < best) {
					si = s;
					best = ti;
				}
			}
			server[si] = max(server[si], t.w);
			add(t, si, i);
		}
		int si = 0, best = 1e9;
		for(int s = 0; s < S; ++s) {
			int ti = max(server[s], t.av[s]);
			if(ti < best) {
				si = s;
				best = ti;
			}
		}
		server[si] = max({server[si], t.w, t.av[si]});
		add(t, si, t.i);
		score += t.g+t.d-server[si];
	}

	cerr << "Score: " << score << endl;
	cout << ans.size() << '\n';
	for(auto [i, s] : ans) cout << i2s[i] << ' ' << s << '\n';

	return 0;
}