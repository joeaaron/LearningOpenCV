#Playing with C++ OpenCV

**Aim 1:** Find squares in an image

##To compile and run

    > cmake .  
    > make
    > ./squares


###Sample input and output images in /imgs


<img src="https://raw.github.com/alyssaq/opencv/master/imgs/2stickies.jpg" width="45%" /> 
&nbsp;&nbsp;&nbsp; 
<img src="https://raw.github.com/alyssaq/opencv/master/imgs/out-2stickies.jpg" width="45%"/>

<br>

<img src="https://raw.github.com/alyssaq/opencv/master/imgs/manyStickies.jpg" width="45%" /> 
&nbsp;&nbsp;&nbsp; 
<img src="https://raw.github.com/alyssaq/opencv/master/imgs/out-manyStickies.jpg" width="45%"/>

###References - To improve results

http://stackoverflow.com/questions/8667818/opencv-c-obj-c-detecting-a-sheet-of-paper-square-detection   
http://stackoverflow.com/questions/10533233/opencv-c-obj-c-advanced-square-detection    
http://stackoverflow.com/questions/14221591/detection-square-opencv-and-c    
http://stackoverflow.com/questions/7755647/opencv-extract-area-of-an-image-from-a-vector-of-squares/7756000#7756000    
http://stackoverflow.com/questions/13523837/find-corner-of-papers/13532779#13532779    
http://opencv-code.com/tutorials/automatic-perspective-correction-for-quadrilateral-objects/    
http://opencv-code.com/tutorials/detecting-simple-shapes-in-an-image/   

----------

**Aim 2:** Search lines and circles in an image

Hough圆检测和线检测

----------

**Aim 3:** Thinning an image

   
###References - To improve results
[http://wenku.baidu.com/link?url=EWMEJdckKwG-Ws4cG1iGCoggiClRA98TsFglT8keOGUZC-9gLLw4x_TjFRLAMYvFvt0kc6-wREScy674VKGakurs-iAOr4XZ0Xdx-PtoEga](http://wenku.baidu.com/link?url=EWMEJdckKwG-Ws4cG1iGCoggiClRA98TsFglT8keOGUZC-9gLLw4x_TjFRLAMYvFvt0kc6-wREScy674VKGakurs-iAOr4XZ0Xdx-PtoEga "Zhang快速并行细化算法")

[细化算法](http://blog.sina.com.cn/s/blog_6f611c3001017y5m.html)

[论文 A fast parallel algorithm for thinning digital patterns](http://wenku.baidu.com/link?url=CUrmS4LgO7DAL7GFl4yWZLkghCfvYnR3VvVsr3gj3lF_VTtS97nFz3B_1q2iXQDLObX_4oPM_qBPUPkdGMk6gXw7NdFP5spItERvqxfbK6W)

----------

**Aim 4:** Calibrate the camera

**Aim 5:**  用OpenCV实现Photoshop算法

1.图像旋转

- warpAffine
- getRotationMatrix2D

2.图像剪切

- MouseCallBack
- rect