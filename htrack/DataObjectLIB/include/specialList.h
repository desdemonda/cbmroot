//////////////////////////////////////////////////////////////////////
/// (C)opyright 2004
/// 
/// Institute of Computer Science V
/// Prof. Männer
/// University of Mannheim, Germany
/// 
/// *******************************************************************
/// 
/// Designer(s):   Steinle
/// 
/// *******************************************************************
/// 
/// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
/// 
/// *******************************************************************
/// 
/// Description:
///
///   class:
///     - template class for handling memory for special lists of objects
///     - because of being a template class the function-bodies must be
///       in this header file, too. There is actually no compiler which
///       can handle a different cxx-source file for the bodies.
///
///
///   CAUTION:
///
///     - if the print()-function is used, there must be an operator like:
///       'operator std::string();'
///       because a conversion from the object to a std:string has to be done.
///       This operator converts one Object into a string-representation.
///       All objects are printed using this operator by separating them with
///       a linefeed. If this operator exists, the value of 'bool isPrintable' could
///       be true to enable the print()-function. Anycase it must be false.
///
///     - if the getActiveObject()-function is used, there must be a function like:
///       'baseClass* baseClass::getPointer();'
///       This function must return the address of the object. This
///       is needed to modify an element which is inserted in the memory.
///       If this function exists, the value of 'bool isModifiable' could
///       be true to enable the getActiveObject()-function. Anycase it must be false.
///
///     - if the isFound()-function (find()) is used, there must be a function like:
///       'int baseClass::compare(baseClass element);'
///       This function must return and int representing the comparisson result
///       of two objects of the baseClass. The meaning of this result is:
///       - negative, if the object is smaller than the element
///       - zero, if the object is equal to the element
///       - positive, if the object is bigger than the element
///       If this function exists, the value of 'bool isFindable' could
///       be true to enable the find()-function. Anycase it must be false.
///
///     - if the 'bool isSorted'-element is used, there must be a function like:
///       'int baseClass::compare(baseClass element);'
///       This function must return and int representing the comparisson result
///       of two objects of the baseClass. The meaning of this result is:
///       - negative, if the object is smaller than the element
///       - zero, if the object is equal to the element
///       - positive, if the object is bigger than the element
///       If this function exists, the pushed values are in a sorted order. But
///       if one object is changed with the getActiveObject()-function, the order
///       may be corrupted. That means if you have the order by a special id and
///       you change this id, it is clear that the order is corrupted. So the
///       memory must be sorted again to be in the order.
///
///     - if the 'bool noDuplicates'-element is used, there must be a function like:
///       'int baseClass::compare(baseClass element);'
///       This function must return and int representing the comparisson result
///       of two objects of the baseClass. The meaning of this result is:
///       - negative, if the object is smaller than the element
///       - zero, if the object is equal to the element
///       - positive, if the object is bigger than the element
///       If this function exists, the pushed values are not inserted if the comparisson
///       result is zero because it is treated as duplicate.
///
///
///
///   USAGE:
///
///     - The function isFound(element, false) can be used to find duplicates in the
///       memory. This is useful for the push(element)-function to avoid duplicates.
///       In the cbm-experiment you can use this function isFound(element, true) 
///       in combination with objectClass* getActiveObject()-function to manipulate
///       the objects in the memory. E. g.
///          if (isFound(element, true))
///              getActiveObject()->frequency++;
///          else
///              push(element);
///
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-08-14 12:32:41 $
/// $Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _SPECIALLIST_H
#define _SPECIALLIST_H


#include "../../MiscLIB/include/errorHandling.h"
#include <list>
#include <string>


/* **************************************************************
 * TEMPLATE CLASS specialList									*
 * **************************************************************/

template<class objectClass, bool isPrintable, bool isFindable, bool isModifiable, bool isSorted, bool noDuplicates> class specialList {

private:

	typename std::list<objectClass>           stackMem;					/**< Memory to store all objects. */
	typename std::list<objectClass>::iterator activeObjectPointer;		/**< Iterator to access one object. */
	bool                                      possibleOrderCorruption;	/**< Variable to store if a sorting is neccessary before doing something else. */

/**
 * method returns the iterator for the last element in memory
 */

	typename std::list<objectClass>::iterator getLastIterator();

/**
 * method pushes the element at the end of the memory.
 */

	typename std::list<objectClass>::iterator pushIntern(objectClass element);

/**
 * method returns the last entry and deletes it
 */

	objectClass popIntern();

/**
 * method returns an iterator to the position of the element
 * where it should be in a sorted style. If it is already in
 * the memory, the iterator is to this element.
 */

	typename std::list<objectClass>::iterator findPosition(objectClass element);

/**
 * method returns an iterator to the element if it is already
 * in the memory. If not it returns an iterator to the end.
 */

	typename std::list<objectClass>::iterator find(objectClass element);

public:

/**
 * Default constructor
 */

	specialList();

/**
 * Constructor
 */

	specialList(const specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>& value);

/**
 * Destructor
 */

	~specialList();

/**
 * operator = ()
 */

	specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>& operator=(const specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>& value);

/**
 * method returns the number of entries
 */

	unsigned long getNumberOfEntries();

/**
 * method sorts the objects in the memory, and returns true, if
 * this can be done successfully.
 */

	bool sort();

/**
 * method pushes the element at the end of the memory and
 * returns true, if this can be done successfully.
 */

	bool push(objectClass element);

/**
 * method returns the last entry and deletes it
 */

	objectClass pop();

/**
 * method returns the first entry and deletes it
 */

	objectClass popF();

/**
 * method returns if the memory is empty
 */

	bool isEmpty();

/**
 * sets the activeObject to the initial value
 */

	void resetActiveObject();

/**
 * returns a pointer to the activeObject
 */

	objectClass* getActiveObject();

/**
 * returns a reference of the activeObject
 */

	objectClass readActiveObject();

/**
 * set the next one to the active object.
 */

	void makeNextOneActive();

/**
 * returns a reference of the activeObject and set the next one
 * to the active object.
 */

	objectClass readActiveObjectAndMakeNextOneActive();

/**
 * erases the activeObject
 */

	void eraseActiveObject();

/**
 * method returns if the element is already in the memory
 * @param element is the element to search
 * @param set indicates if the active object is set to the element if it is found
 */

	bool isFound(objectClass element, bool set = false);

/**
 * method clears the memory
 */

	void clear();

/**
 * method returns a std::string containing a representation of
 * each element.
 */

	std::string print();

};


/* **************************************************************
 * TEMPLATE CLASS specialList									*
 * **************************************************************/


 /**
 * method returns the iterator for the last element in memory
 */

 template<class objectClass, bool isPrintable, bool isFindable, bool isModifiable, bool isSorted, bool noDuplicates> typename std::list<objectClass>::iterator specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>::getLastIterator() {

 	typename std::list<objectClass>::iterator lastIterator;

	lastIterator = stackMem.end();
	
	if (!isEmpty())
		lastIterator--;

	return lastIterator;

 }

 /**
 * method pushes the element at the end of the memory.
 */

template<class objectClass, bool isPrintable, bool isFindable, bool isModifiable, bool isSorted, bool noDuplicates> typename std::list<objectClass>::iterator specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>::pushIntern(objectClass element) {

	typename std::list<objectClass>::iterator returnValue;

	if (isSorted) {

		typename std::list<objectClass>::iterator duplicate;

		duplicate = findPosition(element);

		if (noDuplicates) {

			if (duplicate == stackMem.end())
				returnValue = stackMem.insert(duplicate, element);
			else if (duplicate->compare(element) != 0)
				returnValue = stackMem.insert(duplicate, element);
			else
				returnValue = duplicate;
		
		}
		else 
			returnValue = stackMem.insert(duplicate, element);

	}
	else {

		if (noDuplicates) {

			typename std::list<objectClass>::iterator duplicate;

			duplicate = find(element);

			if (duplicate == stackMem.end()) {

				stackMem.push_back(element);
				returnValue = getLastIterator();
			
			}
			else
				returnValue = duplicate;

		}
		else {

			stackMem.push_back(element);
			returnValue = getLastIterator();

		}

	}

	return returnValue;

}

/**
 * method returns the last entry and deletes it
 */

template<class objectClass, bool isPrintable, bool isFindable, bool isModifiable, bool isSorted, bool noDuplicates> objectClass specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>::popIntern() {

	objectClass returnValue;

	returnValue = stackMem.back();
	stackMem.pop_back();

	return returnValue;

}

 /**
 * method returns an iterator to the position of the element
 * where it should be in a sorted style. If it is already in
 * the memory, the iterator is to this element.
 */

template<class objectClass, bool isPrintable, bool isFindable, bool isModifiable, bool isSorted, bool noDuplicates> typename std::list<objectClass>::iterator specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>::findPosition(objectClass element) {

 	typename std::list<objectClass>::iterator returnValue;

	if (isSorted) {

		if (possibleOrderCorruption)
			sort();

		for (returnValue = stackMem.begin(); returnValue != stackMem.end(); returnValue++) {

			if (returnValue->compare(element) >= 0)
				break;

		}

	}
	else
		returnValue = stackMem.end();

	return returnValue;

 }

/**
 * method returns if the element is already in the memory
 */

template<class objectClass, bool isPrintable, bool isFindable, bool isModifiable, bool isSorted, bool noDuplicates> typename std::list<objectClass>::iterator specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>::find(objectClass element) {

	typename std::list<objectClass>::iterator returnValue;

	if (isFindable) {

		if (isSorted) {

			returnValue = findPosition(element);

			if (returnValue != stackMem.end()) {

				if (returnValue->compare(element) != 0)
					returnValue = stackMem.end();

			}

		}
		else {

			for (returnValue = stackMem.begin(); returnValue != stackMem.end(); returnValue++) {

				if (returnValue->compare(element) == 0)
					break;

			}

		}

	}
	else
		returnValue = stackMem.end();

	return returnValue;

}

/**
 * Default constructor
 */

template<class objectClass, bool isPrintable, bool isFindable, bool isModifiable, bool isSorted, bool noDuplicates> specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>::specialList() :  stackMem(), activeObjectPointer(), possibleOrderCorruption(false)
{

  //	possibleOrderCorruption = false;

	stackMem.clear();
	resetActiveObject();

}

/**
 * Constructor
 */

template<class objectClass, bool isPrintable, bool isFindable, bool isModifiable, bool isSorted, bool noDuplicates> specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>::specialList(const specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>& value) 
:  stackMem(value.stackMem), activeObjectPointer(value.activeObjectPointer), possibleOrderCorruption(value.possibleOrderCorruption)
{

  //	*this = value;

}

/**
 * Destructor
 */

template<class objectClass, bool isPrintable, bool isFindable, bool isModifiable, bool isSorted, bool noDuplicates> specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>::~specialList() {

}

/**
 * operator = ()
 */

template<class objectClass, bool isPrintable, bool isFindable, bool isModifiable, bool isSorted, bool noDuplicates> specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>& specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>::operator=(const specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>& value) {

	typename std::list<objectClass>::iterator iterator;
	unsigned long                             difference;

	stackMem.clear();
	stackMem                = value.stackMem;

	iterator                = value.activeObjectPointer;
	difference              = 0;

	if (isFindable) {

		activeObjectPointer = find(*value.activeObjectPointer);

	}
	else {

		while (iterator != value.stackMem.begin()) {

			difference++;
			iterator--;

		}

		activeObjectPointer     = stackMem.begin();

		for (unsigned long i = 0; i < difference; i++)
			makeNextOneActive();

	}

	possibleOrderCorruption = value.possibleOrderCorruption;

	return *this;

}

/**
 * method returns the number of entries
 */

template<class objectClass, bool isPrintable, bool isFindable, bool isModifiable, bool isSorted, bool noDuplicates> unsigned long specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>::getNumberOfEntries() {

	typename std::list<objectClass>::size_type numberOfEntries;

	numberOfEntries = stackMem.size();

	return (unsigned long) numberOfEntries;

}

/**
 * method sorts the objects in the memory, and returns true, if
 * this can be done successfully.
 */

template<class objectClass, bool isPrintable, bool isFindable, bool isModifiable, bool isSorted, bool noDuplicates> bool specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>::sort() {

	bool returnValue;

	if (isSorted) {

		typename std::list<objectClass> tempStackMem;
		objectClass                     element;

		element = readActiveObject();

		while (!isEmpty())
			tempStackMem.push_back(popIntern());

		possibleOrderCorruption = false;

		while(!tempStackMem.empty()) {

			pushIntern(tempStackMem.back());
			tempStackMem.pop_back();

		}

		activeObjectPointer = find(element);

		returnValue = true;

	}
	else
		returnValue = false;

	return returnValue;

}

/**
 * method pushes the element at the end of the memory and
 * returns true, if this can be done successfully.
 */

template<class objectClass, bool isPrintable, bool isFindable, bool isModifiable, bool isSorted, bool noDuplicates> bool specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>::push(objectClass element) {

	activeObjectPointer = pushIntern(element);

	return true;

}

/**
 * method returns the last entry and deletes it
 */

template<class objectClass, bool isPrintable, bool isFindable, bool isModifiable, bool isSorted, bool noDuplicates> objectClass specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>::pop() {

	objectClass returnValue;

	if (isSorted)
		if (possibleOrderCorruption)
			sort();

	if ((activeObjectPointer == getLastIterator()) && (!isEmpty()))
		activeObjectPointer--;

	returnValue = popIntern();

	return returnValue;

}

/**
 * method returns the first entry and deletes it
 */

template<class objectClass, bool isPrintable, bool isFindable, bool isModifiable, bool isSorted, bool noDuplicates> objectClass specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>::popF() {

	objectClass returnValue;

	if (isSorted)
		if (possibleOrderCorruption)
			sort();

	if ((activeObjectPointer == stackMem.begin()) && (!isEmpty()))
		makeNextOneActive();

	returnValue = stackMem.front();
	stackMem.pop_front();

	return returnValue;

}

/**
 * method returns if the memory is empty
 */

template<class objectClass, bool isPrintable, bool isFindable, bool isModifiable, bool isSorted, bool noDuplicates> bool specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>::isEmpty() {

	return stackMem.empty();

}

/**
 * sets the activeObject to the initial value
 */

template<class objectClass, bool isPrintable, bool isFindable, bool isModifiable, bool isSorted, bool noDuplicates> void specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>::resetActiveObject() {

	activeObjectPointer = stackMem.begin();

}

/**
 * returns a pointer to the activeObject
 */

template<class objectClass, bool isPrintable, bool isFindable, bool isModifiable, bool isSorted, bool noDuplicates> objectClass* specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>::getActiveObject() {

	objectClass* returnValue;

	if (isModifiable) {

		if (!isEmpty()) {
			returnValue             = (objectClass*)activeObjectPointer->getPointer();
			possibleOrderCorruption = true;
		}
		else
			returnValue = NULL;

	}
	else
		returnValue = NULL;

	return returnValue;

}

/**
 * returns a reference of the activeObject
 */

template<class objectClass, bool isPrintable, bool isFindable, bool isModifiable, bool isSorted, bool noDuplicates> objectClass specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>::readActiveObject() {

	if (!isEmpty())
		return *activeObjectPointer;
	else
		throw cannotReadEmptyMemoryError(DATAOBJECTLIB);

}

/**
 * set the next one to the active object.
 */

template<class objectClass, bool isPrintable, bool isFindable, bool isModifiable, bool isSorted, bool noDuplicates> void specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>::makeNextOneActive() {

	if (activeObjectPointer == getLastIterator())
		resetActiveObject();
	else
		activeObjectPointer++;

}

/**
 * returns a reference of the activeObject and set the next one
 * to the active object.
 */

template<class objectClass, bool isPrintable, bool isFindable, bool isModifiable, bool isSorted, bool noDuplicates> objectClass specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>::readActiveObjectAndMakeNextOneActive() {

	objectClass returnValue;

	if (isSorted)
		if (possibleOrderCorruption)
			sort();

	returnValue = readActiveObject();

	makeNextOneActive();

	return returnValue;

}


/**
 * erases the activeObject
 */

template<class objectClass, bool isPrintable, bool isFindable, bool isModifiable, bool isSorted, bool noDuplicates> void specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>::eraseActiveObject() {

	typename std::list<objectClass>::iterator position;

	if (isSorted)
		if (possibleOrderCorruption)
			sort();

	if (activeObjectPointer != stackMem.end()) {

		position = activeObjectPointer;

		makeNextOneActive();
	
		stackMem.erase(position);

	}

}

/**
 * method returns if the element is already in the memory
 * @param element is the element to search
 * @param set indicates if the active object is set to the element if it is found
 */

template<class objectClass, bool isPrintable, bool isFindable, bool isModifiable, bool isSorted, bool noDuplicates> bool specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>::isFound(objectClass element, bool set) {

	typename std::list<objectClass>::iterator elementPointer;
	bool                                      returnValue;
	
	elementPointer = find(element);

	if (elementPointer == stackMem.end())
		returnValue = false;
	else {
		returnValue = true;
		if (set)
			activeObjectPointer = elementPointer;
	}

	return returnValue;

}

/**
 * method clears the memory
 */

template<class objectClass, bool isPrintable, bool isFindable, bool isModifiable, bool isSorted, bool noDuplicates> void specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>::clear() {

	possibleOrderCorruption = false;

	stackMem.clear();
	resetActiveObject();

}

/**
 * method returns a std::string containing a representation of
 * each element.
 */

template<class objectClass, bool isPrintable, bool isFindable, bool isModifiable, bool isSorted, bool noDuplicates> std::string specialList<objectClass, isPrintable, isFindable, isModifiable, isSorted, noDuplicates>::print() {

	std::string returnValue;

	returnValue.clear();

	if (isPrintable) {

		for (typename std::list<objectClass>::iterator i = stackMem.begin(); i != stackMem.end(); i++) {
		
			returnValue += *i;
			returnValue += "\n";

		}

	}

	return returnValue;

}

#endif
