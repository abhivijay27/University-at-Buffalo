import sys
arr = []
delarr = []
heapsize = 0
i=0
root=0
parent = 0

                    #Insert function in heap
def insertheap(j):
    global heapsize,root,i
    i = heapsize
    #print(i)
    heapsize = heapsize+1
    #print(heapsize)
    arr.append(j)
    root = int((i-1)/2)
    while i!=0 and arr[int(root)]>arr[int(i)]:        
        arr[int(root)],arr[int(i)]= arr[int(i)], arr[int(root)]
        i = int(root)
        root = int((i-1))/2

                    #Heapify function
def minheap(j):
    left = 2*j +1
    right = 2*j +2
    s = j
    if left<heapsize and arr[left]<arr[j]:
        #print("in left")
        s = left
    if right<heapsize and arr[right]<arr[j] and arr[right]<arr[left]:
      #  print("in right")
        s = right
    if s!=j:
        arr[s],arr[j] = arr[j],arr[s]
        minheap(s)

                    #Extracting the root
def extractminheap():
    global heapsize,parent
    if heapsize == 0:
        print("No values in heap")
    elif heapsize == 1:
        parent = arr[0]
        #print(arr[0])
        heapsize = 0
        del arr[heapsize]
    else:
        parent = arr[0]
        #print(parent)
        heapsize = heapsize-1
        arr[0] = arr[heapsize]
        del arr[heapsize]
        minheap(0)    
    return parent

                    #Main function
if __name__ == "__main__":

 lines =[]
 while(1):
    line1 = sys.stdin.readline().strip() 
    if not line1:
        break
    elif line1[0] == 'A':
        num = int(line1[2:])
        insertheap(num)
    elif line1[0] == 'E':
        d = extractminheap()
        print(d)
    #lines.append(line1)