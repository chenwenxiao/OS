#include <cstdio>
#include <cstring>
#include <algorithm>
#include <iostream>
using namespace std;

int P = 31;
int Q = 63;
int L = 127;

int page[1000][100];

int main() {
	int address;
	freopen("input.txt", "r", stdin);
	freopen("output.txt", "w", stdout);

	for (int i = 0; i < 128; ++i) {
		string str;
		cin >> str;
		cin >> str;
		for (int j = 0; j < 32; ++j)
			cin >> hex >> page[i][j];
	}

	cin >> hex >> address;
	
	int pde = 17, pte, psy;
	int pde_index = (address >> 10) & P;
	int pte_index = (address >> 5 ) & P;
	int psy_index = address & Q;
	pte = page[pde][pde_index];
	cout << hex << "pde index: " << pde_index << " pde contents: " << pte << " valid:" << (pte >> 7) << " pfn:" << (pte & L) << endl;
	if ((pte >> 7) == 0) {
		cout << "Fault (page directory entry not valid)" << endl;
		return ;
	}
	pte = pte & L;
	psy = page[pte][pte_index];
	cout << hex << "pte index: " << pte_index << " contents: " << psy << " valid:" << (psy >> 7) << " pfn:" << (psy & L) << endl;
	if ((psy >> 7) == 0) {
		cout << "Fault (page table entry not valid)" << endl;
		return ;
	}
	psy = psy & L;
	cout << hex << "translate : " << (psy << 5) + psy_index << " contents: " << page[psy][psy_index] << endl;
	

	return 0;
}
