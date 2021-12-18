"""
Morphology Image Processing
(Due date: Nov. 26, 11:59 P.M., 2021)

The goal of this task is to experiment with commonly used morphology
binary image processing techniques. Use the proper combination of the four commonly used morphology operations, 
i.e. erosion, dilation, open and close, to remove noises and extract boundary of a binary image. 
Specifically, you are given a binary image with noises for your testing, which is named 'task3.png'.  
Note that different binary image might be used when grading your code. 

You are required to write programs to: 
(i) implement four commonly used morphology operations: erosion, dilation, open and close. 
    The stucturing element (SE) should be a 3x3 square of all 1's for all the operations.
(ii) remove noises in task3.png using proper combination of the above morphology operations. 
(iii) extract the boundaries of the objects in denoised binary image 
      using proper combination of the above morphology operations. 
Hint: 
• Zero-padding is needed before morphology operations. 

Do NOT modify the code provided to you.
You are NOT allowed to use OpenCV library except the functions we already been imported from cv2. 
You are allowed to use Numpy libraries, HOWEVER, 
you are NOT allowed to use any functions or APIs directly related to morphology operations.
Please implement erosion, dilation, open and close operations ON YOUR OWN.
"""

from cv2 import imread, imwrite, imshow, IMREAD_GRAYSCALE, namedWindow, waitKey, destroyAllWindows
import numpy as np
import os
SED= np.array([[1,1,1], [1,1,1],[1,1,1]])

def morph_erode(image):
    img_final = np.zeros((len(image)+2,len(image[0])+2))
    imgg = np.zeros((len(image),len(image[0])))
    img_final[1:-1, 1:-1] = image
    m,n = image.shape
    for i in range(m):
        for j in range(n):
            imgg[i, j] = (SED * img_final[i: i+3, j: j+3]).sum()
            if(imgg[i][j]<2295):
                imgg[i][j] = 0
            else:
                imgg[i][j] = 255
    return imgg

def morph_dilate(image):

    img_final = np.zeros((len(image)+2,len(image[0])+2))
    imgg = np.zeros((len(image),len(image[0])))
    img_final[1:-1, 1:-1] = image
    m,n = image.shape
    for i in range(m):
        for j in range(n):
            imgg[i, j] = (SED * img_final[i: i+3, j: j+3]).sum()
            if(imgg[i][j]>0):
                imgg[i][j] = 255
            else:
                imgg[i][j] = 0
    return imgg




def morph_open(img):

    denoise_img = morph_erode(img)
    #imwrite('results/task3_denoise_erode1.jpg', denoise_img)
    denoise_img = morph_dilate(denoise_img)
    #imwrite('results/task3_desnoise_dilate1.jpg',denoise_img)
    return denoise_img


def morph_close(img):

    img = morph_dilate(img)
    #imwrite('results/task3_desnoise_dilate2.jpg',img)
    close_img = morph_erode(img)
    #imwrite('results/task3_denoise_erode2.jpg',close_img)
    return close_img
    
    


def denoise(img):

    #img = imread('task3.png', IMREAD_GRAYSCALE)
    denoise_img = morph_open(img)
    denoise_img = morph_close(denoise_img)
    return denoise_img




def boundary(img):

    denoise_img = morph_erode(img)
    bound_img = img - denoise_img
    return bound_img


if __name__ == "__main__":
    img = imread('task3.png', IMREAD_GRAYSCALE)
    denoise_img = denoise(img)
    imwrite('results/task3_denoise.jpg',denoise_img)
    bound_img = boundary(denoise_img)
    imwrite('results/task3_denoisebound.jpg', bound_img)





