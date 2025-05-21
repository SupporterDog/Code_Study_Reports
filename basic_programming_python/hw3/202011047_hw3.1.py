#202011047 김승태
def dict(words):                                                    #조건에 맞는 함수 지정
    import operator                                                 #operator를 import함
    splitlist=words.split()                                         #split을 이용해서, 띄어쓰기로 단어를 나눔
    def count_items(sequence):                                      #단어들을 세는 함수 지정
        count={}                                                    #count를 하는 딕셔너리 만듦
        for item in sequence:                                       #sequence에 있는 item을 얻어서 count에 넣어서 셀 수 있도록 만듦
            count[item] =count.get(item,0)+1
        return count                                                #이렇게 만든 count를 반환함
    v=count_items(splitlist)                                        #splitlist의 단어들을 세기 위하여 앞에서 세운 함수를 넣음, 이를 v라 함
    newlist=sorted(v.items(),key=operator.itemgetter(0))            #operator 의 itemgetter을 이용하고sort기능을 이용해 newlist에 v를 넣어서 리스트 안의 튜플들 형태로 만듦
    return newlist                                                  #newlist 반환






if __name__ =='__main__':
    example ="""
    you are learning a programming class
    i hope you will learn funny class
    """
    print(dict(example))
    print(dict('have a nice nice day day day'))