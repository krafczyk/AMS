#!/usr/bin/env python2.2

import sys, os
sys.path.append(os.environ['AMSWD'] + '/python/linux')

from AMS import *

app = TRint("My application", 0, [])

hfile = TFile("amstest.root","RECREATE","AMSPython test")

hist = TH1F("hist","Z vertex position (cm)",25,-50.0,150.0)

ams = AMSChain()
ams.Add("/f2users/choutko/g3v1g3.root")
ndata = ams.GetEntries()

for i in range(ndata):
  ev = ams.GetEvent()
  if ev.nVertex() == 1:
      vtx = ev.Vertex(0)
      zpos = vtx.Vertex[2]
      hist.Fill(zpos) 

hist.Fit("gaus")

cvar.gStyle.SetOptStat(0)
cvar.gStyle.SetOptFit(1111)
hist.Draw()

hfile.Write()

print "We have read and analyzed %d entries" % ndata

app.Run()
