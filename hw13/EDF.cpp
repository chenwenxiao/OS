#include <cstdio>
#include <cstring>
#include <algorithm> 
using namespace std;

const int INF = 1 << 30;
const int MAXN = 100;

int n;
int start[MAXN], work[MAXN], end[MAXN];

int main() {
	freopen("input.txt", "r", stdin);
	freopen("output.txt", "w", stdout); 
	while (scanf("%d %d %d", &start[n], &work[n], &end[n]) != EOF)
		++n;
	
	for (int time = 0; true; ) {
		int minA = INF, chooseA, minB = INF, chooseB;
		for (int i = 0; i < n; ++i)
			if (end[i] < minA && work[i] > 0 && start[i] <= time) {
				minA = end[i];
				chooseA = i;
			}
		for (int i = 0; i < n; ++i)
			if (start[i] < minB && start[i] > time) {
				minB = start[i];
				chooseB = i;
			}
		if (minA == INF) {
			if (minB == INF) {
				printf("Done\n");
				break;
			} else {
				time = minB;
			}
		} else if (work[chooseA] + time < minB) {
			if (time + work[chooseA] - 1 > end[chooseA]) {
				printf("Schedule failed\n");
				exit(0);
			}
			printf("Do proc %d from time %d to %d\n", chooseA, time, time + work[chooseA] - 1);
			time = time + work[chooseA];
			work[chooseA] = 0;
		} else {
			printf("Do proc %d from time %d to %d\n", chooseA, time, start[chooseB] - 1);
			work[chooseA] -= minB - time;
			time = start[chooseB];
		}
		
	}
	fclose(stdin);
	fclose(stdout);
	return 0;
}
