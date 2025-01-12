#pragma once

#include "texture/texture.h"
#include "core/stb/stb.h"

#include <filesystem>

#include "texture/backend/Supercell/ScPixel.hpp"

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

	static inline bool def_generate_mip_maps = true;
	static inline bool def_compression = true;
	static inline bool def_padding = false;
	static inline sc::texture::ScPixel::Type def_pixel_type = sc::texture::ScPixel::Type::ASTC_RGBA8_8x8;

public:
	SCTXSerializer(std::filesystem::path path, bool is_binary);

public:
	void load_binary(std::filesystem::path path);
	void load_serialized(std::filesystem::path path);

public:
	void save_serialized(std::filesystem::path path, ImagesT& images);
	void save_binary(std::filesystem::path path, bool save_compressed, bool save_padded);

public:
	void load_default_image(std::filesystem::path path);

private:
	static void decode_texture(sc::texture::SupercellTexture& texture, wk::Ref<wk::RawImage>& image);

private:
	wk::Ref<sc::texture::SupercellTexture> m_texture;
};