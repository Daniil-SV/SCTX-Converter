#include "converter.h"
#include <nlohmann/json.hpp>

using json = nlohmann::ordered_json;
namespace fs = std::filesystem;

constexpr auto kTexture = "texture";
constexpr auto kTextures = "textures";
constexpr auto kPixelType = "type";
constexpr auto kGenerateMips = "generate_mip_maps";
constexpr auto kCompression = "compressed";

constexpr auto kUnkFlag1 = "unknown_flag";
constexpr auto kUnkFlag2 = "unknown_flag_1";

constexpr auto kStreaming = "streaming";
constexpr auto kStreamingIds = "ids";

SCTXSerializer::SCTXSerializer(fs::path path, bool is_binary)
{
	if (is_binary)
	{
		load_binary(path);
	}
	else
	{
		if (path.extension() == ".json")
		{
			load_serialized(path);
		}
		else
		{
			load_default_image(path);
		}

	}
}

void SCTXSerializer::load_default_image(std::filesystem::path path)
{
	using namespace sc::texture;

	wk::Ref<wk::RawImage> texture;

	wk::InputFileStream texture_file(path);
	wk::stb::load_image(texture_file, texture);

	m_texture = wk::CreateRef<SupercellTexture>(*texture, SCTXSerializer::def_pixel_type, SCTXSerializer::def_generate_mip_maps);
	m_texture->unknown_flag2 = true;
	m_texture->use_padding = SCTXSerializer::def_padding;
	m_texture->use_compression = SCTXSerializer::def_compression;
}

void SCTXSerializer::load_binary(std::filesystem::path path)
{
	m_texture = wk::CreateRef<sc::texture::SupercellTexture>(path);
	m_texture->read_data();
}

void SCTXSerializer::load_serialized(std::filesystem::path path)
{
	using namespace sc::texture;

	std::ifstream file(path);
	json data = json::parse(file);

	fs::path working_dir = path.parent_path();

	{
		fs::path texture_path = fs::path(working_dir) / data[kTexture];
		ScPixel::Type pixel_type = ScPixel::from_string(data[kPixelType]);
		bool generate_mips = data[kGenerateMips];

		wk::Ref<wk::RawImage> texture;

		wk::InputFileStream texture_file(texture_path);
		wk::stb::load_image(texture_file, texture);
		m_texture = wk::CreateRef<SupercellTexture>(*texture, pixel_type, generate_mips);
		m_texture->unknown_flag1 = data[kUnkFlag1];
		m_texture->unknown_flag2 = data[kUnkFlag2];
	}

	auto& variants_data = data[kStreaming];
	auto& variants_ids = variants_data[kStreamingIds];
	auto& variants_textures = variants_data[kTextures];

	if (variants_ids.is_array())
	{
		m_texture->streaming_ids = variants_ids.template get<std::vector<uint32_t>>();
	}

	if (variants_textures.is_array())
	{
		size_t variants_count = variants_textures.size();
		m_texture->streaming_variants = SupercellTexture::VariantsArray();
		m_texture->streaming_variants->reserve(variants_count);

		for (size_t i = 0; variants_count > i; i++)
		{
			auto& variant_texture = variants_textures[i];

			fs::path texture_path = fs::path(working_dir) / variant_texture[kTexture];
			ScPixel::Type pixel_type = ScPixel::from_string(data[kPixelType]);

			wk::Ref<wk::RawImage> texture;
			wk::InputFileStream texture_file(texture_path);
			wk::stb::load_image(texture_file, texture);
			m_texture->streaming_variants->emplace_back(*texture, pixel_type, false);
		}
	}
}

void SCTXSerializer::save_serialized(std::filesystem::path path, ImagesT& images)
{
	using namespace sc::texture;

	fs::path basename = fs::path(path).stem();
	json data = json::object();
	
	{
		ImageInstance& image = images.emplace_back();
		image.name = basename.string().append(SCTXSerializer::image_postfix);
		SCTXSerializer::decode_texture(*m_texture, image.image);
		data[kTexture] = image.name;
		data[kPixelType] = ScPixel::to_string(m_texture->pixel_type());
		data[kGenerateMips] = m_texture->level_count() > 1;
		data[kUnkFlag1] = m_texture->unknown_flag1;
		data[kUnkFlag2] = m_texture->unknown_flag2;
	}

	auto streaming_textures = json::object();

	if (m_texture->streaming_ids.has_value())
	{
		streaming_textures[kStreamingIds] = m_texture->streaming_ids.value();
	}

	if (m_texture->streaming_variants.has_value())
	{
		json variants_info = json::array();

		auto& variants = m_texture->streaming_variants.value();
		for (size_t i = 0; variants.size() > i; i++)
		{
			auto& variant = variants[i];
			ImageInstance& image = images.emplace_back();
			image.name = basename.string().append("_variant_").append(std::to_string(i)).append(SCTXSerializer::image_postfix);
			SCTXSerializer::decode_texture(variant, image.image);

			json variant_info = json::object();
			variant_info[kTexture] = image.name;
			variant_info[kPixelType] = ScPixel::to_string(variant.pixel_type());
			variants_info.push_back(variant_info);
		}

		streaming_textures[kTextures] = variants_info;
	}

	data[kStreaming] = streaming_textures;

	wk::OutputFileStream stream(path);
	auto result = data.dump(2);
	stream.write(result.data(), result.size());
}

void SCTXSerializer::save_binary(std::filesystem::path path, bool save_compressed, bool save_padded)
{
	wk::OutputFileStream file(path);

	m_texture->use_compression = save_compressed;
	m_texture->use_padding = save_padded;
	m_texture->write(file);
}

void SCTXSerializer::decode_texture(sc::texture::SupercellTexture& texture, wk::Ref<wk::RawImage>& image)
{
	image = wk::CreateRef<wk::RawImage>(texture.width(), texture.height(), texture.depth(), texture.colorspace());
	wk::SharedMemoryStream stream(image->data(), image->data_length());
	texture.decompress_data(stream);
}