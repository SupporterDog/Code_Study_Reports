#202011047 김승태
def print_lines(sequence):                                          #조건에 맞는 함수 설정
    for i in range(len(sequence)):                                  #sequence의 인수 개수를 세서
        print(sequence[i])                                          #sequence의 i번째 인수를 print한다. 이는 한 줄에 하나씩 print하기 위함이다.
    print("")                                                       #띄어쓰기의 ""

def string_to_image(pic_str):                                       #조건에 맞는 함수 설정
    list=[]                                                         #빈 리스트 만듦
    for i in range(len(pic_str)):                                   #pic str의 인수 개수만큼 실행
        inlist=[]                                                   #여기서 쓰일 list 새로 생성
        for j in range(len(pic_str[i])):                            #i번째 인수(리스트)의 인수 개수만큼 j를 돌림
            if pic_str[i][j]==' ':                                  #i번째 인수의 j번째가 ' '이면 0을 넣음
                inlist.append(0)
            if pic_str[i][j]=='*':                                  #위와 마찬가지로 '*'일 때 1 넣음
                inlist.append(1)
        list.append(inlist)                                         #list에 inlist를 한 인수로 넣음
    return list                                                     #list를 반환

def image_to_str(umcompressed_img):                                 #조건에 맞느 함수 설정
    list = []                                                       #빈 리스트 생성
    for i in range(len(umcompressed_img)):                          #i를 또 인수만큼 돌림
        inlist = []                                                 #새로운 빈 리스트 생성
        for j in range(len(umcompressed_img[i])):                   #string_to_image의 함수와 거의 동일하게 0을 받으면 ' ', 1을 받으면 '*'을 inlist에 넣어서 list에 다시 넣도록 함.(23~28)
            if umcompressed_img[i][j] == 0 :
                inlist.append(' ')
            if umcompressed_img[i][j] == 1:
                inlist.append('*')
        list.append(inlist)

    newlist=[]                                                      #newlist를 만듦
    for k in range(len(list)):                                      #list의 인수 개수만큼 돌림
        str=""                                                      #string 빈 것을 만듦
        for q in list[k]:                                           #list의 k번째 인수 중의 인수들에 인해서
            str += q                                                #빈 string에 q를 넣음
        newlist.append(str)                                         #newlist에 str를 넣음. 이를 통해 리스트처럼 나오는 것이 아닌, 리스트가 string으로 변경되어서 이미지화됨.
    return newlist

def compress_img(umcompressed_image):                               #조건에 맞는 함수 설정
    finallist=[]                                                    #finallist라는 빈 리스트 만듦
    for j in range(len(umcompressed_image)):                        #인수만큼 반복문을 돌림
        newlist=[]                                                  #newlist 빈리스트 생성
        count=0                                                     #count=0, 이를 통해 반복하면 올라가게 하고, 반복이 끊기면 그만큼을 리스트에 넣음
        for i in range(0,len(umcompressed_image[j])-1,1):           #오류를 막기 위해서 len(umcompressed_image[j])-1을 사용함
            if umcompressed_image[j][i] == 0:                       #j i번째가 0일 때,
                if umcompressed_image[j][i] == umcompressed_image[j][i + 1]:    #이 때일때 이 항과 뒷 항이 같으면, count를 리스트에 넣지 않고 값만 1 올림
                    count += 1
                else:                                                           #그렇지 않을 때에는, 반복되지 않으므로 1을 newlist에 append함. 그리고 count를 0으로 초기화함(48~50)
                    count += 1
                    newlist.append(count)
                    count = 0

            if i == 0 and umcompressed_image[j][0] == 1:                        #i가 0, 즉 j번째 인수의 첫번째 인수가 1이면 0을 넣는다(52~53)
                newlist.append(0)

            if umcompressed_image[j][i] == 1:                                   #(55~61)은 위의 45~50과 같은 방식으로 j i 번째가 1일 때를 count하고 리스트에 넣음
                if umcompressed_image[j][i] == umcompressed_image[j][i + 1]:
                    count += 1
                else:
                    count += 1
                    newlist.append(count)
                    count = 0
        if umcompressed_image[j][len(umcompressed_image[j]) - 1] == umcompressed_image[j][len(umcompressed_image[j]) - 2]:  #아까 len(umcompressed_image[j])-1까지만 했으므로, 이 항도 마지막 항과 중복되었을 때에는 count를 올려 주어야 함.
            count += 1                                                                                                      #count를 올리면, 마지막은 어짜피 그전과 같으므로 append하면 된다.(63~65)
            newlist.append(count)
            count = 0
        elif umcompressed_image[j][len(umcompressed_image[j]) - 1] != umcompressed_image[j][len(umcompressed_image[j]) - 2]:    #62가 아닌 상황
            newlist.append(1)                                                                                                   #그냥 멈추고 마지막에 1을 집어넣는다. 어짜피 44~61에서 앞의 것이 리스트에 넣어졌으므로, 마지막 것만 넣으면 된다.
        finallist.append(newlist)                                                                                               #반복한 것을 finallist에 넣는다. 이를 반복하는 것이다
    return finallist                #finallist를 반환한다.

def decompress_img(compressed_img):                                     #함수를 정의한다
    finallist=[]                                                        #finallist라는 빈 리스트를 만든다.
    for i in range(len(compressed_img)):                                #주어진 것에 인수 개수만큼 반복시킨다
        newlist=[]                                                      #새로운 리스트 newlist를 만든다.
        if compressed_img[0]==0:                                        #1로 시작하여 처음의 리스트값이 0이 나왔을 경우,
            for j in range(len(compressed_img[i])):                     #또한 여기의 i번째 인수의 개수만큼 j를 또 돌린다.
                if j%2==0:                                              #j가 2의 배수이면, 전부 다 1에 관련이 있으므로, 써져 있는 숫자만큼 1을 리스트에 append한다. (77~79)
                    for k in range(compressed_img[i][j]):
                        newlist.append(1)
                else:
                    for k in range(compressed_img[i][j]):               #그 외일 때는 0에 관련되므로, 반복시켜 0을 리스트에 append시킨다 (80~82)
                        newlist.append(0)

        else:                                                           #원래 0으로 시작해서 지금 받은 리스트에서는 1이상의 값이 나온 경우
            for j in range(len(compressed_img[i])):                     #주어진 것에 인수 개수만큼 반복시킨다.
                if j%2==0:                                              #이번에는 j가 2로 나누어떨어질 때, j번째 수는 0이 연속된 개수이므로 써져 있는 숫자만큼 0을 append한다.(86~88)
                    for k in range(compressed_img[i][j]):
                        newlist.append(0)
                else:
                    for k in range(compressed_img[i][j]):               #짝수가 아닐 때에는 1을 append한다(90~92)
                        newlist.append(1)
        finallist.append(newlist)                                       #newlist를 finallist에 넣는다.
    return finallist                                                    #finallist를 반환한다.
















if __name__ =='__main__':
    image1=[[' ','*','*','*',' '],
            [' ',' ',' ',' ','*'],
            [' ','*','*','*','*'],
            ['*',' ',' ',' ','*'],
            ['*',' ',' ',' ','*'],
            [' ','*','*','*','*']]

    image2=[[0,1,1,1,0],
            [0,0,0,0,1],
            [0,1,1,1,1],
            [1,0,0,0,1],
            [1,0,0,0,1],
            [0,1,1,1,1]]

    result1=string_to_image(image1)
    print_lines(result1)
    result2=image_to_str(image2)
    print_lines(result2)
    result3=compress_img(image2)
    print_lines(result3)
    result4=decompress_img(result3)
    print_lines(result4)