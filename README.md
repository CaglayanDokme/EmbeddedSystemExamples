# EmbeddedSystemExamples
This is the repo where I share the example designs I've created. 
The example designs has only the purpose of exercising. 
I will try to explain each individual design in a blog post. 
The full list of example designs are as below. 
* [Shared BRAM](https://medium.com/@caglayandokme/a-shared-bram-example-with-microblaze-and-zynq-soc-949495b5f540): A system containing the Zynq PS and Microblaze together. CPUs communicate over a shared BRAM. 
* [Microblaze DDR](https://medium.com/@caglayandokme/extending-the-memory-limits-of-microblaze-with-an-external-ddr-6c896e75c218): A system containing a Microblaze core and a MIG 7 series component to interact with external DDR memory. 

The repo also has some utility files. They can be used to enhance/optimize the process of setting up a development environment. 
* **ProjectCreator**: A file for invoking the Vivado and initially running a tickle file in it. There are two versions of this file, one for [Windows](https://github.com/CaglayanDokme/EmbeddedSystemExamples/blob/main/ProjectCreator.bat)*(.bat)* and the other for [Linux](https://github.com/CaglayanDokme/EmbeddedSystemExamples/blob/main/ProjectCreator.sh)*(.sh)*. 
