#pragma once

#include "texture/texture.h"
#include "core/stb/stb.h"

#include <filesystem>

class SCTXSerializer
{
public:
	struct ImageInstance
	{
		std::string name;
		wk::Ref<wk::RawImage> image;
	};

	using ImagesT = std::vector<ImageInstance>;

public:
	static inline std::string image_postfix = ".png";

public:
	SCTXSerializer(std::filesystem::path path, bool is_binary);

public:
	void load_binary(std::filesystem::path path);
	void load_serialized(std::filesystem::path path);

public:
	void save_serialized(std::filesystem::path path, ImagesT& images);
	void save_binary(std::filesystem::path path, bool save_compressed);

private:
	static void decode_texture(sc::texture::SupercellTexture& texture, wk::Ref<wk::RawImage>& image);

private:
	wk::Ref<sc::texture::SupercellTexture> m_texture;
};