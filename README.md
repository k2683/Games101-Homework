# 判断像素中心点是否在三角形内部
   - 计算以下向量的叉乘：
     ```math
      \overrightarrow{AB} \times \overrightarrow{AP} 
     ```
     ```math
      \overrightarrow{BC} \times \overrightarrow{BP}
     ```
     ```math
      \overrightarrow{CA} \times \overrightarrow{CP} 
     ```
  如果三个叉乘的符号一致或为零，则点 \( P \) 在三角形 \( \triangle ABC \) 内或边上。

  如果符号不同，则点 \( P \) 在三角形外部。
  - 对于任意维度的任意多边形:
    可以从点P发射一条射线，计算射线与多边形各边的交点数。

    如果射线与多边形边的交点数为奇数，则点 
P 在多边形内；如果为偶数，则点 
P 在多边形外。
# 深度插值
在三角形中，三角形内部的任何一个点都可以表示为由三角形的三个顶点A,B,C的的线性组合(x,y)=αA+βB+γC

![image](https://github.com/user-attachments/assets/715a9ece-30a8-4d6e-9d90-e2f24b51156a)

该线性组合满足系数α，β，γ加起来一定等于1，α，β，γ都是非负数。

计算公式如下：

$$
\beta = \frac{(y - y_A)(x_c - x_A) - (x - x_A)(y_c - y_A)}{(y_B - y_A)(x_c - x_A) - (x_B - x_A)(y_c - y_A)}
$$

$$
\gamma = \frac{(y - y_A)(x_B - x_A) - (x - x_A)(y_B - y_A)}{(y_c - y_A)(x_B - x_A) - (x_c - x_A)(y_B - y_A)}
$$

$$
\alpha = 1 - \beta - \gamma
$$

深度插值公式为：

$$
\frac{1}{Z} = \alpha \frac{1}{Z_A} + \beta \frac{1}{Z_B} + \gamma \frac{1}{Z_C}
$$

![image](https://github.com/user-attachments/assets/99f8cdd0-e55f-4b28-98e4-374bd3c5ea0a)

更通用的：任意属性插值公式： I 为三角形内目标点的目标属性

$$
I = Z \cdot \left(\alpha' \frac{I_A}{Z_A} + \beta' \frac{I_B}{Z_B} + \gamma' \frac{I_C}{Z_C}\right)
$$

I：屏幕空间三角形内部点的目标属性

I_A，I_B，I_C：三个顶点的对应属性

Z_A，Z_B，Z_C：三个顶点的顶点空间的Z值（投影前的）

# MSAA
## 在每个采样点周围多采几个样作为该点颜色的平均值
仅仅做到这样的话，三角形交界边缘会有黑边。因为交界处被另一个三角形覆盖的部分的深度不够不能覆盖这个像素的颜色。
## 建立四倍大的深度值buffer记录颜色，在该buffer里采样颜色
