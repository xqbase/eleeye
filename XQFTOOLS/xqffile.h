/*
XQF->PGN Convertor - a Chinese Chess Score Convertion Program
Designed by Morning Yellow, Version: 2.02, Last Modified: Apr. 2007
Copyright (C) 2004-2007 www.elephantbase.net

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef XQFFILE_H
#define XQFFILE_H

struct XqfHeaderStruct {
  char szTag[16];
  char szPiecePos[32];
  char szResult[16];
  char szSetUp[16];
  char szTitle[64];
  char szReserved1[64];
  char szEvent[64];
  char szDate[16];
  char szSite[16];
  char szRed[16];
  char szBlack[16];
  char szRuleTime[64];
  char szRedTime[16];
  char szBlackTime[16];
  char szReserved2[32];
  char szAnnotator[16];
  char szAuthor[16];
  char szReserved3[16];
}; // xqfhd

struct XqfMoveStruct {
  unsigned char ucSrc, ucDst, ucTag, ucReserved;
}; // xqfmv

#endif
