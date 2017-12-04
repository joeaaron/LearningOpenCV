#Playing with OpenCV

### 一、从双目立体视差图中重建三维点云 ###
<center>
![](http://img.blog.csdn.net/20160926195440706)
</center>



1.【[视差与深度信息](http://www.360doc.com/content/14/0205/15/10724725_349968116.shtml)】

2.【[用VS+Opencv3.1从双目立体视差图中重建三维点云](http://blog.csdn.net/u014283958/article/details/52673304)】

----------
### 二、斑点检测 ###

Opencv中提供了SimpleBlobDetector的特征点检测方法。

<center>
![](http://img.blog.csdn.net/20170530204013167?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvR29vZF9Cb3l6cQ==/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)
</center>

#### Reference: ####
1.【 [Opencv中SimpleBlobDetector的使用（斑点检测）](http://blog.csdn.net/good_boyzq/article/details/72811687)】

2.【[Opencv2.4.9源码分析——SimpleBlobDetector](http://blog.csdn.net/zhaocj/article/details/44886475)】

----------
### 三、寻找图像中最亮的点 ###

逐行遍历，计算像素值最大的点。

----------
### 四、用OpenCV实现Photoshop算法 ###
1.图像旋转

- warpAffine
- getRotationMatrix2D

2.图像剪切

- MouseCallBack
- rect

3.图像匹配

- matchTemplate
- minMaxLoc

4.抠图

- imgmask
- floodFill

5.亮度对比度调整

- adjustBrightnessContrast

6.毛玻璃效果

<center>
![](http://img0.ph.126.net/o0Yr53dFETDCjcZVeaqR5w==/6632266233376805322.png)
</center>

7.图像修复

<center>
![](http://img1.ph.126.net/_h_sX2gpYYf-SM9uXMeOuQ==/6632607081981437701.png)
</center>

8.图像取反

<center>
![](http://img0.ph.126.net/cwNVkFXf9Xthq3tSSP7duQ==/6632604882958156521.png)
</center>

9.图像马赛克
<center>
![](http://img2.ph.126.net/5LouijW69L0PjarJVCCbuQ==/91197892473663194.png)
</center>

#### Reference: ####
1.【[用OpenCV实现Photoshop算法](http://blog.csdn.net/c80486/article/details/52505061)】

----------
### 五、用OpenCV实现Photoshop算法 ###

自己需要用写了一个，和Matlab中bwareaopen的效果应该是几乎一致的，依赖于opencv新函数connectedComponentsWithStats，再也不用先找轮廓再填充了。

<center>
![](http://img0.ph.126.net/8TFOYaoH500ifFlyzH-CGA==/6632445453772137000.jpg)
![](http://img2.ph.126.net/1sfEiE8LFKUjrqoi4VRR7w==/1285777693633699196.jpg)
</center>
----------
### 六、单目相机标定 ###

输入5张图，得到`out_camera_data.xml`,采用张正友标定算法。

----------
### 七、扫描全能王软件 ###
相信很多人手机里都装了个“扫描全能王”APP，平时可以用它来可以扫描一些证件、文本，确实很好用，第一次用的时候确实感觉功能很强大啊算法很牛逼啊。但是仔细一想，其实这些实现起来也是很简单的，我想了下，实现的步骤应该就只有下面三个：

- 将证件轮廓找到
- 提取证件矩形轮廓四点进行透视变换
- 二值化
<center>
![](http://img.blog.csdn.net/20170912174818019?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvbHh5XzIwMTE=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)
</center>

#### Reference: ####
1.[[OpenCV探索之路（二十二）：制作一个类“全能扫描王”的简易扫描软件](http://www.cnblogs.com/skyfsm/p/7324346.html)】

----------

### 八、生成棋盘格 ###
<center>
![](http://img2.ph.126.net/Ovzf1YI4msrkrJeex4Py7g==/6632525718120958625.jpg)
</center>

----------

### 九、图片校正 ###

对任意倾斜的图片，校正到水平位置。
<center>
![](http://img0.ph.126.net/ioOfVkOuzTZxI1L3LiNDHA==/6632589489795371670.png)
![](http://img2.ph.126.net/p1ZJ1XJgaMzDKzYXxZ5rHA==/6632269531911688526.jpg)
</center>

----------
### 十、腐蚀膨胀 ###

- getStructuringElement
- erode
- dilate

----------
### 十一、找到图片中的方块 ###
<center>
<img src="https://raw.github.com/alyssaq/opencv/master/imgs/2stickies.jpg" width="45%" /> 
&nbsp;&nbsp;&nbsp; 
<img src="https://raw.github.com/alyssaq/opencv/master/imgs/out-2stickies.jpg" width="45%"/>

<br>

<img src="https://raw.github.com/alyssaq/opencv/master/imgs/manyStickies.jpg" width="45%" /> 
&nbsp;&nbsp;&nbsp; 
<img src="https://raw.github.com/alyssaq/opencv/master/imgs/out-manyStickies.jpg" width="45%"/>

</center>
----------
### 十二、图像拼接 ###
图像拼接在实际的应用场景很广，比如无人机航拍，遥感图像等等，图像拼接是进一步做图像理解基础步骤，拼接效果的好坏直接影响接下来的工作，所以一个好的图像拼接算法非常重要。

再举一个身边的例子吧，你用你的手机对某一场景拍照，但是你没有办法一次将所有你要拍的景物全部拍下来，所以你对该场景从左往右依次拍了好几张图，来把你要拍的所有景物记录下来。那么我们能不能把这些图像拼接成一个大图呢？我们利用opencv就可以做到图像拼接的效果！
#### Reference: ####
1.【[OpenCV探索之路（二十四）图像拼接和图像融合技术](http://www.cnblogs.com/skyfsm/p/7411961.html)】

----------
### 十三、寻找最大内切圆 ###
- FindBigestContour
- pointPolygonTest

<center>
![](http://img1.ph.126.net/Vgf0abm6mGJeKP-nlX2snA==/2605050909476536579.jpg)
</center>

----------
### 十四、拼图游戏 ###

- 随机调换所有的子图像序列的位置，用于在 Splite image中显示
- 鼠标回调函数，用于获取需要查找的子图像在原图像中的位置，并叠加显示在目标图像中

#### Reference: ####
1.【[儿时经典“手游”—拼图板小游戏Opencv实现](http://tieba.baidu.com/p/4702664058)】

----------
### 十五、获得物体的主要方向 ###
- 分别获得每个工件的轮廓；
- 处理每个轮廓，采用pca(主成分分析）方法，获得所有轮廓点的集合的中点，主要方向等信息；
- 绘图并返回结果。
#### Reference: ####
1.【[如何获得物体的主要方向](http://www.cnblogs.com/jsxyhelu/p/7690699.html)？】 

----------

### 十六、图像细化 ###
<center>
![](http://img2.ph.126.net/-Gamt5EA655zN9M7GqCVjg==/6632574096632575397.png)
![](http://img0.ph.126.net/4iGNvQsPyB2CR4Kvxwuaeg==/6632538912260488725.png)
</center>

#### Reference: ####
1.[http://wenku.baidu.com/link?url=EWMEJdckKwG-Ws4cG1iGCoggiClRA98TsFglT8keOGUZC-9gLLw4x_TjFRLAMYvFvt0kc6-wREScy674VKGakurs-iAOr4XZ0Xdx-PtoEga](http://wenku.baidu.com/link?url=EWMEJdckKwG-Ws4cG1iGCoggiClRA98TsFglT8keOGUZC-9gLLw4x_TjFRLAMYvFvt0kc6-wREScy674VKGakurs-iAOr4XZ0Xdx-PtoEga "Zhang快速并行细化算法")

2.[细化算法](http://blog.sina.com.cn/s/blog_6f611c3001017y5m.html)

3.[论文 A fast parallel algorithm for thinning digital patterns](http://wenku.baidu.com/link?url=CUrmS4LgO7DAL7GFl4yWZLkghCfvYnR3VvVsr3gj3lF_VTtS97nFz3B_1q2iXQDLObX_4oPM_qBPUPkdGMk6gXw7NdFP5spItERvqxfbK6W)

----------


