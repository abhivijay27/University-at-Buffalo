import sys
def LPSLength(str1):
    m = len(str1)
    n = m
    str2 = str1[::-1] 
    dp = [[0 for i in range(m+1)] for j in range(m+1)]
    for i in range(1, n + 1):
        for j in range(1, n + 1):
            if str1[i - 1] == str2[j - 1]:
                dp[i][j] = dp[i - 1][j - 1] + 1
            else:
                dp[i][j] = max(dp[i - 1][j], dp[i][j - 1])
    return dp[n][n]
 
def LPSString(str1):
    m = len(str1)
    n = m
    str2 = str1[::-1] 
    dp = [[0 for i in range(m+1)] for j in range(m+1)]
    for i in range(m+1):
        for j in range(n+1):
            if i == 0 or j == 0:
                dp[i][j] = 0
            elif str1[i-1] == str2[j-1]:
                dp[i][j] = dp[i-1][j-1] + 1
            else:
                dp[i][j] = max(dp[i-1][j], dp[i][j-1])
    index = dp[m][n]
    lps = [""] * (index+1)
    lps[index] = ""
    i = m
    j = n
    while i > 0 and j > 0:
        if str1[i-1] == str2[j-1]:
            lps[index-1] = str1[i-1]
            i -= 1
            j -= 1
            index -= 1
        elif dp[i-1][j] > dp[i][j-1]:
            i -= 1
        else:
            j -= 1
    lps_str = ""
    lps_str = lps_str.join(lps)       
    return lps_str
 
if __name__ == '__main__': 
    #string1 = 'hjksahduashdusadhsakjhdksajhda'
    string1 = sys.stdin.readline().strip()  
    #Returns the length of the longest palindromic subsequence
    lps_length = LPSLength(string1)
    print(lps_length)
    #Returns the longest palindromic subsequence
    lps_string = LPSString(string1)
    print(lps_string)