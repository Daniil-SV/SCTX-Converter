#include "main.h"
#include "core/stb/stb.h"

namespace fs = std::filesystem;

void decode(std::filesystem::path input, std::filesystem::path output)
{
	if (output.empty())
	{
		output = fs::path(input).replace_extension(".json");
	}
	fs::path basepath = output.parent_path();

	SCTXSerializer serializer(input, true);

	SCTXSerializer::ImagesT images;
	serializer.save_serialized(output, images);

	for (auto& image : images)
	{
		fs::path image_path = fs::path(basepath) / image.name;
		wk::OutputFileStream stream(image_path);

		wk::stb::write_image(*image.image, wk::stb::ImageFormat::PNG, stream);
	}
}

void encode(std::filesystem::path input, std::filesystem::path output)
{
	if (output.empty())
	{
		output = fs::path(input).replace_extension(".sctx");
	}
}

void program(wk::ArgumentParser& args)
{
	fs::path input = args.get("input");
	fs::path output = args.get("output");
	std::string mode = args.get("mode");

	if (mode == "decode")
	{
		decode(input, output);
	}
}

int main(int argc, char* argv[])
{
	std::filesystem::path executable = argv[0];
	std::filesystem::path executable_name = executable.stem();

	wk::ArgumentParser parser(executable_name.string(), "Tool for compress and decompress Supercell Textures (SCTX)");

	parser.add_argument("mode")
		.help("Possible values: compress, decompress")
		.choices("decode", "encode");

	parser.add_argument("input")
		.help("Path to sctx or info file");

	parser.add_argument("output")
		.default_value("")
		.help("Path to output sctx or info file. Optional.");

	try
	{
		parser.parse_args(argc, argv);
	}
	catch (const std::exception&)
	{
		parser.print_help();
		return 0;
	}

	program(parser);
	return 0;
}