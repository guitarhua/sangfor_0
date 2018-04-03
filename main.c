#include "options.h"


void init(int prev[], int dist[])
{
    int i;
    for(i = 0; i < CNT; ++i) {
        prev[i] = -1;
        dist[i] = INF;  // 顶点i的前驱顶点为-1。
    }
}

int main()
{
    Graph g;
    // char in;
    int flag = 0;
    int n = 0, i;
    int prev[CNT];  //前驱顶点数组。即，prev[i]的值是"顶点vs"到"顶点i"的最短路径所经历的全部顶点中，位于"顶点i"之前的那个顶点。
    int dist[CNT];  //长度数组。即，dist[i]是"顶点vs"到"顶点i"的最短路径的长度。
    init(prev, dist);
    initGraph(&g);

    srand((unsigned int)time(NULL)); // 时间做种,每次产生不一样的随机数序列
    if(flag == 0) {
        initStroageInfo(&g); //初始化各个容器存储情况为0
        setStorageInfo_v1(&g);    // 设定容器存储情况
        printStroageInfo(&g);    // 输出容器存储情况
        n = rand() % 8;
        n = 3;
        printf("要插入的数据: %c\n", n + 'A');
        findNiche(&g, n, prev, dist);
        printf("\n\n\n");
    } else if(flag == 1) {
        for(i = 0; i < 3; ++i) {

            initStroageInfo(&g); //初始化各个容器存储情况为0
            setStorageInfo(&g);    // 设定容器存储情况
            printStroageInfo(&g);    // 输出容器存储情况
            init(prev, dist);

            n = rand() % 8;
            printf("要插入的数据: %c\n", n + 'A');
            findNiche(&g, n, prev, dist);
            printf("\n\n\n");
        }
    } else if(flag == 2) {
        initStroageInfo(&g); //初始化各个容器存储情况为0
        setStorageInfo_v1(&g);    // 设定容器存储情况
        printStroageInfo(&g);    // 输出容器存储情况
        n = 3;
        printf("要插入的数据: %c\n", n + 'A');
        findNiche(&g, n, prev, dist);
        printf("\n\n\n");
    }

    return 0;
}

