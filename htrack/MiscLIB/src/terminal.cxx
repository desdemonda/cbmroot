/* *******************************************************************
// (C)opyright 2004
// 
// Institute of Computer Science V
// Prof. Männer
// University of Mannheim, Germany
// 
// *******************************************************************
// 
// Designer(s):   Steinle
// 
// *******************************************************************
// 
// Project:     Trackfinder for CBM-Project at GSI-Darmstadt, Germany
// 
// *******************************************************************
// 
// Description:
//
//   file:
//     - functions to manipulate the terminal output
//
// *******************************************************************
//
// $Author: csteinle $
// $Date: 2008-10-24 16:41:39 $
// $Revision: 1.4 $
//
// *******************************************************************/


#include "../include/conversionRoutines.h"
#include "../include/errorHandling.h"
#include "../include/terminal.h"


/****************************************************************
 * This method creates a default terminalSequence for status.	*
 ****************************************************************/

void createTerminalStatusSequence(terminalSequence* sequence, std::streambuf* terminal, std::string startText, unsigned int statusMax, unsigned int statusDiv, bool relative, bool newline, unsigned int statusStart, int radix) {

	unsigned int localStatusDiv;
	char         buffer[intConversionDigits+1];
	int          bufferLength;

	if (sequence != NULL) {

		if (statusDiv == 0)
			localStatusDiv = 1;
		else
			localStatusDiv = statusDiv;

		bufferLength = uitos((unsigned int)(((double)statusMax / (double)localStatusDiv) + 0.5), buffer, radix, intConversionDigits);
		if (bufferLength == -1)
			bufferLength = 1;
		sequence->terminal                   = terminal;
		sequence->startText                  = startText + " ";

		if (relative) {

			bufferLength                     = 3;

			sequence->endText                = "% of 100% (";
			sequence->endText               += buffer;
			sequence->endText               += ")";
			sequence->relativityCoefficient  = (double)statusMax / 100;

		}
		else {

			sequence->endText                = " of ";
			sequence->endText               += buffer;
			sequence->relativityCoefficient  = 1;

		}

		sequence->overwriteText.clear();
		for (unsigned short i = 0; i < bufferLength - 1; i++)
			sequence->overwriteText         += " ";
		sequence->overwriteText             += "0";
		sequence->newline                    = newline;
		sequence->relativeInsertion          = (unsigned int)(((double)statusStart / sequence->relativityCoefficient) + 0.5);;
		sequence->absoluteInsertion          = statusStart;

	}

}

/****************************************************************
 * This method initializes the terminal for overwriting.		*
 ****************************************************************/

void terminalInitialize(terminalSequence& sequence) {

	const char moveCursor = 8;		/* Decimal ascii number of backspace */

	if (sequence.terminal != NULL) {

		sequence.terminal->sputn(sequence.startText.c_str(), (std::streamsize)sequence.startText.size());
		sequence.terminal->sputn(sequence.overwriteText.c_str(), (std::streamsize)sequence.overwriteText.size());
		sequence.terminal->sputn(sequence.endText.c_str(), (std::streamsize)sequence.endText.size());
		for (unsigned short i = 0; i < (unsigned short)sequence.endText.size(); i++)
			sequence.terminal->sputn(&moveCursor, 1);
		sequence.terminal->pubsync();
	}

}

/****************************************************************
 * This method replaces the cursor to that position where the	*
 * overwriting sequence begins and overwrites the old sequence.	*
 ****************************************************************/

void terminalOverwriteWithIncrement(terminalSequence& sequence, unsigned int incrementInsertion, int radix) {

	terminalOverwrite(sequence, sequence.absoluteInsertion + incrementInsertion, radix);

}
void terminalOverwrite(terminalSequence& sequence, unsigned int absoluteInsertion, int radix) {

	unsigned short i;
	char*          buffer;
	int            bufferLength;
	const char     moveCursor = 8;		/* Decimal ascii number of backspace */

	if (sequence.terminal != NULL) {

		if (sequence.absoluteInsertion < absoluteInsertion)
			sequence.absoluteInsertion  = absoluteInsertion;

		if (sequence.relativeInsertion < (unsigned int)(((double)absoluteInsertion / sequence.relativityCoefficient) + 0.5)) {

			sequence.relativeInsertion = (unsigned int)(((double)absoluteInsertion / sequence.relativityCoefficient) + 0.5);

			buffer = new char[(unsigned short)sequence.overwriteText.size() + 2];
			if (buffer == NULL)
				throw memoryAllocationError(MISCLIB);

			bufferLength = uitos(sequence.relativeInsertion, buffer, radix, (unsigned short)sequence.overwriteText.size());
			if (bufferLength == -1)
				bufferLength = 1;

			for (i = 0; i < (unsigned short)sequence.overwriteText.size(); i++)
				sequence.terminal->sputn(&moveCursor, 1);

			for (i = 0; i < (unsigned short)sequence.overwriteText.size() - (unsigned short)bufferLength; i++)
				sequence.terminal->sputn(" ", 1);

			sequence.terminal->sputn(buffer, bufferLength);
			sequence.terminal->pubsync();

			if (buffer != NULL) {
				delete[] buffer;
				buffer = NULL;
			}

		}

	}

}

/****************************************************************
 * This method replaces the cursor to the next line.			*
 ****************************************************************/

void terminalFinalize(terminalSequence& sequence) {

	if (sequence.terminal != NULL) {

		sequence.terminal->sputn(sequence.endText.c_str(), (std::streamsize)sequence.endText.size());
		if (sequence.newline)
			sequence.terminal->sputn("\n", 1);
		sequence.terminal->pubsync();

	}

}
