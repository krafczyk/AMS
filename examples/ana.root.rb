#!/usr/bin/env ruby

# The following line is not necessary if the AMS library is
# already installed as a general Ruby library 
$LOAD_PATH.unshift(ENV['AMSDir'] + "/ruby/linux")

require 'AMS'
include AMS

app = TRint.new("", 0, [])

ams = AMSChain.new
ams.Add("/f2users/choutko/g3v1g3.root")
#ams.Add("http://pcamsf0.cern.ch/f2dah1/MC/AMS02/2004A/protons/el.pl1.10200/738197524.0000001.root");
#ams.Add("rfio:/castor/cern.ch/ams/MC/AMS02/2004A/protons/el.pl1.10200/738197524.0000001.root");

chfile = "amstest.root"
hfile = TFile.new(chfile, "RECREATE")

hrig = TH1F.new("hrig", "Momentum (GeV)", 50, -10.0, 10.0)

ndata = ams.GetEntries

ndata.times do
  ev = ams.GetEvent
  break if not ev
  for i in 0...ev.nParticle
      part = ev.Particle(i)
      hrig.Fill(part.Momentum) 
  end
end

hrig.Draw
hfile.Write

puts "\n>>> We have processed %d events" % ndata
puts "\n>>> Histograms saved in '%s'" % chfile

app.Run
