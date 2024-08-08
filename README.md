# Blinn-Phong Reflectance Model
- 镜面高光（specular highlights）
  ![image](https://github.com/user-attachments/assets/dfbe263a-91e7-4d6a-b5b6-5a21f44297da)

- 漫反射（diffuse reflection）
  ![image](https://github.com/user-attachments/assets/2db19bba-379c-4589-a4db-de10339535fa) 


- 环境光照（ambient lighting）
  ![image](https://github.com/user-attachments/assets/21f6a42f-e811-4156-a35c-675bf1948d02)

# 切线空间
切线空间是相对于某个表面定义的局部坐标系，常用于法线贴图和凹凸贴图。在这个坐标系中，表面的法线通常是（0, 0, 1），切线和副切线分别定义了表面的两个方向（通常与纹理坐标的U和V方向对应）。
# TBN矩阵
TBN矩阵包含三个向量：

Tangent（T）: 沿着表面U方向的向量。

Bitangent（B）: 沿着表面V方向的向量（也叫副切线）。

Normal（N）: 表面的法线向量。

![image](https://github.com/user-attachments/assets/b787bffd-cafb-4c59-93a4-dbe31b793f29)
```math
\mathbf{E}_1 = \Delta U_1 T + \Delta V_1 B
```

```math
\mathbf{E}_2 = \Delta U_2 T + \Delta V_2 B
```
两个方程两个未知数（T和B）就可以求解

计算出TBN三个轴后，这三个轴对于面内所有三角形的所有点是共用的，所以一个面只需要找一个三角形计算TBN即可
# 法线贴图
常见的法线贴图处理方式
 - 直接使用法线贴图：

在简单的法线贴图处理中，可以直接从法线贴图获取切线空间的法线，然后将其转换为模型空间或世界空间。这种方法不涉及对法线向量的进一步扰动，通常用于实现基本的法线贴图效果。
典型步骤：

从法线贴图中获取法线向量。

将法线向量转换到模型空间或世界空间。

使用转换后的法线向量进行光照计算。
 - 使用扰动法线向量：

通过扰动表面的法线向量来实现更复杂的效果，比如凹凸贴图（Bump Mapping）。这种方法不仅需要法线贴图，还需要基于纹理坐标的导数（如 dU 和 dV）来计算法线的扰动，从而实现对表面凹凸的模拟。
典型步骤：

计算 dU 和 dV，得到法线扰动。

结合 dU、dV 以及原始法线向量，生成新的扰动法线向量 ln。

    dU = kh * kn * (color(u+1/w,v)-color(u,v))，
    dV = kh * kn * (color(u,v+1/h)-color(u,v))，
    Vector ln = (-dU, -dV, 1)
    Normal n = normalize(TBN * ln)
    
使用 TBN 矩阵将 ln 转换到模型空间或世界空间。

使用转换后的法线向量进行光照计算。


 - 位移映射（Displacement Mapping）
 通过直接修改几何体的顶点，使得表面形状真正发生变化，增加或减少表面的细节。

典型步骤：

计算 dU 和 dV，得到法线扰动。

结合 dU、dV 以及原始法线向量，生成新的扰动法线向量 ln。

将ln转换回模型空间并归一化。接着，更新顶点位置 point，将其沿着新的法线方向进行位移，位移量由高度图的值控制。

    normal = (TBN * ln).normalized();
    point += kn * normal * payload.texture->getColor(u, v).norm();

然后再对每个点运用光照模型即可

作业最后的双线性插值是简单的，就不多赘述了
