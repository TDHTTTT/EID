import json
import statistics as stat
import csv
import math

def saveStats(res,cf,t):
    w = csv.writer(cf, delimiter=',',
                   quotechar=';', quoting=csv.QUOTE_MINIMAL)
    w.writerow([t]+res)

def printStats(d,cf):
    lkup = {"ee":"ECal_E","et":"ECal_ET","he":"HCal_E","ht":"HCal_ET"}
    for k,v in d.items():
        i = 1
        res = []
        for kk,vv in v.items():
            u = stat.mean(vv)
            stdev = stat.stdev(vv)
            unc = stat.stdev(vv)/math.sqrt(len(vv))
            t = lkup[kk]
            res.append("{:.5}±{:.5}".format(u,unc)) 
            if i%4 == 0:
                saveStats(res, cf, k)
            i += 1
            print(k, t, "{:.5}±{:.5}".format(u,unc))
            #print("{}:\n  {}: Mean={}, Standard Deviation={}, Uncertainty(SE)={}".format(k,kk,u,stdev,unc))

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

def wHead(cf):
    w = csv.writer(cf, delimiter=',',
                   quotechar=';', quoting=csv.QUOTE_MINIMAL)
    w.writerow(["x","ECal_E","ECal_ET","HCal_E", "HCal_ET"])

if __name__ == "__main__":
    f = open("./result/auc.json")
    cf = open("./result/stats.csv", "w", newline="")
    wHead(cf)
    j = json.load(f)
    d = pickOutlier(j)
    print(d)
    printStats(d, cf)
    f.close()
    cf.close()
