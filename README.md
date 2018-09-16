# ssann

This repository provides a brief introduction to Safe Synchronous Artificial Neural Networks (SSANNs).

## Publications and Benchmarks

This work was submitted to DATE conference 2019. The benchmarks referred to at this paper may be found in this repository under the `DATE_2019_Benchmarks` folder.

## About

Neural networks are being used in Cyber-Physical Systems (CPSs) to influence
real-time decisions with safety implications. Consequently, there has
been a convergence of AI and formal methods to ensure that such
applications operate safely at all times. This has led to the development of new
techniques for functional verification of AI-based CPS [1]. However,
 the issue of timing verification has received scant attention. 

This repository provides a proposal for developing safe synchronous artificial neural networks, 
termed SSANNs, using runtime enforcement to enforce safety policies for each neural network in the system. 
Each SSANN is formally defined, and both its neural network and enforer both follow synchronous semantics. 
The neural networks, defined as synchronous artificial neural networks (SANNs) [2] are created in 
Esterel and C using our own custom code for basic feedforward ANNs, 
while the Darknet library [3] is used for convolutional neural networks (CNNs).

The enforcers follows semantics described in [4], are are bi-directional enforcers. The policies implemented by them are defined by safety automata.

As this is the first work on safe synchronous neural networks,
there are some limitations and hence several avenues
for future research exist. First, all benchmarks use off-line learning. A
second limitation is that we are relying on ``compiling away'' Esterel
concurrency, which is not ideal for many-core processors. We will develop parallel implementations
of SSANN in the near future.

1. S. A. Seshia, D. Sadigh, and S. S. Sastry, “Towards verified artificial
intelligence,” arXiv preprint arXiv:1606.08514, 2016.
2. P. S. Roop, H. Pearce, and K. Monadjem, “Synchronous neural networks
for cyber-physical systems,” in MEMOCODE ’18 Proceedings of the
16th ACM-IEEE International Conference on Formal Methods and
Models for System Design, 2018.
3. J. Redmon, “Darknet: Open source neural networks in c,” http://pjreddie.
com/darknet/, 2013–2016.
4. S. Pinisetty, P. Roop, S. Smyth, N. Allen, S. Tripakis, and R. von
Hanxleden, “Runtime enforcement of cyber-physical systems,” vol. 16,
pp. 1–25, 09 2017.
