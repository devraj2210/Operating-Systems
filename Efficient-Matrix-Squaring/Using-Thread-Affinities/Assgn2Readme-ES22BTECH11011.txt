Instructions for executing the program:

Two numbers in the text file "InFile.txt" separated by a space.
The first number is N and the second number is M.
When the program is run, it opens the file, reads these numbers from the file and then closes the file.

Output of the program:

All the vampire numbers found along with the total count are printed into the text file "OutFile.txt".

Extra: In this program I have made the runner program such that it directly prints the vampire numbers into the output file. But I have also stored the vampire numbers locally and transferred them to a global array upon completion of the thread. If you wish to use the vampire numbers stored in a main array, please comment out line 113 and uncomment lines 170 to 175.