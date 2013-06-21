#include "ACQtProducer.h"

#define DEBUG 0
#define INFO_OUT_TAG "ACQtProducer> "
#include <debugging.hh>

// Hacks which allow ACQtProducer to build AC::Events, even though all members are private.
// This could be replaced with a nicer solution, but satisfies our needs for now.
#define private public
#include "Database.h"
#include "Event.h"
#include "RunHeader.h"
#undef private

// Work around OS macro definition in Platform.h
#undef OS

#ifndef _PGTRACK_
#define _PGTRACK_
#endif

// AMS Includes go here.
#include <amschain.h>
#include <DynAlignment.h>
#include <TofTrack.h>
#include <TrExtAlignDB.h>
#include <TrdKCluster.h>
#include <Tofrec02_ihep.h>
#include <point.h>
#include <TrkLH.h>
#include <TrRecon.h>
#include <bcorr.h>
