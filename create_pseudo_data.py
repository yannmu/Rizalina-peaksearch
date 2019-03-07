from ROOT import TFile, TDirectoryFile, TH1
import root_numpy



f = TFile("gerda-results/Figures/background-model/gmodel-results-18-06.root")
d = (TDirectoryFile)f.Get("histos")


#get pdf for BEGe
h = (TH1)d.Get("hmodel_ch0")



