# Description
Compiler and linker of Sleptsov net program -- To complete the conversion from high-level Sleptsov net (HSN) to low-level Sleptsov net (LSN). 
Sleptsov nets represent graphical language for concurrent programming. Some transitions in the Sleptsov net (SN) can be substituted by subnets, resulting in a hierarchical Sleptsov net, which we call the high-level Sleptsov net (HSN). In this way, we can facilitate the function extension on the basis of the existingnetmodel. And those nets without transition substitution can be called plain Sleptsov nets or low-level Sleptsov nets (LSNs). 
However, only LSN can run on SN processors, so we need to convert HSN to LSN.

How to use `HSNtoLSN` as a part of experimental `SNC IDE&VM`:
------------------------------------------------------------

We list references to components in "Compatibility" section.

1) Use `Tina` `nd` as graphical editor and its labels with special syntax (section "Transition substitution label") to specify transition substitution of `HSN`.

2) Use `NDRtoSN` to convert `NDR` file of `Tina` into `HSN` or `LSN`. 

3) Use `HSNtoLSN` to compile and link HSN file and mentioned in it `LSN` files into a single `LSN` file.

4) Run `LSN` file on `SN-VM` or `SN-VM-GPU`.


Compatibility: 
-------------- 

`Tina`, `nd`, and `NDR` file format according to https://projects.laas.fr/tina/index.php

`NDRtoSN` and transition substitution labels according to https://github.com/dazeorgacm/NDRtoSN

`SN-VM` and `LSN` file format according to https://github.com/zhangq9919/Sleptsov-net-processor

`HSNtoLSN` and `HSN` file format according to https://github.com/HfZhao1998/Compiler-and-Linker-of-Sleptsov-net-Program

`SN-VM-GPU` and `MSN` file format according to https://github.com/tishtri/SN-VM-GPU

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

And matrix_mul_2by2.ndr, matrix_par_2by2.ndr defines an HSN using label, which can be converted into an HSN file using NDRtoSN. 

Generators:
------------

Generators used to generate HSN files for testing.

Command line example:

gen-k-add-hsn 10 addHSN10.txt

gen-k-mul-hsn 10 mulHSN10.txt

gen-k-matrix-mul-hsn 5 matrixHSN5.txt

gen-k-matrix-par-hsn 5 matrixHSN5.txt

References:
------------
1. Zaitsev D.A. Sleptsov Nets Run Fast, IEEE Transactions on Systems, Man, and Cybernetics: Systems, 2016, Vol. 46, No. 5, 682 - 693. http://dx.doi.org/10.1109/TSMC.2015.2444414

2. Zaitsev D.A., Jürjens J. Programming in the Sleptsov net language for systems control, Advances in Mechanical Engineering, 2016, Vol. 8(4), 1-11. https://doi.org/10.1177%2F1687814016640159

3. Zaitsev D.A. Universal Sleptsov Net, International Journal of Computer Mathematics, 94(12) 2017, 2396-2408. http://dx.doi.org/10.1080/00207160.2017.1283410

4. Dmitry A. Zaitsev, Strong Sleptsov nets are Turing complete, Information Sciences, Volume 621, 2023, 172-182. https://doi.org/10.1016/j.ins.2022.11.098

5. Qing Zhang, Ding Liu, Yifan Hou, Sleptsov Net Processor, International Conference ”Problems of Infocommunications. Science and Technology” (PICST2022), 10-12 October, 2022, Kyiv, Ukraine.

6. Hongfei Zhao, Ding Liu, Yifan Hou, Compiler and Linker of Sleptsov Net Program,International Conference ”Problems of Infocommunications. Science and Technology” (PICST2022), 10-12 October, 2022, Kyiv, Ukraine.

7. Sleptsov Net Computing Resolves Modern Supercomputing Problems, The April 21, 2023, edition of ACM TechNews, https://technews.acm.org/archives.cfm?fo=2023-04-apr/apr-21-2023.html
