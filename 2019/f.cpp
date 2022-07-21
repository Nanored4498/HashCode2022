#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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

	for(int i = 0; i < T;) {
		if(c[ts[i].i] > ts[i].d) {
			if(i+1 < T) ts[i] = move(ts.back());
			ts.pop_back();
			-- T;
		} else ++i;
	}
	cerr << "new T: " << T << endl;

	int score = 0;
	vector<pii> ans;
	int s = 0;
	for(Target &t : ts) {
		++ s;
		int time = 0;
		for(int i : unordered_set(in[t.i].begin(), in[t.i].end())) {
			time += c[i];
			ans.emplace_back(i, s);
		}
		time += c[t.i];
		ans.emplace_back(t.i, s);
		score += t.g + t.d - time;
	}

	cerr << "Score: " << score << endl;
	cout << ans.size() << '\n';
	for(auto [i, s] : ans) cout << i2s[i] << ' ' << s << '\n';

	return 0;
}