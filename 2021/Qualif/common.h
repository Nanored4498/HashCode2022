#include <iostream>
#include <vector>
#include <unordered_map>
#include <numeric>
#include <queue>
#include <cmath>
#include <algorithm>
#include <random>

using namespace std;
using vi = vector<int>;
using pii = pair<int, int>;
using vii = vector<pii>;

bool use_dbg = true;
template<typename A, typename B>
ostream& operator<<(ostream &out, const pair<A, B> &p) {
	return out << '(' << p.first << ", " << p.second << ')';
}
template<typename T>
ostream& operator<<(ostream &out, const vector<T> &v) {
	out << '[';
	for(int i = 0; i < (int)v.size(); ++i) {
		if(i) out << ", ";
		out << v[i];
	}
	return out << ']';
}
void dbg() {
	if(use_dbg) cerr << endl;
}
template<typename T, typename... Ts>
void dbg(const T &x, Ts... vals) {
	if(use_dbg) {
		cerr << x << ' ';
		dbg(vals...);
	};
}

struct Street {
	int a, b, l;
	string name;
};