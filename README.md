# 一些物理量
- 立体角 (Solid Angles)
 ![image](https://github.com/user-attachments/assets/a0f6120c-6c66-459c-9ba1-dc0a2e4d77ff)

- Irradiance
表示单位面积上的辐射通量。
  
$$
E(x) = \frac{d\Phi(x)}{dA}
$$
- Radiant
表示单位立体角，单位投影面积上的辐射通量。

这里多出来一个COS是因为Lamber's Cosine Law，即阳光在斜照的时候强度会由于倾角变成原来的COS倍。

$$
L(p, \omega) = \frac{d^2\Phi(p,\omega)}{d\omega \cdot dA \cos \theta}
$$

#  Bidirectional Reflectance Distribution Function (BRDF)

![image](https://github.com/user-attachments/assets/d9a61a8f-1490-4699-a8c3-5ed661a067ca)

# 渲染方程

发射光=自身发光+接受光，cos用点积代替（假设所有方向都朝外）

$$
L_o(p, \omega_o) = L_e(p, \omega_o) + \int_{\Omega_+} L_i(p, \omega_i) f_r(p, \omega_i, \omega_o) (n \cdot \omega_i) d\omega_i
$$

它最后可以改写成

$$
L=E+KE+K_
2
 E+K_
3
 E+……
$$

其物理意义：

E为直接从该物体发出的光源

KE为光源经过该物体一次反射（弹射一次）「到这一步是着色能做到的，之后就不好做到」

K^2E为已经经过反射或折射的光经过该物体一次反射（弹射两次）
……（弹射多次）

# Path Tracing
## 直接光照
考虑光路直接从光源发出，就能得到直接光照的结果

    void shade(p, wo)
    {
        //随机生成N个方向的wi并以pdf(wi)分布
        L0 = 0.0;
        //对每条wi
        For each wi
        {
            //向wi方向追踪一条射线ray r(p,wi)
            Trace a ray r(p,wi)
            //如果射线击中光源（相交）
            If ray r hit the light
                //写出求和式
                L0 += (1 / N) * L_i * f_r * cosine / pdf(wi)
        }
        return L0;
    }

## 间接光照
光线直接打到Q点后反射到P点，计算P点接收Q点反射光后的反射光
### 初步想法
像直接光照一样计算
### 存在的问题
 - 问题一：递归爆炸
   
每个反射的光线再次反射后会产生N倍的光线，造成计算爆炸

解决办法：每个光线只反射一次
 - 问题二：没有停止条件

解决方法：俄罗斯轮盘赌算法

 - 问题三：算法低效
即使解决了上面的两个问题，现在这种从着色点向外出射光线的采样方法，打中光源的概率完全看运气，光源面积大概率就大；光源面积小概率就小，由此可见这种方法并不高效。
![image](https://github.com/user-attachments/assets/083ffb55-3876-4726-bdd8-4f6c8fc5d5a9)

解决办法：采样光源

对场景里的所有光源进行采样。原本的方法是对立体角 $w_i$ 积分的，因此需要先转换

$$
d\omega = \frac{dA \cos \theta'}{\|x' - x\|^2}
$$

![image](https://github.com/user-attachments/assets/d3102a3c-939e-4811-a53a-c652328430b3)

$$
L_o(x, \omega_o) = \int_{\Omega_+} L_i(x, \omega_i) f_r(x, \omega_i, \omega_o) \frac{\cos\theta \cos\theta'}{d_{is}(x, x')^2} dA
$$

$$
= \frac{A}{N} \sum_{i=1}^N L_i(p, \omega_i) f_r(p, \omega_i, \omega_o) \frac{\cos\theta \cos\theta'}{d_{is}(x, x')^2}
$$

采样过程伪代码：

    shade (p, wo)
    //直接光照
    //在光源上均匀选择一个采样点x'，以pdf_light分布
    Uniformly sample the light at x' ( pdf_light = 1 / A)
    //首先判断光源是否被遮挡：发射一条连接物体p和x'的射线
    Shoot a ray from p to x'
    //如果射线不被遮挡，则计算直接光照：
    If the ray is not blocked in the middle
        L_dir = L_i * f_r * cos_theta * cos_theta' / |x'-p|^2 / pdf_light
    
    //间接光照
    L_indir = 0.0
    //以某种方法确定一个概率P_RR(0<P_RR<1)
    Test Russian Roulette with probability P_RR
    //在均匀分布在[0,1]上的样本中随机选取一个值ksi
    If ksi>P_RR
        Return L_dir;
    //半球上随机生成一个方向wi，以pdf=1/2Π分布
    Uniformly sample the hemisphere toward wi ( pdf_hemi = 1 / 2pi)
    Trace a ray r(p, wi)
    //如果射线r击中一个不自发光的物体（非光源）
    If ray r hit a non - emitting object at q    
        L_indir = shade (q, -wi) * f_r * cos_theta / pdf_hemi / P_RR
 
    Return L_dir + L_indir
