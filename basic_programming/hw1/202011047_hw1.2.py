#202011047 김승태
def calcPath(trace_map,x,y):                                                  # 문제 조건에 맞는 함수를 지정하였다.
    if x>len(trace_map)-1 or y>len(trace_map)-1:                              # 밖으로 나갔을 때 0값을 return하는 것, 이것은 나중에 return값들을 더해서 총 횟수가 나올 때, 바깥으로 나간 것은 횟수에 포함되지 않기 때문이다.
        return 0
    elif x == len(trace_map)-1 and y == len(trace_map)-1:                     #완벽히 끝에 도달하였을 때 return값을 1로 지정함
        return 1
    else:
        x_variation= calcPath(trace_map,x + trace_map[x][y],y)                # x변화량을 정의하여 그 정의값에 재귀함수가 돌아가도록 만듦
        y_variation= calcPath(trace_map,x,y+trace_map[x][y])                  # y변화량을 정의하여 그 정의값에 재귀함수가 돌아가도록 만듦 ,8줄과 9줄을 통해, 재귀함수를 이용한 이동 경우의 수를 전부 고려할 수 있음.
        return x_variation + y_variation                                      # x_variation과 y_variation의 합을 구함으로써, 각각의 x_variation과 y_variation을 돌아 결국 나오게 된 각각의 return값을 모두 더한 값을 반환하도록 만듦. 그것이 총 횟수임.

if __name__=='__main__': #호출
    trace_map=[[1,2,9,4,9],
               [9,9,9,9,9],
               [9,3,9,9,2],
               [9,9,9,9,9],
               [9,9,9,1,0]]
    print("총 횟수는 %d회" %(calcPath(trace_map,0,0)))
    trace_map=[[1,1,1],
               [1,1,2],
               [1,2,0]]
    print("총 횟수는 %d회" %(calcPath(trace_map,0,0)))