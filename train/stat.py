import json
import statistics as stat

def printStats(d):
    lkup = {"ee":"ECal_E","et":"ECal_ET","he":"HCal_E","ht":"HCal_ET"}
    for k,v in d.items():
        for kk,vv in v.items():
            u = stat.mean(vv)
            stdev = stat.stdev(vv)
            unc = stat.stdev(vv)/len(vv)
            print("{}:\n  {}: Mean={}, Standard Deviation={}, Standard Uncertainty={}".format(k,kk,u,stdev,unc))

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

if __name__ == "__main__":
    f = open("auc.json")
    j = json.load(f)
    d = pickOutlier(j)
    print(d)
    printStats(d)

