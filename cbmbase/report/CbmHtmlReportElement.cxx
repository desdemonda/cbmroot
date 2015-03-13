/**
 * \file CbmHtmlReportElement.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmHtmlReportElement.h"
#include <sstream>

using std::stringstream;
using std::endl;

CbmHtmlReportElement::CbmHtmlReportElement()
{
}

CbmHtmlReportElement::~CbmHtmlReportElement()
{

}

string CbmHtmlReportElement::TableBegin(
      const string& caption,
      const vector<string>& colNames) const
{
   stringstream ss;
   ss << "<h2>" << caption << "</h2>";
   ss << "<table id=\"efficiency\" >" << endl;
   ss << "<tr>";
   for (int i = 0; i < colNames.size(); i++) {
      ss << "<th>" << colNames[i] << "</th>";
   }
   ss << "</tr>" << endl;
   return ss.str();
}

string CbmHtmlReportElement::TableEnd() const
{
   return "</table>";
}

string CbmHtmlReportElement::TableEmptyRow(
      int nofCols,
      const string& name) const
{
   stringstream ss;
   ss << "<td colspan=\"" << nofCols << "\"><center><b>" << name << "</b></center></td></tr>" << endl;
   return ss.str();
}

string CbmHtmlReportElement::TableRow(
      const vector<string>& row) const
{
   string st = "<tr>";
   for (int i = 0; i < row.size(); i++) {
      st += "<td>" + row[i] + "</td>";
   }
   st += "</tr> \n";
   return st;
}

string CbmHtmlReportElement::Image(
      const string& title,
      const string& file) const
{
   stringstream ss;
   ss << "<h3>" << title << "</h3>";
   ss << "<img src=\"" << file << ".png\" alt=\"" << title << "\" />";

   return ss.str();
}

string CbmHtmlReportElement::DocumentBegin() const
{
   string str = "<html><body><head><style type=\"text/css\">";
   str += "#efficiency";
   str += "{";
   str += "font-family:Verdana, Arial, Helvetica, sans-serif;";
//   str += "width:100%;";
   str += "border-collapse:collapse;";
   str += "}";
   str += "#efficiency td, #efficiency th";
   str += "{";
   str += "font-size:1em;";
   str += "border:1px solid #98bf21;";
   str += "padding:3px 7px 2px 7px;";
   str += "}";
   str += "#efficiency th";
   str += "{";
   str += "font-size:1.1em;";
   str += "text-align:left;";
   str += "padding-top:5px;";
   str += "padding-bottom:4px;";
   str += "background-color:#A7C942;";
   str += "color:#ffffff;";
   str += "}";
   str += "#efficiency tr.alt td";
   str += "{";
   str += "color:#000000;";
   str += "background-color:#EAF2D3;";
   str += "}";
   str += "</style></head>";
   return str;
}

string CbmHtmlReportElement::DocumentEnd() const
{
   return "</body></html>";
}

string CbmHtmlReportElement::Title(
      int size,
      const string& title) const
{
   stringstream ss;
   ss << "<h" << size + 1 << ">" << title << "</h" << size + 1 << ">";
   return ss.str();
}

ClassImp(CbmHtmlReportElement)
