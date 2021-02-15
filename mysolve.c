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
double lambda = 0.0;//要件等
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
		sum += dist(p[v->value], p[v->next->value ]);
	sum += dist(p[tour.head->next->value], p[tour.tail->prev->value]);
	return sum;
}


//list lにnが含まれているかどうかを調べる.含まれてるならtrue,ないならfalse
bool is_contain(struct list l, int n){
	for(struct node *v = l.head->next; v->next != 0; v = v->next) if(!(v->value ^ n))return true;
	return false;
}

//リストが空であるかどうかを判定.空ならtrue,何かしら入ってるならfalse
bool is_empty(struct list l){
	if(l.head->next == l.tail) return true;
	else return false;
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

void write_tour_data(char *filename, int n, struct list tour){
	FILE *fp;
	int i;
	// 構築した巡回路をfilenameという名前のファイルに書き出すためにopen
	if((fp=fopen(filename,"wt"))==NULL){
		fprintf(stderr,"Error: File %s open failed.\n",filename);
		exit(EXIT_FAILURE);
	}
	fprintf(fp,"%d\n",n);
	for(struct node *v = tour.head->next; v->next != 0; v = v->next) fprintf(fp, "%d ", v->value);
	fprintf(fp,"\n");
	fclose(fp);
}

void read_tsp_data(char *filename, struct point p[MAX_N],int *np, struct list* prec, int *mp) {
	FILE *fp;
	char buff[500];
	int i;
	if ((fp=fopen(filename,"rt")) == NULL) {// 指定ファイルを読み込み用に開く
		fprintf(stderr,"Error: File %s open failed.\n",filename);
		exit(EXIT_FAILURE);
	}
	while((fgets(buff,sizeof(buff),fp)!=NULL)   // PRECEDENCE_CONSTRAINTS:で始まる行に出会う
		&&(strncmp("PRECEDENCE_CONSTRAINTS:",buff,23)!=0)) ; // まで読み飛ばす.
	if(strncmp("PRECEDENCE_CONSTRAINTS:",buff,23)==0)  {
		sscanf(buff+24,"%d",mp);
		for(int i = 0; i < *mp; i++){
			int tmp;
			fscanf(fp,"%d ",&tmp);
			insertBefore(prec->tail, tmp);
		}
	} else {
		fprintf(stderr,"Error: There is no precedence constraint in file %s.\n",filename);
		exit(EXIT_FAILURE);
	}
	while((fgets(buff,sizeof(buff),fp)!=NULL)   // DIMENSION で始まる行に出会う
		&&(strncmp("DIMENSION",buff,9)!=0)) ; // まで読み飛ばす.
	sscanf(buff,"DIMENSION: %d",np);           // 点の数 *np を読み込む
	while((fgets(buff,sizeof(buff),fp)!=NULL)   // NODE_COORD_SECTIONで始まる
		&&(strncmp("NODE_COORD_SECTION",buff,18)!=0)) ; // 行に出会うまで,
	// 読み飛ばす.
	for(i=0;i<*np;i++) {                       // i=0 から i=(*np)-1まで
		if(fgets(buff,sizeof(buff),fp)!=NULL)
		sscanf(buff,"%*d %d %d",&(p[i].x),&(p[i].y)); // i番目の点の座標を読み込む
	}
	fclose(fp);
}

int size(struct list tour){
	int size = 0;
	for(struct node *v = tour.head->next; v->next != 0; v = v->next) size++;
	return size;
}

void clear(struct list *l){
	for(struct node *v = l->head->next; v->next != 0; v = v->next) erase(v);
}

void TwoOpt(struct point p[MAX_N], int n, struct list *tour, struct list *prec){
	bool flag = true;
	int do_counter = 0;
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
						//sprintf(fileName, "./tourfile/tour%08d.dat", ++num); write_tour_data(fileName, n, *tour);
						printf("\rTwoOpt done, counter = %3d", ++do_counter);
					}
				}
			}
		}
	}
	putchar('\n');
}

void OrOpt(struct point p[MAX_N], int n, struct list *tour, struct list *prec){
	bool flag;
	int counter = 0;
	do{
		flag = false;
		for(struct node *r = tour->head->next; r->next != 0 ; r = r->next){
			for(struct node *v = tour->head->next; v->next->next != 0; v = v->next){
				struct node *u = v->next;
				if(v == r || u == r) continue;
				if((dist(p[r->prev->value], p[r->value]) + dist(p[r->next->value], p[r->value]) + dist(p[v->value], p[u->value]) > dist(p[v->value], p[r->value]) + dist(p[u->value], p[r->value]) + dist(p[r->prev->value], p[r->next->value])) && !is_contain(*prec, r->value)){
					flag = true;
					r->next->prev = r->prev;
					r->prev->next = r->next;
					r->prev = v;
					r->next = u;
					v->next = r;
					u->prev = r;
					printf("\ror-opt done, counter = %6d", ++counter);
					//sprintf(fileName, "./tourfile/tour%08d.dat", ++num); write_tour_data(fileName, n, *tour);
				}
			}
		}
	}while(flag);
	putchar('\n');
}

void OrOpt2(struct point p[MAX_N], int n, struct list *tour, struct list *prec){
	bool flag;
	int counter = 0;
	do{
		flag = false;
		for(struct node *a = tour->head->next; a->next->next->next->next != 0; a = a->next){
			if(is_contain(*prec, a->next->value) || is_contain(*prec, a->next->next->value)) continue;
			struct node *c = a->next;
			struct node *d = c->next;
			struct node *b = d->next;
			for(struct node *e = tour->head->next; e->next->next != 0; e = e->next){
				struct node *f = e->next;
				if(e == c || e == d || e == a || e == b || f == c || f == d || f == a || f == b) continue;
				if((dist(p[a->value], p[c->value]) + dist(p[c->value], p[d->value]) + dist(p[d->value], p[b->value]) + dist(p[e->value], p[f->value]) > dist(p[a->value], p[b->value]) + dist(p[e->value], p[c->value]) + dist(p[c->value], p[d->value]) + dist(p[d->value], p[f->value]))){
					a->next = b;
					b->prev = a;
					c->prev = e;
					d->next = f;
					e->next = c;
					f->prev = d;
					printf("\ror-opt2 done, counter = %6d", ++counter);
					flag = true;
					break;
				}
			}
		}
	}while(flag);
	putchar('\n');
}

struct node* find_nearest(struct point *p, struct node *current, struct list *unvisit){
	struct node *nearest = unvisit->head->next;
	double min_len = INF;
	for(struct node *v = unvisit->head->next; v->next != 0; v = v->next){
		double tmp_len = dist(p[current->value], p[v->value]);
		if(min_len > tmp_len){
			min_len = tmp_len;
			nearest = v;
		}
	}
	return nearest;
}

struct node* find_next_nearest(struct point *p, struct node *current, struct list *unvisit){
	struct node *nearest = unvisit->head->next;
	struct node *next_nearest = unvisit->head->next;
	double min_len = INF;
	double second_len = INF;
	for(struct node *v = unvisit->head->next; v->next != 0; v = v->next){
		double tmp_len = dist(p[current->value], p[v->value]);
		if(min_len > tmp_len || second_len > tmp_len){
			next_nearest = min_len > tmp_len? nearest: next_nearest;
			nearest = min_len > tmp_len? v: nearest;
			if(min_len > tmp_len && second_len > tmp_len){
				second_len = min_len;
				min_len = tmp_len;
			}
			else if(min_len < tmp_len && second_len > tmp_len){
				second_len = tmp_len;
			}
		}
	}
	return next_nearest;
}

struct node* find(struct list *l, int val){
	for(struct node *v = l->head->next; v->next != 0; v = v->next) if(!(v->value ^ val)) return v;
	return NULL;
}

void nn(struct point p[MAX_N],int n,struct list* tour, struct list* prec){
	bool did_visit[n];
	memset(did_visit, false, n * sizeof(bool));
	//訪問順制約で最初の現れる都市
	struct node *start = prec->head->next;
	did_visit[start->value] = true;
	//訪問順制約で最初に現れる都市を訪問する
	insertAfter(tour->head, start->value);
	//訪問した都市をprecから外す
	//erase(start);
	struct node *next_prec = start->next;
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
		struct node *nearest = NULL;
		//vに一番近い都市を探す.
		nearest = find_nearest(p, tour->tail->prev, &unvisit);
		//printf("nearest->value is %d\n", nearest->value);
		//順序制約に関係ない数の場合
		if(!is_contain(*prec, nearest->value)){
			insertBefore(tour->tail, nearest->value);
			erase(nearest);
		}
		//順序制約に関係のある場合
		else if(!(nearest->value ^ next_prec->value)){
			insertBefore(tour->tail, nearest->value);
			erase(nearest);
			next_prec = next_prec->next;
			//erase(prec->head->next);
		}
		//順序制約に関係はあるけど,順序制約を破ってしまう場合
		else{
			//2番目に近いやつを探す
			nearest = find_next_nearest(p, tour->tail->prev, &unvisit);
			//2番目に近いやつも制約順序を破ってしまうとき,もうprecのから引っ張ってくる
			if(is_contain(*prec, nearest->value)){
			        nearest = next_prec;
				next_prec = next_prec->next;
			}
			insertBefore(tour->tail, nearest->value);
			erase(find(&unvisit, nearest->value));
		}
	}
	//sprintf(fileName, "./tourfile/tour%08d.dat", ++num); write_tour_data(fileName, n, *tour);
}

void ci(struct point p[MAX_N],int n,struct list* tour, struct list* prec){
	bool did_visit[n];
	//当然最初はどこにも行ってないんだから全部falseで埋める
	memset(did_visit, false, n * sizeof(bool));
	struct list unvisited; initialize(&unvisited);
	///tourにprecをコピってく.ついでにtourに入れた都市は訪問したってことにしとく
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
	//sprintf(fileName, "tour%08d.dat", ++num); write_tour_data(fileName, n, *tour);
}

unsigned int gen_rand(void) {
	static unsigned int tx = 123456789, ty=362436069, tz=521288629, tw=88675123;
	unsigned int tt = (tx^(tx<<11));
	tx = ty; ty = tz; tz = tw;
	return ( tw=(tw^(tw>>19))^(tt^(tt>>8)) );
}

void push_prec(struct list *tour, struct node *v, bool *flag){
	insertBefore(tour->tail, v->value);
	flag[v->value] = true;
	if(v->next == 0) lambda = 0;
}

void push_not_prec(struct list *tour, int n, struct list *not_prec, struct list *prec, bool *did_visit){
	unsigned int city = (gen_rand() + rand()) % n;
	while(did_visit[city] || is_contain(*prec, city)) city = (gen_rand() + rand()) % n;
	for(struct node *v = not_prec->head->next; v->next != 0; v = v->next){
		if(!(v->value ^ city)){
		       	erase(v);
			break;
		}
	}
	insertBefore(tour->tail, city);
	did_visit[city] = true;
}

//一様乱数がλよりも大きければfalseつまりはprecは挿入しない
bool prec_or_not(void){
	return lambda > ((double)rand())/RAND_MAX? true: false;
}

//arrayの中身がすべてtrueならtrue, それ以外はfalseを返す
bool is_all_true(bool *array, int n){
	for(int i = 0; i < n; i++) if(!array[i]) return false;
	return true;
}

void random_insert(struct point p[MAX_N],int n,struct list* tour,int m, struct list* prec){
	struct node *prec_ptr = prec->head->next;
	struct list not_prec; initialize(&not_prec);
	for(int i = 0; i < n; i++){
		if(is_contain(*prec, i)) continue;
		else insertBefore(not_prec.tail, i);
	}
	bool did_visit[n]; memset(did_visit, false, n * sizeof(bool));
	//precの先頭に訪れる
	push_prec(tour, prec_ptr, did_visit);
	prec_ptr = prec_ptr->next;
	while(!is_empty(not_prec) || prec_ptr->next != 0){
		if(prec_or_not()){
			push_prec(tour, prec_ptr, did_visit);
			prec_ptr = prec_ptr->next;
		}
		else if(is_empty(not_prec)) continue;
		else push_not_prec(tour, n, &not_prec, prec, did_visit);
	}
	sprintf(fileName, "tour%08d.dat", ++num);
	write_tour_data(fileName, n, *tour);
}

void copy(struct list from, struct list *to){
	for(struct node *v = from.head->next; v->next != 0; v = v->next) insertBefore(to->tail, v->value);
}

//適当な2点を入れ替える関数
void conjurer_joke(struct list *l, int n, struct list prec){
	int a = gen_rand() % n;
	int b = gen_rand() % n;
	if(a > b) SWAP(a, b);
	struct node *v = NULL;
	struct node *u = NULL;
	int counter = 0;
	printf("Finished setup\n");
	for(struct node *w = l->head->next; w->next != 0; w = w->next, counter++){
		if(!(counter ^ a)){
			printf("In first if\n");
		        if(!is_contain(prec, w->value)) v = w;
		        while(is_contain(prec, v->value)) v = v->next;
		}
		if(!(counter ^ b)){
			printf("In second if\n");
		        u = w;
		        while(is_contain(prec, u->value)) u = u->next;
			break;
		}
		printf("end for\n");
	}
	struct node *p; struct node *ne;
	v->prev->next = u;
	v->next->prev = u;
	u->next->prev = v;
	u->prev->next = v;
	p = v->prev; ne = v->next;
	v->next = u->next;
	v->prev = u->prev;
	u->next = ne;
	u->prev = p;
	sprintf(fileName, "tour%08d.dat", ++num); write_tour_data(fileName, n, *l);
}

//引数に依存する確率でtrueを返す
bool gambling_dog(double m, double c, double T){
	return exp((m-c)/T) > ((double)rand())/RAND_MAX;
}

void SA(struct point *p, int n, struct list *tour, struct list prec, struct list *tmp_tour){
	double temperature = 100.0;
	const double crystall = 0.02;
	double cool_rate = 0.99;
	double current_dist = tour_length(p, n, *tour);
	double tmp_dist = INF;
	//何故かmにtourをコピーしようとするとセグフォ起こる...何でや!
	while(temperature > crystall){
		conjurer_joke(tmp_tour, n, prec);
		tmp_dist = tour_length(p, n, *tmp_tour);
		if(current_dist < tmp_dist || gambling_dog(tmp_dist, current_dist, temperature)){
			current_dist = tmp_dist;
			copy(*tmp_tour, tour);
		}
		printf("temperature = %lf\n", temperature);
		temperature *= cool_rate;
	}
}
