// Copyright 2021 Jan Niklas Hasse <jhasse@bixense.com>
// For conditions of distribution and use, see copyright notice in LICENSE.txt
#pragma once

#include "jngl/ImageData.hpp"

#include <webp/decode.h>

namespace jngl {

class ImageDataWebP : public ImageData {
public:
	explicit ImageDataWebP(std::string filename, FILE*, double scaleFactor);
	~ImageDataWebP() override;
	ImageDataWebP(const ImageDataWebP&) = delete;
	ImageDataWebP& operator=(const ImageDataWebP&) = delete;
	ImageDataWebP(ImageDataWebP&&) noexcept;
	ImageDataWebP& operator=(ImageDataWebP&&) noexcept;

	const uint8_t* pixels() const override;
	int getWidth() const override;
	int getHeight() const override;

	/// we might scale the image using WebP built-in filter. These functions will return the
	/// original image size, not the size of the resulting pixel buffer.
	int getImageWidth() const;
	int getImageHeight() const;

private:
	std::unique_ptr<std::thread> thread;
	VP8StatusCode result;
	WebPDecoderConfig config{};
	std::string filename;
	int imgWidth = 0;
	int imgHeight = 0;
	int scaledWidth = 0;
	int scaledHeight = 0;
};

} // namespace jngl