# 实现三维中的绕z轴旋转
简单地应用旋转矩阵即可
# 从三维空间中构建透视投影矩阵
## 正交投影(orthograhpics projection)
• 相机在坐标原点，向-Z轴看，up direction为Y

• 物体Z轴扔掉

• 平移缩放物体到[−1,1]^2范围（即x和y轴范围在-1和1之间）

$$
M_{ortho} = \begin{bmatrix}
\frac{2}{r-l} & 0 & 0 & 0 \\
0 & \frac{2}{t-b} & 0 & 0 \\
0 & 0 & \frac{2}{n-f} & 0 \\
0 & 0 & 0 & 1
\end{bmatrix}
\begin{bmatrix}
1 & 0 & 0 & -\frac{r+l}{2} \\
0 & 1 & 0 & -\frac{t+b}{2} \\
0 & 0 & 1 & -\frac{n+f}{2} \\
0 & 0 & 0 & 1
\end{bmatrix}
$$
## 透视投影(perspective projection)
• 把视锥体(frustum)压缩成立方体
```math
M_{\text{persp} \to \text{ortho}} = \begin{pmatrix}
n & 0 & 0 & 0 \\
0 & n & 0 & 0 \\
0 & 0 & n+f & -nf \\
0 & 0 & 1 & 0
\end{pmatrix}
```
[矩阵推导](正交投影.pdf)

• 进行正交投影
## 视角变换
    float t = -n * tan(fov / 2.);
    float b = -t;
    float r = aspect_ratio * t;
    float l = -r;
[图像说明](视角变换.pdf)
