import h5py
import numpy as np
import matplotlib.pyplot as plt
import random
from mpl_toolkits.axes_grid1 import make_axes_locatable
from matplotlib.colors import LogNorm
import matplotlib

def plot1(data,fname,vmin,vmax,tit,eorh,label="Energy [GeV]"):
    #matplotlib.rc('axes',titilesize=24)
    fig, ax = plt.subplots(figsize=(8,8))
    ax.set_xlabel(r'$\eta$',fontsize=24)
    ax.set_ylabel(r'$\phi$',fontsize=24)
    for tick in ax.xaxis.get_major_ticks():
        tick.label.set_fontsize(20)
        tick.label.set_rotation(45)
    for tick in ax.yaxis.get_major_ticks():
        tick.label.set_fontsize(20)
    #plt.style.use('default')
    #plt.rc('axes', labelsize=12)
    if tit != '': ax.set_title(tit, fontsize=14)
    if eorh == 'e':
        pos = ax.imshow(data,cmap='plasma',interpolation='none',
                        extent=[-0.3875,0.3875,-15.5*np.pi/126,15.5*np.pi/126],
                        norm=LogNorm(vmin=vmin,vmax=vmax))
    elif eorh == 'h':
        pos = ax.imshow(data,cmap='plasma',interpolation='none',
                        extent=[-0.4,0.4,-4*np.pi/31,4*np.pi/31],
                        norm=LogNorm(vmin=vmin,vmax=vmax))
        
    #divider = make_axes_locatable(ax)
    #cax = divider.append_axes("right", size="5%", pad=0.05)
    cbar = fig.colorbar(pos, fraction=0.0435, pad=0.04, ax=ax)
    cbar.ax.tick_params(labelsize=20)
    cbar.minorticks_on()
    cbar.set_label(label, labelpad=10, fontsize=24)
    plt.savefig(fname, bbox_inches='tight')
     
def plot1_nAvg(s,e,data,fname,vmin,vmax,tit,eorh):
    datax = np.copy(data[0])
    for i in range(s,e):
        datax += data[i]
    datax -= data[0]
    datax = datax/(e-s)
    plot1(datax,fname,vmin,vmax,tit,eorh)

# Separate bg/sig for visualizing the jet images
def splitBgSig(x, y):
    i_bg, i_sig = np.where(y == 0)[0], np.where(y == 1)[0]
    x_bg = x[i_bg]
    x_sig = x[i_sig]
    return x_bg, x_sig

def main():
  idx = "he"
  print("Due to memory limit, now plotting {}".format(idx))
  print("Reading data file... ")

  f = h5py.File('/pub/daohangt/hep/data/all_107442.h5','r')

  if idx == 'ee':
    ee_x = f['features']['Ecal_E'][:]
    ee_y = f['targets']['Ecal_E'][:]
    #ee_x = ee_x/np.max(ee_x)
    ee_x_bg, ee_x_sig = splitBgSig(ee_x, ee_y)
    print("Plotting... {}".format(idx))
    plot1_nAvg(1,20000,ee_x_bg,'./plots/ee_avg_bg.png',0.0001,200,'','e')
    plot1_nAvg(1,20000,ee_x_sig,'./plots/ee_avg_sig.png',0.0001,200,'','e')
    plot1(ee_x_bg[0],'./plots/ee_one_bg.png',0.0001,200,'','e')
    plot1(ee_x_sig[0],'./plots/ee_one_sig.png',0.0001,200,'','e')
  elif idx == 'et':
    et_x = f['features']['Ecal_ET'][:]
    et_y = f['targets']['Ecal_ET'][:]
    et_x_bg, et_x_sig = splitBgSig(et_x, et_y)
  elif idx == 'he':
    he_x = f['features']['Hcal_E'][:]
    he_y = f['targets']['Hcal_E'][:]
    #he_x = he_x/np.max(he_x)
    he_x_bg, he_x_sig = splitBgSig(he_x, he_y)
    print("Plotting... {}".format(idx))
    plot1_nAvg(1,20000,he_x_bg,'./plots/he_avg_bg.png',0.0001,200,'','h')
    plot1_nAvg(1,20000,he_x_sig,'./plots/he_avg_sig.png',0.0001,200,'','h')
    plot1(he_x_bg[0],'./plots/he_one_bg.png',0.0001,200,'','h')
    plot1(he_x_sig[0],'./plots/he_one_sig.png',0.0001,200,'','h')
  elif idx == 'ht':
    ht_x = f['features']['Hcal_ET'][:]
    ht_y = f['targets']['Hcal_ET'][:]
    ht_x_bg, ht_x_sig = splitBgSig(ht_x, ht_y)


if __name__ == "__main__":
  main()
