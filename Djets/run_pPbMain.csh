#!/bin/bash
#run_pPbMain.csh
#pPbDir=`pwd`
otherbinning=0
if [ $otherbinning -eq 1 ]; then
ptbinning=0 			# 0: pp bining 1: finer binning 2: Pb-Pb binning

################################################
###############
################################################
jetpttruemin=3
jetpttruemax=50
jetptmeasmin=3
jetptmeasmax=50

bkgRMtype=0
unfType=0 # 0: bayes, 1: SVD
regPar=3
isPrior=0
priorType=0

doBkg=0
doPrior=0
doBkgPrior=0
doCutVar=0
doJESSys=0
doFDSys=0
doSys=0
doRawCutVarSys=0


############### Default
doRawSignal=0
doSignal=1
./run_pPbDzeroAnalysis.csh  $ptbinning $jetpttruemin $jetpttruemax $jetptmeasmin $jetptmeasmax $bkgRMtype $unfType $regPar $isPrior $priorType $doBkg $doPrior $doBkgPrior $doCutVar $doJESSys $doFDSys $doSys $doRawCutVarSys $doRawSignal $doSignal


exit 1
fi

################################################
############### systematics only
################################################
quickSys=0
if [ $quickSys -eq 1 ]; then
ptbinning=0 			# 0: pp bining 1: finer binning 2: Pb-Pb binning

################################################
############### ptmeas: 3-50, pttrue: 3-50, reg=3, pp binning
################################################
jetpttruemin=3
jetpttruemax=50
jetptmeasmin=3
jetptmeasmax=50

bkgRMtype=0
unfType=0 #bayes
regPar=3
isPrior=0
priorType=0

doBkg=0
doPrior=0
doBkgPrior=0
doCutVar=1
doJESSys=0
doFDSys=0
doSys=0
doRawCutVarSys=1

############### Raw Signal only

doRawSignal=0
doSignal=0
./run_pPbDzeroAnalysis.csh  $ptbinning $jetpttruemin $jetpttruemax $jetptmeasmin $jetptmeasmax $bkgRMtype $unfType $regPar $isPrior $priorType $doBkg $doPrior $doBkgPrior $doCutVar $doJESSys $doFDSys $doSys $doRawCutVarSys $doRawSignal $doSignal

exit 1
fi


################################################
############### FULL with sys
################################################

ptbinning=2 			# 0: pp bining 1: more fine binning 2: Pb-Pb binning 5: more fine pp binning

dbkg=1       #  systematics
dprior=1     #  systematics
dcutvar=1    # 	do separately from systematics
dJESsys=1    #  systematics
dsys=0       # will run JetSpectrumSys.C
dFDsys=1     #  systematics
drawcutvar=0 # raw, spectra w/o eff correction... draws the histograms.. depends on dcutvar, doRawSignal.
################################################
############### ptmeas: 3-50, pttrue: 3-50, reg=3
################################################
jetptmeasmin=3
jetptmeasmax=50
jetpttruemin=3
jetpttruemax=50

bkgRMtype=0
unfType=0 #0: bayes. 1: SVD
regPar=3
isPrior=0
priorType=0

doBkg=$dbkg
doPrior=$dprior
doBkgPrior=0
doCutVar=$dcutvar
doJESSys=$dJESsys
doFDSys=$dFDsys
doSys=$dsys
doRawCutVarSys=$drawcutvar

############### Raw Signal only
doRawSignal=1
doSignal=0
./run_pPbDzeroAnalysis.csh  $ptbinning $jetpttruemin $jetpttruemax $jetptmeasmin $jetptmeasmax $bkgRMtype $unfType $regPar $isPrior $priorType $doBkg $doPrior $doBkgPrior $doCutVar $doJESSys $doFDSys $doSys $doRawCutVarSys $doRawSignal $doSignal


############### Default
doRawSignal=0
doSignal=1
./run_pPbDzeroAnalysis.csh  $ptbinning $jetpttruemin $jetpttruemax $jetptmeasmin $jetptmeasmax $bkgRMtype $unfType $regPar $isPrior $priorType $doBkg $doPrior $doBkgPrior $doCutVar $doJESSys $doFDSys $doSys $doRawCutVarSys $doRawSignal $doSignal

exit 1

#####################################################
############### ptmeas: 3-50, pttrue: 5-50, reg=3  ##
#####################################################
jetptmeasmin=3
jetptmeasmax=50
jetpttruemin=5
jetpttruemax=50

bkgRMtype=0
unfType=0 #bayes
regPar=3
isPrior=0
priorType=0

doBkg=$dbkg
doPrior=$dprior
doBkgPrior=0
doCutVar=$dcutvar
doJESSys=$dJESsys
doFDSys=$dFDsys
doSys=$dsys
doRawCutVarSys=$drawcutvar


############### Raw Signal only
doRawSignal=1
doSignal=0
./run_pPbDzeroAnalysis.csh  $ptbinning $jetpttruemin $jetpttruemax $jetptmeasmin $jetptmeasmax $bkgRMtype $unfType $regPar $isPrior $priorType $doBkg $doPrior $doBkgPrior $doCutVar $doJESSys $doFDSys $doSys $doRawCutVarSys $doRawSignal $doSignal

############### Default
doRawSignal=0
doSignal=1
./run_pPbDzeroAnalysis.csh  $ptbinning $jetpttruemin $jetpttruemax $jetptmeasmin $jetptmeasmax $bkgRMtype $unfType $regPar $isPrior $priorType $doBkg $doPrior $doBkgPrior $doCutVar $doJESSys $doFDSys $doSys $doRawCutVarSys $doRawSignal $doSignal


################################################
############### ptmeas: 3-50, pttrue: 3-50, reg=4
################################################

jetptmeasmin=3
jetptmeasmax=50
jetpttruemin=3
jetpttruemax=50

bkgRMtype=0
unfType=0 #bayes
regPar=4
isPrior=0
priorType=0

doBkg=$dbkg
doPrior=$dprior
doBkgPrior=0
doCutVar=$dcutvar
doJESSys=$dJESsys
doFDSys=$dFDsys
doSys=$dsys
doRawCutVarSys=$drawcutvar


############### Raw Signal only
doRawSignal=1
doSignal=0
./run_pPbDzeroAnalysis.csh  $ptbinning $jetpttruemin $jetpttruemax $jetptmeasmin $jetptmeasmax $bkgRMtype $unfType $regPar $isPrior $priorType $doBkg $doPrior $doBkgPrior $doCutVar $doJESSys $doFDSys $doSys $doRawCutVarSys $doRawSignal $doSignal

############### Default
doRawSignal=0
doSignal=1
./run_pPbDzeroAnalysis.csh  $ptbinning $jetpttruemin $jetpttruemax $jetptmeasmin $jetptmeasmax $bkgRMtype $unfType $regPar $isPrior $priorType $doBkg $doPrior $doBkgPrior $doCutVar $doJESSys $doFDSys $doSys $doRawCutVarSys $doRawSignal $doSignal



################################################
############### ptmeas: 3-50, pttrue: 5-50, reg=4
################################################
jetptmeasmin=3
jetptmeasmax=50
jetpttruemin=5
jetpttruemax=50

bkgRMtype=0
unfType=0 #bayes
regPar=4
isPrior=0
priorType=0

doBkg=$dbkg
doPrior=$dprior
doBkgPrior=0
doCutVar=$dcutvar
doJESSys=$dJESsys
doFDSys=$dFDsys
doSys=$dsys
doRawCutVarSys=$drawcutvar


############### Raw Signal only
doRawSignal=1
doSignal=0
./run_pPbDzeroAnalysis.csh  $ptbinning $jetpttruemin $jetpttruemax $jetptmeasmin $jetptmeasmax $bkgRMtype $unfType $regPar $isPrior $priorType $doBkg $doPrior $doBkgPrior $doCutVar $doJESSys $doFDSys $doSys $doRawCutVarSys $doRawSignal $doSignal

############### Default
doRawSignal=0
doSignal=1
./run_pPbDzeroAnalysis.csh  $ptbinning $jetpttruemin $jetpttruemax $jetptmeasmin $jetptmeasmax $bkgRMtype $unfType $regPar $isPrior $priorType $doBkg $doPrior $doBkgPrior $doCutVar $doJESSys $doFDSys $doSys $doRawCutVarSys $doRawSignal $doSignal


exit 1

################################################
############### ptmeas: 5-50, pttrue: 5-50, reg=3
################################################
jetptmeasmin=5
jetptmeasmax=50
jetpttruemin=5
jetpttruemax=50

bkgRMtype=0
unfType=0 #bayes
regPar=3
isPrior=0
priorType=0

doBkg=$dbkg
doPrior=$dprior
doBkgPrior=0
doCutVar=$dcutvar
doJESSys=$dJESsys
doFDSys=$dFDsys
doSys=$dsys
doRawCutVarSys=$drawcutvar


############### Raw Signal only
doRawSignal=1
doSignal=0
#./run_pPbDzeroAnalysis.csh  $ptbinning $jetpttruemin $jetpttruemax $jetptmeasmin $jetptmeasmax $bkgRMtype $unfType $regPar $isPrior $priorType $doBkg $doPrior $doBkgPrior $doCutVar $doJESSys $doFDSys $doSys $doRawCutVarSys $doRawSignal $doSignal

############### Default
doRawSignal=0
doSignal=1
./run_pPbDzeroAnalysis.csh  $ptbinning $jetpttruemin $jetpttruemax $jetptmeasmin $jetptmeasmax $bkgRMtype $unfType $regPar $isPrior $priorType $doBkg $doPrior $doBkgPrior $doCutVar $doJESSys $doFDSys $doSys $doRawCutVarSys $doRawSignal $doSignal

################################################
############### ptmeas: 5-50, pttrue: 5-50, reg=4
################################################
jetptmeasmin=5
jetptmeasmax=50
jetpttruemin=5
jetpttruemax=50

bkgRMtype=0
unfType=0 #bayes
regPar=4
isPrior=0
priorType=0

doBkg=1
doPrior=1
doBkgPrior=0
doCutVar=1
doJESSys=1
doFDSys=1
doSys=1
doRawCutVarSys=0

############### Raw Signal only
doRawSignal=1
doSignal=0
#./run_pPbDzeroAnalysis.csh  $ptbinning $jetpttruemin $jetpttruemax $jetptmeasmin $jetptmeasmax $bkgRMtype $unfType $regPar $isPrior $priorType $doBkg $doPrior $doBkgPrior $doCutVar $doJESSys $doFDSys $doSys $doRawCutVarSys $doRawSignal $doSignal

############### Default
doRawSignal=0
doSignal=1
./run_pPbDzeroAnalysis.csh  $ptbinning $jetpttruemin $jetpttruemax $jetptmeasmin $jetptmeasmax $bkgRMtype $unfType $regPar $isPrior $priorType $doBkg $doPrior $doBkgPrior $doCutVar $doJESSys $doFDSys $doSys $doRawCutVarSys $doRawSignal $doSignal

exit 1
