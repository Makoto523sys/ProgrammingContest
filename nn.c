#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<stdbool.h>

const int MAX_N = 10000;
const int INF = 1000000;

struct node {
  int value;
  struct node* prev;
  struct node* next;
};

struct list {
  struct node* head;
  struct node* tail;
};

struct point{
    int x;
    int y;
};

double dist(struct point p, struct point q);
double tour_length(struct point p[MAX_N], int n, struct list* tour);
void write_tour_data(char *filename, int n, struct list* tour);
void read_tsp_data(char *filename, struct point p[MAX_N],int *np);
int find_nearest(struct point p[MAX_N], int city, int n, bool visited[MAX_N]);
bool is_all_true(bool visited[MAX_N], int n);
void nn(struct point p[MAX_N],int n,struct list* tour);
void printString(struct list* p);
void printNumbers(struct list* p);
void erase(struct node* p);
void insertAfter(struct node* p, int v);
void insertBefore(struct node* p, int v);
void initialize(struct list* p);
void solve(struct point p[MAX_N], int n, struct list* tour, bool optimize);
void twoopt(struct point p[MAX_N], int n, struct list* tour);
void reconstruction(struct node *v, struct node *w);

int main(int argc, char *argv[]) {
  int  n;                   // 点の数
  struct point  p[MAX_N];   // 各点の座標を表す配列
  struct list tour;   // 巡回路を表現するリスト
  initialize(&tour);
  bool optimize = false;

  if(argc != 2) {
    fprintf(stderr,"Usage: %s <tsp_filename>\n",argv[0]);
    exit(EXIT_FAILURE);
  }

  // 点の数と各点の座標を1番目のコマンドライン引数で指定されたファイルから読み込む
  read_tsp_data(argv[1],p, &n);
  // 最近近傍法による巡回路構築
  solve(p, n, &tour, optimize);
  // 巡回路長を画面に出力
  //printf("%lf\n",tour_length(p,n,&tour));
  // 巡回路をテキストファイルとして出力
  write_tour_data("tour.dat",n,&tour);

  exit(EXIT_SUCCESS);
}

void printNumbers(struct list* p) {
  for (struct node* i = p->head->next; i->next != 0; i = i->next)
    printf("%d ", i->value);
  //printf("\n");
}
void printString(struct list* p) {
  for (struct node* i = p->head->next; i->next != 0; i = i->next)
    printf("%c", i->value);
  //printf("\n");
}

double tour_length(struct point p[MAX_N], int n, struct list* tour) {
  double sum=0.0;
  for(struct node* v = tour->head->next; v->next->next != NULL; v = v->next)
      sum += dist(p[v->value], p[v->next->value]);
  sum += dist(p[tour->head->next->value], p[tour->tail->prev->value]);
  //for(i=0;i<n;i++) sum+=dist(p[tour[i]],p[tour[(i+1)%n]]);
  return sum;// 総距離が関数の戻り値
}

double dist(struct point p, struct point q) { // pとq の間の距離を計算
  return sqrt((p.x-q.x)*(p.x-q.x)+(p.y-q.y)*(p.y-q.y));
}

void write_tour_data(char *filename, int n, struct list* tour){
    FILE *fp;
    int i;
    // 構築した巡回路をfilenameという名前のファイルに書き出すためにopen
    if((fp=fopen(filename,"wt"))==NULL){
        fprintf(stderr,"Error: File %s open failed.\n",filename);
        exit(EXIT_FAILURE);
    }
    fprintf(fp,"%d\n",n);
    for(i=0;i<n; i++){
        fprintf(fp,"%d ",tour->head->next->value);
        erase(tour->head->next);
    }
    fprintf(fp,"\n");
    fclose(fp);
    }

void read_tsp_data(char *filename, struct point p[MAX_N],int *np) {
    FILE *fp;
    char buff[100];
    int i;

    if ((fp=fopen(filename,"rt")) == NULL) {// 指定ファイルを読み込み用に開く
        fprintf(stderr,"Error: File %s open failed.\n",filename);
        exit(0);
    }

    while((fgets(buff,sizeof(buff),fp)!=NULL)   // DIMENSION で始まる行に出会う
    &&(strncmp("DIMENSION",buff,9)!=0)) ; // まで読み飛ばす.
    sscanf(buff,"DIMENSION : %d",np);           // 点の数 *np を読み込む

    while((fgets(buff,sizeof(buff),fp)!=NULL)   // NODE_COORD_SECTIONで始まる
    &&(strncmp("NODE_COORD_SECTION",buff,18)!=0)) ; // 行に出会うまで読み飛ばす.
    for(i=0;i<*np;i++) {                       // i=0 から i=(*np)-1まで
        if(fgets(buff,sizeof(buff),fp)!=NULL)
            sscanf(buff,"%*d %d %d",&(p[i].x),&(p[i].y)); // i番目の点の座標を読み込む
    }

    fclose(fp);
}

int find_nearest(struct point p[MAX_N], int city, int n, bool visited[MAX_N]){
    int nearest;
    double min_dist = INF;//初期値はクソデカくしとく
    for(int i = 0; i < n; i++){
        //すでに訪れている場合はスキップ
        if(visited[i]) continue;
        double tmp = dist(p[city], p[i]);
        if(tmp < min_dist) nearest = i;
    }
    return nearest;
}

bool is_all_true(bool visited[MAX_N], int n){
    //printf("start is_all_true\n");
    for(int i = 0; i < n; i++){
        if(!visited[i]) return false;
    }
    return true;
}

void nn(struct point p[MAX_N],int n,struct list* tour){
    int start = 0;
    bool visited[n];
    //printf("Before call memset\n");
    memset(visited, false, n * sizeof(bool));
    //printf("After call memset\n");
    visited[start] = true;
    //printf("Before insertBefore\n");
    insertBefore(tour->tail, start);
    //printf("After insertBefore\n");
    while(!is_all_true(visited, n)){
        //printf("Before find_nearest\n");
        int next = find_nearest(p, tour->head->next->value, n, visited);
        //printf("After find_nearest\n");
        visited[next] = true;
        //printf("Before insertAfter\n");
        insertBefore(tour->tail, next);
        //printf("After insertAfter\n");
    }
}

void erase(struct node* p) {
  p->prev->next = p->next;
  p->next->prev = p->prev;
  free(p);
}

void insertAfter(struct node* p, int v) {
  struct node* newNode = (struct node*)malloc(sizeof(struct node));
  newNode->value = v;
  newNode->prev = p;
  p->next->prev = newNode;
  newNode->next = p->next;
  p->next = newNode;
}

void insertBefore(struct node* p, int v) {
  struct node* newNode = (struct node*)malloc(sizeof(struct node));
  newNode->value = v;
  newNode->next = p;
  p->prev->next = newNode;
  newNode->prev = p->prev;
  p->prev = newNode;
}

void initialize(struct list* p) {
  p->head = (struct node*)malloc(sizeof(struct node));
  p->tail = (struct node*)malloc(sizeof(struct node));
  p->head->prev = 0;
  p->head->next = p->tail;
  p->tail->prev = p->head;
  p->tail->next = 0;
}

void solve(struct point p[MAX_N], int n, struct list *tour,bool optimize){
    nn(p,n,tour);
    if(optimize) twoopt(p, n, tour);
}

void twoopt(struct point p[MAX_N], int n, struct list *tour){
    double old_dist;
    double new_dist;
    for(struct node* u = tour->head->next; u->next->next != NULL; u = u->next){
        struct node* v = u->next;
        for(struct node* w = tour->head->next; w->next->next != NULL; w = w->next){
            struct node* x = w->next;
            int a = u->value, b = v->value, c = w->value, d = x->value;
            old_dist = dist(p[a], p[b]) + dist(p[c], p[d]);
            new_dist = dist(p[a], p[c]) + dist(p[b], p[d]);
            if(old_dist < new_dist) reconstruction(v, w);
            else continue;
        }
    }
}

void reconstruction(struct node* v, struct node* w){
    struct node *v_next = v->next;
    struct node *w_prev = w->prev;
    for(struct node *f = v_next; f != w; f = f->next){
    }
}
