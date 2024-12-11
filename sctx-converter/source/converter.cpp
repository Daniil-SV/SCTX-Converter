#include "converter.h"
#include <nlohmann/json.hpp>

using json = nlohmann::ordered_json;
namespace fs = std::filesystem;

SCTXSerializer::SCTXSerializer(fs::path path, bool is_binary)
{
	if (is_binary)
	{
		load_binary(path);
	}
	else
	{
		load_serialized(path);
	}
}

void SCTXSerializer::load_binary(std::filesystem::path path)
{
	m_texture = wk::CreateRef<sc::texture::SupercellTexture>(path);
	m_texture->read_data();
}

void SCTXSerializer::load_serialized(std::filesystem::path path)
{

}

void SCTXSerializer::save_serialized(std::filesystem::path path, ImagesT& images)
{
	fs::path basename = fs::path(path).stem();
	json data = json::object();
	
	{
		ImageInstance& image = images.emplace_back();
		image.name = basename.string().append(SCTXSerializer::image_postfix);
		SCTXSerializer::decode_texture(*m_texture, image.image);
		data["texture"] = image.name;
		data["generate_mip_maps"] = m_texture->level_count() > 1;
	}

	auto& streaming_textures = json::object();

	if (m_texture->streaming_ids.has_value())
	{
		streaming_textures["ids"] = m_texture->streaming_ids.value();
	}

	if (m_texture->streaming_variants.has_value())
	{
		std::vector<std::string> paths;
		auto& variants = m_texture->streaming_variants.value();
		for (size_t i = 0; variants.size() > i; i++)
		{
			auto& variant = variants[i];
			ImageInstance& image = images.emplace_back();
			image.name = basename.string().append("_variant_").append(std::to_string(i)).append(SCTXSerializer::image_postfix);
			SCTXSerializer::decode_texture(variant, image.image);

			paths.push_back(image.name);
		}

		streaming_textures["textures"] = paths;
	}

	data["streaming"] = streaming_textures;

	wk::OutputFileStream stream(path);
	auto result = data.dump(2);
	stream.write(result.data(), result.size());
}

void SCTXSerializer::save_binary(std::filesystem::path path)
{

}

void SCTXSerializer::decode_texture(sc::texture::SupercellTexture& texture, wk::Ref<wk::RawImage>& image)
{
	image = wk::CreateRef<wk::RawImage>(texture.width(), texture.height(), texture.depth(), texture.colorspace());
	wk::SharedMemoryStream stream(image->data(), image->data_length());
	texture.decompress_data(stream);
}