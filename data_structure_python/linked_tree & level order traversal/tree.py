
import collections #collections를 import한다.
class Tree: #Tree 라는 클래스를 명명한다.

    #------------------------------- nested Position class -------------------------------
    class Position: #Position이라는 class를 지정한다.

        #element라는 추상 메서드를 만든다. 
        def element(self):
            # 구현되지 않은 메서드를 호출하면, 예외를 발생시킨다.
            raise NotImplementedError('must be implemented by subclass')

        # __eq__라는 추상 메서드를 만든다.
        def __eq__(self, other):
            #구현되지 않은 메서드를 호출하면, 예외를 발생시킨다.
            raise NotImplementedError('must be implemented by subclass')

        # __ne__ 라는 메서드를 만든다.
        def __ne__(self, other):
            # self와 other가 같지 않을 때, True 반환함.
            return not (self == other)                        # opposite of __eq__

    # ---------- abstract methods that concrete subclass must support ----------
    #root라는 추상 메서드를 만든다.
    def root(self):
        #구현되지 않은 메서드를 호출하면, 예외를 발생시킨다.
        raise NotImplementedError('must be implemented by subclass')

    #parent라는 추상 메서드를 만든다.
    def parent(self, p):
        #구현되지 않은 메서드를 호출하면, 예외를 발생시킨다.
        raise NotImplementedError('must be implemented by subclass')

    #num_children이라는 추상 메서드를 만든다.
    def num_children(self, p):
        #구현되지 않은 메서드를 호출하면, 예외를 발생시킨다.
        raise NotImplementedError('must be implemented by subclass')

    # children이라는 추상 메서드를 만든다.
    def children(self, p):
        #구현되지 않은 메서드를 호출하면, 예외를 발생시킨다.
        raise NotImplementedError('must be implemented by subclass')

    # __len__이라는 추상 메서드를 만든다.
    def __len__(self):
        #구현되지 않은 메서드를 호출하면, 예외를 발생시킨다.
        raise NotImplementedError('must be implemented by subclass')

    # ---------- concrete methods implemented in this class ----------
    #is_root는 트리에서 받은 Position 객체가 root인지 확인하는 메서드이다.
    def is_root(self, p):
        return self.root() == p # 맞으면 True 반환

    #is_leaf는 트리에서 받은 Position 객체가 leaf인지 확인하는 메서드이다.
    def is_leaf(self, p):
        return self.num_children(p) == 0 #맞으면 True 반환

    #is_empty는 트리가 비어있는지 확인하는 메서드이다.
    def is_empty(self):
        return len(self) == 0  #트리의 길이가 0, 즉 트리에 아무것도 없으면 True 반환한다.

    #depth는 Position 객체의 깊이를 반환하는 메서드이다.
    def depth(self, p):
        if self.is_root(p): #self.is_root, 즉 p가 root라면 깊이 0을 반환한다.
            return 0
        else:
            return 1 + self.depth(self.parent(p)) #p 부모의 깊이에다가 부모까지 깊이의 값으로 더해서 반환하여, 총 깊이를 반환한다.

    #_height는 Position 객체의 높이를 반환하는 메서드이다.
    def _height(self, p):                                    
        if self.is_leaf(p): #p 가 leaf이면
            return 0        #height 0을 반환한다.
        else:
            # p의 자식 중에 가장 큰 height를 구해서, 1을 더해서 반환한다.
            return 1 + max(self._height(c) for c in self.children(p))

    # height는 트리의 높이를 계산하는 메서드이다.
    def height(self, p=None): #특정 노드가 지정되지 않으면 트리의 root노드가 된다.
        #p가 선택되지 않은 경우, 트리의 root node로 지정된다.
        if p is None:
            p = self.root()
        return self._height(p)                # start _height2 recursion

    def __iter__(self):
        for p in self.positions():     # use same order as positions()
            yield p.element()          # but yield each element

    def positions(self):
        return self.preorder()         # return entire preorder iteration

    def preorder(self):
        if not self.is_empty():
            for p in self._subtree_preorder(self.root()):    # start recursion
                yield p

    def _subtree_preorder(self, p):
        yield p                       # visit p before its subtrees
        for c in self.children(p):                 # for each child c
            for other in self._subtree_preorder(c):              # do preorder of c's subtree
                yield other                                               # yielding each to our caller

    def postorder(self):
        if not self.is_empty():
            for p in self._subtree_postorder(self.root()):    # start recursion
                yield p

    def _subtree_postorder(self, p):
        for c in self.children(p):       # for each child c
            for other in self._subtree_postorder(c):  # do postorder of c's subtree
                yield other      # yielding each to our caller
        yield p             # visit p after its subtrees
        
    # levelorder는 levelorder 방식으로 트리의 node를 순회한다.
    def levelorder(self):
        if not self.is_empty():# 트리가 비어있지 않을 경우, 아래와 같이 실현되게 한다.
            ddeque = collections.deque() # deque 객체를 생성한다.
            ddeque.append(self.root())   # root node를 deque에 추가한다. 이는 오른쪽부터 추가될 것이다.
        while len(ddeque) > 0: # deque에 남아있는게 있다면,
            p = ddeque.popleft()   #왼쪽부터 뽑아내어 p에 할당한다.
            yield p # p를 계속 반환한다.
            for c in self.children(p): #p의 자식들에 대해서 계속 실행한다.
                ddeque.append(c) #c를 deque에 넣는다.
        
        #즉, 이 코드는 root를 방문하고 deque에 넣고, 생산하고, 그 자식들에 대해서 다시 루프를 돌게 하면서 전부 순차적으로 popleft로 deque 방식에서 뽑아내도록 설계되었다. 
        
        

        
    