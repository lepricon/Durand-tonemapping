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




# OpenEXR section. You may need to change EXR_INCDIR and EXR_LIBDIR.
EXR_INCDIR = -I/usr/include/OpenEXR
EXR_LIBDIR = 
EXR_LIBS   = $(EXR_LIBDIR) -lIlmImf -lImath -lHalf -lIex -lz

# FFTW section. You may need to change FFTW_INCDIR and FFTW_LIBDIR.
FFTW_INCDIR = 
FFTW_LIBDIR = 
FFTW_LIBS   = $(FFTW_LIBDIR) -lfftw3




######################################################
# NO MODIFICATION SHOULD BE NEEDED BEYOND THIS POINT #
######################################################



# General section
INCDIR    = -I. -Ifft_3D -Iinclude $(EXR_INCDIR) $(FFTW_INCDIR)
LIBDIR	  = 
LIBS      = $(LIBDIR) $(EXR_LIBS) $(FFTW_LIBS) -lm
OBJECTS   = tone_mapping.o fft_3D/fft_3D.o
TARGET    = tone_mapping


# Command section
CC	= g++
LINK	= g++
MAKE	= make
RM	= rm -f

# General rules
default: $(OBJECTS)
	$(LINK) $(OBJECTS) $(LIBS) -o $(TARGET)

clean:
	$(RM) $(OBJECTS)
	$(RM) $(TARGET)
	cd fft_3D && $(MAKE) clean

# File rules
tone_mapping.o: tone_mapping.cpp
	$(CC) tone_mapping.cpp -c -o tone_mapping.o $(INCDIR)

fft_3D/fft_3D.o:
	cd fft_3D && $(MAKE)
