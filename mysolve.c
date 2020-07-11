#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "list.h"
#include <stdbool.h>
#include <memory.h>
#include <time.h>
#define MAX_N 10000   // 点の数の最大値
#define INF 100000000 // 無限大の定義
#define SWAP(a,b){a ^= b; b ^= a; a ^= b;}   
const int buff = 51;//ファイル名の長さの最大値.1は\0の分を考慮してる
char fileName[buff];
int num = 0;

struct point {
  int x;
  int y;
};

double dist(struct point p, struct point q) { // pとq の間の距離を計算 
	return sqrt((p.x-q.x)*(p.x-q.x)+(p.y-q.y)*(p.y-q.y));
}

double tour_length(struct point p[MAX_N], int n, struct list tour) {
	double sum=0.0;
	for(struct node *v = tour.head->next; v->next->next != 0; v = v->next)
		sum += dist(p[v->value], p[v->next->value % n]);
	return sum;
}


//list lにnが含まれているかどうかを調べる.含まれてるならtrue,ないならfalse
bool is_contain(struct list l, int n){
	for(struct node *v = l.head->next; v->next != 0; v = v->next) if(v->value == n) true;
	return false;
}

//nnをリストで実装したが,よく考えたらこれこのままだとまずいことに気が付いてしまった.あとで直します.
void nn(struct point p[MAX_N],int n,struct list* tour,int m, struct list* prec){
	bool did_visit[n];
	//当然最初はどこにも行ってないんだから全部falseで埋める
	memset(did_visit, false, n * sizeof(bool));
	struct list unvisited; initialize(&unvisited);
	//tourにprecをコピってく.ついでにtourに入れた都市は訪問したってことにしとく
	for(struct node *v = prec->head->next; v->next != 0; v = v->next){
	       	insertBefore(tour->tail, v->value);
		did_visit[v->value] = true;
	}
	double min_len = INF;
	//ある点から一番近い都市のノードを保存しとく
	struct node *nearest;
	//未だ訪れていない都市群(unvisited)にすべての都市をぶっこむ
	//さっきはprecに指定されてる都市は訪問したことになってるのでここではprecに指定されていない都市群が入る
	for(int i = 0; i < n; i++) if(!did_visit[i]) insertBefore(unvisited.tail, i);

	for(struct node *v = tour->head->next; v->next != 0; v = v->next){
		min_len = INF;
		for(struct node *w = unvisited.head->next; w->next != 0; w = w->next){
			double tmp = dist(p[v->value], p[w->value]);
			//他のどの都市よりもvに近くて,訪れたことなければ放り込む
			if(min_len > tmp && !did_visit[w->value]){
				min_len = tmp;
				nearest = w;
			}
		}
		insertAfter(v, nearest->value);
		did_visit[nearest->value] = true;
		erase(nearest);
	}
	//sprintf(fileName, "tour%08d.dat", ++num);
	//write_tour_data(fileName, n, tour);
}

void TwoOpt(struct point p[MAX_N],int n,struct list* tour){
