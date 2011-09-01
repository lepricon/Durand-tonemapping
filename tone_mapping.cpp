/*! \file
  \verbatim
  
    Copyright (c) 2006, Sylvain Paris and Frédo Durand

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

  \endverbatim
*/

#include <cmath>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#include "load_EXR.h"
#include "linear_bf.h"


using namespace std; 


typedef Image_file::EXR::image_type image_type;
typedef image_type::channel_type    channel_type;



inline double log_function(const double x){

  static const double inv_log_base = 1.0 / log(10.0);
  
  return log(x) * inv_log_base;
}


inline double exp_function(const double x){

  return pow(10.0,x);
}



int main(int argc,char** argv){

  if (argc!=4){
    cerr<<"error: wrong arguments"<<endl;
    cerr<<"usage: "<<argv[0]<<" input.exr output.ppm contrast"<<endl;
    cerr<<"\nadvice:"<<endl;
    cerr<<"meaningful values for the contrast are between 5.0 and 200.0.\n50.0 always gives satisfying results."<<endl;
    exit(1);
  }


  // ##############################################################

  
  cout<<"Load the input image '"<<argv[1]<<"'... "<<flush;

  image_type input_RGBA;
  Image_file::EXR::load(argv[1],&input_RGBA);

  const unsigned width  = input_RGBA.width();
  const unsigned height = input_RGBA.height();

  double contrast;
  istringstream contrast_in(argv[3]);
  contrast_in>>contrast;
  
  cout<<"Done"<<endl;


  // ##############################################################

  
  cout<<"Compute the log-intensity channel... "<<flush;

  channel_type intensity_channel(width,height);
  channel_type log_intensity_channel(width,height);

  for(channel_type::iterator
	i     = intensity_channel.begin(),
	i_end = intensity_channel.end(),
	r     = input_RGBA[image_type::RED].begin(),
	g     = input_RGBA[image_type::GREEN].begin(),
	b     = input_RGBA[image_type::BLUE].begin(),
	l     = log_intensity_channel.begin();
      i != i_end;
      i++,r++,g++,b++,l++){

    // The input value are not gamma-corrected. We do not need to worry about that.
       
    *i = ((20.0*(*r) + 40.0*(*g) + 1.0*(*b)) / 61.0);
    *l = log_function(*i);
  }
 
  cout<<"Done"<<endl;


  // ##############################################################
  
  
  cout<<"Filter the log-intensity channel... "<<flush;

  channel_type filtered_log_intensity_channel(width,height);

  FFT::Support_3D::set_fftw_flags(FFTW_ESTIMATE); // parameter for FFTW

  const double space_sigma = 0.02 * min(width,height);
  const double range_sigma = 0.4;
  
  Image_filter::linear_BF(log_intensity_channel,
			  space_sigma,
			  range_sigma,
			  &filtered_log_intensity_channel);
  
  cout<<"Done"<<endl;


  // ##############################################################
  
  
  cout<<"Compute the detail channel... "<<flush;

  channel_type detail_channel(width,height);

  for(channel_type::iterator
	l     = log_intensity_channel.begin(),
	l_end = log_intensity_channel.end(),
	f     = filtered_log_intensity_channel.begin(),
	d     = detail_channel.begin();
      l != l_end;
      l++,f++,d++){

    *d = (*l) - (*f);
  }

  cout<<"Done"<<endl;


  // ##############################################################

  
  cout<<"Compute the new intensity channel... "<<flush;

  const double max_value = *max_element(filtered_log_intensity_channel.begin(),
					filtered_log_intensity_channel.end());
  
  const double min_value = *min_element(filtered_log_intensity_channel.begin(),
					filtered_log_intensity_channel.end());

  const double gamma = log_function(contrast) /  (max_value - min_value);

  channel_type new_intensity_channel(width,height);

  for(channel_type::iterator
	f     = filtered_log_intensity_channel.begin(),
	f_end = filtered_log_intensity_channel.end(),
	d     = detail_channel.begin(),
	n     = new_intensity_channel.begin();      
      f != f_end;
      f++,d++,n++){

    *n = exp_function((*f) * gamma + (*d));
  }
  
  cout<<"Done"<<endl;

  
  // ##############################################################

  
  cout<<"Recompose the color image... "<<flush;

  image_type output_RGBA = input_RGBA;

  for(channel_type::iterator
	n     = new_intensity_channel.begin(),
	n_end = new_intensity_channel.end(),
	i     = intensity_channel.begin(),
	r     = output_RGBA[image_type::RED].begin(),
	g     = output_RGBA[image_type::GREEN].begin(),
	b     = output_RGBA[image_type::BLUE].begin();
      n != n_end;
      n++,i++,r++,g++,b++){

    const double ratio = (*n) / (*i);
    
    *r *= ratio;
    *g *= ratio;
    *b *= ratio;

  }

  cout<<"Done"<<endl;
  

  // ##############################################################
  
  
  cout<<"Write the output image '"<<argv[2]<<"'... "<<flush;

  // Scale factor to ensure that the base spans [0;1].
  const double scale_factor = 1.0 / exp_function(max_value * gamma);
  
  ofstream ppm_out(argv[2],ios::binary);

  ppm_out<<"P6";
  ppm_out<<' ';
  ppm_out<<width;
  ppm_out<<' ';
  ppm_out<<height;
  ppm_out<<' ';
  ppm_out<<"255";
  ppm_out<<'\n';
  
  for(unsigned y=0;y<height;y++){

    const unsigned ry = height - y - 1; // We flip the vertical axis.
    
    for(unsigned x=0;x<width;x++){

      // The following lines corresponds to values *without* gamma-correction.
//       const char r = static_cast<char>(Math_tools::clamp(0.0,255.0,255.0*scale_factor*output_RGBA[image_type::RED](x,ry)));
//       const char g = static_cast<char>(Math_tools::clamp(0.0,255.0,255.0*scale_factor*output_RGBA[image_type::GREEN](x,ry)));
//       const char b = static_cast<char>(Math_tools::clamp(0.0,255.0,255.0*scale_factor*output_RGBA[image_type::BLUE](x,ry)));
      
      // The following lines corresponds to values *with* gamma-correction.
      const char r = static_cast<char>(Math_tools::clamp(0.0,255.0,255.0*pow(scale_factor*output_RGBA[image_type::RED](x,ry),1.0/2.2)));
      const char g = static_cast<char>(Math_tools::clamp(0.0,255.0,255.0*pow(scale_factor*output_RGBA[image_type::GREEN](x,ry),1.0/2.2)));
      const char b = static_cast<char>(Math_tools::clamp(0.0,255.0,255.0*pow(scale_factor*output_RGBA[image_type::BLUE](x,ry),1.0/2.2)));
      
      ppm_out<<r<<g<<b;
    }
  }
  
  ppm_out.flush();
  ppm_out.close();
  
  cout<<"Done"<<endl;
}
