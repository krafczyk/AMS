//  $Id: ooStats.C,v 1.3 2001/01/22 17:32:30 choutko Exp $
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
 *      RCSid = "$Id: ooStats.C,v 1.3 2001/01/22 17:32:30 choutko Exp $"
 *
 ****************************************************************************
*/

#include "ooStats.h"


ooStats::ooStats()
{
	// Initialize the Object Manager statistics to zero

	FDCreates = 0;
	FDOpens = 0;
	FDCloses = 0;
	FDDeletes = 0;

	DBCreates = 0;
	DBOpens = 0;
	DBCloses = 0;
	DBDeletes = 0;

	OCCreates = 0;
	OCOpens = 0;
	OCCloses = 0;
	OCDeletes = 0;

	ObjCreates = 0;
	ObjOpens = 0;
	ObjRepeats = 0;
	ObjVers = 0;
	ObjCloses = 0;
	ObjRCloses = 0;
	ObjDeletes = 0;

	nNamed = 0;
	extraOCBs = 0;
	Assocs = 0;
	DisAssocs = 0;
	AssocResizes = 0;
	TranBegins = 0;
	Commits = 0;
	CommitHs = 0;
	Aborts = 0;
	AbortSystems = 0;

	// Initialize the Storage Manager statistics to zero

	nBufs = 0;
	rnBufs = 0;
	totalOpen = 0;
	totalCreate = 0;
	openCount = 0;
	bufferRead = 0;
	diskRead = 0;
	diskWrite = 0;
	newWrite = 0;
	openHash = 0;
	overflow = 0;
	OcExtent = 0;
	OcAddPg = 0;
	ObjResize = 0;
}

void	ooStats::Start()
{
	// Read in the values from the Object Manager's workspace

	xomStat	omstatus;
	oomGetStats(&omstatus);

	FDCreates = omstatus.FDCreates;
	FDOpens = omstatus.FDOpens;
	FDCloses = omstatus.FDCloses;
	FDDeletes = omstatus.FDDeletes;

	DBCreates = omstatus.DBCreates;
	DBOpens = omstatus.DBOpens;
	DBCloses = omstatus.DBCloses;
	DBDeletes = omstatus.DBDeletes;

	OCCreates = omstatus.OCCreates;
	OCOpens = omstatus.OCOpens;
	OCCloses = omstatus.OCCloses;
	OCDeletes = omstatus.OCDeletes;

	ObjCreates = omstatus.ObjCreates;
	ObjOpens = omstatus.ObjOpens;
	ObjRepeats = omstatus.ObjRepeats;
	ObjVers = omstatus.ObjVers;
	ObjCloses = omstatus.ObjCloses;
	ObjRCloses = omstatus.ObjRCloses;
	ObjDeletes = omstatus.ObjDeletes;

	nNamed = omstatus.nNamed;
	extraOCBs = omstatus.extraOCBs;
	Assocs = omstatus.Assocs;
	DisAssocs = omstatus.DisAssocs;
	AssocResizes = omstatus.AssocResizes;
	TranBegins = omstatus.TranBegins;
	Commits = omstatus.Commits;
	CommitHs = omstatus.CommitHs;
	Aborts = omstatus.Aborts;
	AbortSystems = omstatus.AbortSystems;

	// Read in the values from the Storage Manager's workspace

	xsmStat	smstatus;
	osmGetStats(&smstatus);

	nBufs = smstatus.nBufs;
	rnBufs = smstatus.rnBufs;
	totalOpen = smstatus.totalOpen;
	totalCreate = smstatus.totalCreate;
	openCount = smstatus.openCount;
	bufferRead = smstatus.bufferRead;
	diskRead = smstatus.diskRead;
	diskWrite = smstatus.diskWrite;
	newWrite = smstatus.newWrite;
	openHash = smstatus.openHash;
	overflow = smstatus.overflow;
	OcExtent = smstatus.OcExtent;
	OcAddPg = smstatus.OcAddPg;
	ObjResize = smstatus.ObjResize;
}

void	ooStats::PrintDelta(ostream& out)
{
	// Read in the values from the Object Manager's workspace

	xomStat	omstatus;
	oomGetStats(&omstatus);

	// Print out the changed values

	out << endl << "Object Manager Statistics:" << endl << endl;
	if (omstatus.FDCreates != FDCreates) {
	   out << "** Delta of federated DBs created =>  " << (omstatus.FDCreates - FDCreates) << endl;
	}
	if (omstatus.FDOpens != FDOpens) {
	   out << "** Delta of federated DBs opened  =>  " << (omstatus.FDOpens - FDOpens) << endl;
	}
	if (omstatus.FDCloses != FDCloses) {
	   out << "** Delta of federated DBs closed  =>  " << (omstatus.FDCloses - FDCloses) << endl;
	}
	if (omstatus.FDDeletes != FDDeletes) {
	   out << "** Delta of federated DBs deleted =>  " << (omstatus.FDDeletes - FDDeletes) << endl;
	}
	if (omstatus.DBCreates != DBCreates) {
	   out << "** Delta of databases created     =>  " << (omstatus.DBCreates - DBCreates) << endl;
	}
	if (omstatus.DBOpens != DBOpens) {
	   out << "** Delta of databases opened      =>  " << (omstatus.DBOpens - DBOpens) << endl;
	}
	if (omstatus.DBCloses != DBCloses) {
	   out << "** Delta of databases closed      =>  " << (omstatus.DBCloses - DBCloses) << endl;
	}
	if (omstatus.DBDeletes != DBDeletes) {
	   out << "** Delta of databases deleted     =>  " << (omstatus.DBDeletes - DBDeletes) << endl;
	}
	if (omstatus.OCCreates != OCCreates) {
	   out << "** Delta of containers created    =>  " << (omstatus.OCCreates - OCCreates) << endl;
	}
	if (omstatus.OCOpens != OCOpens) {
	   out << "** Delta of containers opened     =>  " << (omstatus.OCOpens - OCOpens) << endl;
	}
	if (omstatus.OCCloses != OCCloses) {
	   out << "** Delta of containers closed     =>  " << (omstatus.OCCloses - OCCloses) << endl;
	}
	if (omstatus.OCDeletes != OCDeletes) {
	   out << "** Delta of containers deleted    =>  " << (omstatus.OCDeletes - OCDeletes) << endl;
	}
	if (omstatus.ObjCreates != ObjCreates) {
	   out << "** Delta of objects Created       =>  " << (omstatus.ObjCreates - ObjCreates) << endl;
	}
	if (omstatus.ObjOpens != ObjOpens) {
	   out << "** Delta of objects opened        =>  " << (omstatus.ObjOpens - ObjOpens) << endl;
	}
	if (omstatus.ObjRepeats != ObjRepeats) {
	   out << "** Delta of multiple opens        =>  " << (omstatus.ObjRepeats - ObjRepeats) << endl;
	}
	if (omstatus.ObjVers != ObjVers) {
	   out << "** Delta of new versions          =>  " << (omstatus.ObjVers - ObjVers) << endl;
	}
	if (omstatus.ObjCloses != ObjCloses) {
	   out << "** Delta of objects closed        =>  " << (omstatus.ObjCloses - ObjCloses) << endl;
	}
	if (omstatus.ObjRCloses != ObjRCloses) {
	   out << "** Delta of multiple closes       =>  " << (omstatus.ObjRCloses - ObjRCloses) << endl;
	}
	if (omstatus.ObjDeletes != ObjDeletes) {
	   out << "** Delta of objects deleted       =>  " << (omstatus.ObjDeletes - ObjDeletes) << endl;
	}
	if (omstatus.nNamed != nNamed) {
	   out << "** Delta of objects named         =>  " << (omstatus.nNamed - nNamed) << endl;
	}
	if (omstatus.extraOCBs != extraOCBs) {
	   out << "** Delta of new OCBs              =>  " << (omstatus.extraOCBs - extraOCBs) << endl;
	}
	if (omstatus.Assocs != Assocs) {
	   out << "** Delta of new associations      =>  " << (omstatus.Assocs - Assocs) << endl;
	}
	if (omstatus.DisAssocs != DisAssocs) {
	   out << "** Delta of disassociations       =>  " << (omstatus.DisAssocs - DisAssocs) << endl;
	}
	if (omstatus.AssocResizes != AssocResizes) {
	   out << "** Delta of associations resized  =>  " << (omstatus.AssocResizes - AssocResizes) << endl;
	}
	if (omstatus.TranBegins != TranBegins) {
	   out << "** Delta of transactions started  =>  " << (omstatus.TranBegins - TranBegins) << endl;
	}
	if (omstatus.Commits != Commits) {
	   out << "** Delta of transaction commits   =>  " << (omstatus.Commits - Commits) << endl;
	}
	if (omstatus.CommitHs != CommitHs) {
	   out << "** Delta of commit and holds      =>  " << (omstatus.CommitHs - CommitHs) << endl;
	}
	if (omstatus.Aborts != Aborts) {
	   out << "** Delta of transaction aborts    =>  " << (omstatus.Aborts - Aborts) << endl;
	}
	if (omstatus.AbortSystems != AbortSystems) {
	   out << "** Delta of system aborts         =>  " << (omstatus.AbortSystems - AbortSystems) << endl;
	}

	// Read in the values from the Storage Manager's workspace

	xsmStat	smstatus;
	osmGetStats(&smstatus);

	// Print out the changed values

	out << endl << "Storage Manager Statistics:" << endl << endl;

	if (smstatus.nBufs != nBufs) {
	   out << "** Delta of buffers used          =>  " << (smstatus.nBufs - nBufs) << endl;
	}
	if (smstatus.rnBufs != rnBufs) {
	   out << "** Delta of large buffer entries  =>  " << (smstatus.rnBufs - rnBufs) << endl;
	}
	if (smstatus.totalOpen != totalOpen) {
	   out << "** Delta of SM objects opened     =>  " << (smstatus.totalOpen - totalOpen) << endl;
	}
	if (smstatus.totalCreate != totalCreate) {
	   out << "** Delta of SM objects created    =>  " << (smstatus.totalCreate - totalCreate) << endl;
	}
	if ((smstatus.openCount != 0) && (smstatus.openCount != openCount)) {
	   out << "** Delta of objects still opened  =>  " << (smstatus.openCount - openCount) << endl;
	}
	if (smstatus.bufferRead != bufferRead) {
	   out << "** Delta of buffers read          =>  " << (smstatus.bufferRead - bufferRead) << endl;
	}
	if (smstatus.diskRead != diskRead) {
	   out << "** Delta of disk reads            =>  " << (smstatus.diskRead - diskRead) << endl;
	}
	if (smstatus.diskWrite != diskWrite) {
	   out << "** Delta of old pages written     =>  " << (smstatus.diskWrite - diskWrite) << endl;
	}
	if (smstatus.newWrite != newWrite) {
	   out << "** Delta of new pages written     =>  " << (smstatus.newWrite - newWrite) << endl;
	}
	if (smstatus.openHash != openHash) {
	   out << "** Delta of openHash calls        =>  " << (smstatus.openHash - openHash) << endl;
	}
	if (smstatus.overflow != overflow) {
	   out << "** Delta of hash overflows        =>  " << (smstatus.overflow - overflow) << endl;
	}
	if (smstatus.OcExtent != OcExtent) {
	   out << "** Delta of times OCs extended    =>  " << (smstatus.OcExtent - OcExtent) << endl;
	}
	if (smstatus.OcAddPg != OcAddPg) {
	   out << "** Delta of Pages added to OCs    =>  " << (smstatus.OcAddPg - OcAddPg) << endl;
	}
	if (smstatus.ObjResize != ObjResize) {
	   out << "** Delta of SM objects resized    =>  " << (smstatus.ObjResize - ObjResize) << endl;
	}

	// Reset the internal OM/SM counters
	Start();
}

void	ooStats::PrintCumulative(ostream& out)
{
	// Set the statistics
	Start();

	xomStat	omstatus;
	oomGetStats(&omstatus);

	xsmStat	smstatus;
	osmGetStats(&smstatus);

	// Print the OM Cumulative Status (current state of OM Workspace)

	out << "Object Manager Statistics:" << endl << endl;
	out << "** Number of federated DBs created =>  " << omstatus.FDCreates << endl;
	out << "** Number of federated DBs opened  =>  " << omstatus.FDOpens << endl;
	out << "** Number of federated DBs closed  =>  " << omstatus.FDCloses << endl;
	out << "** Number of federated DBs deleted =>  " << omstatus.FDDeletes << endl;
	out << "**" << endl;
	out << "** Number of databases created     =>  " << omstatus.DBCreates << endl;
	out << "** Number of databases opened      =>  " << omstatus.DBOpens << endl;
	out << "** Number of databases closed      =>  " << omstatus.DBCloses << endl;
	out << "** Number of databases deleted     =>  " << omstatus.DBDeletes << endl;
	out << "**" << endl;
	out << "** Number of containers created    =>  " << omstatus.OCCreates << endl;
	out << "** Number of containers opened     =>  " << omstatus.OCOpens << endl;
	out << "** Number of containers closed     =>  " << omstatus.OCCloses << endl;
	out << "** Number of containers deleted    =>  " << omstatus.OCDeletes << endl;
	out << "**" << endl;
	out << "** Number of objects Created       =>  " << omstatus.ObjCreates << endl;
	out << "** Number of objects opened        =>  " << omstatus.ObjOpens << endl;
	out << "** Number of multiple opens        =>  " << omstatus.ObjRepeats << endl;
	out << "** Number of new versions          =>  " << omstatus.ObjVers << endl;
	out << "** Number of objects closed        =>  " << omstatus.ObjCloses << endl;
	out << "** Number of multiple closes       =>  " << omstatus.ObjRCloses << endl;
	out << "** Number of objects deleted       =>  " << omstatus.ObjDeletes << endl;
	out << "**" << endl;
	out << "** Number of objects named         =>  " << omstatus.nNamed << endl;
	out << "** Number of new OCBs              =>  " << omstatus.extraOCBs << endl;
	out << "** Number of new associations      =>  " << omstatus.Assocs << endl;
	out << "** Number of disassociations       =>  " << omstatus.DisAssocs << endl;
	out << "** Number of associations resized  =>  " << omstatus.AssocResizes << endl;
	out << "** Number of transactions started  =>  " << omstatus.TranBegins << endl;
	out << "** Number of transaction commits   =>  " << omstatus.Commits << endl;
	out << "** Number of commit and holds      =>  " << omstatus.CommitHs << endl;
	out << "** Number of transaction aborts    =>  " << omstatus.Aborts << endl;
	out << "** Number of system aborts         =>  " << omstatus.AbortSystems << endl;

	// Print the SM Cumulative Status (current state of SM Workspace)

	out << endl << "Storage Manager Statistics:" << endl << endl;

	out << "** Number of buffers used          =>  " << smstatus.nBufs << endl;
	out << "** Number of large buffer entries  =>  " << smstatus.rnBufs << endl;
	out << "** Number of SM objects opened     =>  " << smstatus.totalOpen << endl;
	out << "** Number of SM objects created    =>  " << smstatus.totalCreate << endl;
	out << "** Number of objects still opened  =>  " << smstatus.openCount << endl;
	out << "** Number of buffers read          =>  " << smstatus.bufferRead << endl;
	out << "** Number of disk reads            =>  " << smstatus.diskRead << endl;
	out << "** Number of old pages written     =>  " << smstatus.diskWrite << endl;
	out << "** Number of new pages written     =>  " << smstatus.newWrite << endl;
	out << "** Number of openHash calls        =>  " << smstatus.openHash << endl;
	out << "** Number of hash overflows        =>  " << smstatus.overflow << endl;
	out << "** Number of times OCs extended    =>  " << smstatus.OcExtent << endl;
	out << "** Number of Pages added to OCs    =>  " << smstatus.OcAddPg << endl;
	out << "** Number of SM objects resized    =>  " << smstatus.ObjResize << endl;

}

