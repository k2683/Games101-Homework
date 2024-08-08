![image](https://github.com/user-attachments/assets/ffc7724b-90bb-4cdc-8973-9b155c310c83)# 判断光线是否与包围盒相交
在x,y,z三个维度上寻找 $t_{min}$ 和 $t_{max}$

三个维度里最大的 $t_{min}$为入射时间，最小的 $t_{max}$ 为出射时间

如果 $t_{out}>t_{in}$ 且 $t_{out}>0$ 则有交点

# BVH
## BVH构建
BVHBuildNode* BVHAccel::recursiveBuild(std::vector<Object*> objects)
{
    BVHBuildNode* node = new BVHBuildNode(); // 创建一个新的BVH节点

    Bound bound = computeBound(objects); // 计算节点的包围盒
    node->bound = bound;

    if (objects.size() == 1)
    {
        // 如果只有一个对象，创建叶节点
        node->objects = objects;
        node->left = nullptr;
        node->right = nullptr;
    }
    else if (objects.size() == 2)
    {
        // 如果有两个对象，创建二叉树的叶节点
        node->left = recursiveBuild({objects[0]});
        node->right = recursiveBuild({objects[1]});
    }
    else
    {
        // 找到包围盒上最长的维度dim
        int dim = findLongestAxis(bound);

        // 按dim维度对对象进行排序
        std::sort(objects.begin(), objects.end(), [dim](Object* a, Object* b) {
            return getObjectCenter(a)[dim] < getObjectCenter(b)[dim];
        });

        // 将对象分成两部分
        std::vector<Object*> objects_left(objects.begin(), objects.begin() + objects.size() / 2);
        std::vector<Object*> objects_right(objects.begin() + objects.size() / 2, objects.end());

        // 递归构建左右子树
        node->left = recursiveBuild(objects_left);
        node->right = recursiveBuild(objects_right);
    }
    return node; // 返回构建好的节点
}

## 判断光线是否与BVH中的某个物体相交
1、当前节点所代表的大的包围盒与光线无交点：

则左右子树也不比再计算，可直接返回空交点

2、当前节点所代表的大的包围盒与光线有交点：

2.1、当前节点为叶子节点：

只需判断其包围盒内部的物体是否与光线相交

2.2、当前节点还含有左右子树：

递归判断左右子树的包围盒情况
# SAH
SAH是基于表面积的启发式评估划分方法（Surface Area Heuristic，SAH），因为有时候当物体分布不均匀时，划分结果可能会导致包围盒之间有很多重叠，这种重叠会导致后续求交冗余，而求交的代价要比遍历物体划分包围盒的代价高的多，所以这种方法即通过对求交代价和遍历代价进行评估，给出了每一种划分的代价（Cost），而我们的目的便是去寻找代价最小的划分。

所以该算法的核心在于使求交代价最低，而对于某一个大的包围盒，求交代价取决于当前包围盒的大小（影响光线击中包围盒的概率）和包围盒中的物体个数（多少个物体就意味着要求交多少次），现用表面积来代表包围盒大小并假设对每个物体求交的代价是相同的（设为1），并设遍历当前所有包围盒的代价为0.125（因为遍历代价小于求交代价），则可以得到：

