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
	sum += dist(p[tour.head->next->value], p[tour.tail->prev->value]);
	return sum;
}


//list lにnが含まれているかどうかを調べる.含まれてるならtrue,ないならfalse
//というか用意しただけなので使うかどうかは知らん...
bool is_contain(struct list l, int n){
	for(struct node *v = l.head->next; v->next != 0; v = v->next) if(v->value == n) true;
	return false;
}

//リストが空であるかどうかを判定.空ならtrue,何かしら入ってるならfalse
bool is_empty(struct list l){
	if(l.head->next == l.tail) return true;
	else false;
}

//デバッグはしてないのであしからず.多分セグフォ起こる...
void TwoOpt(struct point p[MAX_N], int n, struct list *tour, struct list *prec){
	bool flag = true;
	//show_array(tour, n);
	while(flag){
		flag = false;
		for(struct node *v = tour->head->next; v->next->next->next != 0; v = v->next){
			struct node *u = v->next;
			for(struct node *w = v->next->next; w->next->next != 0; w = w->next){
				struct node *a = v;
				struct node *b = u;
				struct node *c = w;
				struct node *d = w->next;
				int counter = 0;
				if(is_contain(*prec, a->value)) counter++;
				if(is_contain(*prec, b->value)) counter++;
				if(is_contain(*prec, c->value)) counter++;
				if(is_contain(*prec, d->value)) counter++;
				if(counter >= 2) continue;
				if(dist(p[a->value], p[b->value]) + dist(p[c->value], p[d->value])
				> dist(p[a->value], p[c->value]) + dist(p[b->value], p[d->value])){
					a->next = c;
					c->prev = a;
					c->next = b;
					b->prev = c;
					b->next = d;
					d->prev = b;
					flag = true;
//					show_array(tour, n);
				}
			}
		}
	}
}

//これもまだデバッグが済んでない...
void nn(struct point p[MAX_N],int n,struct list* tour,int m, struct list* prec){
	bool did_visit[n];
	memset(did_visit, false, n * sizeof(bool));
	//訪問順制約で最初の現れる都市
	struct node *start = prec->head->next;
	did_visit[start->value] = true;
	insertAfter(tour->head, start->value);
	erase(start);
	struct list unvisit; initialize(&unvisit);

	for(int i = 0; i < n; i++) {
		if(did_visit[i]) continue;
		else {
			insertBefore(unvisit.tail, i);
			did_visit[i] = true;
		}
	}
	for(struct node *v = tour->head->next; v->next != 0; v = v->next){
		double min_len = INF;
		struct node *nearest = NULL;
		bool flag = false;
		//vに一番近い都市を探す.
		for(struct node *u = unvisit.head->next; u->next != 0; u = u->next){
			//順序制約に関係ない数の場合
			if(!is_contain(*prec, u->value)){
				double tmp = dist(p[v->value], p[u->value]);
				if(min_len > tmp){
					min_len = tmp;
					nearest = u;
				}
				flag = true;
			}
			//順序制約に関係のある場合
			else if(is_contain(*prec, u->value) && !(u->value ^ prec->head->next->value)){
				double tmp = dist(p[v->value], p[u->value]);
				if(min_len > tmp){
					min_len = tmp;
					nearest = u;
				}
				flag = false;
			}
		}
		did_visit[nearest->value] = true;
		if(flag) erase(prec->head->next);
		insertAfter(tour->tail, nearest->value);
		erase(nearest);
	}

}

void ci(struct point p[MAX_N],int n,struct list* tour,int m, struct list* prec){
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
	struct node *nearest = NULL;
	struct node *a = NULL;
	struct node *b = NULL;
	//未だ訪れていない都市群(unvisited)にすべての都市をぶっこむ
	//さっきはprecに指定されてる都市は訪問したことになってるのでここではprecに指定されていない都市群が入る
	for(int i = 0; i < n; i++) if(!did_visit[i]) insertBefore(unvisited.tail, i);

	for(struct node *v = tour->head->next; v->next->next != 0; v = v->next){
		struct node *u = v->next;
		min_len = INF;
		for(struct node *r = unvisited.head->next; r->next != 0; r = r->next){
			double tmp = dist(p[v->value], p[r->value]) + dist(p[u->value], p[r->value]) - dist(p[v->value], p[u->value]);
				if(min_len > tmp){
					a = v; b = u; min_len = tmp; nearest = r;
				}
		}
		insertAfter(a, nearest->value);
		erase(nearest);
	}
	//sprintf(fileName, "tour%08d.dat", ++num);
	//write_tour_data(fileName, n, tour);
}
