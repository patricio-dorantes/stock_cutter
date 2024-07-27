#include "stdafx.h"
#include "gen_pttn.h"

#include <queue>
#include <iostream>
#include <ctime>

using namespace std;

gen_pttn::gen_pttn(void) {
	M = 0;
	L = 0;
	MAX_MEMORY = 2048000;
}

gen_pttn::gen_pttn(const gen_pttn& inst) {
	Sb = inst.Sb;
	S = inst.S;
	Lv = inst.Lv;
	M = inst.M;
	MAX_MEMORY = 2048000;
}

gen_pttn::~gen_pttn(void) {}

struct Compare {
	bool operator()(const pttn& lhs, const pttn& rhs) {
		return lhs.c < rhs.c;
	}
};

void gen_pttn::run(void) {
	int Lmin = Lv[M - 1]; // Assume that the minimum L_j value is the last, i.e. L_j is sorted in descending order
	// Initialize sets
	pttn P0;
	P0.a.assign(M, 0);
	P0.c = L;
	Sb.assign(1, P0);
	S.clear();

	// Priority queue to keep most promising patterns
	std::priority_queue<pttn, std::vector<pttn>, Compare> pq;
	pq.push(P0);

	// Initialize timing
	clock_t start = clock();
	int total_patterns = 0;

	// Iterate through j values
	int j, x, Sbmax, ajmax, v;
	pttn P;

	for (j = 0; j < M; ++j) {
		std::vector<pttn> newSb;
		while (!pq.empty()) {
			P = pq.top();
			pq.pop();
			ajmax = P.c / Lv[j];

			// Prune non-promising patterns
			if (ajmax == 0) continue;

			// Construct non-maximal patterns and add to newSb
			if (j != M - 1 && ajmax > 1) {
				for (v = 1; v < ajmax; ++v) {
					pttn newP = P;
					newP.a[j] = v;
					newP.c -= v * Lv[j];
					newSb.push_back(newP);
				}
			}

			// Construct final new pattern and then classify it accordingly
			if (ajmax > 0) {
				if (j != M - 1) {
					P.a[j] = ajmax;
					P.c -= ajmax * Lv[j];
					if (P.c < Lmin) {
						S.push_back(P);
					} else {
						newSb.push_back(P);
					}
				} else {
					P.a[j] = ajmax;
					P.c -= ajmax * Lv[j];
					S.push_back(P);
					total_patterns = S.size();
				}
			}
		}

		// Limit the number of patterns in Sb
		std::sort(newSb.begin(), newSb.end(), [](const pttn& lhs, const pttn& rhs) {
			return lhs.c > rhs.c; // Keep the ones with more capacity
		});

		if (newSb.size() > MAX_MEMORY) {
			newSb.resize(MAX_MEMORY);
		}

		// Update priority queue with new patterns
		for (const auto& p : newSb) {
			pq.push(p);
		}
		// Log information
		clock_t current = clock();
		double elapsed = double(current - start) / CLOCKS_PER_SEC;
		cout << endl;
		cout << "Cycle " << j + 1 << "/" << M << " completed. Time elapsed: " << elapsed << " seconds." << endl;
		cout << "Sb Patterns generated in this cycle: " << newSb.size() << endl;
		cout << "S Patterns generated in this cycle: " << S.size() << endl;
		cout << "Total patterns generated so far: " << total_patterns << endl;

		cout << "Memory used so far: " << ((newSb.size() + S.size()) * (sizeof(pttn) + sizeof(int) * M)/1024/1024) << " MiB" << endl;
	}
}