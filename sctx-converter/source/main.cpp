#include "main.h"
#include "core/stb/stb.h"

namespace fs = std::filesystem;

void decode(std::filesystem::path input, std::filesystem::path output)
{
	if (output.empty())
	{
		output = fs::path(input).replace_extension(texture_only ? "png" : ".json");
	}
	fs::path basepath = output.parent_path();

	SCTXSerializer serializer(input, true);

	SCTXSerializer::ImagesT images;
	serializer.save_serialized(output, images);

	for (auto& image : images)
	{
		fs::path image_path = fs::path(basepath) / image.name;
		wk::OutputFileStream stream(texture_only ? output : image_path);

		wk::stb::write_image(*image.image, wk::stb::ImageFormat::PNG, stream);

		// write only the first (level 0) texture
		if (texture_only) break;
	}
}

void encode(std::filesystem::path input, std::filesystem::path output)
{
	if (output.empty())
	{
		output = fs::path(input).replace_extension(".sctx");
	}

	SCTXSerializer serializer(input, false);
	serializer.save_binary(output, compress_data);
}

void program(wk::ArgumentParser& args)
{
	fs::path input = args.get("input");
	fs::path output = args.get("output");
	std::string mode = args.get("mode");
	compress_data = args.get<bool>("compress-data");
	texture_only = args.get<bool>("texture-only");

	if (mode == "decode")
	{
		decode(input, output);
	}
	else if (mode == "encode")
	{
		encode(input, output);
	}
}

int main(int argc, char* argv[])
{
	std::filesystem::path executable = argv[0];
	std::filesystem::path executable_name = executable.stem();

	wk::ArgumentParser parser(executable_name.string(), "Tool for compress and decompress Supercell Textures (SCTX)");

	parser.add_argument("mode")
		.help("Possible values: decode, encode")
		.choices("decode", "encode");

	parser.add_argument("input")
		.help("Path to sctx or info file");

	parser.add_argument("output")
		.default_value("")
		.help("Path to output sctx/info file/png file. Optional.");

	parser.add_argument("--compress-data", "-c")
		.flag()
		.help("Compress texture data using ZSTD when encoding texture");

	parser.add_argument("--texture-only", "-t")
		.flag()
		.help("Decompress only texture, without json file. But after that you will not be able to convert texture back to sctx");

	try
	{
		parser.parse_args(argc, argv);
	}
	catch (const std::exception&)
	{
		parser.print_help();
		return 0;
	}

#ifndef _DEBUG
	try
	{
#endif
		program(parser);

#ifndef _DEBUG
	}
	catch (const std::exception& exception)
	{
		std::cout << exception.what() << std::endl;
		return 1;
	}
#endif	
	return 0;
}