 Table of content
##################

1- How to install
2- How to use
3- The algorithm
4- The code
5- Improving performances






 1- How to install
###################

a) Requirements
---------------

This code has been made and tested under Linux. The following
libraries need to be installed:
FFTW:    http://www.fftw.org/download.html
OpenEXR: http://www.openexr.com/downloads.html


b) Compiling
------------

Running 'make' should compile everything out of the box. If there are
problems with FFTW or OpenEXR, you might need to provide the path to
their header files and libraries. This is done by editing these two
files: 'Makefile' and 'fft_3D/Makefile'.








 2- How to use
###############

On the command line, type:
./tone_mapping input.exr output.ppm 50.0

The parameters are:

- 'input.exr': It is a HDR image file in the OpenEXR format.

- 'output.ppm': It is the image that will be created. The file format
is PPM. Software such as Photoshop, Gimp, and xv can read this format.

- 50.0: It is the contrast of the result. Meaningful values are
between 5.0 and 200.0. By default, you can use 50.0, that performs
well.









 3- The algorithm
##################

Here is the pseudo-code for the algorithm:
(a) load a HDR RGB image
(b) compute an intensity layer I
(c) compute log(I)
(d) filter log(I) using the bilateral filter to get log(F)
(e) compute a detail channel D = log(I) - log(F)
(f) compute: delta = max[log(F)] - min[log(F)]
(g) compute: gamma = log(constrast) / delta
(h) compute the new intensity layer: N = 10^[gamma*log(F) + D]
(i) scale the RGB values by N/I
(j) save a LDR image

Comments
--------

(b) We use the simple formula: I = (20R + 40G + B) / 61;

(c) We use the logarithm in base 10.

(g) 'constrast' is the parameter given on the command line

(j) To ensure a correct display, the image should be
gamma-corrected. First, we scale the RGB values by
1/max[gamma*log(F)]. This ensures that the new intensity of the base
layer F spans [0:1]. Second, we gamma-correct the RGB values using a
standard gamma value (2.2). Finally, we quantize the RGB values down
to 8 bits.









 4- The code
#############

The code is in C++.

The algorithm described above is in 'tone_mapping.cc'. This is the
file that you may want to edit. The code follows the description
exactly. Comments precede each part. Variable names are long and
self-explanatory.

You should not need to edit the other files. But in case you want to,
here is a short description of each of them.


In the 'include' directory:
---------------------------

- array.h: Classes 2D and 3D arrays of values.

- channel_image.h: Classes for mulit-channel images.

- fft_3D.h: Includes the files need for 3D Fourier transforms.

- linear_bf.h: Provides a fast bilateral filter based on 3D FFT.

- load_EXR.h: C++ wrapper for OpenEXR.

- math_tools.h: Several useful simple functions.

- msg_stream.h: C++ streams for warnings and errors.


In the 'fft_3D' directory:
--------------------------

- convolution_3D.h: Provides convolution between 3D functions.

- fill_3D.h: Fills 3D arrays with function values.

- support_3D.cc, support_3D.h: Provides the basic support for 3D
  Fourier transforms



  


5- Improving performances
#########################

Compiler optimizations can speed up the process a lot. However, the
provided makefile does not use any optimization to maximize the
chances of successful compilation. For instance, with 'gcc' you may
try options such as '-O3' and '-march=pentium4'.



FFTW can also run faster if you use the "wisdom" system. For that, you
need to comment the line:

FFT::Support_3D::set_fftw_flags(FFTW_ESTIMATE);

and to define an environment variable FFTW_WISDOM with the name of the
file that will store FFTW data. These data are machine-dependent
i.e. you cannot share them between different machines. Under Linux
with 'tcsh', you can use this:

setenv FFTW_WISDOM ${HOME}/wisdom.${HOST}.fftw

Then, the first run on a given image size will be slow (up to one hour
on pictures with several megapixels) but the next ones will be faster.

