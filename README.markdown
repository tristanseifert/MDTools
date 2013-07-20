This repository contains some tools that are useful in Mega Drive and Sega CD development:

*   *sinegen* - generates a simple sine wave to a binary file.
*   *bmp2scdasic* - converts a 24-bit bmp file to Sega CD ASIC data (incomplete)
*   *pcmbankmaker* - takes a directory of already Sega CD PCM chip formatted samples and creates a sample bank out of them for an upcomming driver.
*	*VDPCalc* - generates 32-bit VDP command values to use to access VRAM, CRAM, and VSCRAM.
*	*mod2pcm* - converts a MOD file to the format needed for an upcomming PCM driver for the Sega CD. (See tristanseifert/SCDTemplate)
*	*sign2signmag* - converts a regular Two's Compliment sample file to the sign/magnitude format that the Sega CD PCM chip requires.

These tools are all written in C and should run on all platforms (compiled on Mac OS X (clang) and Windows (gcc) are verified) but other compilers/OS combinations should work, so long as they comply with the C99 standard.)


These tools are released under the BSD License:

	Copyright (c) 2012-2013, Tristan Seifert
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	*   Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	*   Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
	*   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
