from tree import Tree # tree.py에서 Tree를 import시켜서 사용하겠다.

class LinkedTree(Tree): # LinkedTree라는 클래스를 Tree를 받아서 정의한다.

    #-------------------------- nested _Node class --------------------------
    class _Node: # LinkedTree의 노드를 나타낼 수 있는 내부 클래스 _Node를 정의한다. 그리고 _Node는 클래스 생성자이다.
        __slots__ = '_element', '_parent', '_children' # streamline memory usage

        def __init__(self, element, parent=None, children=None):
            self._element = element # the element of this node
            self._parent = parent # a link towards the parent
             # If 'children' is None, initialize an empty list for children nodes;
            # otherwise, use the provided list of children nodes
            if children == None:
                self._children = []
            else:
                self._children = children # list of links towards children nodes

    #-------------------------- nested Position class -------------------------
    # LinkedTree에서 위치를 나타내는 클래스 position을 정의함.
    class Position(Tree.Position):
    
        def __init__(self, container, node):
            self._container = container # Position이 속한 LinkedTree의 인스턴스
            self._node = node #node : _Node의 인스턴스

        def element(self):
            #position class가 참조하는 노드에 있는 element를 반환한다.
            return self._node._element

        def __eq__(self, other):
            #두 position 객체가 동일한 노드를 참조할 시, true를 반환한다.
            return type(other) is type(self) and other._node is self._node

    #------------------------------- utility methods -------------------------------
    # _validate는 position이 주어지면, 그것이 유효한 지 확인한다.
    def _validate(self, p):
        # p가 position의 인스턴스인지 확인하고, 아니면 TypeError를 반환한다.
        if not isinstance(p, self.Position):
            raise TypeError('p must be proper Position type')
        # p가 LinkedTree 인스턴스인지 확인해서, 아니면 ValueError를 반환한다.
        if p._container is not self:
            raise ValueError('p does not belong to this container')
            
        if p._node._parent is p._node:            # convention for deprecated nodes
            raise ValueError('p is no longer valid')
        return p._node # p가 유효하면, _Node 인스턴스를 반환한다.

    def _make_position(self, node):
        # _Node 인스턴스에 position 객체를 생성해 준다.
        return self.Position(self, node) if node is not None else None
        # node가 없으면, None을 반환한다.

    #-------------------------- Tree constructor --------------------------
    #LinkedTree에 대해 생성자를 정의한다.
    def __init__(self):
        #LinkedTree의 root node를 None으로 초기ㅔ화하고 사이즈를 0으로 만든다.
        self._root = None
        self._size = 0

    #-------------------------- public accessors --------------------------    
    # root 메서드는 LinkedTree의 root node에 대해서 position 객체를 반환한다.
    def root(self):
        return self._make_position(self._root)

    # parent 메서드는 그 position에 있는 것이 유효한지 확인하고, 부모 노드의 position 객체를 반환한다.
    def parent(self, p):
        node = self._validate(p) # 받은 Position 객체가 유효한지 확인한다.
        return self._make_position(node._parent) # node의 부모 Position 객체를 반환한다.

    # num_children은 position 객체가 참조하는 node의 자식 수를 확인한다.
    def num_children(self, p):
        node = self._validate(p) # 받은 Position 객체가 유효한지 확인한다.
        return len(node._children) # node의 children 개수를 반환한다.
    
    # children 메서드는 지정된 객체가 참조하는 node의 자식에 대한 Position 객체에 대한 yield를 반환한다.
    def children(self, p):
        node = self._validate(p) # 받은 Position 객체가 유효한지 확인한다.
        for child in node._children:
            yield self._make_position(child) # node의 자식 Position 객체를 생성한다.

    # len 메서드는 Linked Tree 크기를 반환한다.
    def __len__(self):
        return self._size 
    
    #-------------------------- nonpublic mutators --------------------------
    
    #_add_root 메서드는 트리에 root node를 추가한다.
    def _add_root(self, e):
        # 이미 루트 노드가 존재하면, ValueError 예외를 발생시킨다.
        if self._root is not None:
            raise ValueError('Root exists')
        self._size = 1  # 트리의 크기를 1로 설정한다.
        self._root = self._Node(e) # 새로운 root node를 생성하고, 트리의 root로 설정한다.
        return self._make_position(self._root)
        # 루트 node를 포함하는 'Position' 객체를 반환함.
    
    
    # _add_child는 parent node p에 새로운 child node e를 추가한다.
    def _add_child(self, p, e):
        parent_node = self._validate(p)  # 부모 node p 가 유효한 Position 객체인지 확인하고, parent_node에 할당한다.
        new_node = self._Node(e, parent_node) #자식 node인 e를 생성하고, parent node에 연결시킨다. 
        parent_node._children.append(new_node) # parent node의 children node 리스트에 new_node, 즉 위에서 만든 새로운 자식 노드인 'new_node'를 추가시킨다.
        self._size += 1 #크기를 1 증가시킨다.
        return self._make_position(new_node)  # child node까지 포함시킨 Position 객체를 반환한다.

    # p 의 원소를 e로 교체하는 메서드이다.
    def _replace(self, p, e):
        node = self._validate(p) # p가 유효한지 검사한 후, p를 node에 할당함.
        old_element = node._element # node의 원소값을 old_element에 저장한다.
        node._element = e # 현 node값을 e라는 원소 값으로 교체한다.
        return old_element #이전 원소 값을 반환한다.

    # 트리의 맨 바깥쪽에 있는 p를 제거하는 메서드이다. 여기서 p가 자식이 있으면, ValueError를 발생시킨다.
    def _delete(self, p):
        node = self._validate(p) # p가 유효한 position 객체인지 확인하고, node에 할당한다.
        # p가 자식을 가지고 있다면, ValueError를 발생시킨다.
        if self.num_children(p) > 0:
            raise ValueError
        parent_node = node._parent # node의 부모 노드를 가져와 parent_node에 할당한다.
        
        # 부모 노드가 없을 시, root를 None으로 지정하고, 그 외의 경우에는 부모 노드의 리스트에서 그것을 제거한다.
        if parent_node is None:
            self._root = None
        else:
            parent_node._children.remove(node)
        self._size -= 1 #트리의 사이즈를 1 줄인다.
        element = node._element # 삭제한 노드의 원소값을 element에 저장한다.
        return element # element를 반환한다.