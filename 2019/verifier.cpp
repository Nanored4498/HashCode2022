#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>

using namespace std;

using ll = int64_t;
using str = string; // yay python! 

using vi = vector<int>;
using vvi = vector<vector<int>>;
using vb = vector<uint8_t>;

#define FORm(i, m, n) for(int i = m; i < (int) n; i++)
#define FOR(i, n) FORm(i, 0, n)
#define rep(a) FOR(_,a)
#define each(a, x) for (auto& a: x)
#define ceach(a, x) for (const auto& a: x)

#define readi(n) int n; std::cin >> n;
#define reads(s) std::string s; std::cin >> s;

template <typename T>
void prln_r(T t) { cerr << t << "\n"; }
template <typename T, typename ...U>
void prln_r(T t, U ...u) { cerr << t; prln_r(u...); }

template <typename T>
void read(T& t) { cin >> t; }
template <typename T, typename ...U>
void read(T& t, U& ...u) { cin >> t; read(u...); }

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	int C, T, S; read(C, T, S);
	unordered_map<str, int> s2i;
	vi tc(C), rc(C);
	vvi dc(C);
	FOR(i, C) {
		reads(name);
		s2i[name] = s2i.size();
		read(tc[i], rc[i]);
		readi(n);
		if (n > 0) {
			dc[i].resize(n);
			each(d, dc[i]) {
				reads(named);
				d = s2i[named];
			}
		}
	}
	vi ddlc(C, 0), gc(C, 0);
	FOR(t, T) {
		reads(name);
		read(ddlc[s2i[name]], gc[s2i[name]]);
	}

	vvi time_ava_c(S, vi(C, (int)1e9));
	vector<queue<int>> waitings(S);
	readi(comp_steps);
	rep(comp_steps) {
		reads(cname);
		int c = s2i[cname];
		readi(sid);
		waitings[sid].push(c);
	}
	
	vb done(C, false);
	vi t_occ(S, 0);
	ll sum_score = 0;
	FOR(step, (int)2e6) {
		FOR(sid, S) if(!waitings[sid].empty() && t_occ[sid] <= step) {
			int c = waitings[sid].front();
			bool flag = true;
			ceach(nc, dc[c]){
				if (time_ava_c[sid][nc] > step) {
					flag = false;
					break;
				}
			}if (flag) {
				waitings[sid].pop();
				time_ava_c[sid][c] = min(time_ava_c[sid][c], step + tc[c]);
				FOR(others, S) if (others != sid) time_ava_c[others][c] = min(time_ava_c[others][c], step + tc[c] + rc[c]);
				if (gc[c] > 0 && !done[c] && (step + tc[c]) <= ddlc[c]) {
					done[c] = true;
					sum_score += gc[c] + ddlc[c] - (step + tc[c]);
				}
				t_occ[sid] = step + tc[c];
			}
		}
	}
	prln_r(sum_score);
	return 0;
}
