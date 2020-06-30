#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <limit.h>
#define MAX_N INT_MAX >> 5   // 点の数の最大値
#define Infty UINT_MAX // 無限大の定義
#define SWAP(a, b) {a = a + b; b = a - b; a = a - b;}

//都市の位置はx, y座標で定義
typedef struct{
  int x;
  int y;
}point_t;

double dist(point_t p, point_t q) { // pとq の間の距離を計算 
  return sqrt((p.x - q.x) * (p.x - q.x) + (p.y - q.y) * (p.y - q.y));
}

void two_opt(point_t p[MAX_N], int n, int tour[MAX_N]){
	bool flag = true;
	while(flag){
		flag = false;
		for(int i = 0; i <= n - 3; i++){
			int j = i + 1;
			for(int k = i + 2; k <= n - 1; k++){
				int l = (k + 1) % n;
				int a = tour[i]; int b = tour[j];
				int c = tour[k]; int d = tour[l];
				if(dist(p[a], p[b]) + dist(p[c], p[d])
				> dist(p[a], p[c]) + dist(p[b], p[d])){
					int g = j, h = k;
					while(g < h){
						SWAP(tour[g], tour[h]);
						g++; h--;
					}
					flag = true;
				}
			}
		}
	}
}
//参考:https://www.jstatsoft.org/index.php/jss/article/view/v008i14/xorshift.pdf
unsigned int gen_rand() {
	static unsigned int tx = 123456789, ty=362436069, tz=521288629, tw=88675123;
	unsigned int tt = (tx^(tx<<11));
	tx = ty; ty = tz; tz = tw;
	return ( tw=(tw^(tw>>19))^(tt^(tt>>8)) );
}

void my_memset(int *array, int n, int k){
	for(int i = 0; i < n; i++) array[i] = k;
}

double length(point_t p[MAX_N], int n, int *tour){
	double total = 0.0f;
	for(int i = 0; i < n; i++) total += dist(p[tour[i]], p[tour[(i + 1) % n]]);
	return total;
}

void randInsert(point_t p[MAX_N], int n, int tour[MAX_N]){
	double min_len = Infty;
	int counter = 0;
	int gen_tour = 0;
	int did_visit[n];
	int tmp_tour[n];
	//スタート位置は0番目の都市
	tmp_tour[0] = 0;
	did_visit[0] = true;
	double tmp_len = Infty;
	int min_j = 0;
	//0番目の都市に最も近い都市は順回路の最後に入れとく
	for(int i = 1; i < n; i++)
		if(dist(p[tmp_tour[0]], p[i]) < tmp_len){
			tmp_tour[n - 1] = min_j = i;
			tmp_len = dist(p[tmp_tour[0]], p[min_j]);
		}
	did_visit[min_j] = true;
	for(int m = 0; m < n; m++){
		//もろもろを初期化
		my_memset(did_visit, n, false);
		did_visit[0] = did_visit[min_j] = true;
		my_memset(tmp_tour, n, -1);
		tmp_tour[0] = 0;
		tmp_tour[n-1] = min_j;
		//道順を乱数に従って決める
	       	for(int j = 1; j < n - 1; j++){ 
			unsigned int city = gen_rand() % n;
			//unsigned city = rand() % n;
			while(did_visit[city]) city = gen_rand() % n;
			tmp_tour[j] = city;
			did_visit[city] = true;
		}
		//fprintf(stderr, "\r%d \t / \t %d", i + 1, 1 << 20);
		//乱数で作った道順の長さを測る
		two_opt(p, n, tmp_tour);
		double tmp = length(p ,n ,tmp_tour);
		if(tmp < min_len){
			for(int k = 0; k < n; k++) tour[k] = tmp_tour[k];
			two_opt(p,n,tour);
			min_len = tour_length(p, n, tour);
			counter++;
		}
	}
}
