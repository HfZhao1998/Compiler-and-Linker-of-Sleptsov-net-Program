# Description
Compiler and linker of Sleptsov net program -- To complete the conversion from high-level Sleptsov net (HSN) to low-level Sleptsov net (LSN). 
Sleptsov nets represent graphical language for concurrent programming. Some transitions in the Sleptsov net (SN) can be substituted by subnets, resulting in a hierarchical Sleptsov net, which we call the high-level Sleptsov net (HSN). In this way, we can facilitate the function extension on the basis of the existingnetmodel. And those nets without transition substitution can be called plain Sleptsov nets or low-level Sleptsov nets (LSNs). 
However, only LSN can run on SN processors, so we need to convert HSN to LSN.

Command line formats:
------------
HSNtoLSN hsn_file.txt result_lsn_file.txt

Basic documents：
------------
HSNtoLSN.c is used to complete the conversion.

Copy.txt and clean_move.txt are functional modules and need to be in the same directory as the HSNtoLSN.c file.

File formats:
------------
New version of file format. Two documents indicate how to define HSN.txt and LSN.txt files.

Examples:
------------
Modules of new file format.

Pdf and ndr files of models:
------------
These files show the model graphically. You can download Tina to load the ndr file.  http://www.laas.fr/tina

And matri_mul_2by2.ndr defines an HSN using label, which can be converted into an HSN file using NDRtoSN. https://github.com/dazeorgacm/NDRtoSN.git

Generators:
------------

Generators used to generate HSN files for testing.

Command line example:

gen-k-add-hsn 10 addHSN10.txt

gen-k-mul-hsn 10 mulHSN10.txt

gen-k-matrix-mul-hsn 5 matrixHSN5.txt

Compatibility:
------------
NDR file format according to https://projects.laas.fr/tina/index.php

LSN file format v1.2 according to https://github.com/zhangq9919/Sleptsov-net-processor

References:
------------
1.D.A. Zaitsev, Universal Sleptsov net, International Journal of Computer Mathematics, 94(12) (2017) 2396–2408. https://doi.org/10. 1080/00207160.2017.1283410
2.D.A. Zaitsev, Sleptsov Nets Run Fast, IEEE Transactions on Systems, Man, and Cybernetics: Systems 46(5) 682–693. https://doi.org/10. 1109/TSMC.2015.2444414
3.D.A. Zaitsev, Sleptsov Net Computing, Chapter 672 in Mehdi KhosrowPour (Ed.) Encyclopedia of Information Science and Technology, Fourth Edition (10 Volumes). IGI-Global: USA, 2017, 7731–7743.HTTPS:// doi.org/10.4018/978-1-5225-2255-3.ch672 
4.D.A. Zaitsev, J. J¨urjens . Programming in the Sleptsov net language for systems control[J]. Advances in Mechanical Engineering, 2016, 8(4).
5.D.A. Zaitsev, S. Tomov and J. Dongarra, Solving Linear Diophantine Systems on Parallel Architectures. IEEE Transactions on Parallel and Distributed Systems, vol. 30, no. 5, pp. 1158-1169, 1 May 2019, doi: 10.1109/TPDS.2018.2873354. 
6.A. I. Sleptsov, ”Equations and equivalent transformations of loaded Petri nets (algebraic approach)”, Proc. Comm. All-Union Conf. Formal Models Parallel Comput., pp. 151-158, 1988.
7.Zhi Gang Zhu. Analysis and Research of Sorting Algorithm in Data Structure Based on C Language[J]. Journal of Physics Conference Series,2020,1544(1).
8.Qing Zhang, Ding Liu, Yifan Hou, Sleptsov Net Processor, International Conference ”Problems of Infocommunications. Science and Technology” (PICST2022), 10-12 October, 2022, Kyiv, Ukraine.
9.Hongfei Zhao, Ding Liu, Yifan Hou, Compiler and Linker of Sleptsov Net Program,International Conference ”Problems of Infocommunications. Science and Technology” (PICST2022), 10-12 October, 2022, Kyiv, Ukraine.
