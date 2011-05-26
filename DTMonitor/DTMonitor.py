#!/usr/bin/env python

from ROOT import TCanvas, TGraph, gPad,TPaveText,gStyle,gSystem
from ROOT import gROOT
from array import array
import os
import time
#options
folder = "/fc02dat0/FRAMES/SCIBPB/RT" #default folder
folder2 = "/fc02dat0/FRAMES/SCI/RT" #default folder2

c1 = TCanvas( 'c1', 'Frame Monitor', 200, 10, 1000, 900 )
c1.SetLogy(1)

#saves[ 'c1' ] = c1

def DrawFilesPlot(timewin):
	global folder,folder2
	c1.Clear()

	filesR_size,filesR_cdate = [],[]
	filesH_size,filesH_cdate = [],[]

	for (path, dirs, files) in os.walk(folder):
	    for file in files:
	        filename = os.path.join(path, file)
	        fsize=os.path.getsize(filename)
		cdate=os.path.getctime(filename)
		if not filename.startswith("lastfile"):
			if cdate > timewin - 60*60:
				filesR_size.append(fsize)
				filesR_cdate.append(cdate)
	fileRlist=zip(filesR_cdate,filesR_size)
	fileRlist.sort()
        for (path, dirs, files) in os.walk(folder2):
            for file in files:
                filename = os.path.join(path, file)
                fsize=os.path.getsize(filename)
                cdate=os.path.getctime(filename)
                if not filename.startswith("lastfile"):
                        if cdate > timewin - 60*60:
                                filesH_size.append(fsize)
                                filesH_cdate.append(cdate)
        fileHlist=zip(filesH_cdate,filesH_size)
        fileHlist.sort()

	#Draw plot
#	c1 = TCanvas( 'c1', 'Sizes of Frame Files', 200, 10, 1000, 900 )
	c1.SetFillColor( 7)
        c1.Divide(1,2)

#	pad1 = TPad( 'pad1', 'Frame Sizes (1 hour)', 0.02, 0.02, 0.48, 0.83, 33 )
#s	pad2 = TPad( 'pad2', 'Frame Sizes (1 hour)', 0.52, 0.02, 0.98, 0.83, 33 )

#draw 1 hour plot /SCIBPB/RT
	c1.cd(1)
	gPad.SetGrid()
	gPad.SetLogy(1)


	n = len(fileRlist)
	x, y = array( 'd' ), array( 'd' )
	nofile=False
	for c,s in fileRlist:
	   x.append(c-3600)
	   y.append(s/1024)
	if n == 0:
		nofile=True		
		pt= TPaveText(0.20,0.7,0.5,0.87, "NDC")
		pt.SetFillColor(0) 
		pt.SetTextSize(0.04) 
		pt.SetTextAlign(12)
		pt.AddText("No incoming frame in the past hour")
	#   print ' i %i %f %f ' % (i,x[i],y[i])
#	gSystem.Setenv("TZ","GMT+2")
	x.append(timewin-7200)
	y.append(0)
	x.append(timewin-3600)
	y.append(0)
	n =n+2
	gr2 = TGraph( n, x, y )
	gr2.SetLineColor( 2 )
	gr2.SetLineWidth( 4 )
	gr2.SetMarkerColor( 4 )
	gr2.SetMarkerStyle( 21 )
	gr2.SetTitle( '1-Hour Plot (SCIBPB/RT)' )
	gr2.GetXaxis().SetTitle( 'Date' )
	gStyle.SetTimeOffset(0); 
	gr2.GetXaxis().SetTimeDisplay(1)
	#gr.GetXaxis().SetNdivisions(-60)
	gr2.GetXaxis().SetTimeFormat("%m/%d %H:%M");
	gr2.GetYaxis().SetTitle( 'Size [KB]' )
	gr2.GetXaxis().SetLabelSize(0.03)
	
	gr2.Draw('AP')
	if nofile == True:
		pt.Draw()
	gPad.Draw()	
	gPad.Modified();

#draw 1 hour plot /SCI/RT
	c1.cd(2)
	gPad.SetGrid()
	gPad.SetLogy(1)
#	gPad.SetBottomMargin(0.05);
#     	gPad.SetTopMargin(0.05);

	n1 = len(fileHlist)
	x1, y1 = array( 'd' ), array( 'd' )

	for c,s in fileHlist:
	   x1.append(c-3600)
	   y1.append(s/1024)
	nofileH = False
	if n1 == 0:
		nofileH=True		
		pt2= TPaveText(0.20,0.7,0.5,0.87, "NDC")
		pt2.SetFillColor(0) 
		pt2.SetTextSize(0.04) 
		pt2.SetTextAlign(12)
		pt2.AddText("No incoming frame in the past hour")
	x1.append(timewin-7200)
	y1.append(0)
	x1.append(timewin-3600)
	y1.append(0)
	n1 = n1 + 2
	gr1 = TGraph( n1, x1, y1 )
	gr1.SetLineColor( 2 )
	gr1.SetLineWidth( 4 )
	gr1.SetMarkerColor( 4 )
	gr1.SetMarkerStyle( 21 )
	gr1.SetTitle( '1-hour Plot (SCI/RT) ' )
	gr1.GetXaxis().SetTitle( 'Date' )
	gr1.GetXaxis().SetTimeDisplay(1)
	#gr.GetXaxis().SetNdivisions(-60)
	gr1.GetXaxis().SetTimeFormat("%m/%d %H:%M");
	gr1.GetYaxis().SetTitle( 'Size [(KB]' )
	gr1.GetXaxis().SetLabelSize(.03)

	#gr1.SetMarkerSize(.6)

	gr1.Draw('AP')
  	if nofileH== True:
		pt2.Draw()
	gPad.Draw()
	gPad.Modified();

	c1.Modified()
	c1.Update()

	time.sleep(15)

if __name__ == '__main__':
	count =0
#	saves[ 'c1' ] = c1
	gROOT.Reset()
        print "Monitor folder: "+folder
	while True:
		curtime = time.time()
		count = count + 1
		print "Start making new plot", "[",count,"]","at", time.ctime()
		DrawFilesPlot(curtime)
	






