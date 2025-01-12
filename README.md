<p align="center">
<h1 align="center" style="font-size: 32px;"> Supercell Texture Converter </h1>
</p>

# Description
Supercell continues to improve their Titan engine and now the modding community is faced with another galactic-scale problem in the form of .sctx files.  
Seriously though, Supercell has been using .sctx files more and more lately, this is a texture files that are basically a flatbuffer that supports things like mip-mapping or provides texture streaming support  
So this program can extract all data from sctx into .png and also a small .json file which contains some data which cannot be simply entered through console at the moment  

# How to use
Download the program from [Releases](https://github.com/Daniil-SV/SCTX-Converter/releases), unpack it to a convenient place and open the console in that place.  
To convert `SCTX` to a pair of `JSON` and `PNG` use a command like that:  
`
SctxConverter.exe decode file.sctx output.json
`  
Where the first argument describes mode (decode from sctx), the second argument describes the path to the input file (sctx) and the second argument is the output, for the decoding option it is json (or to png with `-t` flag). Also the output argument is optional, without it path to the output will be automatically determined based on the input argument.  

After that you can make all the necessary changes in png  

To convert `JSON` or `PNG` back to `SCTX` use a command like this:  
`
SctxConverter.exe encode input.json output.sctx
`  
Where the first argument describes mode (encode to sctx), the second argument describes the path to the input file (json or png) and the second argument is the output. The output argument is also optional, just like for decode.  

Also a little about additional flags:

- `--compress-data` or `-c`:  use this option while encoding to compress data and make the resulting file smaller  

- `--texture-only` or `-t`: use this option if you just want to get png texture, without json. Also keep in mind that without json file you will not be able to convert png back to sctx  

- `--use-padding` or `-p`: aligns file to 16 bytes. I don't know why, but this feature is supported by format itself  


