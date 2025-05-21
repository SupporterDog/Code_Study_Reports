class RedBlackTree():
    class _Node:
        RED = object()
        BLACK = object()
        """Lightweight, nonpublic class for storing a node."""
        __slots__ = '_element', '_parent', '_left', '_right', '_color' # streamline memory usage

        def __init__(self, element, parent=None, left=None, right=None, color=RED):
            self._element = element
            self._parent = parent
            self._left = left
            self._right = right
            self._color = color

    def __init__(self):
        """Create an initially empty binary tree."""
        self._root = None
        self._size = 0

    def __len__(self):
        return self._size

    # Search for the element in the red-black tree.
    # return: _Node object, or None if it's non-existing
    def search(self, element): # search라는 메서드 지정
        node = self._root # root 노드부터 탐색을 시작하기 위해서 node를 root로 지정함.
        while node is not None: # node가 None이 아닐 때까지 반복
            if node._element == element:
                # element 와 일치하는지 확인함. 맞다면,
                return node #노드를 반환
            elif element < node._element:
                # element가 현재보다 작으면, 왼쪽으로 감.
                node = node._left
            else:
                # 아니면 오른쪽으로 감.
                node = node._right
        return None

    def insert(self, element):
        node = self._Node(element) # element 값 가진 노드를 생성
        self._insert_help(node) # insert_help 호출해서 노드를 insert함.
        self._size += 1 # 트리 사이즈를 1 증가시켜서, 사이즈를 맞춰줌.

    def _insert_help(self, node):
        if self._root is None: #트리가 비어있으면
            self._root = node # self._root 값을 node로 지정하고,
            node._color = self._Node.BLACK # 그것을 검은색으로 지정함.
            return

        current = self._root # current를 루트로 지정함.
        while True:
            if node._element < current._element: # node가 current보다 작을 때
                if current._left is None:
                    # current의 왼쪽 자식이 비었으면,
                    current._left = node # 왼쪽을 노드로 지정하고
                    node._parent = current # 노드의 부모를 current로 지정함.
                    break
                current = current._left # 왼쪽 자식이 있으면, 현재 노드를 왼쪽으로 변경해서 넘김.
            else:
                if current._right is None: # 오른쪽 자식이 없으면
                    current._right = node # 오른쪽 자식을 노드로 지정하고
                    node._parent = current # 노드의 부모를 current로 지정함.
                    break
                current = current._right # 오른쪽 자식이 있으면, 현재 노드를 오른쪽으로 변경해서 넘김. 

        self._fix_violations(node) # 규칙을 유지하려고 _fix_violations를 호출함.

    def _fix_violations(self, node):
        while node._parent and node._parent._color == self._Node.RED:
            # 노드의 부모가 존재하고, 부모가 빨간색일 동안 반복함.
            if node._parent == node._parent._parent._left:
                # 노드의 부모가 부모의 부모의 왼쪽 자식이면
                uncle = node._parent._parent._right # uncle을 노드의 부모의 부모의 오른쪽 자식으로 지정한다.
                if uncle and uncle._color == self._Node.RED: # uncle이 존재하고, 빨간 색인 경우
                    node._parent._color = self._Node.BLACK # 부모 노드의 색상을 검은색으로 변경하고
                    uncle._color = self._Node.BLACK # uncle 노드의 색상도 검은색으로 변경함.
                    node._parent._parent._color = self._Node.RED # 조부모 색을 빨강으로 변경
                    node = node._parent._parent # 현재 노드를 조부모로 지정함.
                else:
                    if node == node._parent._right: # 노드가 부모 오른쪽 자식이면
                        node = node._parent # 노드를 부모로 지정하고
                        self._left_rotate(node) # left rotate를 시킴.
                    node._parent._color = self._Node.BLACK # 노드의 부모 색을 검은색으로 하고
                    node._parent._parent._color = self._Node.RED #노드의 조부모 색상을 빨간색으로 함.
                    self._right_rotate(node._parent._parent) # 그리고 right rotate를 시킴.
            else:
                uncle = node._parent._parent._left # uncle을 노드의 조부모의 왼쪽 자식으로 지정하고
                if uncle and uncle._color == self._Node.RED: # uncle이 존재하고 uncle의 색이 빨강이면
                    node._parent._color = self._Node.BLACK # 노드의 부모 색을 검은색으로 하고
                    uncle._color = self._Node.BLACK # uncle의 색을 검을색으로 지정한다. 
                    node._parent._parent._color = self._Node.RED # 노드의 조부모의 색을 빨강으로 지정한다.
                    node = node._parent._parent # 노드를 노드의 조부모로 지정한다. 
                else:
                    if node == node._parent._left: # 노드가 노드의 왼쪽 자식이면
                        node = node._parent # 노드를 노드의 부모로 지정하고
                        self._right_rotate(node) # 오른쪽으로 회전시킴. ( right_rotate 사용)
                    node._parent._color = self._Node.BLACK # 노드의 부모 색을 검은색으로 지정
                    node._parent._parent._color = self._Node.RED # 노드의 조부모 색을 빨강으로 지정
                    self._left_rotate(node._parent._parent) # left_rotate를 시킴.

        self._root._color = self._Node.BLACK # root의 색을 검은색으로 지정

    def delete(self, element): # delete 메서드 지정
        node = self.search(element) # 일단 노드를 search를 통해 찾음.
        if node is None: # node가 없으면 끝냄.
            return
        self._delete_help(node) # delete 부가 메서드를 호출함.
        self._size -= 1 # 트리 사이즈를 1 줄임.

    def _delete_help(self, node): # delete help 메서드를 지정
        if node._left and node._right: # 노드의 왼쪽 자식이나 오른쪽 자식이면,
            successor = self._minimum(node._right) # 삭제할 노드의 오른쪽 트리에서 가장 작은 값을 가진 successor를 찾는다.
            node._element = successor._element # 후계자(successor)의 값을 삭제할 노드로 복사함. 
            node = successor # 삭제할 노드를 successor로 지정함.

        child = node._left if node._left else node._right # child를 삭제할 노드의 자식으로 지정함. 왼쪽 자식이 있으면 왼쪽, 아니면 오른쪽으로 지정함.
        parent = node._parent # parent를 노드의 부모 노드로 지정함.

        if child:
            child._parent = parent # child이면, child의 부모를 parent로 지정

        if parent: # parent이면,
            if node == parent._left: # 노드가 부모 왼쪽 자식이면, parent._left를 child로 지정
                parent._left = child
            else: # 아니면 오른쪽 자식을 child로 지정
                parent._right = child
        else: # parent가 아니면 root에 child를 지정함.
            self._root = child

        if node._color == self._Node.BLACK: #노드의 색이 검정색이면,
            self._fix_double_black(child, parent) # fix double black을 실행

    def _fix_double_black(self, node, parent): # 메서드 지정
        while (not node or node._color == self._Node.BLACK) and node != self._root: # 노드가 없거나 검은색인 경우, 노드가 root 노드가 아닌 경우를 처리함.
            if node == parent._left: # 삭제된 노드가 부모의 왼쪽 자식인 경우
                sibling = parent._right # sibling 을 부모의 오른쪽 자식으로 지정
                if sibling._color == self._Node.RED:
                    # sibling의 색이 빨간 경우
                    sibling._color = self._Node.BLACK # sibling의 색을 검은색으로 지정함.
                    parent._color = self._Node.RED # 부모의 색을 빨강으로 지정
                    self._left_rotate(parent) # left rotate 를 parent로 돌림.
                    sibling = parent._right # sibling을 부모의 오른쪽 자식으로 지정
                if (not sibling._left or sibling._left._color == self._Node.BLACK) and \
                        (not sibling._right or sibling._right._color == self._Node.BLACK): # sibling의 자식 노드들이 모두 검은색인 경우 ( 더블 블랙 문제를 해결하기 위함 ) 
                    sibling._color = self._Node.RED # sibling의 색을 빨강으로 변경
                    node = parent # node를 parent로 지정
                    parent = node._parent # parent를 node의 부모로 재지정.
                else:
                    if not sibling._right or sibling._right._color == self._Node.BLACK: # sibling의 오른쪽 자식이 없거나 검은색인 경우 처리
                        sibling._left._color = self._Node.BLACK # sibling의 왼쪽 자식 색을 검은색으로 지정
                        sibling._color = self._Node.RED # sibling 색을 빨강으로 지정
                        self._right_rotate(sibling) # right rotate 실행
                        sibling = parent._right # sibling을 부모의 오른쪽으로 지정
                    sibling._color = parent._color # sibling의 색을 부모의 색과 같게 만듦
                    parent._color = self._Node.BLACK # parent 색을 검정으로 지정
                    sibling._right._color = self._Node.BLACK # sibling 오른쪽 색을 검정으로 지정
                    self._left_rotate(parent) # left rotate를 실행함
                    node = self._root # node를 root로 지정함.
            else: # sibling 자식 중 적어도 하나가 빨간색일 경우
                sibling = parent._left # sibling을 부모 왼쪽 자식으로 지정
                if sibling._color == self._Node.RED: # sibling 색이 빨간색이면
                    sibling._color = self._Node.BLACK # sibling 색을 검정색으로 지정
                    parent._color = self._Node.RED # 부모 색을 빨강으로 지정
                    self._right_rotate(parent) # parent에서 right rotate 실행
                    sibling = parent._left # sibling을 부모 왼쪽 자식으로 지정
                if (not sibling._right or sibling._right._color == self._Node.BLACK) and \
                        (not sibling._left or sibling._left._color == self._Node.BLACK): # sibling의 자식 노드들이 모두 검정색일 경우
                    sibling._color = self._Node.RED # sibling의 색을 빨강으로 지정
                    node = parent # node를 parent로 지정
                    parent = node._parent # parent를 노드의 부모로 지정
                else: # 적어도 하나가 빨간색일 경우
                    if not sibling._left or sibling._left._color == self._Node.BLACK: # sibling의 왼쪽 자식이 없거나 검은색인 경우
                        sibling._right._color = self._Node.BLACK # sibling의 오른쪽 색을 검정으로 지정
                        sibling._color = self._Node.RED # sibling의 색을 빨강으로 지정
                        self._left_rotate(sibling) # sibling에서 left rotate를 시킴.
                        sibling = parent._left # sibling을 parent의 왼쪽 자식으로 지정함.
                    sibling._color = parent._color # sibling의 색을 부모의 색과 같게 지정.
                    parent._color = self._Node.BLACK # parent의 색을 검정으로 지정
                    sibling._left._color = self._Node.BLACK # sibling의 왼쪽 자식의 색을 검정으로 지정
                    self._right_rotate(parent) # parent에서 right rotate를 함.
                    node = self._root # node를 root로 지정.

        if node:
            node._color = self._Node.BLACK # node가 존재하면, 색을 검정으로 지정.

    def _left_rotate(self, node): # 주어진 노드를 기반으로 왼쪽으로 회전하는 메서드 지정
        right_child = node._right # right child를 node의 오른쪽 자식으로 지정함.
        node._right = right_child._left # node의 오른쪽 자식을 right child의 왼쪽 자식으로 변경함.
        if right_child._left: # right child의 왼쪽 자식이 존재하면,
            right_child._left._parent = node # 그 자식의 부모를 현재 노드로 설정함.
        right_child._parent = node._parent # right child 의 부모를 node의 부모로 설정함.
        if not node._parent: #  현재 노드의 부모가 없는 경우
            self._root = right_child # right child를 root로 지정
        elif node == node._parent._left: # 현재 노드가 부모의 왼쪽 자식인 경우
            node._parent._left = right_child #  right child를 노드의 부모의 왼쪽 자식으로 설정함
        else: # 노드가 부모의 오른쪽 자식인 경우
            node._parent._right = right_child # 부모의 오른쪽 자식으로 right_child를 설정함.
        right_child._left = node # right child의 왼쪽 자식을 노드로 지정
        node._parent = right_child # right child를 node로 지정

    def _right_rotate(self, node): # 주어진 노드를 기반으로 오른쪽으로 회전하는 메서드 지정
        left_child = node._left # left child를 node의 왼쪽 자식으로 지정
        node._left = left_child._right #  node의 왼쪽 자식을 left child의 오른쪽 자식으로 변경함.
        if left_child._right: # left child의 오른쪽 자식이 존재하면
            left_child._right._parent = node # 그 자식의 부모를 현재 노드로 설정함.
        left_child._parent = node._parent #  left child의 부모를 node의 부모로 설정함.
        if not node._parent: # 현재 노드의  부모가 없는 경우
            self._root = left_child # left child를 root로 지정
        elif node == node._parent._left: # 현재 노드가 부모의 오른쪽 자식인 경우
            node._parent._left = left_child #  left child를 노드의 부모의 왼쪽 자식으로 지정
        else: # 노드가 부모의 오른쪽 자식인 경우
            node._parent._right = left_child #  부모의 오른쪽 자식으로 left child를 설정함.
        left_child._right = node # left child의 오른쪽 자식을 노드로 지정
        node._parent = left_child # left child를 node로 지정

    def _minimum(self, node): # 노드의 서브트리에서 가장 작은 값을 찾는 메서드
        while node._left: # 노드의 왼쪽 자식이 존재할 때 반복
            node = node._left # 노드를 노드 자식으로 지정함.
        return node
        
    # BONUS FUNCTIONS -- use them freely if you want
    def _is_black(self, node):
        return node == None or node._color == self._Node.BLACK

    def _successor(self, node):
        successor = node._right
        while successor._left != None:
            successor = successor._left
        return successor

    def _sibiling(self, node):
        parent = node._parent

        if parent._left == node:
            return parent._right
        else:
            return parent._left

    # Supporting functions -- DO NOT MODIFY BELOW
    def display(self):
        print('--------------')
        self._display(self._root, 0)
        print('--------------')

    def _display(self, node, depth):
        if node == None:
            return

        if node._right != None:
            if node._right._parent != node:
                print("parent-child error - ", node._element, node._right._element)
            self._display(node._right, depth+1)

        if node == self._root:
            symbol = '>'
        else:
            symbol = '*'

        if node._color == self._Node.RED:
            colorstr = 'R'
        else:
            colorstr = 'B'
        print(f'{"    "*depth}{symbol} {node._element}({colorstr})')
        if node._left != None:
            if node._left._parent != node:
                print("parent error - ", node._element, node._left._element)
            self._display(node._left, depth+1)

    def inorder_traverse(self):
        return self._inorder_traverse(self._root)

    def _inorder_traverse(self, node):
        if node == None:
            return []
        else:
            return self._inorder_traverse(node._left) + [node._element] + self._inorder_traverse(node._right)

    def check_tree_property_silent(self):
        if self._root == None:
            return True

        if not self._check_parent_child_link(self._root):
            print('Parent-child link is violated')
            return False
        if not self._check_binary_search_tree_property(self._root):
            print('Binary search tree property is violated')
            return False
        if not self._root._color == self._Node.BLACK:
            print('Root black property is violated')
            return False
        if not self._check_double_red_property(self._root):
            print('Internal property is violated')
            return False
        if self._check_black_height_property(self._root) == 0:
            print('Black height property is violated')
            return False
        return True

    def check_tree_property(self):
        if self._root == None:
            print('Empty tree')
            return

        print('Checking binary search tree property...')
        self._check_parent_child_link(self._root)
        self._check_binary_search_tree_property(self._root)
        print('Done')

        print('Checking root black property...')
        print(self._root._color == self._Node.BLACK)
        print('Done')

        print('Checking internal property (=no double red)...')
        self._check_double_red_property(self._root)
        print('Done')

        print('Checking black height property...')
        self._check_black_height_property(self._root)
        print('Done')

    def _check_parent_child_link(self, node):
        if node == None:
            return True

        test_pass = True

        if node._right != None:
            if node._right._parent != node:
                print("parent-child error - ", node._element, node._right._element)
            test_pass = test_pass and self._check_parent_child_link(node._right)
        if node._left != None:
            if node._left._parent != node:
                print("parent error - ", node._element, node._left._element)
            test_pass = test_pass and self._check_parent_child_link(node._left)

        return test_pass

    def _check_binary_search_tree_property(self, node):
        if node == None:
            return True

        test_pass = True

        if node._left != None:
            if node._left._element > node._element:
                print("Binary search tree property error - ", node._element, node._left._element)
                return False
            test_pass = test_pass and self._check_binary_search_tree_property(node._left)

        if node._right != None:
            if node._right._element < node._element:
                print("Binary search tree property error - ", node._element, node._right._element)
                return False
            test_pass = test_pass and self._check_binary_search_tree_property(node._right)

        return test_pass

    def _check_double_red_property(self, node):
        if node == None:
            return True

        test_pass = True

        if node._color == self._Node.RED:
            if node._left != None:
                if node._left._color == self._Node.RED:
                    print("Double red property error - ", node._element, node._left._element)
                    return False
            if node._right != None:
                if node._right._color == self._Node.RED:
                    print("Double red property error - ", node._element, node._right._element)
                    return False

        if node._left != None:
            test_pass = test_pass and self._check_double_red_property(node._left)
        if node._right != None:
            test_pass = test_pass and self._check_double_red_property(node._right)

        return test_pass


    def _check_black_height_property(self, node):
        if node == None:
            return 1

        left_height = self._check_black_height_property(node._left)
        right_height = self._check_black_height_property(node._right)

        if left_height != right_height:
            print("Black height property error - ", node._element, left_height, right_height)
            return 0

        if node._color == self._Node.BLACK:
            return left_height + 1
        else:
            return left_height
    