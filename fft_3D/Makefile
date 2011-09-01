#     Copyright (c) 2006, Sylvain Paris and Frédo Durand

#     Permission is hereby granted, free of charge, to any person
#     obtaining a copy of this software and associated documentation
#     files (the "Software"), to deal in the Software without
#     restriction, including without limitation the rights to use, copy,
#     modify, merge, publish, distribute, sublicense, and/or sell copies
#     of the Software, and to permit persons to whom the Software is
#     furnished to do so, subject to the following conditions:

#     The above copyright notice and this permission notice shall be
#     included in all copies or substantial portions of the Software.

#     THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#     EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
#     MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
#     NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#     HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
#     WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#     OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#     DEALINGS IN THE SOFTWARE.



# FFTW section. You may need to change FFTW_INCDIR and FFTW_LIBDIR.
FFTW_INCDIR = 
FFTW_LIBDIR = 
FFTW_LIBS   = $(FFTW_LIBDIR) -lfftw3




######################################################
# NO MODIFICATION SHOULD BE NEEDED BEYOND THIS POINT #
######################################################



# General section
INCDIR  = -I../include $(FFTW_INCDIR)
LIBDIR	= 
LIBS    = $(LIBDIR) $(EXR_LIBS) $(FFTW_LIBS) -lm
TARGET  = fft_3D.o

# Command section
CC	= g++
RM	= rm -f

# General rules
default: support_3D.cpp
	$(CC) support_3D.cpp -c -o $(TARGET) $(INCDIR)
clean:
	$(RM) $(TARGET)
