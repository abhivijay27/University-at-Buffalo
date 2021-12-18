"""
 Grayscale Image Processing
(Due date: Nov. 26, 11:59 P.M., 2021)

The goal of this task is to experiment with two commonly used 
image processing techniques: image denoising and edge detection. 
Specifically, you are given a grayscale image with salt-and-pepper noise, 
which is named 'task2.png' for your code testing. 
Note that different image might be used when grading your code. 

You are required to write programs to: 
(i) denoise the image using 3x3 median filter;
(ii) detect edges in the denoised image along both x and y directions using Sobel operators (provided in line 30-32).
(iii) design two 3x3 kernels and detect edges in the denoised image along both 45° and 135° diagonal directions.
Hint: 
• Zero-padding is needed before filtering or convolution. 
• Normalization is needed before saving edge images. You can normalize image using the following equation:
    normalized_img = 255 * frac{img - min(img)}{max(img) - min(img)}

Do NOT modify the code provided to you.
You are NOT allowed to use OpenCV library except the functions we already been imported from cv2. 
You are allowed to use Numpy for basic matrix calculations EXCEPT any function/operation related to convolution or correlation. 
You should NOT use any other libraries, which provide APIs for convolution/correlation ormedian filtering. 
Please write the convolution code ON YOUR OWN. 
"""

from cv2 import imread, imwrite, imshow, IMREAD_GRAYSCALE, namedWindow, waitKey, destroyAllWindows
import numpy as np

# Sobel operators are given here, do NOT modify them.
sobel_x = np.array([[1, 0, -1], [2, 0, -2], [1, 0, -1]]).astype(int)
sobel_y = np.array([[1, 2, 1], [0, 0, 0], [-1, -2, -1]]).astype(int)


def filter(img):
    """
    :param img: numpy.ndarray(int), image
    :return denoise_img: numpy.ndarray(int), image, same size as the input image

    Apply 3x3 Median Filter and reduce salt-and-pepper noises in the input noise image
    """

    # TO DO: implement your solution here
    #raise NotImplementedError
    median = []
    img_final = []
    #print(img)
    img_final = np.zeros((len(img)+2,len(img[0])+2))
    img_final[1:-1, 1:-1] = img
    #print(img_final.shape)
    #print(img_final)
    m,n = img_final.shape
    #print(m)
    #print(n)
    for i in range(1,m-1):
        for j in range(1,n-1):
                    median = [img_final[i-1, j-1],
                            img_final[i-1, j],
                            img_final[i-1, j + 1],
                            img_final[i, j-1],
                            img_final[i, j],
                            img_final[i, j + 1],
                            img_final[i + 1, j-1],
                            img_final[i + 1, j],
                            img_final[i + 1, j + 1]]
                    median = sorted(median)
                    img_final[i,j] = median[4]
    img = img_final[1:-1,1:-1]
    denoise_img = img 
    #print(img.shape)
    return denoise_img


def convolve2d(img, kernel):
    """
    :param img: numpy.ndarray, image
    :param kernel: numpy.ndarray, kernel
    :return conv_img: numpy.ndarray, image, same size as the input image

    Convolves a given image (or matrix) and a given kernel.
    """

    # TO DO: implement your solution here
    #raise NotImplementedError
    img_list = []
    img_final = []
    img1 = np.zeros(img.shape)
    # Flip the kernel
    kernel = np.flipud(np.fliplr(kernel))
    #print(kernel)
    # Add zero padding to the input image
    img_final = np.zeros((len(img)+2,len(img[0])+2))
    img_final[1:-1, 1:-1] = img
    m,n = img.shape
    for i in range(m):
        for j in range(n):
            # element-wise multiplication of the kernel and the image
            img1[i, j] = (kernel * img_final[i: i+3, j: j+3]).sum()
            img_list.append(img1[i, j])
    img1 = 255 * ((img1 - min(img_list))/(max(img_list) - min(img_list)))
    conv_img=img1
    # imshow('a',img1)
    # waitKey()
    del img_list[:]
    #del img1[:][:]
    return conv_img


def edge_detect(img):
    """
    :param img: numpy.ndarray(int), image
    :return edge_x: numpy.ndarray(int), image, same size as the input image, edges along x direction
    :return edge_y: numpy.ndarray(int), image, same size as the input image, edges along y direction
    :return edge_mag: numpy.ndarray(int), image, same size as the input image, 
                      magnitude of edges by combining edges along two orthogonal directions.

    Detect edges using Sobel kernel along x and y directions.
    Please use the Sobel operators provided in line 30-32.
    Calculate magnitude of edges by combining edges along two orthogonal directions.
    All returned images should be normalized to [0, 255].
    """

    # TO DO: implement your solution here
    #raise NotImplementedError
    edge_x = convolve2d(img, sobel_x)
    edge_y = convolve2d(img, sobel_y)
    # Flip the kernel
    kernel1 = np.flipud(np.fliplr(sobel_x))
    GX = np.zeros((len(img)+2,len(img[0])+2))
    GX[1:-1, 1:-1] = img
    GX1 = np.zeros(img.shape)
    m,n = img.shape
    for i in range(m):
        for j in range(n):
            # element-wise multiplication of the kernel and the image
            GX1[i, j] = (kernel1 * GX[i: i+3, j: j+3]).sum()
    kernel2 = np.flipud(np.fliplr(sobel_y))
    GY = np.zeros((len(img)+2,len(img[0])+2))
    GY[1:-1, 1:-1] = img
    GY1 = np.zeros(img.shape)
    m,n = img.shape
    for i in range(m):
        for j in range(n):
            # element-wise multiplication of the kernel and the image
            GY1[i, j] = (kernel2 * GY[i: i+3, j: j+3]).sum()
    edge_mag = np.zeros(edge_x.shape)    
    img_list = []
    for i in range(edge_x.shape[0]):
        for j in range(edge_x.shape[1]):
            edge_mag[i][j] = (GX1[i][j]**2 + GY1[i][j]**2)**(1/2)
            img_list.append(edge_mag[i][j])
    edge_mag = ((edge_mag - min(img_list))/(max(img_list) - min(img_list)))*255
    return edge_x, edge_y, edge_mag


def edge_diag(img):
    """
    :param img: numpy.ndarray(int), image
    :return edge_45: numpy.ndarray(int), image, same size as the input image, edges along x direction
    :return edge_135: numpy.ndarray(int), image, same size as the input image, edges along y direction

    Design two 3x3 kernels to detect the diagonal edges of input image. Please print out the kernels you designed.
    Detect diagonal edges along 45° and 135° diagonal directions using the kernels you designed.
    All returned images should be normalized to [0, 255].
    """
    # TO DO: implement your solution here
    #raise NotImplementedError
    kernel45 = np.array([[2,1,0],[1,0,-1],[0,-1,-2]]) 
    print('kernel45',kernel45)  
    edge_45 = convolve2d(img, kernel45)
    kernel135 = np.array([[0,-1,-2],[1,0,-1],[2,1,0]])
    print('kernel135',kernel135)
    edge_135 = convolve2d(img, kernel135)
    return edge_45, edge_135


if __name__ == "__main__":
    noise_img = imread('task2.png', IMREAD_GRAYSCALE)
    denoise_img = filter(noise_img)
    imwrite('results/task2_denoise.jpg', denoise_img)
    edge_x_img, edge_y_img, edge_mag_img = edge_detect(denoise_img)
    imwrite('results/task2_edge_x.jpg', edge_x_img)
    imwrite('results/task2_edge_y.jpg', edge_y_img)
    imwrite('results/task2_edge_mag.jpg', edge_mag_img)
    edge_45_img, edge_135_img = edge_diag(denoise_img)
    imwrite('results/task2_edge_diag1.jpg', edge_45_img)
    imwrite('results/task2_edge_diag2.jpg', edge_135_img)