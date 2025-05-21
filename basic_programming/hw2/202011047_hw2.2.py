#202011047 김승태
def find_circles(data,student_name):                                                    #주어진 조건에 맞는 함수 결정
    list=[]                                                                             #빈 리스트를 만든다.
    for club, name in data.items():                                                     #data.items를 받고, 그 안에서 club,name이라고 각각 명명하여, 그 안에서의 for문을 작성함.
        if student_name in name:                                                        #받은 student_name이 name 안에 있을 때
            list.append(club)                                                           #list에 club명을 추가한다.
    v=set(list)                                                                         #v변수를 리스트를 세트로 변환한 값으로 설정함.
    if list == []:                                                                      #리스트가 빈 리스트였을 때, 즉 찾는 값이 없을 때
        return ("There is no data for %s." %student_name)                               #없다는 것을 반환함
    else:
        return v                                                                        #그 외에는 v(세트화 된 것)을 반환함.

def find_members(data,*members):                                                        #주어진 조건에 맞는 함수 결정
    gyojiplist=[]                                                                       #gyojiplist(교집리스트)(빈 리스트)를 만듦.
    for i in range(len(list(members))):                                                 #members를 리스트화해서 개수를 센 후, i를 그 범위에서 돌림.
        gyojiplist = gyojiplist + data.get(members[i])                                  #gyojiplist에다가 data에서 members의 i번째를 get한값을 계속 집어넣음.
    lastlist=[]                                                                         #lastlist라는 빈 리스트를 만듦.
    while len(gyojiplist)!=0:                                                           #gyojiplist가 0이 될 때 까지, 즉, 밑에서 값을 삭제할 것인데, 그것 때문에, 삭제되어 0이 될 때까지 가는 while문을 작성함.
        if gyojiplist.count(gyojiplist[0])==len(list(members)):                         #gyojiplist의 첫째값이 members의 인수들 개수만큼 있을 때,
            lastlist.append(gyojiplist[0])                                              #gyojiplist의 첫째값을 lastlist에 추가한다.
        gyojiplist.remove(gyojiplist[0])                                                #이후 gyojiplistt에서 해당하는 값을 제거한다.
    if len(lastlist)!=0:                                                                #lastlist에 무언가 담겨있을 경우
        return lastlist                                                                 #준비한 lastlist를 반환함.
    if len(lastlist)==0:                                                                #lastlist에 아무것도 담겨있지 않을 경우,
        return  "No result."                                                            #아무것도 없음을 반환함.(print하기위함)


if __name__=='__main__':
    members ={'Music club':['Bob', 'Alice', 'Dan', 'Heidi', 'Victor'],
              'Football club': ['Mallory', 'Victor', 'Bob', 'Dan', 'Frank', 'Peggy'],
              'Tennis club': ['Alice','Bob', 'Carol', 'Victor', 'Frank']}
    print(find_circles(members,'Alice'))
    print(find_circles(members,'Bob'))
    print(find_circles(members, 'Dennins'))
    print(find_circles(members, 'Peggy'))
    print(find_members(members, 'Football club','Tennis club'))
    print(find_members(members, 'Music club','Football club','Tennis club'))