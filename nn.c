#include<stdio.h>
#include<math.h>
#include<string.h>
#include"list.h"
#include<stdbool.h>

const int MAX_N = 10000;
const int INF = 1000000;

struct point{
    double x;
    double y;
};

double tour_length(struct point p[MAX_N], int n, int tour[MAX_N]) {
  int i;
  double sum=0.0;
  for(i=0;i<n;i++) sum+=dist(p[tour[i]],p[tour[(i+1)%n]]);
  return sum;// 総距離が関数の戻り値
}

double dist(struct point* p, struct point* q) { // pとq の間の距離を計算
  return sqrt((p->x-q->x)*(p->x-q->x)+(p->y-q->y)*(p->y-q->y));
}

double tour_length(struct point p[MAX_N], int n, int tour[MAX_N]) {
  int i;
  double sum=0.0;
  for(i=0;i<n;i++) sum+=dist(p[tour[i]],p[tour[(i+1)%n]]);
  return sum;// 総距離が関数の戻り値
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
	&&(strncmp("NODE_COORD_SECTION",buff,18)!=0)) ; // 行に出会うまで,
                                                        // 読み飛ばす.
  for(i=0;i<*np;i++) {                       // i=0 から i=(*np)-1まで
    if(fgets(buff,sizeof(buff),fp)!=NULL)
      sscanf(buff,"%*d %d %d",&(p[i].x),&(p[i].y)); // i番目の点の座標を読み込む
  }

  fclose(fp);
}

  fclose(fp);
}

int find_nearest(struct point p[MAX_N], int city, int n, bool visited[MAX_N]){
    int nearest;
    double min_dist = INF;//初期値はクソデカくしとく
    for(int i = 0; i < n; i++){
        //すでに訪れている場合はスキップ
        if(!visited[i]) continue;
        double tmp = dist(p + city, p + i);
        if(tmp < min_dist) nearest = i;
    }
    return nearest;
}

bool is_all_true(bool visited[MAX_N], int n){
    for(int i = 0; i < n; i++) if(!visited[i]) return false;
    return true;
}

void nn(struct point p[MAX_N],int n,struct list* tour){
    int start = 0;
    bool visited[n];
    memset(visited, false, n * sizeof(bool));
    visited[start] = true;
    insetAfter(tour->tail, start);
    while(is_all_true(visited)){
        int next = find_nearest(p, tour->head->next->value, n);
        visited[next] = true;
        insertAfter(tour->tail, next)
    }
}

main(int argc, char *argv[]) {
  int  n;                   // 点の数
  struct point  p[MAX_N];   // 各点の座標を表す配列
  list tour;   // 巡回路を表現する配列
  initialize(&tour);

  if(argc != 2) {
    fprintf(stderr,"Usage: %s <tsp_filename>\n",argv[0]);
    exit(EXIT_FAILURE);
  }

  // 点の数と各点の座標を1番目のコマンドライン引数で指定されたファイルから読み込む
  read_tsp_data(argv[1],p, &n);
  // 最近近傍法による巡回路構築
  nn(p,n,&tour);
  // 巡回路長を画面に出力
  printf("%lf\n",tour_length(p,n,tour));
  // 巡回路をテキストファイルとして出力
  write_tour_data("tour.dat",n,&tour);

  exit(EXIT_SUCCESS);
}
