#!/bin/bash

set -x

# This is a master file to run the emulator macro and then print out rates on multiple datasets at once
# You can find the up-to-date list of samples here: https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideL1TValidationSamples

  # enum algoVariation {
  #0   nominal,
  #1   zeroWalls,
  #2   doubleSubtraction,
  #3   sigmaSubtraction,
  #4   barrelOnly,
  #5   oneByOne,
  #6   twoByTwo,
  #7  oneByOneANDzeroWalls,
  #8  oneByOneANDzeroWallsANDsigmaSubtraction,
  #9  twoByTwoANDzeroWalls,
  #10  twoByTwoANDzeroWallsANDsigmaSubtraction
  #11 legacy
# };

LABELS=("" "_2GeVBin" "_overlapGuard")
LABEL=2

InputType=(MBData Hydjet276 Hydjet502 Hydjet502Dijet30 Hydjet502Dijet80)
InputHiForest=("/mnt/hadoop/cms/store/user/luck/L1Emulator/minbiasForest_merged_v2/HiForest_PbPb_Data_minbias_fromSkim_v3.root" "/mnt/hadoop/cms/store/user/ginnocen/Hydjet1p8_TuneDrum_Quenched_MinBias_2760GeV/HiMinBias_Forest_26June2014/d9ab4aca1923b3220eacf8ee0d550950/*.root" "/mnt/hadoop/cms/store/user/luck/L1Emulator/HydjetMB_502TeV_740pre8_MCHI2_74_V3_rctconfigNoCuts_HiForestAndEmulatorAndHLT_v7.root" "/mnt/hadoop/cms/store/user/luck/L1Emulator/PyquenUnquenched_DiJet_pt30_PbPb_5020GeV_actuallyEmbedded_HiForest.root" "/mnt/hadoop/cms/store/user/dgulhan/HiForest_Ncoll_Dijet_pthat80_740pre8_MCHI2_74_V3_mergedx2/HiForest_Ncoll_Dijet_pthat80_740pre8_MCHI2_74_V3_merged_forest_0.root")
InputL1=("/mnt/hadoop/cms/store/user/men1/L1Data/HIL1DPG/MinBias/HIMinBiasUPC_Skim_HLT_HIMinBiasHfOrBSC_v2_CaloRegionEta516_CMSSW740pre7/L1NTupleMBHIFS.root" "/export/d00/scratch/luck/Hydjet1p8_2760GeV_L1UpgradeAnalyzer_GT_run1_mc_HIon_L1UpgradeAnalyzer.root" "/mnt/hadoop/cms/store/user/luck/L1Emulator/HydjetMB_502TeV_740pre8_MCHI2_74_V3_rctconfigNoCuts_HiForestAndEmulatorAndHLT_v7.root" "/mnt/hadoop/cms/store/user/luck/L1Emulator/PyquenUnquenched_DiJet_pt30_PbPb_5020GeV_actuallyEmbedded_HiForest.root" "/mnt/hadoop/cms/store/user/luck/L1Emulator/PyquenUnquenched_Dijet_pthat80_740pre8_MCHI2_74_V3_noHoEorFG_2x2jets.root")

AlgoVariations=(nominal zeroWalls doubleSubtraction sigmaSubtraction barrelOnly oneByOne twoByTwo oneByOneAndzeroWalls oneByOneANDzeroWallsANDsigmaSubtraction twoByTwoANDzeroWalls twoByTwoANDzeroWallsANDsigmaSubtraction legacy)

# compile the macros with g++
g++ L1JetEmulator.C $(root-config --cflags --libs) -Werror -Wall -Wextra -O2 -o L1JetEmulator.exe || exit 1
g++ makeRateCurve.C $(root-config --cflags --libs) -Werror -Wall -Wextra -O2 -o makeRateCurve.exe || exit 1
g++ makeTurnOn.C $(root-config --cflags --libs) -Werror -Wall -Wextra -O2 -o makeTurnOn.exe || exit 1
g++ makeTurnOn_fromSameFile.C $(root-config --cflags --libs) -Werror -Wall -Wextra -O2 -o makeTurnOn_fromSameFile.exe || exit 1
g++ plotTurnOn.C $(root-config --cflags --libs) -Werror -Wall -Wextra -O2 -o plotTurnOn.exe || exit 1
g++ findthes.C $(root-config --cflags --libs) -Werror -Wall -Wextra -O2 -o findthes.exe || exit 1

# for sampleNum in 2 3 4
# do
#     for algo in 9
#     do
# 	L1Output="~/scratch/EmulatorResults/${InputType[sampleNum]}_JetResults_${AlgoVariations[algo]}${LABELS[LABEL]}.root"
# 	HistOutput="hist_${InputType[sampleNum]}_${AlgoVariations[algo]}${LABELS[LABEL]}.root"
# 	PlotOutputTag="${InputType[sampleNum]}_${AlgoVariations[algo]}${LABELS[LABEL]}"
# 	THRESHOUTFILE="rate_${sample}_${algo}${LABELS[LABEL]}"
# 	./L1JetEmulator.exe "${InputL1[sampleNum]}" "$L1Output" $algo || exit 1
# 	./makeRateCurve.exe "$L1Output" || exit 1
# 	if [[ $sampleNum -eq 0 ]]
# 	then
# 	    ./makeTurnOn.exe "$L1Output" "${InputHiForest[sampleNum]}" "$HistOutput" 0 0 || exit 1
# 	elif [[ $sampleNum -eq 1 ]] || [[ $sampleNum -eq 4 ]]
# 	then
# 	    ./makeTurnOn.exe "$L1Output" "${InputHiForest[sampleNum]}" "$HistOutput" 1 0 || exit 1
# 	elif [[ $sampleNum -eq 2 ]] || [[ $sampleNum -eq 3 ]]
# 	then
# 	   ./makeTurnOn_fromSameFile.exe "$L1Output" "${InputHiForest[sampleNum]}" "$HistOutput" 1 0 || exit 1
# 	fi
# 	./plotTurnOn.exe "$HistOutput" "$PlotOutputTag" || exit 1
#     done
# done

# run this last just so the output is cleaner.
for sampleNum in 2 3 4
do
    for algo in 9
    do
	L1Output="~/scratch/EmulatorResults/Hydjet502_JetResults_${AlgoVariations[algo]}${LABELS[LABEL]}.root"
	HistOutput="hist_${InputType[sampleNum]}_${AlgoVariations[algo]}${LABELS[LABEL]}.root"
	THRESHOUTFILE="rate_${InputType[sampleNum]}_${AlgoVariations[algo]}${LABELS[LABEL]}"
	echo "Analyzing ${InputType[sampleNum]}_${AlgoVariations[algo]}${LABELS[LABEL]}"
	./findthes.exe "$L1Output" "$HistOutput" "$THRESHOUTFILE" 1.0 0 || exit 1
	echo ""
    done
done
