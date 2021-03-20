# Learning to Identify Electrons

This is the code used in

Learning to Identify Electrons

Julian Collado, Jessica N. Howard, Taylor Faucett, Tony Tong, Pierre Baldi, Daniel Whiteson

https://arxiv.org/abs/2011.01984

### Abstract 
We investigate whether state-of-the-art classification features commonly used to distinguish electrons from jet backgrounds in collider experiments are overlooking valuable information. A deep convolutional neural network analysis of electromagnetic and hadronic calorimeter deposits is compared to the performance of typical features, revealing a ≈5% gap which indicates that these lower-level data do contain untapped classification power. To reveal the nature of this unused information, we use a recently developed technique to map the deep network into a space of physically interpretable observables. We identify two simple calorimeter observables which are not typically used for electron identification, but which mimic the decisions of the convolutional network and nearly close the performance gap. 

### Training
Make sure to update the path to the data location in [line 10][line10] and remove the assert statement in the previous line.

To train the models simply execute the python script training.py

```bash
python training.py 
```

### Data 
Please go to UCI [MLPhysics Portal][MLPhysics] to download the dataset used in the paper. For the details about making images from the ROOT files generated from Delphes, please check the scripts in `src`.


[root-url]: https://root.cern.ch/
[delphes-url]: https://cp3.irmp.ucl.ac.be/projects/delphes
[line10]: https://github.com/TDHTTTT/eID/blob/b2356c0e1f9bd6a9d0949dae3cae87e557802bab/train/data_loader.py#L10
[MLPhysics]: http://mlphysics.ics.uci.edu/data/2020_electron/
