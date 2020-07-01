# WindbgReg
A project that helps fix the problem with registers context acquisition inside Windbg.

## Abstract:
We develop a product, and here we get a crash dump. We need to analyze it, but instead of analysis, we get blank register fields!
The problem lies in the new register, which supports the Intel microprocessor - xcr0. The problem is frequent for machines where hardware virtualization is allowed.
The thing is that the register itself is determined by DbgEng but its state is not defined. The bug lies in another product - WinDbg, it has a special reaction for this register. Since this register is incorrect, then the rest can also not be shown!

We have two options: use the old WinDbg which knows nothing about the register, but to suffer from the inconvenience. Or use a new one in which context to receive through the command line.

Thanks to one smart person, I found the third option - we will not show this register to WinDbg. The question is solved by a plugin that will adjust the data received from the DbgEng COM interface.

## Use case
* Open a dump to WinDbg
* Load the plugin into memory 
.load windbgreg
* hook DbgEng interface 
!regfix hook
* analyze a dump

This is how the registers definition problem looks.
![https://github.com/GShabanov/WindbgReg/blob/master/images/pic1.jpg](https://github.com/GShabanov/WindbgReg/blob/master/images/pic1.jpg)

This is how the problem looks after adjusting the register value.
![https://github.com/GShabanov/WindbgReg/blob/master/images/pic2.jpg](https://github.com/GShabanov/WindbgReg/blob/master/images/pic2.jpg)


Compared to the original code, I refused to use a boost, as well as someone third-party library.
COM hooks are really easy. Therefore, the usual WinAPI is enough.

## Version 
   **1.0.0** _first public working code_


##References
1. [Original problem discussion](https://stackoverflow.com/questions/35961246/windbg-not-showing-register-values)
2. [Original code](https://github.com/mbikovitsky/WingDbg)