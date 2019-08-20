import json
import statistics as stat
import csv
import math
import argparse
from decimal import Decimal

def saveStats(res,cf,t):
    w = csv.writer(cf, delimiter=',',
                   quotechar=';', quoting=csv.QUOTE_MINIMAL)
    w.writerow([t]+res)

def printStats(d,cf,lorh):
    head = ["x"]
    lkup = {"ee":"ECal_E","et":"ECal_ET","he":"HCal_E","ht":"HCal_ET", 
            "Rhad":"Rhad","wEta2":"wEta2","Rphi":"Rphi","Reta":"Reta","sigmaEtaEta":"sigmaEtaEta","DelR03":"DelR03","DelR04":"DelR04", "all":"all"}
    for k,v in d.items():
        i = 1
        res = []
        for kk,vv in v.items():
            head.append(lkup[kk])
            u = stat.mean(vv)
            stdev = stat.stdev(vv)
            unc = stat.stdev(vv)/math.sqrt(len(vv))
            t = lkup[kk]
            res.append("{:.5}±{:.5}".format(u,unc)) 
            if lorh == "l":
                if i%4 == 0:
                    saveStats(res, cf, k)
            elif lorh == "h":
                if i%8 == 0:
                    saveStats(res, cf, k)
            i += 1
            print(k, t, "{:.4}±{:.4E}".format(u,Decimal(str(unc))))
            #print("{}:\n  {}: Mean={}, Standard Deviation={}, Uncertainty(SE)={}".format(k,kk,u,stdev,unc))
    return head

def pickOutlier(j):
    d = dict()
    for k,v in j.items():
        dd = dict()
        for kk,vv in v.items():
            if 0.5 not in vv:
                dd[kk] = vv
            else:
                print("Outlier!")
                dd[kk] = [i for i in vv if i !=0.5]
        d[k] = dd
    return d

def wHead(cf,lorh,head):
    w = csv.writer(cf, delimiter=',',
                   quotechar=';', quoting=csv.QUOTE_MINIMAL)
    w.writerow(head)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Where is High?')
    parser.add_argument('--horl','-hl',metavar='str', help='High or Low', type=str, required=True)
    parser.add_argument('--json-filename','-jfn',metavar='str', help='json file name', type=str, required=True)
    parser.add_argument('--csv-filename','-cfn',metavar='str', help='csv file name', type=str, required=True)
    args = vars(parser.parse_args())
    jfn = args["json_filename"]
    cfn = args["csv_filename"]
    horl = args["horl"]
    f = open(jfn)
    cf = open(cfn, "w", newline="")
    j = json.load(f)
    d = pickOutlier(j)
    print(d)
    head = printStats(d, cf, horl)
    wHead(cf,horl,head)
    f.close()
    cf.close()
