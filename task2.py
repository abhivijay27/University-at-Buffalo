###############
##Design the function "calibrate" to  return 
# (1) intrinsic_params: should be a list with four elements: [f_x, f_y, o_x, o_y], where f_x and f_y is focal length, o_x and o_y is offset;
# (2) is_constant: should be bool data type. False if the intrinsic parameters differed from world coordinates. 
#                                            True if the intrinsic parameters are invariable.
#It is ok to add other functions if you need
###############
import numpy as np
from cv2 import imread, cvtColor, imshow, COLOR_BGR2GRAY, TERM_CRITERIA_EPS, TERM_CRITERIA_MAX_ITER, findChessboardCorners, cornerSubPix, drawChessboardCorners, waitKey

def calibrate(imgname):
    # Defining the dimensions of checkerboard
    CHESSBOARD = (9,4)
    criteria = TERM_CRITERIA_EPS + TERM_CRITERIA_MAX_ITER, 30, 0.001

#3D points for each chessboard
    objpoints = []
#2D points for each chessboard
    imgpoints = [] 
    #gray = 0

# 3D world points
    objp = np.zeros((1, CHESSBOARD[0] * CHESSBOARD[1], 3), np.float32)
    objp[0,:,:2] = np.mgrid[0:CHESSBOARD[0], 0:CHESSBOARD[1]].T.reshape(-1, 2)
    
    image = imread(imgname)
    imshow('img',image)

    gray = cvtColor(image,COLOR_BGR2GRAY)

    ret, corner = findChessboardCorners(gray, CHESSBOARD, None)

    if ret == True:
        objpoints.append(objp)
        # 2D pixel points
        corners2 = cornerSubPix(gray, corner, (11,11),(-1,-1), criteria)
        
        imgpoints.append(corners2)

    image = drawChessboardCorners(image, CHESSBOARD, corners2, ret)
    
    imshow('img',image)
    waitKey(0)

    #Defining World coordinates
    world = [
            [40, 0, 10],
            [30, 0, 10],
            [20, 0, 10],
            [10, 0, 10],
            [0, 0, 10],
            [0, 10, 10],
            [0, 20, 10],
            [0, 30, 10],
            [0, 40, 10],
            [40, 0, 20],
            [30, 0, 20],
            [20, 0, 20],
            [10, 0, 20],
            [0, 0, 20],
            [0, 10, 20],
            [0, 20, 20],
            [0, 30, 20],
            [0, 40, 20],
            [40, 0, 30],
            [30, 0, 30],
            [20, 0, 30],
            [10, 0, 30],
            [0, 0, 30],
            [0, 10, 30],
            [0, 20, 30],
            [0, 30, 30],
            [0, 40, 30],
            [40, 0, 40],
            [30, 0, 40],
            [20, 0, 40],
            [10, 0, 40],
            [0, 0, 40],
            [0, 10, 40],
            [0, 20, 40],
            [0, 30, 40],
            [0, 40, 40]]
    corners = corners2.reshape(-1,2)  
    #Creating the matrix     
    index = 0
    A = np.zeros((72, 12))
    for i in range(0, len(corners)):
        Xw = world[i][0]
        Yw = world[i][1]
        Zw = world[i][2]
        x,y = corners[i][0], corners[i][1]
        A[index] = [Xw, Yw, Zw, 1, 0, 0, 0, 0, -x*Xw, -x*Yw, -x*Zw, -x]
        A[index + 1] = [0, 0, 0, 0, Xw, Yw, Zw, 1, -y*Xw, -y*Yw, -y*Zw, -y]
        index = index + 2
    #print(A)
    #Normalization
    U, s, V = np.linalg.svd(A)
    matrix = V[11,:].reshape(3, 4)
    m31 = matrix[2][0]
    m32 = matrix[2][1]
    m33 = matrix[2][2]
    m34 = matrix[2][3]
    #Finding Lambda
    Scale = 1/(np.sqrt(np.square(m31)+np.square(m32)+np.square(m33)))
    #print(Scale)
    matrix = matrix*Scale
    #print(matrix)
    M1 = matrix[0][:3]
    M2 = matrix[1][:3]
    M3 = matrix[2][:3]
    # print(matrix)
    # print(m33)
    intrinsic_params = [] 
    M1 = np.transpose(M1)
    M2 = np.transpose(M2)
    M3 = np.transpose(M3)
    Tm1 = np.transpose(M1)
    Tm2 = np.transpose(M2)
    Tm3 = np.transpose(M3)
    #Computing Fx, Fy, Ox, Oy
    Ox = np.dot(Tm1,M3)
    Oy = np.dot(Tm2,M3)
    Fx = np.sqrt(np.dot(Tm1,M1)-np.square(Ox))
    Fy = np.sqrt(np.dot(Tm2,M2)-np.square(Oy))
    #print(Fx,Fy,Ox,Oy)
    #is_constant = []
    intrinsic_params.append(Fx)
    intrinsic_params.append(Fy)
    intrinsic_params.append(Ox)
    intrinsic_params.append(Oy)
    return intrinsic_params,True
    

if __name__ == "__main__":
    intrinsic_params, is_constant = calibrate('checkboard.png')
    print(intrinsic_params)
    print(is_constant)