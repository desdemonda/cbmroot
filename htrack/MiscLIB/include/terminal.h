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
///   file:
///     - functions to manipulate the terminal output
///
/// *******************************************************************
///
/// $Author: csteinle $
/// $Date: 2008-10-24 16:41:39 $
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////


#ifndef _TERMINAL_H
#define _TERMINAL_H


#include <string>
#include <iostream>


/**
 * This struct easily handles the terminal's output including
 * the overwriting sequence.
 */

struct terminalSequence {

terminalSequence() : terminal(), startText(), overwriteText(), endText(), newline(false), relativityCoefficient(0.), relativeInsertion(0), absoluteInsertion(0) {};

	std::streambuf* terminal;				/**< Pointer to the buffer for the output. */
	std::string     startText;				/**< Variable to store the constant beginning of the printed text. */
	std::string     overwriteText;			/**< Variable to store the variable part of the printed text which should be overwritten. */
	std::string     endText;				/**< Variable to store the constant end of the printed text. */
	bool            newline;				/**< Variable to determine if a newline should be printed at the last printing. */
	double          relativityCoefficient;	/**< Variable to store the upgrade factor for each printing in percent. */
	unsigned int    relativeInsertion;		/**< Variable to store the last relative insertion which updates printing. This value prevents printing of the same number. So only changes are printed. */
	unsigned int    absoluteInsertion;		/**< Variable to store the last absolute insertion. */

private:
  terminalSequence(const terminalSequence&);
  terminalSequence& operator=(const terminalSequence&);
};



/**
 * This method creates a default terminalSequence for status.
 * @param terminal is the buffer to print
 * @param startText is the constant beginning of the text to print
 * @param statusMax is the maximal absolute value which is set for 100 percent
 * @param statusDiv divides the maximal absolute value which is set for 100 percent just for the screen
 * @param relative formats the output to percent or absolute number
 * @param newline prints a newline after the last output or not
 * @param statusStart sets up the first number to be printed
 * @param radix formats the number output to the base of radix. Ordinary this is ten.
 * @return sequence is the struct consisting all needed information for the functionality
 */

void createTerminalStatusSequence(terminalSequence* sequence, std::streambuf* terminal, std::string startText, unsigned int statusMax, unsigned int statusDiv = 1, bool relative = true, bool newline = true, unsigned int statusStart = 0, int radix = 10);

/**
 * This method initializes the terminal for overwriting.
 * @param sequence is the created terminalSequence with the functioncreateTerminalStatusSequence(...)
 */

void terminalInitialize(terminalSequence& sequence);

/**
 * This method replaces the cursor to that position where the
 * overwriting sequence begins and overwrites the old sequence.
 * @param sequence is the created terminalSequence with the functioncreateTerminalStatusSequence(...)
 * @param incrementInsertion is the actual increment number to be inserted
 * @param absoluteInsertion is the actual absolute number to be inserted
 * @param radix is the base for the number to be printed
 */

void terminalOverwriteWithIncrement(terminalSequence& sequence, unsigned int incrementInsertion = 1, int radix = 10);
void terminalOverwrite(terminalSequence& sequence, unsigned int absoluteInsertion, int radix = 10);

/**
 * This method replaces the cursor to the next line.
 * @param sequence is the created terminalSequence with the functioncreateTerminalStatusSequence(...)
 */

void terminalFinalize(terminalSequence& sequence);


#endif
