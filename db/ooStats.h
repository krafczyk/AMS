//  $Id: ooStats.h,v 1.3 2001/01/22 17:32:30 choutko Exp $
/* 
 ******************************************************************************
 *
 *      Objectivity Inc.
 *      Copyright (c) 1995, Objectivity, Inc. USA   All rights reserved.
 *
 ****************************************************************************
 *
 *      File:   ooStats.C
 *
 *      Functions(s):
 *
 *      Description:
 *
 *      RCSid = "$Id: ooStats.h,v 1.3 2001/01/22 17:32:30 choutko Exp $"
 *
 ****************************************************************************
*/

/* 
 **********************************************************************************
 *
 * DISCLAIMER: This class contains structures which mirror internal Objectivity
 *             Object and Storage Manager structures. These status structures
 *             are subject to change, so the validity (and integrity) of the
 *             results cannot be guaranteed for releases after/before 3.5.0.
 *             The accuracy of the values provided is the same as for ooRunStatus().
 *
 **********************************************************************************
*/

#ifndef _OOSTATS_H
#define _OOSTATS_H

#include <stdio.h>
#include <iostream.h>
#include <memory.h>
#include <sys/types.h>
#include <oo.h>


struct xomHK {
#ifdef OO_ALPHAOSF1
	unsigned long	x;
#else
	uint32		x;
#endif
	char*		y;
};

typedef	uint32		xomFDID;

typedef struct
{
	xomHK           HK;
	xomFDID         FDID;
	uint32          FDCreates;
	uint32          LFDCreates;
	uint32          DBCreates;
	uint32          OCCreates;
	uint32          ObjCreates;
	uint32          ObjVers;
	uint32          FDOpens;
	uint32          DBOpens;
	uint32          OCOpens;
	uint32          ObjOpens;
	uint32          FDCloses;
	uint32          DBCloses;
	uint32          OCCloses;
	uint32          ObjCloses;
	uint32          ObjRCloses;
	uint32          FDDeletes;
	uint32          LFDDeletes;
	uint32          DBDeletes;
	uint32          OCDeletes;
	uint32          ObjDeletes;
	uint32          ObjLDeletes;
	uint32          ObjRepeats;
	uint32          nNamed;
	uint32          extraOCBs;
	uint32          Assocs;
	uint32          DisAssocs;
	uint32          AssocResizes;
	uint32          ooResponse;
	uint32          TranBegins;
	uint32          Commits;
	uint32          CommitHs;
	uint32          Aborts;
	uint32          AbortSystems;
} xomStat;

typedef struct {
  uint32	nBufs;			/* Num of buffer 		*/
  uint32	rnBufs;			/* Num of buffer 		*/
  uint32        totalOpen;		/* The number of Opened Obj     */
  uint32	totalCreate;		/* The number of obj create     */
  uint32	openCount;		/* Current Opened Object        */
  uint32	openHash;		/* # of openhash calls.		*/
  uint32	overflow;		/* # of openhash in overflow pg	*/
  uint32        bufferRead;		/* number of read from buffer   */
  uint32        bufferDropped;		/* number of replaced RO buffers*/
  uint32        rbufferDropped;		/* number of replaced RO large buffers*/
  uint32	diskRead;		/* number of read from disk	*/
  uint32	diskReadBytes;		/* number of bytes read from disk */
  uint32	diskWrite;		/* number of write to disk	*/
  uint32	newWrite;		/* number of new page write     */
  uint32	diskWriteBytes;		/* number of bytes written to disk */
  uint32	diskSyncs;		/* number of data syncs		*/
  uint32	logRead;		/* number of log reads		*/
  uint32	logReadBytes;		/* number of bytes read from log */
  uint32	logWrite;		/* number of log writes		*/
  uint32	logWriteBytes;		/* number of bytes written to log */
  uint32	logSyncs;		/* number of log syncs		*/
  uint32	logCreates;		/* number of log file creates   */
  uint32	logUnlinks;		/* number of log file Unlinks   */
  uint32	OcExtent;		/* # of Oc extent		*/
  uint32	OcAddPg;		/* # of Oc increase page	*/
  uint32	ObjResize;		/* # of Object resize		*/
} xsmStat;

extern "C" int oomGetStats(xomStat *);
extern "C" int osmGetStats(xsmStat *);

///////////////////////////////////////////////////////////////////////////////
//
// Class: ooStats - ooRunStatus() encapsulation class
//
///////////////////////////////////////////////////////////////////////////////

class ooStats {
private:

	//
	// Object Manager Statistics (ooRunStatus subset & order)
	//

	uint32          FDCreates;
	uint32          FDOpens;
	uint32          FDCloses;
	uint32          FDDeletes;

	uint32          DBCreates;
	uint32          DBOpens;
	uint32          DBCloses;
	uint32          DBDeletes;

	uint32          OCCreates;
	uint32          OCOpens;
	uint32          OCCloses;
	uint32          OCDeletes;

	uint32          ObjCreates;
	uint32          ObjOpens;
	uint32          ObjRepeats;
	uint32          ObjVers;
	uint32          ObjCloses;
	uint32          ObjRCloses;
	uint32          ObjDeletes;

	uint32          nNamed;
	uint32          extraOCBs;
	uint32          Assocs;
	uint32          DisAssocs;
	uint32          AssocResizes;
	uint32          TranBegins;
	uint32          Commits;
	uint32          CommitHs;
	uint32          Aborts;
	uint32          AbortSystems;

	//
	// Storage Manager Statistics (ooRunStatus subset & order)
	//

	// Page Size not in xmsStats (omitted here)
	uint32          nBufs;
	uint32          rnBufs;
	uint32          totalOpen;
	uint32          totalCreate;
	uint32          openCount;
	uint32          bufferRead;
	uint32          diskRead;
	uint32          diskWrite;
	uint32          newWrite;
	uint32          openHash;
	uint32          overflow;
	uint32          OcExtent;
	uint32          OcAddPg;
	uint32          ObjResize;

public:
	ooStats();
	~ooStats() {}
	void		Start();
	void		PrintDelta(ostream& out);
	void		PrintCumulative(ostream& out);
};

#endif

