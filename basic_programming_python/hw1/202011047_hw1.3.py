#202011047 김승태
def print_scores(test_scores,course_titles,student_name):                 #문제 조건에 맞는 함수를 정의함.
    list=[]                                                               #빈 list를 만듦.
    for i in range(0,len(test_scores)):                                   #4~5번째 줄: test_scores의 인수 개수를 세서(len이용), 그 만큼 빈 list에 test_scores에 있는 i번째 학생의 이름을 넣음.(즉, 모든 학생들의 이름을 가진 리스트가 만들어짐.)
        list.append(test_scores[i][0])
    if student_name in list:                                              #함수에서 받은 student_name이 아까 받은 list에 있는지 알아보기 위함.
        a=list.index(student_name)                                        #student_name이 list에서 몇 번째에 있는지 index을 통해 반환값으로 알아내어 a의 값으로 정의함.
        print(test_scores[a][0],":")                                      #print를 통해 a번째 있는 사람의 이름: 을 출력함.
        b=0                                                               #b=0, b를 사용하기 위함.
        for k in range(0,len(course_titles)):                             #len(course_titles)로써 과목 개수를 받고, for문을 돌림으로써 과목 : 테스트 성적 을 돌리도록 프로그래밍한다.
            print("   ",course_titles[k],": ",test_scores[a][1][k])       #각각 과목의 이름을 받고, 그 과목의 번호를 k라 했다면, test_scores 리스트에서 그에 해당하는 점수를 뽑아내서 print한다.
            b+=test_scores[a][1][k]                                       #평균을 구하기 위해서 b에다가 위의 과목 점수값들을 계속해서 더한다. 이를 과목 개수로 나누어 주면 평균이 된다.
        print("    Average: ",b/len(course_titles))                       #형식에 맞추어 print한다.
    else:
        print("There is no student named",student_name)                   #6째줄에 만족하지 않는다면, 학생 이름이 없다는 것을 반환한다.

def get_scores(test_scores,course_titles,course_name):                    #문제 조건에 맞는 함수를 정의함.
    if course_name in course_titles:                                      #만약, course_name이 course_titles에 있다면 실행한다.
        a=course_titles.index(course_name)                                #a를 설정하고, 이것에 course_name의 course_titles에서의 위치값을 반환한 값을 대입한다.
        myList=[]                                                         #새로운 리스트 생성
        for k in range(0, len(test_scores)):                              #myList에 a번째 과목에 해당하는 값들을 뽑아내서 저장한다. 이를 어짜피 호출부분에서 과목마다 반복해서 하므로, 한 과목에 해당하는 것만 반복문에 돌리면 된다.
            myList.append(test_scores[k][1][a])
        return myList
    else:
        return None                                                       #찾는 course가 없으면 None을 반환한다.

def print_midterm_stat_by_course(course_name,subject_scores):             #문제 조건에 맞는 함수를 정의함.
    average=sum(subject_scores)/len(subject_scores)                       #28~31average,maxi,mini를 sum/len,max,min을 써서 나타내어 한번에 프린트한다. 여기서 get_scores를 출력에서 subject_scores로 받았기 때문에 이는 리스트 형태이므로, 이것들을 쓸 수 있다.
    maxi=max(subject_scores)
    mini=min(subject_scores)
    print(course_name,"%0.2f %d %d"%(average,maxi,mini))








course_titles=['Linear Algebra','Cell Biology','Programming']
midterm_scores=[['Alice',[62,60,77]],
                ['Bob',[84,88,87]],
                ['Carol',[88,88,97]],
                ['Chuck',[92,71,93]],
                ['Craig',[81,89,72]],
                ['Dan',[79,100,97]],
                ['Erin',[76,72,65]],
                ['Eve',[69,84,67]],
                ['Faythe',[66,60,70]],
                ['Franke',[88,76,64]],
                ['Grace',[66,72,92]],
                ['Heidi',[93,93,82]],
                ['Mallory',[77,89,82]],
                ['Oscar',[67,63,67]],
                ['Peggy',[70,81,86]],
                ['Sybil',[96,94,62]],
                ['Trent',[67,66,61]],
                ['Trudy',[85,74,90]],
                ['Victor',[82,67,94]],
                ['Walter',[67,84,86]],
                ['Wendy',[97,92,66]],
                ]

if __name__=='__main__':
    print_scores(midterm_scores,course_titles,'Alice')
    print_scores(midterm_scores, course_titles, 'Victor')
    print_scores(midterm_scores, course_titles, 'Elice')

    print("과목별 평균, 최대, 최소값을 출력")
    for course_name in course_titles:
        subject_scores=get_scores(midterm_scores,course_titles,course_name)
        print_midterm_stat_by_course(course_name,subject_scores)