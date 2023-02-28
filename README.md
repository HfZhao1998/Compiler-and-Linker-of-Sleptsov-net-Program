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

Sn file formats:
------------
Two documents indicate how to define HSN.txt and LSN.txt files.

Example:
------------
add_lsn.txt: LSN file for binary adder module;

HSN_example.txt: HSN file with three numbers added，it can be expressed as D=A+B+C;

Result_lsn_example.txt: Result file generated after HSN_example.txt file conversion.

Pdf and ndr files of models:
------------
These files show the model graphically. You can download Tina to load the ndr file.  http://www.laas.fr/tina
