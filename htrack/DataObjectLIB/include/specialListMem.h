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
///       - zero, if the object is equal to the element
///       - else, if the object is not equal than the element
///       If this function exists, the value of 'bool isFindable' could
///       be true to enable the find()-function. Anycase it must be false.
///
///     - if the sort()-function is used, there must be a function like:
///       'int baseClass::isBigger(baseClass element);'
///       This function must return and int representing the comparisson result
///       of two objects of the baseClass. The meaning of this result is:
///       - true, if the object is bigger than the element
///       - false, if the object is not bigger than the element
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
/// $Date: 2008-10-07 10:36:23 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _SPECIALLISTMEM_H
#define _SPECIALLISTMEM_H


#include "../../MiscLIB/include/errorHandling.h"
#include <list>


/* **************************************************************
 * TEMPLATE CLASS specialListMem								*
 * **************************************************************/

template<class objectClass> class specialListMem {

private:

	typename std::list<objectClass>           stackMem;				/**< Memory to store all objects. */
	typename std::list<objectClass>::iterator activeObjectPointer;	/**< Iterator to access one object. */

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
 * method returns an iterator to the element if it is already
 * in the memory. If not it returns an iterator to the end.
 */

	typename std::list<objectClass>::iterator find(objectClass element);

public:

/**
 * Default constructor
 */

	specialListMem();

/**
 * Copy constructor
 */

	specialListMem(const specialListMem<objectClass>& value);

/**
 * Destructor
 */

	~specialListMem();

/**
 * operator = ()
 */

	specialListMem<objectClass>& operator=(const specialListMem<objectClass>& value);

/**
 * method returns the number of entries
 */

	unsigned long getNumberOfEntries();

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
 * method clears the memory
 */

	void sort();

};


/* **************************************************************
 * TEMPLATE CLASS specialListMem								*
 * **************************************************************/


 /**
 * method returns the iterator for the last element in memory
 */

 template<class objectClass> typename std::list<objectClass>::iterator specialListMem<objectClass>::getLastIterator() {

 	typename std::list<objectClass>::iterator lastIterator;

	lastIterator = stackMem.end();
	
	if (!isEmpty())
		lastIterator--;

	return lastIterator;

 }

 /**
 * method pushes the element at the end of the memory.
 */

template<class objectClass> typename std::list<objectClass>::iterator specialListMem<objectClass>::pushIntern(objectClass element) {

	typename std::list<objectClass>::iterator returnValue;
	typename std::list<objectClass>::iterator duplicate;

	duplicate = find(element);

	if (duplicate == stackMem.end()) {

		stackMem.push_back(element);
		returnValue = getLastIterator();
	
	}
	else
		returnValue = duplicate;

	return returnValue;

}


/**
 * method returns the last entry and deletes it
 */

template<class objectClass> objectClass specialListMem<objectClass>::popIntern() {

	objectClass returnValue;

	returnValue = stackMem.back();
	stackMem.pop_back();

	return returnValue;

}

/**
 * method returns if the element is already in the memory
 */

template<class objectClass> typename std::list<objectClass>::iterator specialListMem<objectClass>::find(objectClass element) {

	typename std::list<objectClass>::iterator returnValue;

	for (returnValue = stackMem.begin(); returnValue != stackMem.end(); returnValue++) {

		if (returnValue->compare(element) == 0)
			break;

	}

	return returnValue;

}

/**
 * Default constructor
 */

template<class objectClass> specialListMem<objectClass>::specialListMem() 
: stackMem(), activeObjectPointer() {

	stackMem.clear();
	resetActiveObject();

}

/**
 * Constructor
 */

template<class objectClass> specialListMem<objectClass>::specialListMem(const specialListMem<objectClass>& value) 
: stackMem(value.stackMem), activeObjectPointer(value.activeObjectPointer) {

  //	*this = value;

}

/**
 * Destructor
 */

template<class objectClass> specialListMem<objectClass>::~specialListMem() {

}

/**
 * operator = ()
 */

template<class objectClass> specialListMem<objectClass>& specialListMem<objectClass>::operator=(const specialListMem<objectClass>& value) {

	stackMem.clear();

	stackMem            = value.stackMem;
	activeObjectPointer = find(*value.activeObjectPointer);

	return *this;

}

/**
 * method returns the number of entries
 */

template<class objectClass> unsigned long specialListMem<objectClass>::getNumberOfEntries() {

	typename std::list<objectClass>::size_type numberOfEntries;

	numberOfEntries = stackMem.size();

	return (unsigned long) numberOfEntries;

}

/**
 * method pushes the element at the end of the memory and
 * returns true, if this can be done successfully.
 */

template<class objectClass> bool specialListMem<objectClass>::push(objectClass element) {

	activeObjectPointer = pushIntern(element);

	return true;

}


/**
 * method returns the last entry and deletes it
 */

template<class objectClass> objectClass specialListMem<objectClass>::pop() {

	objectClass returnValue;

	if ((activeObjectPointer == getLastIterator()) && (!isEmpty()))
		activeObjectPointer--;

	returnValue = popIntern();

	return returnValue;

}

/**
 * method returns the first entry and deletes it
 */

template<class objectClass> objectClass specialListMem<objectClass>::popF() {

	objectClass returnValue;

	if ((activeObjectPointer == stackMem.begin()) && (!isEmpty()))
		makeNextOneActive();

	returnValue = stackMem.front();
	stackMem.pop_front();

	return returnValue;

}

/**
 * method returns if the memory is empty
 */

template<class objectClass> bool specialListMem<objectClass>::isEmpty() {

	return stackMem.empty();

}

/**
 * sets the activeObject to the initial value
 */

template<class objectClass> void specialListMem<objectClass>::resetActiveObject() {

	activeObjectPointer = stackMem.begin();

}

/**
 * returns a pointer to the activeObject
 */

template<class objectClass> objectClass* specialListMem<objectClass>::getActiveObject() {

	objectClass* returnValue;

	if (!isEmpty())
		returnValue = (objectClass*)activeObjectPointer->getPointer();
	else
		returnValue = NULL;


	return returnValue;

}

/**
 * returns a reference of the activeObject
 */

template<class objectClass> objectClass specialListMem<objectClass>::readActiveObject() {

	if (!isEmpty())
		return *activeObjectPointer;
	else
		throw cannotReadEmptyMemoryError(DATAOBJECTLIB);

}

/**
 * set the next one to the active object.
 */

template<class objectClass> void specialListMem<objectClass>::makeNextOneActive() {

	if (activeObjectPointer == getLastIterator())
		resetActiveObject();
	else
		activeObjectPointer++;

}

/**
 * returns a reference of the activeObject and set the next one
 * to the active object.
 */

template<class objectClass> objectClass specialListMem<objectClass>::readActiveObjectAndMakeNextOneActive() {

	objectClass returnValue;

	returnValue = readActiveObject();

	makeNextOneActive();

	return returnValue;

}


/**
 * erases the activeObject
 */

template<class objectClass> void specialListMem<objectClass>::eraseActiveObject() {

	typename std::list<objectClass>::iterator position;

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

template<class objectClass> bool specialListMem<objectClass>::isFound(objectClass element, bool set) {

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

template<class objectClass> void specialListMem<objectClass>::clear() {

	stackMem.clear();
	resetActiveObject();

}

/**
 * method sorts the memory
 */

template<class objectClass> void specialListMem<objectClass>::sort() {

	typename std::list<objectClass>::iterator actualMax;

	for (typename std::list<objectClass>::iterator i = stackMem.begin(); i != stackMem.end();) {

		for (typename std::list<objectClass>::iterator j = i; j != stackMem.end(); j++) {

			if (i == j) {

				actualMax = i;

			}
			else {

				if (j->isBigger(*actualMax))
					actualMax = j;

			}

		}

		if (actualMax == i) {

			i++;

		}
		else {
		
			stackMem.insert(i, *actualMax);
			stackMem.erase(actualMax);

		}

	}

}

#endif
