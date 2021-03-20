# EID

Some macro analysis scripts on the [ROOT][root-url] file generated from [Delphes][delphes-url], looking for interesting information about electrons from a certain event and generating jet images which can be used train Neural Networks. Essentially, we want to identify the electrons from background or some interesting physics phenomenon. The two processes of interest:

Background | Signal
--- | --- 
p p > j j | p p > Z' > e+ e-

### Usage
To use the script, first add your delphes directory in `init.sh`, then simply execute the shell script:

```bash
./makeImages.sh <ROOT_FILE_DIR> 
```

When the analysis finishes, you will find: 

```bash
output
├ csv
└ png
```


[root-url]: https://root.cern.ch/
[delphes-url]: https://cp3.irmp.ucl.ac.be/projects/delphes


### Training
Make sure to update the path to the data location in line https://github.com/TDHTTTT/eID/blob/b2356c0e1f9bd6a9d0949dae3cae87e557802bab/train/data_loader.py#L10 and remove the assert statement in the previous line.

To train the models simply execute the python script training.py

```bash
python training.py 
```
