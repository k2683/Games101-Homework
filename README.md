# 实现三维中的绕z轴旋转
简单地应用旋转矩阵即可
# 从三维空间中构建透视投影矩阵
##正交矩阵
$$
M_{persp \to ortho} = \begin{bmatrix}
n & 0 & 0 & 0 \\
0 & n & 0 & 0 \\
0 & 0 & n + f & -nf \\
0 & 0 & 1 & 0
\end{bmatrix}
$$
