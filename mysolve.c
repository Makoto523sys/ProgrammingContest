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
#define SWAP(a, b){a ^= b; b ^= a; a ^= b;}   
char fileName[51];
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

void list_to_array(int *array, struct list *tour, int n){
	int index = 0;
	for(struct node *v = tour->head->next; v->next != 0; v = v->next) array[index++] = v->value;
}

void array_to_list(int *array, struct list *tour, int n){
	struct node *v = tour->head->next;
	for(int i = 0; i < n; i++) {
		array[i] = v->value;
		v = v->next;
	}
}

void list_to_array_n(int *array, struct node *b, int len){
	for(int i = 0; i < len; i++){
	       	array[i] = b->value;
		b = b->next;
	}
}

void array_to_list_n(int *array, struct node *b, int len){
	for(int i = 0; i < len; i++){
		b->value = array[i];
		b = b->next;
	}
}

//デバッグはしてないのであしからず.多分セグフォ起こる...
void TwoOpt(struct point p[MAX_N], int n, struct list *tour, struct list *prec){
	bool flag = true;
	while(flag){
		flag = false;
		for(struct node *v = tour->head->next; v->next->next->next != 0; v = v->next){
			struct node *u = v->next;
			for(struct node *w = u; w->next->next != 0; w = w->next){
				struct node *a = v; struct node *b = u;
				struct node *c = w; struct node *d = w->next;
				if(dist(p[a->value], p[b->value]) + dist(p[c->value], p[d->value])
				> dist(p[a->value], p[c->value]) + dist(p[b->value], p[d->value])){
					int counter = 0;
					//B - C間に含まれるprecに指定されている都市の数を数える
					for(struct node *s = b; s != c->next; s = s->next){
						if(is_contain(*prec, s->value)) counter++;
					}
					//B - C間に含まれてるprecに指定されている都市の数が2つ以上なら2つ前のfor文に戻る
					if(counter >= 2) continue;
					//B - C間に含まれてるprecに指定されている都市の数が1つ以下なら,イイ感じにしてく
					else{
						int bc_len = 0;
						for(struct node *k = b; k != c->next; k = k->next) bc_len++;
						int bc_tour[bc_len];
						list_to_array_n(bc_tour, b, bc_len);
						int g = 0, h = bc_len - 1;
						while(g < h){
							SWAP(bc_tour[g], bc_tour[h]);
							g++; h--;
						}
						array_to_list_n(bc_tour, b, bc_len);
						flag = true;
					}
				}
			}
		}
	}
}

void OrOpt(struct point p[MAX_N], int n, struct list *tour, struct list *prec){
	for(struct node *r = tour->head->next; r->next != 0; r = r->next){
		for(struct node *v = tour->head->next; v->next->next != 0; v = v->next){
			struct node *u = v->next;
			if(v == r || v->next == r) continue;
			if((dist(p[r->prev->value], p[r->value]) + dist(p[r->next->value], p[r->value])
					> dist(p[v->value], p[r->value]) + dist(p[u->value], p[r->value])) &&
				!is_contain(*prec, r->value)){
				r->next->prev = r->prev;
				r->prev->next = r->next;
				r->prev = v;
				r->next = u;
				v->next = r;
				u->prev = r;
			}
		}
	}
}

void OrOpt2(struct point p[MAX_N], int n, struct list *tour, struct list *prec){
	for(struct node *c = tour->head->next->next; c->next->next->next != 0; c = c->next){
		struct node *d = c->next;
		struct node *a = c->prev;
		struct node *b = d->next;
		for(struct node *e = tour->head->next; e->next->next->next != 0; e = e->next){
			struct node *f = e->next;
			if(e == c || e == d || f == c || f == d) continue;
			if((dist(p[a->value], p[c->value]) + dist(p[c->value], p[d->value]) + dist(p[d->value], p[b->value]) + dist(p[e->value], p[f->value])
						> dist(p[a->value], p[b->value]) + dist(p[e->value], p[c->value]) + dist(p[c->value], p[d->value]) + dist(p[d->value], p[f->value])) && (!is_contain(*prec, c->value) && !is_contain(*prec, d->value))){
				a->next = b;
				b->prev = a;
				c->prev = e;
				d->next = f;
				e->next = c;
				f->prev = d;
				//sprintf(fileName, "tour%08d.dat", ++num);
				//write_tour_data(fileName, n, *tour);
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
	//訪問順制約で最初に現れる都市を訪問する
	insertAfter(tour->head, start->value);
	//訪問した都市をprecから外す
	erase(start);
	struct list unvisit; initialize(&unvisit);

	//unisitに最初に訪問した都市以外の都市を突っ込んでいく
	for(int i = 0; i < n; i++) {
		if(did_visit[i]) continue;
		else {
			insertBefore(unvisit.tail, i);
			did_visit[i] = true;
		}
	}

	//訪れていない都市がなくなるまで
	while(!is_empty(unvisit)){
		double min_len = INF;
		struct node *nearest = NULL;
		//vに一番近い都市を探す.
		for(struct node *u = unvisit.head->next; u->next != 0; u = u->next){
			double tmp = dist(p[tour->tail->prev->value], p[u->value]);
			if(tmp < min_len){
				tmp = min_len;
				nearest = u;
			}
		}
		//順序制約に関係ない数の場合
		if(!is_contain(*prec, nearest->value)){
			insertBefore(tour->tail, nearest->value);
			erase(nearest);
		}
		//順序制約に関係のある場合
		else if(is_contain(*prec, nearest->value) && !(nearest->value ^ prec->head->next->value)){
			insertBefore(tour->tail, nearest->value);
			erase(nearest);
			erase(prec->head->next);
		}
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
