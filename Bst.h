#ifndef BST_H
#define BST_H

#include <iostream>
#include <functional>

/// @class Node
/// @brief Template node class for Binary Search Tree
template <class T>
class Node {
public:
    T data;
    Node<T>* left;
    Node<T>* right;

    Node(const T& value) : data(value), left(nullptr), right(nullptr) {}
};

/// @class Bst
/// @brief Template Binary Search Tree with function pointers for traversal
template <class T>
class Bst {
private:
    Node<T>* root;

    // Private recursive helper methods
    Node<T>* insertRec(Node<T>* node, const T& value);
    Node<T>* searchRec(Node<T>* node, const T& value) const;

    // Traversal methods with function pointers
    void inOrderRec(Node<T>* node, void (*visit)(const T&)) const;
    void preOrderRec(Node<T>* node, void (*visit)(const T&)) const;
    void postOrderRec(Node<T>* node, void (*visit)(const T&)) const;

    // Traversal methods that collect data
    void inOrderRec(Node<T>* node, void (*visit)(const T&, void*), void* context) const;

    void deleteTreeRec(Node<T>* node);
    bool checkInvariantRec(Node<T>* node, const T& min, const T& max) const;
    Node<T>* copyTreeRec(Node<T>* node);

public:
    Bst();
    ~Bst();
    Bst(const Bst<T>& other);
    Bst<T>& operator=(const Bst<T>& other);

    void insert(const T& value);
    Node<T>* search(const T& value) const;

    // Traversal methods with function pointers
    void inOrder(void (*visit)(const T&)) const;
    void preOrder(void (*visit)(const T&)) const;
    void postOrder(void (*visit)(const T&)) const;

    // Traversal methods that collect data into context
    void inOrder(void (*visit)(const T&, void*), void* context) const;

    // Simple traversals (for backward compatibility)
    void inOrder() const;
    void preOrder() const;
    void postOrder() const;

    bool checkInvariant() const;
    bool isEmpty() const;
    int size() const;
    int height() const;

private:
    int sizeRec(Node<T>* node) const;
    int heightRec(Node<T>* node) const;
};

// Template implementation

template <class T>
Bst<T>::Bst() : root(nullptr) {}

template <class T>
Bst<T>::~Bst() {
    deleteTreeRec(root);
}

template <class T>
Bst<T>::Bst(const Bst<T>& other) : root(nullptr) {
    root = copyTreeRec(other.root);
}

template <class T>
Bst<T>& Bst<T>::operator=(const Bst<T>& other) {
    if (this != &other) {
        deleteTreeRec(root);
        root = copyTreeRec(other.root);
    }
    return *this;
}

template <class T>
Node<T>* Bst<T>::copyTreeRec(Node<T>* node) {
    if (node == nullptr) return nullptr;

    Node<T>* newNode = new Node<T>(node->data);
    newNode->left = copyTreeRec(node->left);
    newNode->right = copyTreeRec(node->right);
    return newNode;
}

template <class T>
void Bst<T>::deleteTreeRec(Node<T>* node) {
    if (node != nullptr) {
        deleteTreeRec(node->left);
        deleteTreeRec(node->right);
        delete node;
    }
}

template <class T>
Node<T>* Bst<T>::insertRec(Node<T>* node, const T& value) {
    if (node == nullptr) {
        return new Node<T>(value);
    }

    if (value < node->data) {
        node->left = insertRec(node->left, value);
    } else if (value > node->data) {
        node->right = insertRec(node->right, value);
    }
    return node;
}

template <class T>
void Bst<T>::insert(const T& value) {
    root = insertRec(root, value);
}

template <class T>
Node<T>* Bst<T>::searchRec(Node<T>* node, const T& value) const {
    if (node == nullptr || node->data == value) {
        return node;
    }

    if (value < node->data) {
        return searchRec(node->left, value);
    } else {
        return searchRec(node->right, value);
    }
}

template <class T>
Node<T>* Bst<T>::search(const T& value) const {
    return searchRec(root, value);
}

// Traversal with simple function pointer
template <class T>
void Bst<T>::inOrderRec(Node<T>* node, void (*visit)(const T&)) const {
    if (node != nullptr) {
        inOrderRec(node->left, visit);
        visit(node->data);
        inOrderRec(node->right, visit);
    }
}

template <class T>
void Bst<T>::inOrder(void (*visit)(const T&)) const {
    inOrderRec(root, visit);
}

template <class T>
void Bst<T>::preOrderRec(Node<T>* node, void (*visit)(const T&)) const {
    if (node != nullptr) {
        visit(node->data);
        preOrderRec(node->left, visit);
        preOrderRec(node->right, visit);
    }
}

template <class T>
void Bst<T>::preOrder(void (*visit)(const T&)) const {
    preOrderRec(root, visit);
}

template <class T>
void Bst<T>::postOrderRec(Node<T>* node, void (*visit)(const T&)) const {
    if (node != nullptr) {
        postOrderRec(node->left, visit);
        postOrderRec(node->right, visit);
        visit(node->data);
    }
}

template <class T>
void Bst<T>::postOrder(void (*visit)(const T&)) const {
    postOrderRec(root, visit);
}

// Traversal with context for data collection
template <class T>
void Bst<T>::inOrderRec(Node<T>* node, void (*visit)(const T&, void*), void* context) const {
    if (node != nullptr) {
        inOrderRec(node->left, visit, context);
        visit(node->data, context);
        inOrderRec(node->right, visit, context);
    }
}

template <class T>
void Bst<T>::inOrder(void (*visit)(const T&, void*), void* context) const {
    inOrderRec(root, visit, context);
}

// Simple traversals (backward compatibility)
template <class T>
void Bst<T>::inOrder() const {
    inOrder([](const T& value) {
        std::cout << value << " ";
    });
}

template <class T>
void Bst<T>::preOrder() const {
    preOrder([](const T& value) {
        std::cout << value << " ";
    });
}

template <class T>
void Bst<T>::postOrder() const {
    postOrder([](const T& value) {
        std::cout << value << " ";
    });
}

template <class T>
bool Bst<T>::checkInvariantRec(Node<T>* node, const T& min, const T& max) const {
    if (node == nullptr) return true;

    if (!(node->data > min) || !(node->data < max)) {
        return false;
    }

    return checkInvariantRec(node->left, min, node->data) &&
           checkInvariantRec(node->right, node->data, max);
}

template <class T>
bool Bst<T>::checkInvariant() const {
    T minVal = T();
    T maxVal = T();
    return checkInvariantRec(root, minVal, maxVal);
}

template <class T>
bool Bst<T>::isEmpty() const {
    return root == nullptr;
}

template <class T>
int Bst<T>::sizeRec(Node<T>* node) const {
    if (node == nullptr) return 0;
    return 1 + sizeRec(node->left) + sizeRec(node->right);
}

template <class T>
int Bst<T>::size() const {
    return sizeRec(root);
}

template <class T>
int Bst<T>::heightRec(Node<T>* node) const {
    if (node == nullptr) return -1;
    return 1 + std::max(heightRec(node->left), heightRec(node->right));
}

template <class T>
int Bst<T>::height() const {
    return heightRec(root);
}

#endif // BST_H
