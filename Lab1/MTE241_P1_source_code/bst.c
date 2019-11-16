/****************************************************************************
 * This code is copyright (c) 2017 by the University of Waterloo and may    *
 * not be redistributed without explicit written permission.                *
 ****************************************************************************/
 
 /*
 * Names: Ryan Lithgow & Tristan Curry
 *   1.  uWaterloo User ID:  rmlithgo@uwaterloo.ca
 *   2.  uWaterloo User ID:  tscurry@uwaterloo.ca
 */

/*****************************************************************
*
*                   Exercise 4.1 - C Formatting and Style
*
*****************************************************************
* Instructions: Record 5 changes you made to the code to improve
*               the formatting and style here. Include the 
*               function in which the change was made and a brief
*               justification for the change. You may make changes 
*               to main.c or bst.c.
*****************************************************************
Style guide followed available at: https://cs50.readthedocs.io/style/c/
*  1. Functions bst_min, bst_max, bst_delete and bst_erase had comments
		greater than 100 characters on one line, and were therefore split
*  2. Spacing of bst_delete's arguments within the parentheses is inconsistent
		with the other functions' formatting. Space removed after function name,
		and another added between the last parameter and closing parenthesis.
*  3. All 'if' keywords must be followed with a single space. The offending
		functions that have since been reformatted include bst_insert, bst_min,
		bst_max, bst_delete, bst_erase, and main
*  4. Opening and closing curly braces line up vertically without nested code
		interfering along the same vertical line. This was addressed in bst_delete
		and bst_init
*  5. The code contained in a conditional (if/else) must appear on a separate line
		from the 'if' and 'else' keywords for legibility. bst_delete required such
		adjustment.
****************************************************************/

/*****************************************************************
*
*                   Exercise 6.1 - Logic Bugs
*
*****************************************************************
* Instructions: Record 4 logic bugs you found here. Include the 
*               function where the bug was found and a brief
*               description of how you used the tools to find and
*               fix it. They can be in main.c or bst.c.
*****************************************************************
*  1. size += 1 was after a deletion in bst_erase, per the comment "decrement size
		of the tree" this should be size -= 1. Upon suspecting a problem, tree->size
		was added to Watch 1 and monitored for its behaviour. The first node to be
		deleted was also watched in the Call Stack to verify its disappearance.
*  2. Case 3 in bst_delete was left hanging and idled the whole program, breakpoints were
		added within this function until the problem section was isolated between a 
		breakpoint that was reached, and one that was not. To find the max of a subtree,
		the subtree must be traversed to the right, not to the left.
*  3. Case 2 in bst_delete replaces the node-to-be-deleted with its only child, relative
		to the parent node. However, the sub-case of this where the child is to the right
		confirmed that it was its parent's right then replaced the parent's left and vice
		versa, which is incorrect, deleted entire separate subtrees. To locate this issue,
		we noticed in the Call Stack that the nodes began recursively pointing at themselves
		(e.g. location 0x02 points to 0x02 as its left). The only times that pointers are
		rearranged (not a balanced tree) are at the tree's creation, and when nodes are
		deleted. Having looked at 2 of the 3 deletion cases we inspected the third and
		manually noticed the logic error.
*  4. Bitwise ampersand (&) in bst_delete case 3. Logically in C, it works because
		of how booleans evaluate, but it is not programmatically logically correct for
		this situation.
****************************************************************/

#include <stdio.h>
#include "bst.h"

//helper functions
void bsn_init( bsn_t *node, S32 val) 
{
	node->val = val;
	node->left = NULL;
	node->right = NULL;
}


//Initialize the binary search tree so that it is empty. Run time: &Theta(1)
void bst_init( bst_t *tree ) 
{
	tree->root = NULL;
	tree->size = 0;
}

//Remove all nodes from this binary search tree. Run time: T(n) with T(n) memory. 
//Note: Remember that you do not have to keep a search-tree structure while you are 
//destroying the tree. If you maintain the search-tree structure, the run time will be O(n2). 
//This run time is not required for 2014.
void bst_destroy( bst_t *tree ) {}

//Return the number of nodes in the binary search tree. Run time: &Theta(1)
size_t bst_size( bst_t *tree ) 
{
	size_t treeSize = 0;
	treeSize = tree->size;
	return treeSize;
}

//Insert the integer n into the binary search tree and return false if n is 
//already in the tree (do not add a duplicate into the tree) and true otherwise. 
//Ideally, such a function should not be implemented using recursion. 
//See the marking scheme at the end. Run time: O(h)
bool bst_insert( bst_t *tree, S32 val ) 
{	
	bsn_t *p_currNode = tree->root;
	bsn_t *p_prevNode = p_currNode;
	bsn_t *p_newNode = (bsn_t *) malloc( sizeof( bsn_t ) );
	
	bsn_init(p_newNode, val);
	
	//special case: inserting at the root node
	if (p_currNode == NULL) {
	tree->root = p_newNode;
	tree->size = tree->size + 1;	
	return __TRUE;
	}
	
	//anything but root
	while (p_currNode != NULL) 
	{
		if (p_currNode->val == val) 
		{ 
			// the value is already in the tree, don't insert again
			free(p_newNode);
			return __FALSE;
		}
		else if (val < p_currNode->val) 
		{
			// move left in tree
			p_prevNode = p_currNode;
			p_currNode = p_currNode->left;
		}
		else 
		{ 
		    // move right in tree
			p_prevNode = p_currNode;
			p_currNode = p_currNode->right;
		}
	}
	
	//p_prevNode now points to the last node before the leaf where we are inserting
	if (p_prevNode->val > val) //insert as left child
		p_prevNode->left = p_newNode;
	else //insert as right child
		p_prevNode->right = p_newNode;
	
	tree->size = tree->size + 1;
	
	return __TRUE;
}


// Returns the smallest integer in the binary search tree. Return INT_MAX if the tree is empty.
// Run time: O(h)
S32 bst_min( bst_t *tree ) 
{
	bsn_t *p_currNode = tree->root;
	
	if (tree->size == 0) 
		return INT_MAX;
	else 
	{ 
		// smallest int will be the left-most bottom node
		while(p_currNode->left != NULL) 
		p_currNode = p_currNode->left;
	}
	
	return p_currNode->val;
}


// Returns the largest integer in the binary search tree. Return INT_MIN if the tree is empty.
// Run time: O(h)
S32 bst_max( bst_t *tree ) 
{
	bsn_t *p_currNode = tree->root;
	
	if (tree->size == 0)
	{
		return INT_MIN;
	}
	else
	{ 
		//largest int will be the right-most bottom node
		while(p_currNode->right != NULL) 
			p_currNode = p_currNode->right;
	}
	
	return p_currNode->val;
}

// Perform the actual deletion of the node
// There are three possible cases:
//	1. It is a leaf node
//  2. It has only one child
//	3. It has two children
void bst_delete( bsn_t *p_currNode, bsn_t *p_parentNode )
{
	bsn_t *p_swapNode = NULL;
	bsn_t *p_swapParent = NULL;
	
	// case 1: leaf node, just delete
	if ((p_currNode->right == NULL) && (p_currNode->left == NULL)) 
	{
		//update the pointer on the parent node to NULL
		if (p_parentNode->left == p_currNode) 
		{
			p_parentNode->left = NULL;
		}
		else 
		{
			p_parentNode->right = NULL;
		}
	}
	// case 3: two children, pick one to replace with
	// balancing was not a parameter, so we will always take max of left branch to replace it with
	else if ((p_currNode->right != NULL) && (p_currNode->left != NULL)) 
	{
		p_swapNode = p_currNode->left;
		p_swapParent = p_currNode;
		
		// traverse until we find the max of the subtree
		while(p_swapNode->right  != NULL) 
		{
			p_swapParent = p_swapNode;
			p_swapNode = p_swapNode->right;
		}
		
		// we now have the max of the subtree. By definition, it can only have one left or no
		// children. If it is a leaf, we can simply copy the value into the current node and
		// delete the leaf instead
		p_currNode->val = p_swapNode->val;
		
		if (p_swapParent->left == p_swapNode)
			p_swapParent->left = p_swapNode->left; 
		else
			p_swapParent->right = p_swapNode->left;
		p_currNode = p_swapNode;
	}
	// case 2: one child, replace with child, delete child
	else
	{
		if (p_currNode->left != NULL) //child is to the left
		{
			// need to know whether we are to the left or right of the parent
			if (p_parentNode->left == p_currNode)
			{
				p_parentNode->left = p_currNode->left;
			}
			else
			{
				p_parentNode->right = p_currNode->left;
			}
		}
		else // child is to the right
		{
			// need to know whether we are to the left or right of the parent
			if (p_parentNode->left == p_currNode)
			{
				p_parentNode->left = p_currNode->right;
			}
			else
			{
				p_parentNode->right = p_currNode->right;
			}
		}
	}
	
	p_currNode->left = NULL;
	p_currNode->right = NULL;
	p_currNode->val = 0;
	free(p_currNode);
}

// If the object is in the binary search tree, remove it and return true; otherwise,
// return false and do nothing. Run time: O(h)
bool bst_erase( bst_t *tree, S32 val ) 
{
	bsn_t *p_currNode = tree->root;
	bsn_t *p_parentNode = p_currNode;
	
	while (p_currNode != NULL)
	{
		if (p_currNode->val == val)
		{
			if (p_parentNode == p_currNode) // we are removing the root with one child left
			{
				if ((p_currNode->left != NULL) && (p_currNode->right != NULL)) 
				{ 
					// can swap in from left as usual
					bst_delete ( p_currNode, p_parentNode);
				}
				else
				{
					if ((p_currNode->left == NULL) && (p_currNode->right == NULL)) 
					{
						// last element in tree
						tree->root = NULL;
					}
					else if (p_currNode->left != NULL) 
					{
						tree->root = p_currNode->left;
					}
					else 
						tree->root = p_currNode->right;
					free(p_currNode);
				}
			}
			else 
			{
				bst_delete (p_currNode, p_parentNode);
			}
			tree->size -= 1;	//decrement size of the tree
			return __TRUE;
		}
		else if (val < p_currNode->val) 
		{ 
			//move left in tree
			p_parentNode = p_currNode;
			p_currNode = p_currNode->left;
		}
		else 
		{ 
			//move right in tree
			p_parentNode = p_currNode;
			p_currNode = p_currNode->right;
		}
	}
	return __FALSE;
}
