#202011047 김승태
def character(decoding_table,code):                                 #주어진 조건에 맞는 함수 결정
    return decoding_table[code]                                     #코드를 받고, 코드에 맞는 번호가 쓰인 것의 값을 호출함.

def ordinal(decoding_table,char):                                   #주어진 조건에 맞는 함수 결정
    for code, name in decoding_table.items():                       #decoding_table의 인수를 각각 code, name이라 하고, item을 찾는다.
        if name == char:                                            #name부분과 받은 char 부분이 같을 때
            return code                                             #code(즉, 키를 반환한다.)

def decode(decoding_table,codes):                                   #주어진 조건에 맞는 함수 결정
    new=""                                                          #new라는 비어있는 변수를 설정
    for i in range(len(codes)):                                     #i가 codes의 수 만큼 돌아가는 for문
        new+= decoding_table[codes[i]]                              #i번째 codes의 인수를 new라는 새로운 변수에 계속적으로 집어넣어 이를 연속적으로 나오게 만든다.
    return new                                                      #이렇게 하면, 각각의 정수들 키에 맞는 값들을 반환할 수 있다.

def encode(decoding_table,string):                                  #주어진 조건에 맞는 함수 결정
    a=[]                                                            #a라는 빈 리스트 생성
    for i in string:                                                #string 안의 i에 대하여 실시한다.
        for code, name in decoding_table.items():                   #다시, decoding_table의 인수를 각각 code, name이라 한다.
            if name == i:                                           # 여기서 name과 i가 같을 때, 즉, 정수와 name이 같을 때,
                a.append(code)                                      #a에다가 그 code를 넣는다.
    return a                                                        #a라는 리스트를 반환한다.




if __name__=='__main__':
    ascii_table={
        32:' ', 33:'!', 34:'"', 35:'#',
        36:'$', 37:'%', 38:'&', 39:"'",
        40:'(', 41:')', 42:'*', 43:'+',
        44:',', 45:'-', 46:'.', 47:'/',
        48:'0', 49:'1', 50:'2', 51:'3',
        52: '4', 53: '5', 54: '6', 55: '7',
        56: '8', 57: '9', 58: ':', 59: ';',
        60: '<', 61: '=', 62: '>', 63: '?',
        64: '@', 65: 'A', 66: 'B', 67: 'C',
        68: 'D', 69: 'E', 70: 'F', 71: 'G',
        72: 'H', 73: 'I', 74: 'J', 75: 'K',
        76: 'L', 77: 'M', 78: 'N', 79: 'O',
        80: 'P', 81: 'Q', 82: 'R', 83: 'S',
        84: 'T', 85: 'U', 86: 'V', 87: 'W',
        88: 'X', 89: 'Y', 90: 'Z', 91: '[',
        92: '\\', 93: ']', 94: '^', 95: '_',
        96: '`', 97: 'a', 98: 'b', 99: 'c',
        100: 'd', 101: 'e', 102: 'f', 103: 'g',
        104: 'h', 105: 'i', 106: 'j', 107: 'k',
        108: 'l', 109: 'm', 110: 'n', 111: 'o',
        112: 'p', 113: 'q', 114: 'r', 115: 's',
        116: 't', 117: 'u', 118: 'v', 119: 'w',
        120: 'x', 121: 'y', 122: 'z', 123: '{',
        124: '|', 125: '}', 126: '~'}

print(character(ascii_table,65))
print(ordinal(ascii_table,'G'))
print(decode(ascii_table,[72,101,108,108,111,44,32,80,121,116,104,111,110,33]))
print(encode(ascii_table,"Have a nice day ~ :)"))