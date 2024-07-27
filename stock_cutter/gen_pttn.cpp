#include "stdafx.h"
#include "gen_pttn.h"

#include <queue>
#include <iostream>
#include <ctime>
#include <unordered_set>
#include <vector>

using namespace std;

gen_pttn::gen_pttn(void) {
    M = 0;
    L = 0;
    MAX_SB = 4096;
}

gen_pttn::gen_pttn(const gen_pttn& inst) {
    Sb = inst.Sb;
    S = inst.S;
    Lv = inst.Lv;
    M = inst.M;
    MAX_SB = 4096;
}

gen_pttn::~gen_pttn(void) {}

struct Compare {
    bool operator()(const pttn& lhs, const pttn& rhs) {
        return lhs.c < rhs.c;
    }
};

struct PttnHash {
    size_t operator()(const pttn& p) const {
        size_t hash = 0;
        for (int i : p.a) {
            hash ^= std::hash<int>{}(i) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        hash ^= std::hash<int>{}(p.c) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        return hash;
    }
};

struct PttnEqual {
    bool operator()(const pttn& lhs, const pttn& rhs) const {
        return lhs.a == rhs.a && lhs.c == rhs.c;
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

    // Set to keep unique patterns
    std::unordered_set<pttn, PttnHash, PttnEqual> uniquePatterns;
    uniquePatterns.insert(P0);

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
                    if (uniquePatterns.find(newP) == uniquePatterns.end()) {
                        newSb.push_back(newP);
                        uniquePatterns.insert(newP);
                        total_patterns++;
                    }
                }
            }

            // Construct final new pattern and then classify it accordingly
            if (ajmax > 0) {
                if (j != M - 1) {
                    P.a[j] = ajmax;
                    P.c -= ajmax * Lv[j];
                    if (uniquePatterns.find(P) == uniquePatterns.end()) {
                        if (P.c < Lmin) {
                            S.push_back(P);
                        } else {
                            newSb.push_back(P);
                        }
                        uniquePatterns.insert(P);
                    }
                } else {
                    P.a[j] = ajmax;
                    P.c -= ajmax * Lv[j];
                    if (uniquePatterns.find(P) == uniquePatterns.end()) {
                        S.push_back(P);
                        uniquePatterns.insert(P);
                    }
                }
                total_patterns++;
            }
        }

        // Limit the number of patterns in Sb
        std::sort(newSb.begin(), newSb.end(), [](const pttn& lhs, const pttn& rhs) {
            return lhs.c > rhs.c; // Keep the ones with more capacity
        });

        if (newSb.size() > MAX_SB) {
            newSb.resize(MAX_SB);
        }

        // Update priority queue with new patterns
        for (const auto& p : newSb) {
            pq.push(p);
        }

        // Log information
        cout <<endl;
        clock_t current = clock();
        double elapsed = double(current - start) / CLOCKS_PER_SEC;
        cout << "Cycle " << j + 1 << " completed. Time elapsed: " << elapsed << " seconds." << endl;
        cout << "Patterns generated in this cycle: " << newSb.size() << endl;
        cout << "Total patterns generated so far: " << total_patterns << endl;

    }
}
