#!/usr/bin/env python2.2

import sys, os
# The following line is not necessary if the AMS module is
# already installed as a general Python module 
sys.path.append(os.environ['AMSWD'] + '/python')

from AMS import *

app = TRint("", 0, [])

ams = AMSChain()
ams.Add("/f2users/choutko/g3v1g3.root")

hrig = TH1F("hrig", "Momentum (GeV)", 50, -10., 10.)

ndata = ams.GetEntries()

for entry in range(ndata):
  ev = ams.GetEvent()
  if not ev: break
  for i in range(ev.nParticle()):
      part = ev.Particle(i)
      hrig.Fill(part.Momentum) 

hrig.Draw()

print "We have processed %d events" % ndata

app.Run()
