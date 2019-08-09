import json
import statistics as stat
import csv

def saveStats(u,stdev,unc,cf):
    w = csv.writer(cf, delimiter=',',
                   quotechar=';', quoting=csv.QUOTE_MINIMAL)
    w.writerow([u,stdev,unc])

def printStats(d,cf):
    lkup = {"ee":"ECal_E","et":"ECal_ET","he":"HCal_E","ht":"HCal_ET"}
    for k,v in d.items():
        for kk,vv in v.items():
            u = stat.mean(vv)
            stdev = stat.stdev(vv)
            unc = stat.stdev(vv)/len(vv)
            saveStats(u, stdev, unc, cf)
             
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
    f = open("./result/auc.json")
    cf = open("./result/stats.csv", "w", newline="")
    j = json.load(f)
    d = pickOutlier(j)
    print(d)
    printStats(d, cf)
    f.close()
    cf.close()

