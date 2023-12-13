#!/bin/sh
# $1 file, $2 name , $3 name autosafe
#go4analysis -lib libGo4UserAnalysis.so -file $1 -asf toto.root -step Sort-factory -disable-step -step Calibr-factory -disable-step -step Analysis-factory -disable-step -step User-factory -disable-step
#go4analysis -lib libGo4UserAnalysis.so -file $1 -asf toto.root -step Sort-factory -enable-step -step Calibr-factory -enable-step -step Analysis-factory -enable-step -step User-factory -enable-step
#go4analysis -lib libGo4UserAnalysis.so -file $1 -asf toto$2.root -step Unpack -store unp$2.root -enable-store -step Sort -store sort$2.root -enable-store -step Calibr -store calib$2.root -enable-store -step Analysis -enable-store -store ana$2.root
go4analysis -lib libGo4UserAnalysis.so -file $1 -asf toto$2.root -step Unpack -step Sort -store sort$2.root -enable-store -step Calibr -store calib$2.root -enable-store -step Analysis -enable-store -store ana$2.root
#go4analysis -lib libGo4UserAnalysis.so -file $1 -number 34000 -asf $3toto$2.root 
#go4analysis -lib libGo4UserAnalysis.so -file $1 -asf $3toto$2.root 
#go4analysis -lib libGo4UserAnalysis.so -revserv lxg1268  

