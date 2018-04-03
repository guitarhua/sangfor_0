#include "options.h"

/**
 * 寻找插入的合适的位置
 * 首先要找能够直接插进去的
 * 然后才是找能间接插进去的
 * @param  g 图
 * @param  data 要插入的类型号
 * @param prev  前驱顶点数组。即，prev[i]的值是"顶点vs"到"顶点i"的最短路径所经历的全部顶点中，位于"顶点i"之前的那个顶点。
 * @param dist  长度数组。即，dist[i]是"顶点vs"到"顶点i"的最短路径的长度。
 * @return      适合插进去的容器编号, -1为没找到
 */
int findNiche(Graph *g, int data, int prev[], int dist[])
{
    int i = 0, j = -1;
    int min, minid;
    int start[CNT];  // 用来记录能够允许直接插入的容器编号,以作dijskra的输入
    int end = -1;     // 用来记录执行dijksra算法之后找到的start对应的一条最短路径的终点

    printf("接到数据%c\n", data + 'A');
    // 先找能直接插进去的
    for(i = 0; i < CNT; ++i) {
        start[i] = -1;      // 先初始化

        if(0 != (g->vexs[i].types & (0x01 << data))) {     // 有这种基本型的
            start[++j] = i;
            if(g->vexs[i].len < 256) {
                printf("找到直接目标容器%c\n%c\n", g->vexs[i].name, g->vexs[i].name);
                return i;
            }
        }
    }


    // 没有找到可以直接插进去的, 那就只好运行dijkstra算法了
    i = 0;
    minid = -1;
    min = INF;
    do {
        end = dijkstra(g, start[i], prev, dist);    // 所以说, dijkstra这个算法还得改进

        // 如果在执行到某次时,找到了路径为1(直接下层)且满足条件的容器, 那就直接返回,因为执行算法还是有一定的时间开销
        if((end != -1) && (dist[end] < min)) {
            minid = end;
            min = dist[minid];
            if(min == CONN) {
                printf("找到间接目标容器%c\n", g->vexs[minid].name);
                printPath(g, prev, minid);
                // 这里的返回和dijkstra算法里的中途返回衔接在一起,省去许多开销
                return minid;
            }
        }
    } while(start[++i] != -1);

    if(minid != -1)	printPath(g, prev, minid);
    else	printf("没有找到可用容器.\n");

    return minid;
}








/**
 * 设置容器的存储情况
 * @param cont     指向待设定容器的指针
 * @param not_full 设定为满或者非满的标志
 */
void setCont(VertexType *cont, int not_full)
{
    int i = 0;
    int tmp;
    int count = 16;
    int base_type_info[TYPES];
    int base_type_num = 0;    // 容器cont的基本类型数目, 也是base_type_info的有效长度

    // 首先你得知道容器有几种基本型
    for(i = 0; i < sizeof(unsigned int) * 8; ++i) {
        if((cont->types & (0x01 << i)) != 0) {
            base_type_info[base_type_num++] = i;
        }
    }

    srand((unsigned int)time(NULL)); // 时间做种,每次产生不一样的随机数序列
    // 容器容量为256(2^8), 分成16组:16*16, 16次添加, 然后rand生成存放类型的顺序
    if(not_full == 1)   count = rand() % 16; // 少于16组
    else    count = 16;

    for(i = 0; i < count; ++i) { //16组数据
        tmp = rand() % base_type_num;

        // 将一组16条数据添加到基本型base_type_info[tmp]
        cont->len += 16; // 容器总容量
        cont->stroageinfo[base_type_info[tmp]] += 16;  // 基本型
    }
}


/**
 * 设定各容器的存储情况, 包括:
 *     1. 允许存储的基本类型
 *     2. 先在已经存储的信息
 * 各容器满与非满的情况随机生成
 * @param g 图
 */
void setStorageInfo_v0(Graph *g)
{
    int i;
    int full[CNT];  // 为0则初始化为没满,1则初始化为满


    srand((unsigned int)time(NULL)); // 时间做种,每次产生不一样的随机数序列
    for(i = 0; i < CNT; ++i)
        full[i] = rand() % 2;

    for(i = 0; i < CNT; ++i) {
        if(full[i] == 1) setCont(&g->vexs[i], 0); // 设置容器为满
        else    setCont(&g->vexs[i], 1);      // 设置容器为不满
    }
}



/**
 * 设定各容器的存储情况, 包括:
 *     1. 允许存储的基本类型
 *     2. 先在已经存储的信息
 * 各容器满与非满的情况随机生成
 * @param g 图
 */
void setStorageInfo(Graph *g)
{
    int i;
    int full[CNT];  // 为0则初始化为没满,1则初始化为满


    srand((unsigned int)time(NULL)); // 时间做种,每次产生不一样的随机数序列
    for(i = 0; i < CNT; ++i)
        full[i] = rand() % 2;

    for(i = 0; i < CNT; ++i) {
        if(full[i] == 1) setCont(&g->vexs[i], 0); // 设置容器为满
        else    setCont(&g->vexs[i], 1);      // 设置容器为不满
    }
}





/**
 * Dijkstra最短路径。即，统计图(g)中"顶点vs"到其它各个顶点的最短路径。
 * @param g    图
 * @param vs   起始顶点(start vertex)。即计算"顶点vs"到其它顶点的最短路径
 * @param prev 前驱顶点数组。即，prev[i]的值是"顶点vs"到"顶点i"的最短路径所经历的全部顶点中，位于"顶点i"之前的那个顶点。
 * @param dist 长度数组。即，dist[i]是"顶点vs"到"顶点i"的最短路径的长度。
 */
void dijkstra_v0(Graph *g, int vs, int prev[], int dist[])
{
    int i, j;
    int min, minid;
    int flag[CNT];      // flag[i]=1表示"顶点vs"到"顶点i"的最短路径已成功获取。

    for(i = 0; i < g->vexnum; i++) {
        dist[i] = INF;
        flag[i] = 0;              // 顶点i的最短路径还没获取到。
        prev[i] = -1;              // 顶点i的前驱顶点为-1。
    }

    // 对"顶点vs"自身进行初始化
    minid = vs;
    min = 0;
    flag[vs] = 1;       // 先将vs加入到最小生成树中
    dist[vs] = 0;
    adjustEdge(g, prev, dist, flag, vs, min);     // 设置与根节点vs相连接的边和点




    // 遍历g->vexnum-1次；每次找出一个顶点的最短路径。
    for(i = 1; i < g->vexnum; i++) {
        // 寻找当前最小的路径. 即，在未获取最短路径的顶点中，找到离vs最近的顶点(k)。
        min = INF;
        for(j = 0; j < g->vexnum; j++) {
            if(flag[j] == 0 && dist[j] < min) {
                min = dist[j];
                minid = j;
            }
        }

        // 标记"顶点minid"为已经获取到最短路径, 并调整minid所连接点的路径
        flag[minid] = 1;
        adjustEdge(g, prev, dist, flag, minid, min);
    }



    // 打印dijkstra最短路径的结果
    printf("dijkstra(%c): \n", g->vexs[vs].name);
    for (i = 0; i < g->vexnum; i++)
        if(dist[i] != INF)
            printf("  shortest(%c, %c)=%d\n", g->vexs[vs].name, g->vexs[i].name, dist[i]);
}


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
void adjustEdge(Graph *g, int prev[], int dist[], int flag[], int minid, int min)
{
    int i, j;
    int tmp;
    for(i = 0; i < g->vexnum; i++) {
        tmp = g->vexs[minid].types & g->vexs[i].types; // 类型是否有交叉
        if((flag[i] == 0) && 0 != tmp) {    // 尚未加入到最小生成树中且类型有交叉
            for(j = 0; j < TYPES; ++j) {
                if(((tmp & (0x01 << j)) != 0) && (g->vexs[minid].stroageinfo[j] > 0)) { // 上游的交叉类型要有数据
                    g->edge[minid][i] = g->edge[i][minid] = CONN;   //两顶点连接
                    if((min + g->edge[minid][i]) < dist[i]) {   // 修正当前最短路径和前驱顶点
                        dist[i] = min + g->edge[minid][i];
                        prev[i] = minid;
                    }
                    break;  // 只要有一种交叉类型有数据就联通
                }
            }
        }
    }
}



/**
 * Dijkstra最短路径。即，统计图(g)中"顶点vs"到其它各个顶点的最短路径。
 * @param g    图
 * @param vs   起始顶点(start vertex)。即计算"顶点vs"到其它顶点的最短路径
 * @param prev 前驱顶点数组。即，prev[i]的值是"顶点vs"到"顶点i"的最短路径所经历的全部顶点中，位于"顶点i"之前的那个顶点。
 * @param dist 长度数组。即，dist[i]是"顶点vs"到"顶点i"的最短路径的长度。
 * @return      返回最接近的未满容器的编号
 */
int dijkstra(Graph *g, int vs, int prev[], int dist[])
{
    int i, j;
    int min, minid;
    int flag[CNT];      // flag[i]=1表示"顶点vs"到"顶点i"的最短路径已成功获取。

    for(i = 0; i < g->vexnum; i++) {
        dist[i] = INF;
        flag[i] = 0;              // 顶点i的最短路径还没获取到。
        prev[i] = -1;              // 顶点i的前驱顶点为-1。
    }

    // 对"顶点vs"自身进行初始化
    minid = vs;
    min = 0;
    flag[vs] = 1;       // 先将vs加入到最小生成树中
    dist[vs] = 0;
    adjustEdge(g, prev, dist, flag, vs, min);     // 设置与根节点vs相连接的边和点


    // 如果在执行到某次时,找到了路径为1(直接下层)且满足条件的容器, 那就直接返回,因为执行算法还是有一定的时间开销
    for(i = 0; i < g->vexnum; ++i)
        if((dist[i] == CONN) && (g->vexs[i].len < SIZE))     // 在根节点的孩子节点里找到合适的容器直接返回
            return i;


    // 遍历g->vexnum-1次；每次找出一个顶点的最短路径。
    // 原始算法里,默认顶点0为起始节点,所以i是从1开始. 但是,这里指定了起始节点,所以,不该这样循环
    for(i = 0; i < g->vexnum; i++) {
        // 寻找当前最小的路径. 即，在未获取最短路径的顶点中，找到离vs最近的顶点(k)。
        min = INF;
        for(j = 0; j < g->vexnum; j++) {
            if(flag[j] == 0 && dist[j] < min) {
                min = dist[j];
                minid = j;
            }
        }

        // 标记"顶点minid"为已经获取到最短路径, 并调整minid所连接点的路径
        flag[minid] = 1;
        adjustEdge(g, prev, dist, flag, minid, min);
    }



    // 打印dijkstra最短路径的结果
    // printf("dijkstra(%c): \n", g->vexs[vs].name);
    // for (i = 0; i < g->vexnum; i++)
    // if(dist[i] != INF)
    // printf("  shortest(%c, %c)=%d\n", g->vexs[vs].name, g->vexs[i].name, dist[i]);

    min = INF;
    minid = -1;
    // 这个改进版的算法应该返回最短且没满的容器minid
    for(i = 0; i < g->vexnum; ++i) {
        if((dist[i] < min) && (g->vexs[i].len < SIZE)) {
            minid = i;
            min = dist[minid];
        }
    }
    return minid;
}





/**
 * 定制每个容器的存储情况.
 *  *
 * 基本存储类型:
 *      0   1   2   3   4   5   6   7
 * A0   9   1   1   9
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
void setStorageInfo_v1(Graph *g)
{
    // A
    g->vexs[0].stroageinfo[0] = 200;
    // g->vexs[0].stroageinfo[1] = 96;
    // g->vexs[0].stroageinfo[2] = 48;
    g->vexs[0].stroageinfo[3] = 56;
    g->vexs[0].len = 256;

    // B
    // g->vexs[1].stroageinfo[0] = 16;
    // g->vexs[1].stroageinfo[2] = 48;
    g->vexs[1].stroageinfo[4] = 256;
    // g->vexs[1].stroageinfo[5] = 48;
    g->vexs[1].len = 256;

    // C
    // g->vexs[2].stroageinfo[0] = 80;
    // g->vexs[2].stroageinfo[2] = 32;
    g->vexs[2].stroageinfo[4] = 256;
    // g->vexs[2].stroageinfo[5] = 48;
    // g->vexs[2].stroageinfo[6] = 64;
    g->vexs[2].len = 256;

    // D
    // g->vexs[3].stroageinfo[2] = 32;
    // g->vexs[3].stroageinfo[4] = 64;
    g->vexs[3].stroageinfo[5] = 56;
    // g->vexs[3].stroageinfo[6] = 32;
    // g->vexs[3].stroageinfo[7] = 112;
    g->vexs[3].len = 56;

    // E
    // g->vexs[4].stroageinfo[2] = 32;
    // g->vexs[4].stroageinfo[4] = 32;
    // g->vexs[4].stroageinfo[5] = 64;
    // g->vexs[4].stroageinfo[6] = 64;
    g->vexs[4].stroageinfo[7] = 256;
    g->vexs[4].len = 256;

    // F
    g->vexs[5].stroageinfo[0] = 256;
    // g->vexs[5].stroageinfo[1] = 48;
    // g->vexs[5].stroageinfo[2] = 64;
    // g->vexs[5].stroageinfo[5] = 48;
    // g->vexs[5].stroageinfo[6] = 48;
    g->vexs[5].len = 256;

    // G
    // g->vexs[6].stroageinfo[1] = 64;
    // g->vexs[6].stroageinfo[2] = 48;
    // g->vexs[6].stroageinfo[5] = 32;
    g->vexs[6].stroageinfo[7] = 256;
    g->vexs[6].len = 256;

    // H
    // g->vexs[7].stroageinfo[1] = 32;
    // g->vexs[7].stroageinfo[2] = 48;
    // g->vexs[7].stroageinfo[5] = 64;
    g->vexs[7].stroageinfo[7] = 256;
    g->vexs[7].len = 256;


}





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
void initTypes(Graph *g)
{
    /*设定可存储类型
    stroageinfo: 表示存储信息  -1: 不允许放; 0: 允许放但未放; >0: 已经存放条数
    */
    // A
    g->vexs[0].types |= (0x01 << 0); // 位0
    g->vexs[0].types |= (0x01 << 1); // 位1
    g->vexs[0].types |= (0x01 << 2); // 位2
    g->vexs[0].types |= (0x01 << 3); // 位3
    // B
    g->vexs[1].types |= (0x01 << 0); // 位0
    g->vexs[1].types |= (0x01 << 2); // 位2
    g->vexs[1].types |= (0x01 << 4); // 位4
    g->vexs[1].types |= (0x01 << 5); // 位5
    g->vexs[1].types |= (0x01 << 6); // 位6
    // C
    g->vexs[2].types |= (0x01 << 0); // 位0
    g->vexs[2].types |= (0x01 << 2); // 位2
    g->vexs[2].types |= (0x01 << 4); // 位4
    g->vexs[2].types |= (0x01 << 5); // 位5
    g->vexs[2].types |= (0x01 << 6); // 位6
    // D
    g->vexs[3].types |= (0x01 << 2); // 位2
    g->vexs[3].types |= (0x01 << 4); // 位4
    g->vexs[3].types |= (0x01 << 5); // 位5
    g->vexs[3].types |= (0x01 << 6); // 位6
    g->vexs[3].types |= (0x01 << 7); // 位7
    // E
    g->vexs[4].types |= (0x01 << 2); // 位2
    g->vexs[4].types |= (0x01 << 4); // 位4
    g->vexs[4].types |= (0x01 << 5); // 位5
    g->vexs[4].types |= (0x01 << 6); // 位6
    g->vexs[4].types |= (0x01 << 7); // 位7
    // F
    g->vexs[5].types |= (0x01 << 0); // 位0
    g->vexs[5].types |= (0x01 << 1); // 位1
    g->vexs[5].types |= (0x01 << 2); // 位2
    g->vexs[5].types |= (0x01 << 5); // 位5
    g->vexs[5].types |= (0x01 << 6); // 位6
    // G
    g->vexs[6].types |= (0x01 << 1); // 位1
    g->vexs[6].types |= (0x01 << 2); // 位2
    g->vexs[6].types |= (0x01 << 5); // 位5
    g->vexs[6].types |= (0x01 << 7); // 位7
    // H
    g->vexs[7].types |= (0x01 << 1); // 位1
    g->vexs[7].types |= (0x01 << 2); // 位2
    g->vexs[7].types |= (0x01 << 5); // 位5
    g->vexs[7].types |= (0x01 << 7); // 位7
}



/**
 * 初始化各个容器存储情况为0, 这个函数不应该被更改
 * @param g 图
 */
 void initStroageInfo(Graph *g)
{
    //A
    g->vexs[0].stroageinfo[0] = 0;  // 位0
    g->vexs[0].stroageinfo[1] = 0;  // 位1
    g->vexs[0].stroageinfo[2] = 0;  // 位2
    g->vexs[0].stroageinfo[3] = 0;  // 位3
    g->vexs[0].len = 0;
    //B
    g->vexs[1].stroageinfo[0] = 0;  // 位0
    g->vexs[1].stroageinfo[2] = 0;  // 位2
    g->vexs[1].stroageinfo[4] = 0;  // 位4
    g->vexs[1].stroageinfo[5] = 0;  // 位5
    g->vexs[1].stroageinfo[6] = 0;  // 位6
    g->vexs[1].len = 0;
    //C
    g->vexs[2].stroageinfo[0] = 0;  // 位0
    g->vexs[2].stroageinfo[2] = 0;  // 位2
    g->vexs[2].stroageinfo[4] = 0;  // 位4
    g->vexs[2].stroageinfo[5] = 0;  // 位5
    g->vexs[2].stroageinfo[6] = 0;  // 位6
    g->vexs[2].len = 0;
    //D
    g->vexs[3].stroageinfo[2] = 0;  // 位2
    g->vexs[3].stroageinfo[4] = 0;  // 位4
    g->vexs[3].stroageinfo[5] = 0;  // 位5
    g->vexs[3].stroageinfo[6] = 0;  // 位6
    g->vexs[3].stroageinfo[7] = 0;  // 位7
    g->vexs[3].len = 0;
    //E
    g->vexs[4].stroageinfo[2] = 0;  // 位2
    g->vexs[4].stroageinfo[4] = 0;  // 位4
    g->vexs[4].stroageinfo[5] = 0;  // 位5
    g->vexs[4].stroageinfo[6] = 0;  // 位6
    g->vexs[4].stroageinfo[7] = 0;  // 位7
    g->vexs[4].len = 0;
    //F
    g->vexs[5].stroageinfo[0] = 0;  // 位0
    g->vexs[5].stroageinfo[1] = 0;  // 位1
    g->vexs[5].stroageinfo[2] = 0;  // 位2
    g->vexs[5].stroageinfo[5] = 0;  // 位5
    g->vexs[5].stroageinfo[6] = 0;  // 位6
    g->vexs[5].len = 0;
    //G
    g->vexs[6].stroageinfo[1] = 0;  // 位1
    g->vexs[6].stroageinfo[2] = 0;  // 位2
    g->vexs[6].stroageinfo[5] = 0;  // 位5
    g->vexs[6].stroageinfo[7] = 0;  // 位7
    g->vexs[6].len = 0;
    //H
    g->vexs[7].stroageinfo[1] = 0;  // 位1
    g->vexs[7].stroageinfo[2] = 0;  // 位2
    g->vexs[7].stroageinfo[5] = 0;  // 位5
    g->vexs[7].stroageinfo[7] = 0;  // 位7
    g->vexs[7].len = 0;

}



/**
 * 将所有的边初始化为无穷大
 * @param g 图
 */
void initEdge(Graph *g)
{
    int i, j;
    for(i = 0; i < CNT; i++) {
        g->edge[i][i] = 0;  // 自己到自己当然为0
        for(j = i + 1; j < CNT; j++) {
            g->edge[i][j] = g->edge[j][i] = INF;
        }
    }
}


/**
 * 初始化图
 * @param g 图
 */
void initGraph(Graph *g)
{
    int i, j;
    /* 初始化图 */
    g->vexnum = CNT;
    for(i = 0; i < CNT; ++i) {
        // 先全部初始化为0
        memset(&g->vexs[i], 0, sizeof(struct cont));

        // 设置名称
        g->vexs[i].name = 'A' + i;
        // 初始化不存储任何信息
        //存储信息  -1: 不允许放; 0: 允许放但未放; >0: 已经存放条数
        for(j = 0; j < TYPES; ++j) {
            g->vexs[i].stroageinfo[j] = -1;
        }
    }
    initTypes(g);       // 设定容器存储类型
    initStroageInfo(g); //初始化各个容器存储情况为0
    initEdge(g);// 先将所有的边初始化为无穷大
}






/**
 * 判断两个容器是否联通，只需要联通就好
 * @param  p1 容器1
 * @param  p2 容器2
 * @return    返回CNT，不连通；返回1，联通
 */
int isConnect(VertexType *p1, VertexType *p2)
{
    int i = 0;
    if (p1 == p2)   return INF;
    for(i = 0; i < TYPES; ++i)
        if(p1->stroageinfo[i] * p1->stroageinfo[i] != 0 )
            return 1;
    return INF;
}



/**
 * 打印从节点n到根节点的路径
 * @param g    图
 * @param prev 前驱顶点数组。即，prev[i]的值是"顶点vs"到"顶点i"的最短路径所经历的全部顶点中，位于"顶点i"之前的那个顶点。
 * @param n    终点节点
 */
void printPath(Graph *g, int prev[], int n)
{
    int f = n;
    // int tmp[CNT];
    int i = -1;
    while(prev[f] != -1) {
        // tmp[++i] = g->vexs[f].name;
        printf("%c->", g->vexs[f].name);
        f = prev[f];
    }
    printf("%c\n", g->vexs[f].name);
    // tmp[++i] = g->vexs[f].name;
    // for(; i>0; --i)
    // printf("%c->",tmp[i]);
    // printf("%c\n",tmp[i]);
}



/**
 * 打印所有容器的存储情况
 * @param g 图
 */
void printStroageInfo(Graph *g)
{
    int i, j;
    // 输出stroageinfo信息
    printf("stroageinfo信息:\n\t\t\t类型\t\n");
    printf("%12c", ' ');
    for(i = 0; i < CNT; i++)
        printf("%4c", i + 'A');
    printf("  len\n");
    for(i = 0; i < CNT; i++) {
        if(g->vexs[i].len == SIZE)  printf("容器%c(已满):", g->vexs[i].name);
        else    printf("容器%c(未满):", g->vexs[i].name);
        // for(j = 0; j < TYPES; ++j) {
        // 若是两个容器的相同类型数据都不为零, 则联通
        for(j = 0; j < CNT; ++j)
            printf("%4d", g->vexs[i].stroageinfo[j]);
        printf("%5d\n", g->vexs[i].len);
        printf("\n");
    }
}