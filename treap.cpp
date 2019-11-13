// CMSC 341 - Fall 2019 - Project 3
// File: treap.cpp

using namespace std ;

#include "treap.h"

Treap::Treap() {
  _nptr = nullptr ;
}

Treap::Treap(const Treap& other) {
  //
  // Implement copy constructor
  //

  //calls copy to create a treap recursively
  _nptr = copy(other._nptr);
}

Treap::~Treap() {
  //
  // Implement destructor
  //

  //calls recursive clear function to free the treap
  cout << " called " << endl;
  clear();
}

const Treap& Treap::operator=(const Treap& rhs) {
  //
  // Implement assignment operator
  //
  
  //frees old treap and creates new treap
  if(this != &rhs){
    clear();
    _nptr = copy(rhs._nptr);
  }
  return *this;
}

// Return -1 if the treap is empty; otherwise, return value in _height.
int Treap::height() const {
  return _nptr ? _nptr->_height : -1 ;
}

// Return the priority of the root node of a Treap.
// Note: numeric_limits<priority_t>::min() returns the minimum value
// of the priority type.  We will only use int priorities, so this is
// the minimum integer value.
priority_t Treap::priority() const {
  return _nptr ? _nptr->_pri : numeric_limits<priority_t>::min() ;
}

const data_t* Treap::find(const data_t& x) {
  //
  // Implement find function
  //

  if(_nptr == nullptr || _nptr->_data == x){
    return &_nptr->_data;
  }
  if(x > _nptr->_data){
    return _nptr->_right.find(x);
  }
  return _nptr->_left.find(x);
}

// Inorder traversal.  Recommend you NOT change this!!!
void Treap::inorder() {

  if ( empty() ) return ;

  cout << "(" ;
  _nptr->_left.inorder() ;

  cout << _nptr->_data << ":" << priority() << ":" << height() ;

  _nptr->_right.inorder() ;
  cout << ")" ;

  return  ;
}

// Used for debugging and grading.  Interprets the characters in
// position[] as directions in the tree.  E.g., "RLR" means, "starting
// from root, go right, then left, then right."  If position[] is an
// empty string, then it just locats the value a the root.
// See project description for details.
bool Treap::locate (const char position[], data_t& x, priority_t& p, int& h, int offset) {

  if (_nptr == nullptr) return false ;

  if (position[offset] == '\0') {
    x = _nptr->_data ;
    p = _nptr->_pri ;
    h = _nptr->_height ;
    return true ;
  }

  if (position[offset] == 'L') {
    return _nptr->_left.locate(position, x, p, h, offset+1) ;
  }

  if (position[offset] == 'R') {
    return _nptr->_right.locate(position, x, p, h, offset+1) ;
  }

  cerr << "Bad position character!\n" ;
  exit(17) ;

  return false ; 
}


// Basic BST insertion.  Does not allow duplicate values.
//
// Complete this function to do Treap insertion.
//
void Treap::insert(const data_t& x, const priority_t& p) {
  //when node reaches the point to insert, a new node is made
  if ( empty() ) {
    _nptr = new TreapNode(x, p) ;
  }
  //Cancels insert if a duplicate value is detected
  else if(_nptr->_data == x){
    cout << "Duplicate value. Insertion canceled" << endl;
    return;
  }
  
  //Traverse right
  if (_nptr->_data < x) {
    _nptr->_right.insert(x, p) ;
  //Traverse left
  } else if (x < _nptr->_data ) {
    _nptr->_left.insert(x, p) ;
  }

  
  //rotate right to maintain Treap structure
  if((_nptr->_left._nptr != nullptr) && (_nptr->_left._nptr->_pri > _nptr->_pri)){
    rotateRight();
    int leftHeight = _nptr->_right._nptr->_left.height();
    int rightHeight = _nptr->_right._nptr->_right.height();
    _nptr->_right._nptr->_height = 1 + ( leftHeight > rightHeight ? leftHeight : rightHeight );
  }
  //rotate left to maintain Treap structure
  else if((_nptr->_right._nptr != nullptr) && (_nptr->_right._nptr->_pri > _nptr->_pri)){
    rotateLeft();
    int leftHeight = _nptr->_left._nptr->_left.height();
    int rightHeight = _nptr->_left._nptr->_right.height();
    _nptr->_left._nptr->_height = 1 + ( leftHeight > rightHeight ? leftHeight : rightHeight );
  }
  //updates height
    int leftHeight = _nptr->_left.height();
    int rightHeight = _nptr->_right.height();
    _nptr->_height = 1 + ( leftHeight > rightHeight ? leftHeight : rightHeight );
  
}

bool Treap::remove(const data_t& x) {
  //
  // Implement treap element removal
  //
  
  //if the node is not found return false
  if(_nptr == nullptr){
    return false;
  }
  //Traverses left using BST values on the nodes
  if(_nptr->_data > x){
    _nptr->_left.remove(x);
    int leftHeight2 = _nptr->_left.height();
    int rightHeight2 = _nptr->_right.height();
    _nptr->_height = 1 + ( leftHeight2 > rightHeight2 ? leftHeight2 : rightHeight2 );
  }
  //Traverses right using BST values on the nodes
  else if(_nptr->_data < x){
    _nptr->_right.remove(x);
    int leftHeight2 = _nptr->_left.height();
    int rightHeight2 = _nptr->_right.height();
    _nptr->_height = 1 + ( leftHeight2 > rightHeight2 ? leftHeight2 : rightHeight2 );
  }

  //Once the code gets to this point, the node to be removed is found
  //deletes to the left if the node is a leaf node
  else if(_nptr->_right._nptr == nullptr){
    TreapNode * tempNode = _nptr->_left._nptr;
    _nptr->_left._nptr = nullptr;
    delete _nptr;
    _nptr = tempNode;
    return true;
  }
  //deletes to the right if the node is a leaf node
  else if(_nptr->_left._nptr == nullptr){
    TreapNode * tempNode = _nptr->_right._nptr;
    tempNode->_height = 0;
    _nptr->_right._nptr = nullptr;
    delete _nptr;
    _nptr = tempNode;
    return true;
  }
  //rotates right to maintain structure and shift the target node down to leaf position
  else if(_nptr->_left._nptr->_pri > _nptr->_right._nptr->_pri){
    rotateRight();
    if(_nptr->_left._nptr != nullptr){
      int leftHeight = _nptr->_right._nptr->_left.height();
      int rightHeight = _nptr->_right._nptr->_right.height();
      _nptr->_right._nptr->_height = 1 + ( leftHeight > rightHeight ? leftHeight : rightHeight );
    }
    //continues rotating down until the target becomes a leaf
    remove(x);
  }
  //rotates left to maintain structure and shift the target node down to leaf position
  else{
    rotateLeft();
    if(_nptr->_right._nptr != nullptr){
      int leftHeight = _nptr->_right._nptr->_left.height();
      int rightHeight = _nptr->_right._nptr->_right.height();
      _nptr->_left._nptr->_height = 1 + ( leftHeight > rightHeight ? leftHeight : rightHeight );
    }
    //continues rotating down until the target becomes a leaf
    remove(x);
  }
  return false;
}

// Treap::dump() just calls TreapNode::dump()
void Treap::dump() {
  if ( !empty() ) _nptr->dump() ;
}

void Treap::rotateLeft(){
  TreapNode * tempHead = _nptr;
  TreapNode * tempRight = _nptr->_right._nptr;

  //if right node is not null, an AVL left rotation is made
  if(_nptr->_right._nptr != nullptr){
    _nptr->_right._nptr = _nptr->_right._nptr->_left._nptr;
    tempRight->_left._nptr = tempHead;
    _nptr = tempRight;
  }
  else{
    //If the right node is null, does nothing
  }
}

void Treap::rotateRight(){
  TreapNode * tempHead = _nptr;
  TreapNode * tempLeft = _nptr->_left._nptr;

  //if left node is not null, an AVL right rotation is made
  if(_nptr->_left._nptr != nullptr){
    _nptr->_left._nptr = _nptr->_left._nptr->_right._nptr;
    tempLeft->_right._nptr = tempHead;
    _nptr = tempLeft;
  }
  else{
    //If the left node is null, does nothing
  }
}

//frees all the dynamic nodes from the treap
void Treap::clear(){
  if(_nptr){
    _nptr->_left.clear();
    _nptr->_right.clear();
    delete _nptr;
    _nptr = nullptr;
  }
}

//Creates a copy of a treapnode to return by using post order traversal
TreapNode* Treap::copy(TreapNode * original)const{
  if(original == nullptr){
    return nullptr;
  }
  else{
    TreapNode * node = new TreapNode(original->_data, original->_pri);
    node->_height = original->_height;
    //recursively makes left nodes in the treap
    if(original->_left._nptr != nullptr){
      node->_left._nptr = copy(original->_left._nptr);
    }
    //recursively make right nodes in the treap
    if(original->_right._nptr != nullptr){
      node->_right._nptr = copy(original->_right._nptr);
    }
    return node;
  }
}

ostream& operator<< (ostream& os, const Treap& T) {
  os << T._nptr ; 
  return os ;
}


TreapNode::TreapNode() : _height(0) {
  // Empty constructor body
}

TreapNode::TreapNode(const data_t& x, const priority_t& p) 
  : _data(x), _pri(p), _height(0) { 
  // Empty constructor body
}

TreapNode::~TreapNode() {
  // Empty destructor
  // Retained for debugging
}

// Dump the contents of a node
void TreapNode::dump() {
  cout << "===== TreapNode::dump()\n" ;
  cout << "      this = " << this << endl ; 
  cout << "      left = " << _left << endl ; 
  cout << "      right= " << _right << endl ; 
  cout << "      data = " << _data << endl ;
  cout << "      height = " << _height << endl ;
  _left.dump() ;
  _right.dump() ;
}

