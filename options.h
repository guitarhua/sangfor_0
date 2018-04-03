#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE 256    // 容器的容量
//      bit3 bit2 bit1 bit0
//       D    C     B    A
//          共16种
#define TYPES 16    // 每个容器里可能出现的组合情况总数
#define CNT 8   //8 个容器
#define INF 666
#define CONN 1

struct head_list {
    struct head_list *pre;      // 前驱
    struct head_list *suc;      // 后继
};


// 真正用来存储数据的结构, 只能存一个基本类型
// 如果有组合,则用head链接;
struct value {
    struct head_list list;
    int n;      // n 为基本型数据
};

struct node {
    struct value value;
    int type;   //所以,每条纪录的数据类型由添加进来的纪录自己存
};

// 来，我们来设计容器结构
typedef struct cont {
    struct head_list list;      //用于链接

    // 存储实体
    struct node *values[SIZE];   // 每个容器的容量为256条纪录

    //存储信息  -1: 不允许放; 0: 允许放但未放; >0: 已经存放条数
    int stroageinfo[TYPES];  // 容器里已经存放的组合情况：下标代表类型(16种)，数值代表数量；
    int len;    // 容器内已入库数据量

    unsigned int types;   // 允许放信息类型(4*8=32位，32种类型, 只需要前16位)
    char name;    //容器名称
    int flags;  // 已存数据类型
} VertexType;


// 邻接矩阵
typedef struct _graph {
    struct cont vexs[CNT];       // 顶点集合
    int vexnum;           // 顶点数
    int edgnum;           // 边数
    int edge[CNT][CNT]; // 邻接矩阵
} Graph, *PGraph;



/**
 * 在dijkstra算法运行的时候动态的调整新加入节点与尚未加入节点的连接性.
 * 顶点连接的条件(A->B):
 *     1. A和B的存储类型有交叉.
 *     2. 上游节点A要有数据才能够连接.
 * @param g     图
 * @param prev  前驱顶点数组. 即，prev[i]的值是"顶点vs"到"顶点i"的最短路径所经历的全部顶点中，位于"顶点i"之前的那个顶点。
 * @param dist  长度数组. 即，dist[i]是"顶点vs"到"顶点i"的最短路径的长度。
 * @param flag  已获得最短路径的标志数组. flag[i]=1表示"顶点vs"到"顶点i"的最短路径已成功获取。
 * @param minid 新接入节点
 * @param min   新接入节点的最短路径
 */
void adjustEdge(Graph *g, int prev[], int dist[], int flag[], int min, int minid);

/**
 * 判断两个容器是否联通，只需要联通就好
 * @param  p1 容器1
 * @param  p2 容器2
 * @return    返回CNT，不连通；返回1，联通
 */
int isConnect(VertexType *p1, VertexType *p2);

/**
 * Dijkstra最短路径。即，统计图(g)中"顶点vs"到其它各个顶点的最短路径。
 * @param g    图
 * @param vs   起始顶点(start vertex)。即计算"顶点vs"到其它顶点的最短路径
 * @param prev 前驱顶点数组。即，prev[i]的值是"顶点vs"到"顶点i"的最短路径所经历的全部顶点中，位于"顶点i"之前的那个顶点。
 * @param dist 长度数组。即，dist[i]是"顶点vs"到"顶点i"的最短路径的长度。
 */
void dijkstra_v0(Graph *g, int vs, int prev[], int dist[]);

/**
 * Dijkstra最短路径。即，统计图(g)中"顶点vs"到其它各个顶点的最短路径。
 * @param g    图
 * @param vs   起始顶点(start vertex)。即计算"顶点vs"到其它顶点的最短路径
 * @param prev 前驱顶点数组。即，prev[i]的值是"顶点vs"到"顶点i"的最短路径所经历的全部顶点中，位于"顶点i"之前的那个顶点。
 * @param dist 长度数组。即，dist[i]是"顶点vs"到"顶点i"的最短路径的长度。
 * @return      返回最接近的未满容器的编号
 */
int dijkstra(Graph *g, int vs, int prev[], int dist[]);


/**
 * 初始化每个容器所能存放的基本数据类型.
 * 这个函数不应该更改
 *
 * 基本存储类型:
 *     0   1   2   3   4   5   6   7
 * A   1   1   1   1
 * B   1       1       1   1   1
 * C   1       1       1   1   1
 * D           1       1   1   1   1
 * E           1       1   1   1   1
 * F   1   1   1           1   1
 * G       1   1           1       1
 * H       1   1           1       1
 *
 * @param g 图
 */
void initTypes(Graph *g);

/**
 * 定制每个容器的存储情况.
 *  *
 * 基本存储类型:
 *      0   1   2   3   4   5   6   7
 * A0   1   1   9   9
 * B1   1       1       9   1   1
 * C2   1       1       9   1   1
 * D3           1       1   6   1   1
 * E4           1       1   1   1   9
 * F5   9   1   1           1   1
 * G6       1   1           1       9
 * H7       1   1           1       9
 *
 * @param g 图
 */
void setStorageInfo_v1(Graph *g) ;

/**
 * 将所有的边初始化为无穷大
 * @param g 图
 */
void initEdge(Graph *g);

/**
 * 初始化图
 * @param g 图
 */
void initGraph(Graph *g);

/**
 * 初始化各个容器存储情况为0, 这个函数不应该被更改
 * @param g 图
 */
void initStroageInfo(Graph *g);

/**
 * 设置容器的存储情况
 * @param cont     指向待设定容器的指针
 * @param not_full 设定为满或者非满的标志
 */
void setCont(VertexType *cont, int not_full);

/**
 * 设定各容器的存储情况, 包括:
 *     1. 允许存储的基本类型
 *     2. 先在已经存储的信息
 * 各容器满与非满的情况随机生成
 * @param g 图
 */
void setStorageInfo(Graph *g);

/**
 * 设定各容器的存储情况, 包括:
 *     1. 允许存储的基本类型
 *     2. 先在已经存储的信息
 * 各容器满与非满的情况随机生成
 * @param g 图
 */
void setStorageInfo_v0(Graph *g);



/**
 * 打印所有容器的存储情况
 * @param g 图
 */
void printStroageInfo(Graph *g);

/**
 * 打印从节点n到根节点的路径
 * @param g    图
 * @param prev 前驱顶点数组。即，prev[i]的值是"顶点vs"到"顶点i"的最短路径所经历的全部顶点中，位于"顶点i"之前的那个顶点。
 * @param n    终点节点
 */
void printPath(Graph *g, int prev[], int n);

/**
 * 寻找插入的合适的位置
 * 首先要找能够直接插进去的
 * 然后才是找能间接插进去的
 * @param  g 图
 * @param  n 要插入的类型号
 * @param prev 前驱顶点数组。即，prev[i]的值是"顶点vs"到"顶点i"的最短路径所经历的全部顶点中，位于"顶点i"之前的那个顶点。
 * @param dist 长度数组。即，dist[i]是"顶点vs"到"顶点i"的最短路径的长度。
 * @return 适合插进去的容器编号, -1为没找到
 */
int findNiche(Graph *g, int n, int prev[], int dist[]);
