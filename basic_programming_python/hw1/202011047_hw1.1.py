#202011047 김승태
#이 코딩은 예를 들어 받는 수가 3이었을 때, 3까지 포함하여 박수를 세도록 하였다. ex)game_369(3)의 결과값은1Clap!이다.(1은 복수가 아니므로 Clap!의 단위를 두었다.
def game_369(a):                                           #함수 game_369를 정의한다.
    if 1<a<100:                                            #받는 수 값이 1과 100 사이일 때 다음과 같이 실행함.
        gugulist=[]                                        #구구리스트라는 리스트를 생성한다. 임의의 이름이고, 구구단과는 다른 기능을 가짐.
        for i in range(1,a+1):                             #임의의 i가 1부터 받은 수 a까지 반복문을 돌도록 함. 그래서 a+1을 넣음.
            if i%10!=0 and (i%10)%3==0:                    #i가 10으로 나누어지지 않고(이것은 10,20,30,40....을 배제하기 위해서임.) 1의 자리가 3,6,9,인 것들을 고름.
                gugulist.append(i)                         #구구리스트에 i를 추가함(나중에 len으로 리스트의 인수 개수를 세면 되기 때문.)
            if i>=10 and ((i-(i%10))/10%3==0):             #10보다 크며, 10의자리 수가 3,6,9,인 수를 골라내기 위한 if절
                gugulist.append(i)                         #이를 통해, gugulist에는 일의 자리가 3,6,9인 것들과, 십의 자리가 3,6,9,인 것들이 다 담기게 됨. 여기서, 33과 같은 수는 두 번 담기게 되어 박수 두번에 차질이 없음.
            else:
                pass                                       #나머지는 패스시킴(if 두 절에 걸리지 않은 아이들)
        b=len(gugulist)                                    #b값을 구구리스트의 인수 개수만큼을 받게 함.
        if b<2:                                            #b가 1개 이하이면, clap을 반영함.
            print(b,"Clap!")
        else:                                              #위의 경우가 아니라면, 박수 개수와 claps!다위를 붙인 print를 실행함.
            print(b,"Claps!")


    else:
        print("out of range!")                             #이는 1<a<100의 범위를 만족시키지 못한 경우이므로 따로 뽑아냄.





if __name__== '__main__':
    game_369(8)
    game_369(2)
    game_369(50)
    game_369(100)