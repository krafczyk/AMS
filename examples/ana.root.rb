#!/usr/bin/env ruby

# The following line is not necessary if the AMS library is
# already installed as a general Ruby library 
$LOAD_PATH << ENV['AMSWD'] + "/ruby"

require 'AMS'
include AMS

app = TRint.new("", 0, [])

ams = AMSChain.new
ams.Add("/f2users/choutko/g3v1g3.root")

hrig = TH1F.new("hrig", "Momentum (GeV)", 50, -10.0, 10.0)

ndata = ams.GetEntries

ndata.times do
  ev = ams.GetEvent
  if not ev then; break; end
  for i in 0...ev.nParticle
      part = ev.Particle(i)
      hrig.Fill(part.Momentum) 
  end
end

hrig.Draw

puts "We have processed %d events" % ndata

app.Run
